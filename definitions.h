#define SDL_MAIN_HANDLED
#include <iostream>
#include <vector>
#include "SDL2/SDL.h"
#include "stdlib.h"
#include "stdio.h"
#include "math.h"

using namespace std;
using std::vector;

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

/****************************************************
 * BOUNDING_BOX
 * Describes a box within which objects may be drawn. 
 ****************************************************/
class BoundingBox
{
    public:
        // Default Constructor
        BoundingBox() {}

        // Construct a BoundingBox for a triangle
        BoundingBox(Vertex vert1, Vertex vert2, Vertex vert3)
        {
            maxX = MAX3(vert1.x, vert2.x, vert3.x);
            minX = MIN3(vert1.x, vert2.x, vert3.x);
            maxY = MAX3(vert1.y, vert2.y, vert3.y);
            minY = MIN3(vert1.y, vert2.y, vert3.y);
        }

        int maxX;
        int minX;
        int maxY;
        int minY;
};

class MatrixTransform
{
    public:
        // This is the matrix that will be manipulated into a transform
        double matrix[4][4];
        
        // Default Constructor
        MatrixTransform() {
            // Declare an Identity Matrix
            identity();
        }
    
        void translate(double x, double y)
        {
            this->matrix[0][3] += x;
            this->matrix[1][3] += y;
        }

        void scale(double scaleFactorX, double scaleFactorY)
        {
            // double transformationMatrix[4][4] = {{scaleFactorX,0,0,0},{0,scaleFactorY,0,0},{0,0,1,0},{0,0,0,1}};
            // this->multiply(this->vector, transformationMatrix);
            this->matrix[0][0] *= scaleFactorX;
            this->matrix[1][1] *= scaleFactorY;
        }

        void rotate(double rotateFactorX, double rotateFactorY)
        {
            // double transformationMatrix[4][4] = {{cos(rotateFactorX * M_PI / 180),-sin(rotateFactorX * M_PI / 180),0,0},{sin(rotateFactorX * M_PI / 180),cos(rotateFactorX * M_PI / 180),0,0},{0,0,1,0},{0,0,0,1}};
            // this->multiply(this->vector, transformationMatrix);
            this->matrix[0][0] *= cos((rotateFactorX * M_PI) / 180);
            this->matrix[0][1] *= -sin((rotateFactorX * M_PI) / 180);
            this->matrix[1][0] *= sin((rotateFactorY * M_PI) / 180);
            this->matrix[1][1] *= cos((rotateFactorY * M_PI) / 180);
        }

        /**********************************************************
         * IDENTITY
         * Clears the transformations placed upon the matrix by
         * making the matrix an identity matrix.
         * *******************************************************/
        void identity()
        {
            this->matrix[0][0] = 1;
            this->matrix[0][1] = 0;
            this->matrix[0][2] = 0;
            this->matrix[0][3] = 0;
            this->matrix[1][0] = 0;
            this->matrix[1][1] = 1;
            this->matrix[1][2] = 0;
            this->matrix[1][3] = 0;
            this->matrix[2][0] = 0;
            this->matrix[2][1] = 0;
            this->matrix[2][2] = 1;
            this->matrix[2][3] = 0;
            this->matrix[3][0] = 0;
            this->matrix[3][1] = 0;
            this->matrix[3][2] = 0;
            this->matrix[3][3] = 1;
        }

        void multiplyVector(double vector[4][1]) const
        {
            for (int i = 0; i < 4; ++i)
            {
                double sum = 0;
                for (int j = 0; j < 4; ++j)
                {
                    sum += this->matrix[i][j] * vector[j][0];
                }

                vector[i][0] = sum;
            }
        }

        void multiplyMatrices(double matrix1[4][4], double matrix2[4][4])
        {
            for (int i = 0; i < 4; ++i)
            {
                for (int j = 0; j < 4; ++j)
                {
                    double sum = 0;
                    for (int k = 0; k < 4; ++k)
                    {
                        sum += matrix2[i][j] * matrix1[j][k];
                    }
                    matrix1[i][j] = sum;
                }
            }
        }
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

        // A single pixel that holds a single combination of RGB
        PIXEL color;

        // Reference to an image to interpolate
        void* ptrImg;

        // An array that holds attribute values, such as RGB values or UV Coordinates
        double values[5];

        // A Matrix Transform that can be used to transform a vertex
        MatrixTransform transform;
};	

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

void TestVertShader(Vertex & vertOut, Attributes & attrOut, const Vertex & vertIn, const Attributes & vertAttr, const Attributes & uniforms)
{
    // Create a Vector from the Vertex vertIn
    Vertex transformedVertex;
    double vector[4][1] = {{vertIn.x}, {vertIn.y}, {vertIn.z}, {vertIn.w}};

    // Multiply the vector by the transformation matrix
    uniforms.transform.multiplyVector(vector);

    transformedVertex.x = vector[0][0];
    transformedVertex.y = vector[1][0];
    transformedVertex.z = vector[2][0];
    transformedVertex.w = vector[3][0];

    vertOut = transformedVertex;

    // Modify Attributes
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
