#define SDL_MAIN_HANDLED
#include "SDL2/SDL.h"
#include "stdlib.h"
#include "stdio.h"
#include "math.h"
#include <map>

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

/*****************************************************
 * MATRIX:
 * This class performs several matrix operations
 * which includes: transform, scale, and rotate.
 ****************************************************/
class Matrix
{
    public:
        // Constructor to pass in the default values
        Matrix(): rows(0), cols(0), matrixPtr(NULL), init(false) {};

        // Sets the matrix size
        Matrix(int rows, int cols)
        {
            this->rows = rows;
            this->cols = cols;

            matrixPtr = new double[rows * cols];
            for(int i = 0; i < (rows * cols); i++)
            {
                matrixPtr[i] = 0;
            }
            init = false;
        }

        // Matrix Pointer destructor
        ~Matrix()
        {
            if(matrixPtr != NULL)
            {
                delete [] matrixPtr;
                matrixPtr = NULL;
            }
        }

        /**************************************************
         * TRANSLATE
         * Takes in matrix values to translate the triangle
         **************************************************/
        void translate(double x, double y, double z)
        {
            // sets the size of the matrix to be translated
            Matrix mTranslate(this->rows, this->cols);

            // assign the passed in values to the designated cells to translate
            mTranslate.matrixPtr[1 * this->cols - 1] = x;
            mTranslate.matrixPtr[2 * this->cols - 1] = y;
            mTranslate.matrixPtr[3 * this->cols - 1] = z;

            for(int i = 0; i < this->rows; i++)
                mTranslate.matrixPtr[i * this->cols + i] = 1;
   
            // checks to see if the matrix has values, if not, this will be the default matrix
            if (init == false)
            {
                for(int i = 0; i < (this->rows * this->cols); i++)
                    this->matrixPtr[i] = mTranslate.matrixPtr[i];
                
                init = true;
            }
            // multiply what is in the matrix
            else
                *this *= mTranslate;
        }

        /***********************************************
         * SCALE
         * Takes in matrix values to scale the triangle
         **********************************************/
        void scale(double x, double y, double z)
        {
            // values are put into an array 
            double scaleArray[4] = {x, y, z, 1};

            // sets the size of the matrix to be scaled
            Matrix mScale(this->rows, this->cols);

            // insert values into the matrix to be scaled
            for(int i = 0; i < this->rows; i++)
                mScale.matrixPtr[i * this->cols + i] = scaleArray[i];
            
            // checks to see if the matrix has values, if not, this will be the default matrix
            if (init == false)
            {
                for(int i = 0; i < (this->rows * this->cols); i++)
                    this->matrixPtr[i] = mScale.matrixPtr[i];
                
                init = true;
            }
            // multiply what is in the matrix
            else
                *this *= mScale;
        }

        /*******************************************
         * ROTATE
         * Takes in the angle in radians
         ******************************************/
        void rotate(double angle)
        {
            // Converts the angle
            double sinAngle = sin(angle * M_PI/180.0);
            double cosAngle = cos(angle * M_PI/180.0);

            // Sets the size of the matrix to be rotated
            Matrix mRotate(this->rows, this->cols);

            // Determine which cells the angle will be inserted into the matrix
            mRotate.matrixPtr[0] = cosAngle;
            mRotate.matrixPtr[1] = -sinAngle;
            mRotate.matrixPtr[1 * this->cols] = sinAngle;
            mRotate.matrixPtr[1 * this->cols + 1] = cosAngle;
            mRotate.matrixPtr[2 * this->cols + 2] = 1;
            mRotate.matrixPtr[3 * this->cols + 3] = 1;

            // checks to see if the matrix has values, if not, this will be the default matrix
            if (init == false)
            {
                for(int i = 0; i < (this->rows * this->cols); i++)
                    this->matrixPtr[i] = mRotate.matrixPtr[i];
            
                init = true;
            }
            // multiply what is in the matrix
            else
                *this *= mRotate;
        }

    /*****************************************************************
     * Equals operator.
     * Copies and sets the rows, cols, and init
     ****************************************************************/
    Matrix & operator = (const Matrix & rhs)
    {
        this->rows = rhs.rows;
        this->cols = rhs.cols;
        this->init = rhs.init;
        
        if(this->matrixPtr != NULL)
            delete [] matrixPtr;
        
        matrixPtr = new double[this->rows * this->cols];
        for(int i = 0; i < (this->rows * this->cols); i++)
            this->matrixPtr[i] = rhs.matrixPtr[i];
        
        return *this;
    }

    /*************************************************************************
     * Multiplication operator
     * Operator to multiply matrixies
     ************************************************************************/
    Matrix & operator *= (const Matrix & rhs)
    {
        // Set the size of the temporary matrix
        Matrix tmpMatrix(this->rows, rhs.cols);
        
        for(int i = 0; i < this->rows; i++)
        {
            for(int j = 0; j < rhs.cols; j++)
            {
                double sum = 0;
                for(int k = 0; k < rhs.rows; k++)
                     sum += this->matrixPtr[i * this->cols + k] * rhs.matrixPtr[k * rhs.cols + j];
                
                tmpMatrix.matrixPtr[i * tmpMatrix.cols + j] = sum;
            }
        }
        tmpMatrix.init = true;
        *this = tmpMatrix;
        return *this;
    }

    friend Vertex operator * (const Matrix & rhs, const Vertex & lhs);

    private:
        double *matrixPtr;
        int rows;
        int cols;
        bool init;
};

/**************************************************************
 * Multiplication Operator
 * Operator to multiply verticies
 *************************************************************/
Vertex operator * (const Matrix & rhs, const Vertex & lhs)
{
    Vertex tmpVertex;
    tmpVertex.x = rhs.matrixPtr[0] * lhs.x + rhs.matrixPtr[1] * lhs.y + rhs.matrixPtr[2] * lhs.z + rhs.matrixPtr[3] * lhs.w;
    tmpVertex.y = rhs.matrixPtr[4] * lhs.x + rhs.matrixPtr[5] * lhs.y + rhs.matrixPtr[6] * lhs.z + rhs.matrixPtr[7] * lhs.w;
    tmpVertex.z = rhs.matrixPtr[8] * lhs.x + rhs.matrixPtr[9] * lhs.y + rhs.matrixPtr[10] * lhs.z + rhs.matrixPtr[11] * lhs.w;
    tmpVertex.w = rhs.matrixPtr[12] * lhs.x + rhs.matrixPtr[13] * lhs.y + rhs.matrixPtr[14] * lhs.z + rhs.matrixPtr[15] * lhs.w;

    return tmpVertex;
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
        SDL_Surface* img;           // Reference to the Surface in question
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
            SDL_LockSurface(img);
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
        std::map<char, double> var;            // uses a map to store attribute values
        BufferImage *image;                    // Pointer attribute for images
        Matrix transVertex;
        
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

/*******************************************************
 * INTERPOLATE
 * Find the ratio between the area of the sub-triangles
 * and the area of the triangle.
 * Source:
 * https://www.scratchapixel.com/lessons/3d-basic-rendering/rasterization-practical-implementation/rasterization-stage
 *******************************************************/
double interp(const double triangleArea, const double fDet, const double sDet, const double tDet, 
              const double attr1, const double attr2, const double attr3)
{
    return (((fDet / triangleArea) * attr3) + ((sDet / triangleArea) * attr1) + ((tDet / triangleArea) * attr2));
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
