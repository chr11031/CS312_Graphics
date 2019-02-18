#define SDL_MAIN_HANDLED
#include "SDL2/SDL.h"
#include "stdlib.h"
#include "stdio.h"
#include <math.h>

#ifndef DEFINITIONS_H
#define DEFINITIONS_H

/******************************************************
 * DEFINES:
 * Macros for universal variables/hook-ups.
 *****************************************************/
#define WINDOW_NAME "James' Pipeline"
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
 * Matrix for changing vertices. 
 ****************************************************/
class Matrix
{
  private:
    double * matrixPtr;
    int rows;
    int columns;
    bool init;
  public:
    Matrix(): rows(0), columns(0), matrixPtr(NULL), init(false) {};

    // constructor that sets up the matrix size
    Matrix(int rows, int columns)
    {
        this->rows = rows;
        this->columns = columns;
        matrixPtr = new double[rows * columns];
        for(int i = 0; i < (rows * columns); i++)
        {
            matrixPtr[i] = 0;
        }
        init = false;
    }

    ~Matrix()
    {
        if(matrixPtr != NULL)
        {
            delete [] matrixPtr;
        }
    }

    void addRotation(double rot)
    {
        Matrix rotate(this->rows, this->columns);
        
        if (init == false)
        {
            for(int i = 0; i < (this->rows * this->columns); i++)
            {
                this->matrixPtr[i] = rotate.matrixPtr[i];
            }
            init = true;
        }
        else
        {
            *this = rotate * (*this);
        }
    }

    void addScaling(double scaleX, double scaleY, double scaleZ)
    {
        Matrix scale(this->rows, this->columns);

        if (init == false)
        {
            for(int i = 0; i < (this->rows * this->columns); i++)
            {
                this->matrixPtr[i] = scale.matrixPtr[i];
            }
            init = true;
        }
        else
        {
            *this = scale * (*this);
        }
    }

    void addTranslation(double transX, double transY, double transZ)
    {

        Matrix translate(this->rows, this->columns);
        if (init == false)
        {
            for(int i = 0; i < (this->rows * this->columns); i++)
            {
                this->matrixPtr[i] = translate.matrixPtr[i];
            }
            init = true;
        }
        else
        {
            *this = translate * (*this);
        }
    }

    /*************************************************************************
     * Multiplication operator
     * multiplies the two matrices together.
     ************************************************************************/
    Matrix operator * (Matrix & rhs)
    {
        if (this->columns != rhs.rows)
        {
            // that matrices can't be multiplied so return an empty one.
            Matrix mat;
            return mat;
        }

        Matrix newMatrix(this->rows, rhs.columns);
        int rSize = rhs.rows * rhs.columns;
        int lSize = this->rows * this->columns;
        for(int i = 0; i < this->columns; i++)
        {
            for(int j = 0; j < rhs.rows; j++)
            {
                newMatrix.matrixPtr[i * newMatrix.columns + j] += 
                    this->matrixPtr[i * this->columns + j] * 
                    rhs.matrixPtr[j * rhs.columns + i];
            }
        }
        return newMatrix;
    }
    friend Vertex operator * (Matrix & rhs, Vertex & lhs);
};

Vertex operator * (Matrix & rhs, Vertex & lhs)
{
    if (rhs.columns != 4 || rhs.rows != 4)
    {
        // A vertex has 4 "rows" so the matrix must have 4 columns
        Vertex vert;
        return vert;
    }

    Vertex newVertex;
    newVertex.x = rhs.matrixPtr[0] * lhs.x + rhs.matrixPtr[1] * lhs.y + rhs.matrixPtr[2] * lhs.z + rhs.matrixPtr[3] * lhs.w;
    newVertex.y = rhs.matrixPtr[4] * lhs.x + rhs.matrixPtr[5] * lhs.y + rhs.matrixPtr[6] * lhs.z + rhs.matrixPtr[7] * lhs.w;
    newVertex.z = rhs.matrixPtr[8] * lhs.x + rhs.matrixPtr[9] * lhs.y + rhs.matrixPtr[10] * lhs.z + rhs.matrixPtr[11] * lhs.w;
    newVertex.w = rhs.matrixPtr[12] * lhs.x + rhs.matrixPtr[13] * lhs.y + rhs.matrixPtr[14] * lhs.z + rhs.matrixPtr[15] * lhs.w;

    return newVertex;
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
            SDL_LockSurface(img); // added just because.
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
        PIXEL color;
        void *image;
        Vertex *vert;
        double rgb[3];
        double uv[2];
        float rotation;
        float scale[3];
        float translation[3];

        // Obligatory empty constructor
        Attributes() {}

        // Needed by clipping (linearly interpolated Attributes between two others)
        Attributes(const Attributes & first, const Attributes & second, const double & valueBetween)
        {
            // Your code goes here when clipping is implemented
        }

        ~Attributes()
        {
        }
};	

/*******************************************************
 * CROSS_PRODUCT
 * calculates the cross product of the given vertices.
 ******************************************************/
float triangleArea(double v1[2], double v2[2])
{
    return ((v1[0] * v2[1]) - (v1[1] * v2[0])) / 2;
}

/***************************************************************************
 * LINEAR INTERPOLATION
 * linearly interpolates the values passed in based on the area of the
 * triangles.
 **************************************************************************/
double lerp(float area, float area1, float area2, float area3, double attr1, double attr2, double attr3)
{
    double lerpedAttr = 0.0;
    lerpedAttr = (area1/area * attr1) + (area2/area * attr2) + (area3/area * attr3);
    return lerpedAttr;
}

// Example of a fragment shader
void DefaultFragShader(PIXEL & fragment, const Attributes & vertAttr, const Attributes & uniforms)
{
    // Output our shader color value, in this case red.
    fragment = 0xffff0000;
}

void greenFragShader(PIXEL & fragment, const Attributes & vertAttr, const Attributes & uniforms)
{
    fragment = 0xff00ff00;
}

/********************************************************************
 * COLOR FRAG SHADER
 * adds the red, green, and blue values to the color of the pixel.
 *******************************************************************/
void colorFragShader(PIXEL & fragment, const Attributes & vertAttr, const Attributes & uniforms)
{
    PIXEL color = 0xff000000;
    color += (unsigned int)(vertAttr.rgb[0] * 0xff) << 16;
    color += (unsigned int)(vertAttr.rgb[1] * 0xff) << 8;
    color += (unsigned int)(vertAttr.rgb[2] * 0xff) << 0;

    fragment = color;
}

/*****************************************************************
 * IMAGE FRAG SHADER 
 * prints the image to the screen according to the UV coords.
 *****************************************************************/
void imageFragShader(PIXEL & fragment, const Attributes & vertAttr, const Attributes & uniforms)
{
    BufferImage *ptr = (BufferImage*)uniforms.image;

    int wid = ptr->width() - 1;
    int hgt = ptr->height() - 1;

    int x = vertAttr.uv[0] * hgt;
    int y = vertAttr.uv[1] * wid;

    fragment = (*ptr)[y][x];
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

// Example of a vertex shader
void vertexShader(Vertex & vertOut, Attributes & attrOut, const Vertex & vertIn, const Attributes & vertAttr, const Attributes & uniforms)
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
