#define SDL_MAIN_HANDLED
#include "SDL2/SDL.h"
#include "stdlib.h"
#include "stdio.h"
#include "math.h"
#include <vector>

using namespace std;

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
#define X_KEY 0
#define Y_KEY 1

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

class Matrix
{      
    public:
        // Public member variables
        double mat[4][4];

        // Default constructor
        Matrix()
        {
            for (int i = 0; i < 3; i++)
            {
                for (int j = 0; j < 3; j++)
                {
                    mat[i][j] = 0;
                }
            }

            mat[0][0] = 1;
            mat[1][1] = 1; 
            mat[2][2] = 1; 
            mat[3][3] = 1; 
        }

        double & operator () (int row, int column)
        {
            return mat[row][column];
        }

        void translation(double transX, double transY, double transZ) 
        {
            this->mat[0][3] = transX;
            this->mat[1][3] = transY;
            this->mat[2][3] = transZ;
        }

        void rotation(double degree) 
        {
            this->mat[0][0] = cos(M_PI * degree / 180);
            this->mat[0][1] = -sin(M_PI * degree / 180);
            this->mat[1][0] = sin(M_PI * degree / 180);
            this->mat[1][1] = cos(M_PI * degree / 180);
        }

        void scaling(double scale) 
        {
            this->mat[0][0] = scale;
            this->mat[1][1] = scale;
            this->mat[2][2] = 1;
        }

        void combined(double degree, double transX, double transY, double transZ, double scale) 
        {
            // R->T->S
            Matrix mFinal;
            double mTemp = 0;

            Matrix mRot;
            mRot(0, 0) = cos(M_PI * degree / 180);
            mRot(0, 1) = -sin(M_PI * degree / 180);
            mRot(1, 0) = sin(M_PI * degree / 180);
            mRot(1, 1) = cos(M_PI * degree / 180);

            Matrix mTrans;
            mTrans(0, 3) = transX;
            mTrans(1, 3) = transY;
            mTrans(2, 3) = transZ;

            Matrix mScale;
            mScale(0, 0) = scale;
            mScale(1, 1) = scale;
            mScale(2, 2) = 1;

            for (int i = 0; i < 3; i++)
            {
                for (int j = 0; j < 3; j++)
                {
                    for (int k = 0; k < 3; k++)
                    {
                        mTemp += mScale(j, k) * mRot(k, i);
                    }

                    mFinal(j, i) = mTemp;
                    mTemp = 0;
                }
            }

            for (int i = 0; i < 3; i++)
            {
                for (int j = 0; j < 3; j++)
                {
                    for (int k = 0; k < 3; k++)
                    {
                        mTemp += mFinal(j, k) * mTrans(k, i);
                    }

                    mFinal(j, i) = mTemp;
                    mTemp = 0;
                }
            }

            for (int i = 0; i < 3; i++)
            {
                for (int j = 0; j < 3; j++)
                {
                   this->mat[i][j] = mFinal(i, j);
                }
            }
        }
};	

Vertex operator * (const Matrix & lhs, const Vertex & rhs)
{
    Vertex v; 

    v.x = lhs.mat[0][0] * rhs.x 
         + lhs.mat[0][1] * rhs.y 
         + lhs.mat[0][2] * rhs.z 
         + lhs.mat[0][3] * rhs.w;

    v.y = lhs.mat[1][0] * rhs.x 
         + lhs.mat[1][1] * rhs.y 
         + lhs.mat[1][2] * rhs.z 
         + lhs.mat[1][3] * rhs.w;

    v.z = lhs.mat[2][0] * rhs.x 
         + lhs.mat[2][1] * rhs.y 
         + lhs.mat[2][2] * rhs.z 
         + lhs.mat[2][3] * rhs.w;

    v.w = lhs.mat[3][0] * rhs.x 
         + lhs.mat[3][1] * rhs.y 
         + lhs.mat[3][2] * rhs.z 
         + lhs.mat[3][3] * rhs.w;

    return v;
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
        // Public member variabels
        PIXEL color;
        double r, g, b, u, v;

        vector<double> vAttr;

        void* ptrImg;

        Matrix matrix;

        // Obligatory empty constructor
        Attributes() {}

        // Needed by clipping (linearly interpolated Attributes between two others)
        Attributes(const Attributes & first, const Attributes & second, const double & valueBetween)
        {
            // Your code goes here when clipping is implemented
        }
};

void ImageFragShader(PIXEL & fragment, const Attributes & vertAttr, const Attributes & uniforms)
{
    BufferImage* bf = (BufferImage*)uniforms.ptrImg;
    int x = vertAttr.u * (bf->width()-1);
    int y = vertAttr.v * (bf->height()-1);

    fragment = (*bf)[y][x];
}

void ColorFragShader(PIXEL & fragment, const Attributes & vertAttr, const Attributes & uniforms)
{
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

// Example of a vertex shader
void DefaultVertShader(Vertex & vertOut, Attributes & attrOut, const Vertex & vertIn, const Attributes & vertAttr, const Attributes & uniforms)
{
    // Nothing happens with this vertex, attribute
    vertOut = vertIn;
    attrOut = vertAttr;
}

void CustomVertShader(Vertex & vertOut, Attributes & attrOut, const Vertex & vertIn, const Attributes & vertAttr, const Attributes & uniforms)
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

/***************************************************
 * Function: signum
 **************************************************/
template <typename T> int signum(T val)
{
    return (T(0) < val) - (val < T(0));
}

/***************************************************
 * Function: crossProduct
 * Another version of the determinant function
 **************************************************/
float crossProduct(Vertex u, Vertex v)
{
    return (u.x * v.y) - (v.x * u.y);
}

/****************************************
 * DETERMINANT
 * Find the determinant of a matrix with
 * components A, B, C, D from 2 vectors.
 ***************************************/
inline double determinant(const double & A, const double & B, const double & C, const double & D)
{
    return (A*D - B*C);
}

/****************************************
 * INTERPOLATE
 * Uses the determinants over the area
 * to determine the weights of each
 * attribute
 ***************************************/
double interp(double areaTriangle, double firstDet, double secndDet, double thirdDet, double attr0, double attr1, double attr2)
{
    return (firstDet * attr2 + secndDet * attr0 + thirdDet * attr1) / areaTriangle;
}

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
