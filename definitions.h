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

double multiplyRowAndCol(const double row[], const double col[], const int & length)
{
    double result = 0;

    for (int i = 0; i < length; i++)
    {
        result += row[i] * col[i];
    }

    return result;
}

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
    double data[4][4];
    int numRows;
    int numCols;

    Matrix() : numRows(0), numCols(0) {}


    Matrix(double newData[][4], int numRows, int numCols) : numRows(numRows), numCols(numCols)
    {
        for (int i = 0; i < numRows; i++)
            for (int j = 0; j < numCols; j++)
                data[i][j] = newData[i][j];

        this->numRows = numRows;
        this->numCols = numCols;
    }

    void setData(const double newData[0][4], int numRows, int numCols)
    {
        for (int i = 0; i < numRows; i++)
            for (int j = 0; j < numCols; j++)
                data[i][j] = newData[i][j];

        this->numRows = numRows;
        this->numCols = numCols;
    }

    void operator =(const Matrix & rhs) throw (const char*)
    {
        for (int i = 0; i < rhs.numRows; i++)
            for (int j = 0; j < rhs.numCols; j++)
                this->data[i][j] = rhs.data[i][j];

        this->numRows = rhs.numRows;
        this->numCols = rhs.numCols;
    }
    void operator *=(const Matrix & rhs) throw (const char *);
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
        Attributes() : valuesToInterpolate(0), ptrImage(NULL) {}


        // Needed by clipping (linearly interpolated Attributes between two others)
        Attributes(const Attributes & first, const Attributes & second, const double & valueBetween)
        {
            // Your code goes here when clipping is implemented
        }

        void interpolateValues(const double &area, const double &d1, const double &d2, const double &d3, Attributes* vertAttrs)
        {
            double w1 = d1 / area;
            double w2 = d2 / area;
            double w3 = 1 - w1 - w2;

            for (int i = 0; i < valuesToInterpolate; i++)
            {
                attrValues[i] = vertAttrs[0].attrValues[i] * w2 +
                                vertAttrs[1].attrValues[i] * w3 +
                                vertAttrs[2].attrValues[i] * w1;
            }
        }

        void correctPerspective(double correctedZ)
        {
            for (int i = 0; i < valuesToInterpolate; i++)
            {
                attrValues[i] *= correctedZ;
            }
        }

        void concatenateMatricies(const Matrix & transform) throw (const char *)
        {
            this->matrix *= transform;
        }

        void setMatrixData(const double data[0][4], int numRows, int numCols)
        {
            this->matrix.setData(data, numRows, numCols);
        }


        // Attribute information
        PIXEL color; // Still here for depricated code
        double attrValues[15];
        int valuesToInterpolate;
        void* ptrImage;

        // Matrix information for transformations
        Matrix matrix;
}; 


/**********************************************************************
 *  
 **********************************************************************/
void Matrix::operator*=(const Matrix & rhs) throw (const char *)
{
    // For all intents and purposes we have flipped the matrix order
    // in order to make the order of operations correct while maintaining
    // code readability for this function.

    if (rhs.numCols != this->numRows)
        throw "ERROR: Invalid matrix sizes for concatenate\n";

    // Temp matrix to hold results so we don't mess up data during
    // matrix multiplication
    double tempMatrix[4][4];
    
    // Loop through the number of rows in the RHS
    for (int i = 0; i < rhs.numRows; i++)
    {
        // Loop through the number of cols in LHS
        for (int j = 0; j < this->numCols; j++)
        {
            double colToRow[4];

            //Flatten the LHS cols into a row for maths
            for (int k = 0; k < this->numRows; k++)
            {
                colToRow[k] = this->data[k][j];
            }

            // Do maths on RHS row and LHS col and assign
            tempMatrix[i][j] = multiplyRowAndCol(rhs.data[i], colToRow, rhs.numCols);
        }
    }

    // Make sure this matrix knows it has changed
    this->numRows = rhs.numRows;

    // Overwrite the old matrix with the new one
    for (int i = 0; i < this->numRows ; i++)
        for (int j = 0; j < this->numCols; j++)
            this->data[i][j] = tempMatrix[i][j];
}

/******************************************************
 * MATRIX MULT OVERLOADED OPERATOR
 * Multiplies the two matricies together
 *****************************************************/ 
Vertex operator * (const Vertex & lhs, const Matrix & rhs) throw (const char *)
{
    if (rhs.numCols < 1 || rhs.numCols > 4)
        throw "ERROR: Invalid matrices sizes, cannot do multiplication";

    double newVerts[4] = {lhs.x, lhs.y, lhs.z, lhs.w};
    double currentVerts[4] = {lhs.x, lhs.y, lhs.z, lhs.w};
    
    for (int i = 0; i < rhs.numRows; i++)
        newVerts[i] = multiplyRowAndCol(rhs.data[i], currentVerts, rhs.numCols);

    Vertex temp = {newVerts[0], newVerts[1], newVerts[2], newVerts[3]};
    return temp;
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
        {}

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
