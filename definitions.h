#define SDL_MAIN_HANDLED
#include "SDL2/SDL.h"
#include "stdlib.h"
#include "stdio.h"
#include "math.h"

#ifndef DEFINITIONS_H
#define DEFINITIONS_H

/******************************************************
 * DEFINES:
 * Macros for universal variables/hook-ups.
 *****************************************************/
#define WINDOW_NAME "Pipeline"
#define S_WIDTH     512
#define S_HEIGHT    512
#define PIXEL       Uint32
#define ABS(in) (in > 0 ? (in) : -(in))
#define SWAP(TYPE, FIRST, SECOND) { TYPE tmp = FIRST; FIRST = SECOND; SECOND = tmp; }
#define MIN(A,B) A < B ? A : B
#define MAX(A,B) A > B ? A : B
#define MIN3(A,B,C) MIN((MIN(A,B)),C)
#define MAX3(A,B,C) MAX((MAX(A,B)),C)

// Max # of vertices after clipping
#define MAX_VERTICES 8 

// Pre-computed values of trig functions
#define SIN45 0.707107
#define COS45 0.707107

#define SIN30 0.5
#define COS30 0.866025

/******************************************************
 * Types of primitives our pipeline will render.
 *****************************************************/
enum PRIMITIVES 
{
    TRIANGLE,
    LINE,
    POINT
};

/****************************************************
 * Describes a geometric point in 3D space. 
 ****************************************************/
struct Vertex
{
    double x;
    double y;
    double z;
    double w;
};

/******************************************************
 * BUFFER_2D:
 * Used for 2D buffers including render targets, images
 * and depth buffers. Can be described as frames or 
 * 2D arrays ot type 'T' encapsulated in an object.
 *****************************************************/
template <class T>
class Buffer2D 
{
    protected:
        T** grid;
        int w;
        int h;

        // Private intialization setup
        void setupInternal()
        {
            // Allocate pointers for column references
            grid = (T**)malloc(sizeof(T*) * h);                
            for(int r = 0; r < h; r++)
            {
                grid[r] = (T*)malloc(sizeof(T) * w);
            }
        }

        // Empty Constructor
        Buffer2D()
        {}

    public:
        // Free dynamic memory
        ~Buffer2D()
        {
            // De-Allocate pointers for column references
            for(int r = 0; r < h; r++)
            {
                free(grid[r]);
            }
            free(grid);
        }

        // Size-Specified constructor, no data
        Buffer2D(const int & wid, const int & hgt)
        {
            h = hgt;
            w = wid;
            setupInternal();
            zeroOut();
        }

        // Assignment constructor
        Buffer2D& operator=(const Buffer2D & ib)
        {
            w = ib.width();
            h = ib.height();
            setupInternal();
            for(int r = 0; r < h; r++)
            {
                for(int c = 0; c < w; c++)
                {
                    grid[r][c] = ib[r][c];
                }
            }
        }

        // Set each member to zero 
        void zeroOut()
        {
            for(int r = 0; r < h; r++)
            {
                for(int c = 0; c < w; c++)
                {
                    grid[r][c] = 0;
                }
            }
        }

        // Width, height
        const int & width()  { return w; }
        const int & height() { return h; }

        // The frequented operator for grabbing pixels
        inline T* & operator[] (int i)
        {
            return grid[i];
        }
};


/****************************************************
 * BUFFER_IMAGE:
 * PIXEL (Uint32) specific Buffer2D class with .BMP 
 * loading/management features.
 ***************************************************/
class BufferImage : public Buffer2D<PIXEL>
{
    protected:       
        SDL_Surface* img;                   // Reference to the Surface in question
        bool ourSurfaceInstance = false;    // Do we need to de-allocate?

        // Private intialization setup
        void setupInternal()
        {
            // Allocate pointers for column references
            h = img->h;
            w = img->w;
            grid = (PIXEL**)malloc(sizeof(PIXEL*) * h);                

            PIXEL* row = (PIXEL*)img->pixels;
            row += (w*h);
            for(int i = 0; i < h; i++)
            {
                grid[i] = row;
                row -= w;                    
            }
        }

    public:
        // Free dynamic memory
        ~BufferImage()
        {
            // De-Allocate pointers for column references
            free(grid);

            // De-Allocate this image plane if necessary
            if(ourSurfaceInstance)
            {
                SDL_FreeSurface(img);
            }
        }

        // Assignment constructor
        BufferImage& operator=(const BufferImage & ib)
        {
            img = ib.img;
            w = ib.w;
            h = ib.h;
            ourSurfaceInstance = false;
            grid = (PIXEL**)malloc(sizeof(PIXEL*) * img->h);                
            for(int i = 0; i < img->w; i++)
            {
                grid[i] = ib.grid[i];
            }
        }

        // Constructor based on instantiated SDL_Surface
        BufferImage(SDL_Surface* src) 
        { 
            img = src; 
            w = src->w;
            h = src->h;
            ourSurfaceInstance = false;
            setupInternal();
        }

        // Constructor based on reading in an image - only meant for UINT32 type
        BufferImage(const char* path) 
        {
            ourSurfaceInstance = true;
            SDL_Surface* tmp = SDL_LoadBMP(path);      
            SDL_PixelFormat* format = SDL_AllocFormat(SDL_PIXELFORMAT_ARGB8888);
            img = SDL_ConvertSurface(tmp, format, 0);
            SDL_FreeSurface(tmp);
            SDL_FreeFormat(format);
            setupInternal();
        }
};

union attrib
{
    double d;
    void* ptr;
};

class Attributes; // forward declaration of Attributes

/***************************************************
 * MATRIX - Class to facilitate matrix operations
 **************************************************/
class Matrix {
    public:
    Matrix() { clear(); }               // initialize with identity matrix
    Matrix(double* values);             // initialize with 16 values
    Matrix(const Attributes& uniforms); // initialize with 16 values from the uniforms

    double v[4][4];

    void clear(); // set back to default state (identity matrix)

    const double& operator[](const int i) const { return v[i/4][i%4]; } // array access
    double& operator[](const int i) { return v[i/4][i%4]; }             // non-const array access
};

/***************************************************
 * ATTRIBUTES (shadows OpenGL VAO, VBO)
 * The attributes associated with a rendered 
 * primitive as a whole OR per-vertex. Refactoring
 * done by Brother Christensen:
 * https://github.com/dwc3993/CS312_Graphics/blob/dchristensen_pipeline/definitions.h
 **************************************************/
class Attributes
{      
    public:
        // Members
    	int numMembers = 0;
        attrib arr[16];

        Matrix matrix;

        // Obligatory empty constructor
        Attributes() {numMembers = 0;}

        // Interpolation Constructor
        Attributes( const double & areaTriangle, const double & firstDet, const double & secndDet, const double & thirdDet, 
                    const Attributes & first, const Attributes & secnd, const Attributes & third, const double interpZ)
        {
            while(numMembers < first.numMembers)
            {
                arr[numMembers].d =  (firstDet/areaTriangle) * (third.arr[numMembers].d);
                arr[numMembers].d += (secndDet/areaTriangle) * (first.arr[numMembers].d);
                arr[numMembers].d += (thirdDet/areaTriangle) * (secnd.arr[numMembers].d);
                arr[numMembers].d *= interpZ;
                numMembers += 1;
            }
        }

        // Needed by clipping (linearly interpolated Attributes between two others)
        Attributes(const Attributes & first, const Attributes & second, const double & valueBetween)
        {
            // Your code goes here when clipping is implemented
        }

        // Const Return operator
        const attrib & operator[](const int & i) const
        {
            return arr[i];
        }

        // Return operator
        attrib & operator[](const int & i) 
        {
            return arr[i];
        }

        // Insert Double Into Container
        void insertDbl(const double & d)
        {
            arr[numMembers].d = d;
            numMembers += 1;
        }
    
        // Insert Pointer Into Container
        void insertPtr(void * ptr)
        {
            arr[numMembers].ptr = ptr;
            numMembers += 1;
        }
}; 

// Constructor that takes an array of 16 values for the matrix
Matrix::Matrix(double* values) {
    for (int r = 0; r < 4; r++)
    for (int c = 0; c < 4; c++)
        this->v[r][c] = values[r*4 + c];
}

// Constructor that takes a uniforms object and gets 16 values from there
Matrix::Matrix(const Attributes& uniforms) {
    for (int r = 0; r < 4; r++)
    for (int c = 0; c < 4; c++)
        this->v[r][c] = uniforms[r*4 + c].d;
}

// Sets the values of the matrix to the identity matrix
void Matrix::clear() {
    for (int r = 0; r < 4; r++)
    for (int c = 0; c < 4; c++)
        this->v[r][c] = (r == c ? 1 : 0);
}

// Multiplication of 4x4 by 4x1 matrix
Vertex operator* (const Matrix& lhs, const Vertex& rhs) {
    Vertex result = {
          lhs[0] * rhs.x
        + lhs[1] * rhs.y
        + lhs[2] * rhs.z
        + lhs[3] * rhs.w,

          lhs[4] * rhs.x
        + lhs[5] * rhs.y
        + lhs[6] * rhs.z
        + lhs[7] * rhs.w,

          lhs[8] * rhs.x
        + lhs[9] * rhs.y
        + lhs[10] * rhs.z
        + lhs[11] * rhs.w,

          lhs[12] * rhs.x
        + lhs[13] * rhs.y
        + lhs[14] * rhs.z
        + lhs[15] * rhs.w,
    };
    return result;
}

// Multiplication of 4x4 by 4x4 matrix
Matrix operator* (const Matrix& lhs, const Matrix& rhs) {
    Matrix result;
    
    // Loop over every cell in result
    for (int i = 0; i < 16; i++) {
        int r = 4 * (i / 4);
        int c = i % 4;
        double sum = 0;

        // Loop 4 times (row is 4 long, col is 4 long)
        for (int j = 0; j < 4; j++) {
            sum += (lhs[r] * rhs[c]);
            r++;
            c += 4;
        }
        result[i] = sum;
    }

    return result;
}

/***************************************************
 * Matrix transformation functions - return a
 * matrix that has the specified transformation
 **************************************************/
void translateMatrix(Attributes& attr, Vertex v) {
    attr.matrix[3]  = v.x;
    attr.matrix[7]  = v.y;
    attr.matrix[11] = v.z;
}

void scaleMatrix(Attributes& attr, Vertex v) {
    attr.matrix[0]  = v.x;
    attr.matrix[5]  = v.y;
    attr.matrix[10] = v.z;
}

// This function takes an angle in degrees and converts it to radians
void rotateZMatrix(Attributes& attr, double angle) {
    angle = angle * M_PI / 180.0;
    // compute trig functions here so we dont have to do them twice
    double sinangle = sin(angle);
    double cosangle = cos(angle);

    attr.matrix[0] = cosangle;
    attr.matrix[1] = -sinangle;
    attr.matrix[4] = sinangle;
    attr.matrix[5] = cosangle;
}

// These functions are not used in the week05 project, but they are still here
void rotateXMatrix(Attributes& attr, double angle) {
    angle = angle * M_PI / 180.0;
    // compute trig functions here so we dont have to do them twice
    double sinangle = sin(angle);
    double cosangle = cos(angle);

    attr.matrix[5]  = cosangle;
    attr.matrix[6]  = -sinangle;
    attr.matrix[9]  = sinangle;
    attr.matrix[10] = cosangle;
}

void rotateYMatrix(Attributes& attr, double angle) {
    angle = angle * M_PI / 180.0;
    // compute trig functions here so we dont have to do them twice
    double sinangle = sin(angle);
    double cosangle = cos(angle);

    attr.matrix[0]  = cosangle;
    attr.matrix[2]  = sinangle;
    attr.matrix[8]  = -sinangle;
    attr.matrix[10] = cosangle;
}

// Does all three previous operations at once in right to left order
void STRMatrix(Attributes& attr, Vertex s, Vertex t, double r) {
    Attributes sAttr;
    sAttr.matrix.clear();
    scaleMatrix(sAttr, s);
    Attributes tAttr;
    tAttr.matrix.clear();
    translateMatrix(tAttr, t);
    Attributes rAttr;
    rAttr.matrix.clear();
    rotateZMatrix(rAttr, r);

    attr.matrix = rAttr.matrix * tAttr.matrix * sAttr.matrix;
}

/***************************************************
 * LERP = Find the value between two doubles that
 * is some specific amount in between
 **************************************************/
inline double lerp(double a, double b, double amount) {
    return a + (b - a) * amount;
}

/***************************************************
 * INTERP - Function used to interpolate values,
 * taking advantage of the determinant's relationship
 * to the area of the triangle
 **************************************************/
inline double interp(double areaTriangle, double firstDet, double secndDet, double thirdDet, double attr0, double attr1, double attr2) {
    return (attr2*firstDet + attr0*secndDet + attr1*thirdDet) / areaTriangle;
}

// Example of a fragment shader
void DefaultFragShader(PIXEL & fragment, const Attributes & vertAttr, const Attributes & uniforms)
{
    // Output our shader color value, in this case red
    fragment = 0xffff0000;
}

/*******************************************************
 * FRAGMENT_SHADER
 * Encapsulates a programmer-specified callback
 * function for shading pixels. See 'DefaultFragShader'
 * for an example. 
 ******************************************************/
class FragmentShader
{
    public:
 
        // Get, Set implicit
        void (*FragShader)(PIXEL & fragment, const Attributes & vertAttr, const Attributes & uniforms);

        // Assumes simple monotone RED shader
        FragmentShader()
        {
            FragShader = DefaultFragShader;
        }

        // Initialize with a fragment callback
        FragmentShader(void (*FragSdr)(PIXEL & fragment, const Attributes & vertAttr, const Attributes & uniforms))
        {
            setShader(FragSdr);
        }

        // Set the shader to a callback function
        void setShader(void (*FragSdr)(PIXEL & fragment, const Attributes & vertAttr, const Attributes & uniforms))
        {
            FragShader = FragSdr;
        }
};

// Example of a vertex shader
void DefaultVertShader(Vertex & vertOut, Attributes & attrOut, const Vertex & vertIn, const Attributes & vertAttr, const Attributes & uniforms)
{
    // Nothing happens with this vertex, attribute
    vertOut = vertIn;
    attrOut = vertAttr;
}

/**********************************************************
 * VERTEX_SHADER
 * Encapsulates a programmer-specified callback
 * function for transforming vertices and per-vertex
 * attributes. See 'DefaultVertShader' for a pass-through
 * shader example.
 *********************************************************/
class VertexShader
{
    public:
        // Get, Set implicit
        void (*VertShader)(Vertex & vertOut, Attributes & attrOut, const Vertex & vertIn, const Attributes & vertAttr, const Attributes & uniforms);

        // Assumes simple monotone RED shader
        VertexShader()
        {
            VertShader = DefaultVertShader;
        }

        // Initialize with a fragment callback
        VertexShader(void (*VertSdr)(Vertex & vertOut, Attributes & attrOut, const Vertex & vertIn, const Attributes & vertAttr, const Attributes & uniforms))
        {
            setShader(VertSdr);
        }

        // Set the shader to a callback function
        void setShader(void (*VertSdr)(Vertex & vertOut, Attributes & attrOut, const Vertex & vertIn, const Attributes & vertAttr, const Attributes & uniforms))
        {
            VertShader = VertSdr;
        }
};

// Stub for Primitive Drawing function
/****************************************
 * DRAW_PRIMITIVE
 * Prototype for main drawing function.
 ***************************************/
void DrawPrimitive(PRIMITIVES prim, 
                   Buffer2D<PIXEL>& target,
                   const Vertex inputVerts[], 
                   const Attributes inputAttrs[],
                   Attributes* const uniforms = NULL,
                   FragmentShader* const frag = NULL,
                   VertexShader* const vert = NULL,
                   Buffer2D<double>* zBuf = NULL);             
       
#endif
