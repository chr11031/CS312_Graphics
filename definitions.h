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
#define DEG_TO_RAD M_PI / 180.0

// Max # of vertices after clipping
#define MAX_VERTICES 8 

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

/**********************************************************
 * MATRIX
 * Encapsulates a transformation matrix
 *********************************************************/
class Matrix
{
    private:
        double matrix[4][4];

        void initialize(double matrix[4][4])
        {
            for(int y = 0; y < 4; y++)
            {
                for(int x = 0; x < 4; x++)
                {
                    this->matrix[y][x] = matrix[y][x];
                }
            }
        }

    public:
        Matrix() 
        {
            double data[4][4] = {
                {1.0, 0.0, 0.0, 0.0},
                {0.0, 1.0, 0.0, 0.0},
                {0.0, 0.0, 1.0, 0.0},
                {0.0, 0.0, 0.0, 1.0}};
            initialize(data);
        }

        Matrix(double matrix[4][4])
        {
            initialize(matrix);
        }

        double* operator[](int i)
        {
            if(i < 0 || i >= 4)
                throw(false);
            return matrix[i];
        }

        const double* operator[](int i) const
        {
            if(i < 0 || i >= 4)
                throw(false);
            return matrix[i];
        }
};

// The identity matrix
Matrix IdentityMatrix()
{
    double data[4][4] = {
        {1.0, 0.0, 0.0, 0.0}, 
        {0.0, 1.0, 0.0, 0.0}, 
        {0.0, 0.0, 1.0, 0.0}, 
        {0.0, 0.0, 0.0, 1.0}};
    return Matrix(data);
}

// Transformation matrix that translates
Matrix TranslationMatrix(double dx, double dy, double dz)
{
    double data[4][4] = {
        {1.0, 0.0, 0.0, dx}, 
        {0.0, 1.0, 0.0, dy}, 
        {0.0, 0.0, 1.0, dz}, 
        {0.0, 0.0, 0.0, 1.0}};
    return Matrix(data);
}

// Transformation matrix that scales
Matrix ScaleMatrix(double sx, double sy, double sz)
{
    double data[4][4] = {
        { sx, 0.0, 0.0, 0.0}, 
        {0.0,  sy, 0.0, 0.0}, 
        {0.0, 0.0,  sz, 0.0}, 
        {0.0, 0.0, 0.0, 1.0}};
    return Matrix(data);
}

// Transformation matrix that rotates ccw around the x-axis
Matrix XRotationMatrix(double rad)
{
    double data[4][4] = {
        {1.0,       0.0,       0.0, 0.0}, 
        {0.0,  cos(rad), -sin(rad), 0.0}, 
        {0.0,  sin(rad),  cos(rad), 0.0}, 
        {0.0,       0.0,       0.0, 1.0}};
    return Matrix(data);
}

// Transformation matrix that rotates ccw around the y-axis
Matrix YRotationMatrix(double rad)
{
    double data[4][4] = {
        { cos(rad), 0.0, sin(rad), 0.0}, 
        {      0.0, 1.0,      0.0, 0.0}, 
        {-sin(rad), 0.0, cos(rad), 0.0}, 
        {      0.0, 0.0,      0.0, 1.0}};
    return Matrix(data);
}

// Transformation matrix that rotates ccw around the z-axis
Matrix ZRotationMatrix(double rad)
{
    double data[4][4] = {
        {cos(rad), -sin(rad), 0.0, 0.0}, 
        {sin(rad),  cos(rad), 0.0, 0.0}, 
        {     0.0,       0.0, 1.0, 0.0}, 
        {     0.0,       0.0, 0.0, 1.0}};
    return Matrix(data);
}

// Multiplies two matrices
Matrix operator*(const Matrix &lhs, const Matrix &rhs)
{
    Matrix result;
    for(int y = 0; y < 4; y++)
    {
        for(int x = 0; x < 4; x++)
        {
            result[y][x] = lhs[y][0] * rhs[0][x] + lhs[y][1] * rhs[1][x] + lhs[y][2] * rhs[2][x] + lhs[y][3] * rhs[3][x];
        }
    }
    return result;
}

// Muliplies a matrix by a vertex
Vertex operator*(const Matrix &lhs, const Vertex &rhs)
{
    Vertex result;
    result.x = rhs.x * lhs[0][0] + rhs.y * lhs[0][1] + rhs.z * lhs[0][2] + rhs.w * lhs[0][3];
    result.y = rhs.x * lhs[1][0] + rhs.y * lhs[1][1] + rhs.z * lhs[1][2] + rhs.w * lhs[1][3];
    result.z = rhs.x * lhs[2][0] + rhs.y * lhs[2][1] + rhs.z * lhs[2][2] + rhs.w * lhs[2][3];
    result.w = rhs.x * lhs[3][0] + rhs.y * lhs[3][1] + rhs.z * lhs[3][2] + rhs.w * lhs[3][3];
    return result;
}

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

/***************************************************
 * ATTRIBUTES (shadows OpenGL VAO, VBO)
 * The attributes associated with a rendered 
 * primitive as a whole OR per-vertex. Will be 
 * designed/implemented by the programmer. 
 **************************************************/
class Attributes
{      
    public:
        // Obligatory empty constructor
        Attributes() {}

        // Needed by clipping (linearly interpolated Attributes between two others)
        Attributes(const Attributes & first, const Attributes & second, const double & valueBetween)
        {
            // Your code goes here when clipping is implemented
        }
        
        PIXEL color;

        double u;
        double v; 
        void* ptrImg;

        double r;
        double g;
        double b;

        Matrix transform;
};	

// Image Fragment Shader 
void ImageFragShader(PIXEL & fragment, const Attributes & vertAttr, const Attributes & uniforms)
{
    BufferImage* bf = (BufferImage*)uniforms.ptrImg;
    int x = vertAttr.u * (bf->width()-1);
    int y = vertAttr.v * (bf->height()-1);

    fragment = (*bf)[y][x];
}

// My Fragment Shader for color interpolation
void ColorFragShader(PIXEL & fragment, const Attributes & vertAttr, const Attributes & uniforms)
{
    // Output our shader color value, in this case red
    PIXEL color = 0xff000000;
    color += (unsigned int)(vertAttr.r *0xff) << 16;
    color += (unsigned int)(vertAttr.g *0xff) << 8;
    color += (unsigned int)(vertAttr.b *0xff) << 0;

    fragment = color;
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

void ColorVertexShader(Vertex & vertOut, Attributes & attrOut, const Vertex & vertIn, const Attributes & vertAttr, const Attributes & uniforms)
{
    vertOut = uniforms.transform * vertIn;
    attrOut = vertAttr;
}

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

/*************************************************************
 * DETERMINANT
 * Calculates the determinant of two 2D vectors using AD-CB. 
 ************************************************************/
double determinant(double v1x, double v1y, double v2x, double v2y) 
{
    // AD - CB
    return v1x * v2y - v1y * v2x;
}

/*************************************************************
 * LERP
 * Linearly interpolates three attributes. 
 ************************************************************/
double lerp(double area, double d1, double d2, double d3, double a1, double a2, double a3) 
{
    return ((d1 * a3) + (d2 * a1) + (d3 * a2)) / area;
}   
       
#endif
