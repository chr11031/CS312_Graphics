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
            SDL_LockSurface(img);// For Project04
            setupInternal();
        }
};

class Matrix
{
    public:
        double cell[4][4];
        int numRows;
        int numCols;

    // Default constructor
    Matrix ()
    {
        numRows = 0;
        numCols = 0;
    }
    // Non-default constructor
    Matrix(double data[][4], int rows, int cols)
    {
        this->numRows = rows;
        this->numCols = cols;
        for (int i = 0; i < numRows; i++)
        {
            for (int j = 0; j < numCols; j++)
            {
                cell[i][j] = data[i][j];
            }
        }
    }
    //  Operators
    void operator *= (const Matrix & rhs);
    void operator = (const Matrix & rhs);
};

/**************************************************
 * Assignment operator
 ***************************************************/
void Matrix :: operator = (const Matrix & rhs)
{
        for (int i = 0; i < rhs.numRows; i++)
        {
            for (int j = 0; j < rhs.numCols; j++)
            {
                this->cell[i][j] = rhs.cell[i][j];
            }
        }
        this->numRows = rhs.numRows;
        this->numCols = rhs.numCols;
}

/***************************************
 * Override *= operator for matrices
 * *************************************/
void Matrix :: operator *= (const Matrix & rhs)
{
    if (this->numRows != rhs.numCols)
    {
        throw "ERROR: Cannot concatenate matrices\n";
    }

    // Temporary matrix to help with multiplication
    double tempMatrix[4][4];

    for (int i = 0; i < rhs.numRows; i++)
    {
        for (int j = 0; j < this->numCols; j++)
        {
            double colToRow[4];
            for (int k = 0; k < this->numRows; k++)
            {
                colToRow[k] = this->cell[k][j];
            }
            tempMatrix[i][j] = 0;
            // multiplies rows and columns
            for (int n = 0; n < rhs.numCols; n++)
            {
                tempMatrix[i][j] += (rhs.cell[i][n] * colToRow[n]);
            }
        }
    }
    this->numRows = rhs.numRows;
    // Write onto the matrix
    for (int i = 0; i < this->numRows; i++)
    {
        for (int j = 0; j < this->numCols; j++)
        {
            this->cell[i][j] = tempMatrix[i][j];
        }
    }
}

/*****************************************************
 * Override multiplication operator for vertices and matrices
 * ****************************************************/
Vertex operator * (const Vertex & lhs, const Matrix & rhs)
{
    if (rhs.numCols < 1 || rhs.numCols > 4)
    {
        throw "ERROR: Cannot multiply matrices\n";
    }
    double currentVerts[4] = {lhs.x, lhs.y, lhs.z, lhs.w};
    double tempVerts[4] = {lhs.x, lhs.y, lhs.z, lhs.w};
    for (int i = 0; i < rhs.numRows; i++)
    {
        // multiplies rows and columns
        tempVerts[i] = 0;
        for (int n = 0; n < rhs.numCols; n++)
        {
            tempVerts[i] += (rhs.cell[i][n] * currentVerts[n]);
        }
    }
    return {tempVerts[0], tempVerts[1], tempVerts[2], tempVerts[3]};
}

/***************************************************
 * ATTRIBUTES (shadows OpenGL VAO, VBO)
 * The attributes associated with a rendered 
 * primitive as a whole OR per-vertex. Will be 
 * designed/implemented by the programmer. 
 **************************************************/
class Attributes
{      
    public:
        // value[] is used for r, g, b, u, and v values
        double value[3];
        int numValues;
        // Used for image fragment shader
        void * ptrImg;

        // For matrices
        Matrix matrix;

        // Obligatory empty constructor
        Attributes() {}

        // Needed by clipping (linearly interpolated Attributes between two others)
        Attributes(const Attributes & first, const Attributes & second, const double & valueBetween)
        {
            // Your code goes here when clipping is implemented
        }

        PIXEL color;
};	

void ImageFragShader(PIXEL & fragment, const Attributes & vertAttr, const Attributes & uniforms)
{
    // Output a bitmap image
    BufferImage * ptr = (BufferImage *)uniforms.ptrImg;
    int wid = ptr->width();
    int hgt = ptr->height();
    int x = vertAttr.value[0] * (wid - 1);
    int y = vertAttr.value[1] * (hgt - 1);
    fragment = (*ptr)[y][x];
}

void ColorFragShader(PIXEL & fragment, const Attributes & vertAttr, const Attributes & uniforms)
{
    // Output the shader color value
    PIXEL color = 0xff000000;
    color += (unsigned int)(vertAttr.value[0] *0xff) << 16;
    color += (unsigned int)(vertAttr.value[1] *0xff) << 8;
    color += (unsigned int)(vertAttr.value[2] *0xff) << 0;
    fragment = color;
}

void SineShader(PIXEL & fragment, const Attributes & vertAttr, const Attributes & uniforms)
{
    BufferImage* buffer = (BufferImage*)uniforms.ptrImg;
    //int x = vertAttr.value[0] = (buffer -> width()); <--
    //int y = vertAttr.value[1] = (buffer -> height()); <--
    int x = (buffer -> width());
    int y = (buffer -> height());
    if (y > sin(x / 10.0) * 10 + 20)
    {
        fragment = 0xFFFFFFFF;
    }
    else
    {
        fragment = 0xFF00FF00;
    }
}

void StaticNoiseShader(PIXEL & fragment, const Attributes & vertAttr, const Attributes & uniforms)
{
    if (rand() % 2)
       fragment = 0xFFFFFFFF;
    else
       fragment = 0xFF000000;
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

// Transformation Vertex Shader
void transVertShader(Vertex & vertOut, Attributes & attrOut, const Vertex & vertIn, const Attributes & vertAttr, const Attributes & uniforms)
{
    vertOut = (vertIn * uniforms.matrix);
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
