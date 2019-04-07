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
            setupInternal();
        }
};

/*******************************************
 * MatrixMult
 * multiply 4X4 and 4X1 matrices
*******************************************/
class Transformer
{
    public:
        int row;
        int col;
        double matrix[4][4]; 
        
        void initialize(double matrix[4][4])
        {
            for(int i = 0; i < 4; i++)
                for(int j = 0; j < 4; j++)
                    this->matrix[i][j] = matrix[i][j];
        }

        Transformer() 
        {
            this->row = 4;
            this->col = 4;
            double data[4][4] = {{1,0,0,0},
                                 {0,1,0,0},
                                 {0,0,1,0},
                                 {0,0,0,1}};
            initialize(data);
        }

        Transformer(double matrix[4][4])
        {
           initialize(matrix);
        }

        Transformer(double vertex[4][1])
        {
            this->row = 4;
            this->col = 4;
            for (int i = 0; i < 4; i++)
                matrix[i][0] = vertex[i][0];
        }

        Transformer(Vertex vert)
        {
            this->row = 4;
            this->col = 1;
            this->matrix[0][0] = vert.x;
            this->matrix[1][0] = vert.y;
            this->matrix[2][0] = vert.z;
            this->matrix[3][0] = vert.w;
        }

        Transformer scale(double x, double y, double z)
        {
            double matrix[4][4] = {{x,0,0,0},
                                   {0,y,0,0},
                                   {0,0,z,0},
                                   {0,0,0,1}};
            return Transformer(matrix);
        }

        Transformer translate(double x, double y, double z)
        {
            double matrix[4][4] = {{1,0,0,x},
                                   {0,1,0,y},
                                   {0,0,1,z},
                                   {0,0,0,1}};
            return Transformer(matrix);
        }

        Transformer zRotate(double angle)
        {
            double sinRot = sin(angle * M_PI / 180.0);
            double cosRot = cos(angle * M_PI / 180.0);

            double matrix[4][4] = {{cosRot,-sinRot,0,0},
                                   {sinRot,cosRot,0,0},
                                   {0,0,1,0},
                                   {0,0,0,1}};

            return Transformer(matrix);
        }

        double* operator[](int i)
        {
            return this->matrix[i];
        }

        const double* operator[](int i) const
        {
            return this->matrix[i];
        }
};

Transformer operator*(const Transformer &lhs, const Transformer &rhs)
        {
            Transformer matrix;
            for(int y = 0; y < 4; y++)
                for(int x = 0; x < 4; x++)
                    matrix[y][x] = lhs[y][0] * rhs[0][x] + lhs[y][1] * rhs[1][x] + lhs[y][2] * rhs[2][x] + lhs[y][3] * rhs[3][x];
            return matrix;
        }
//matrix to vertex multiplication
Vertex operator*(const Transformer &matrix, const Vertex &vertex)
{
    Vertex vert;
    vert.x = vertex.x * matrix[0][0] + vertex.y * matrix[0][1] + vertex.z * matrix[0][2] + vertex.w * matrix[0][3];
    vert.y = vertex.x * matrix[1][0] + vertex.y * matrix[1][1] + vertex.z * matrix[1][2] + vertex.w * matrix[1][3];
    vert.z = vertex.x * matrix[2][0] + vertex.y * matrix[2][1] + vertex.z * matrix[2][2] + vertex.w * matrix[2][3];
    vert.w = vertex.x * matrix[3][0] + vertex.y * matrix[3][1] + vertex.z * matrix[3][2] + vertex.w * matrix[3][3];
    return vert;
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
        PIXEL color;
        //for r, g, b and u, v  colors and coordinates = cc but for now I only need 3 because they 
        //get overwritten.
        double cc[3];
        // For image interpolation
        double size = 3;
        void* ptrImg;
        Transformer matrix;
               
        // Obligatory empty constructor
        Attributes() {}

        // Needed by clipping (linearly interpolated Attributes between two others)
        Attributes(const Attributes & first, const Attributes & second, const double & valueBetween)
        {
            // Your code goes here when clipping is implemented
        }
};	

// Example of a fragment shader
void DefaultFragShader(PIXEL & fragment, const Attributes & vertAttr, const Attributes & uniforms)
{
    // Output our shader color value, in this case red
    fragment = 0xffff0000;
}

/*******************************************************************************************
 * ImageFragShader
 * takes uv coordinates and draws them to the fragment.
********************************************************************************************/
void ImageFragShader(PIXEL & fragment, const Attributes & vertAttr, const Attributes & uniform)
{
    BufferImage * imgPtr = (BufferImage*)uniform.ptrImg;

    int x = vertAttr.cc[0] * (imgPtr->width() - 1);
    int y = vertAttr.cc[1] * (imgPtr->height() - 1);

    fragment = (*imgPtr)[y][x];
}

/*************************************************************************
 * ColorFragShader
 * use bit shifting to get individual colors and draw them to fragment.
*******************************************************************************************/
void ColorFragShader(PIXEL & fragment, const Attributes & vertAttr, const Attributes & uniform)
{
    PIXEL color = 0xff000000;

    // adding colors, using bit shifting to effect only r, then g, then b values.
    color += (unsigned int)(vertAttr.cc[0] * 0xff) << 16;
    color += (unsigned int)(vertAttr.cc[1] * 0xff) << 8;
    color += (unsigned int)(vertAttr.cc[2] * 0xff) << 0;

    fragment = color;
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

/**************************
 * vShader is the vertex shader 
 *******************************/
void vShader(Vertex & vertOut, Attributes & attrOut, const Vertex & vertIn, const Attributes & vertAttr, const Attributes & uniforms)
{
    vertOut = uniforms.matrix * vertIn;
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
