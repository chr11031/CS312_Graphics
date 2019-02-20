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
        /*
        ~Buffer2D()
        {
            // De-Allocate pointers for column references
            for(int r = 0; r < h; r++)
            {
                free(grid[r]);
            }
            free(grid);
        }
        */

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

        // Public member variables
        PIXEL color; 
        int numValues; // number of values to interpolate (3 for rgb, 2 for UV, etc.)
        void* pointerImg; // address -> pointer without a base type
        void* pointerImg2; // temporarily have a second pointer image
        double attrValues[5]; // according to the slides, we will likely have at most 5 attribute values

        // For Matrix Multiplication
        int numRows;
        int numCols;
        double matrix[4][4]; // row col

        // Obligatory empty constructor
        Attributes() : numValues(0), numRows(0), numCols(0) {}

        // Non default const
        Attributes(const int & numRows, const int & numCols) : numValues(0), pointerImg(NULL)
        {
            this->numRows = numRows;
            this->numCols = numCols;
        }

        // Needed by clipping (linearly interpolated Attributes between two others)
        Attributes(const Attributes & first, const Attributes & second, const double & valueBetween)
        {
            // Your code goes here when clipping is implemented
        }

        void operator *= (const Attributes & rhs);

        void interpolateValues(const double & det1, const double & det2, const double & det3, const double & area, Attributes* vertAttrs)
        {
            double w1 = det1 / area;
            double w2 = det2 / area;
            double w3 = 1 - w2 - w1;

            for (int i = 0; i < numValues; i++)
            {
                attrValues[i] = vertAttrs[0].attrValues[i] * w2 +
                                vertAttrs[1].attrValues[i] * w3 +
                                vertAttrs[2].attrValues[i] * w1;
            }
        }

        void correctPerspective(double z)
        {
            for (int i = 0; i < numValues; i++)
                attrValues[i] *= z;
        }

        void concatMatrices(const Attributes & transform) throw (const char *)
        {
            *this *= transform;
        }

};	

/******************************************************
 * Helper function for the overloaded multiply operation
 *****************************************************/
double multRowCol(const double row[], const double col[], const int & size)
{
    double result = 0;

    for (int i = 0; i < size; i++)
        result += row[i] * col[i];

    return result;
}

/******************************************************
 * Helper function for the overloaded multiply equals operation
 * * * IMPORTANT NOTE:
 * * For the purposes of making things more comprehensible,
 * * we have flipped the rhs and lhs from what it actually is
 * * in a matrix. For example, when multiplying matrices A B and C
 * * matrix multiplication says that we must first multiply C, 
 * * then B, then A, so we have flipped rhs and lhs in our function
 * * so that in order to multiple A B and C, we can simply put
 * * C * B * A in the CORRECT order that they must be multiplied.
 *****************************************************/
void Attributes::operator*=(const Attributes & rhs)
{
    // check to see if the "lhs" rows is equal to the "rhs" cols
    if (this->numRows != rhs.numCols)
        throw "ERROR: Cannot multiple matrices of invalid sizes.";

    // we need a matrix that we can change without messing up the multiplaction
    double tempMatrix[4][4];

    // this loop goes through the rows of the "rhs"
    for (int i = 0; i < rhs.numRows; i++)
    {
        // this loop goes through the cols of the "lhs"
        for (int j = 0; j < this->numCols; j++)
        {
            // this is a 1x4 "matrix" which is pretty much just the column turned sideways to resemble a row
            double colToRow[4];

            // loop through the rows of the specified column and put it into our 1x4 matrix
            for (int k = 0; k < this->numRows; k++)
                colToRow[k] = this->matrix[k][j];

            // use helper funciton to multiply and add the current rows and columns
            tempMatrix[i][j] += multRowCol(rhs.matrix[i], colToRow, rhs.numCols);
        }
    }

    // make sure we update the rows
    this->numRows = rhs.numRows;
    
    // loop through the matrix and overwrite it with the new matrix
    for (int i = 0; i < this->numRows; i++)
    {
        for (int j = 0; j < this->numCols; j++)
            this->matrix[i][j] = tempMatrix[i][j];
    }
    return;
}

/******************************************************
 * Overloading the * operator
 *****************************************************/
Vertex operator * (const Vertex & lhs, const Attributes & rhs)
{
    // lhs = a, rhs = b -> rhs is matrix
    if (rhs.numCols <= 4 && rhs.numCols >= 1)
        throw "ERROR: Cannot multiply matrices of invalid sizes.";

    double tempVerts[4] = {0, 0, 0, 0};
    double currentVerts[4] = {lhs.x, lhs.y, lhs.z, lhs.w};

    for (int i = 0; i < rhs.numRows; i++)
        tempVerts[i] = multRowCol(rhs.matrix[i], currentVerts, rhs.numCols);

    Vertex resultVerts = (Vertex){tempVerts[0], tempVerts[1], tempVerts[2], tempVerts[3]};
    return resultVerts;
}

// Example of a fragment shader
void DefaultFragShader(PIXEL & fragment, const Attributes & vertAttr, const Attributes & uniforms)
{
    // Output our shader color value, in this case red
    fragment = 0xffff0000;
}

void GrayFragShader(PIXEL & fragment, const Attributes & vertAttr, const Attributes & uniforms)
{
    PIXEL avgChannel = ((vertAttr.color >> 16) && 0xff) + ((vertAttr.color >> 8) && 0xff) + ((vertAttr.color) && 0xff);
    
    avgChannel /= 3;
    fragment = 0xff000000 + (avgChannel << 16) + (avgChannel << 8) + avgChannel;
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
