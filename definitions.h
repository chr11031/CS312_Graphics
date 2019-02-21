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
#define S_WIDTH 512
#define S_HEIGHT 512
#define PIXEL Uint32
#define ABS(in) (in > 0 ? (in) : -(in))
#define SWAP(TYPE, FIRST, SECOND) \
    {                             \
        TYPE tmp = FIRST;         \
        FIRST = SECOND;           \
        SECOND = tmp;             \
    }
#define MIN(A, B) A < B ? A : B
#define MAX(A, B) A > B ? A : B
#define MIN3(A, B, C) MIN((MIN(A, B)), C)
#define MAX3(A, B, C) MAX((MAX(A, B)), C)
#define X_Key 0
#define Y_Key 1

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
    T **grid;
    int w;
    int h;

    // Private intialization setup
    void setupInternal()
    {
        // Allocate pointers for column references
        grid = (T **)malloc(sizeof(T *) * h);
        for (int r = 0; r < h; r++)
        {
            grid[r] = (T *)malloc(sizeof(T) * w);
        }
    }

    // Empty Constructor
    Buffer2D()
    {
    }

  public:
    // Free dynamic memory
    ~Buffer2D()
    {
        // De-Allocate pointers for column references
        for (int r = 0; r < h; r++)
        {
            free(grid[r]);
        }
        free(grid);
    }

    // Size-Specified constructor, no data
    Buffer2D(const int &wid, const int &hgt)
    {
        h = hgt;
        w = wid;
        setupInternal();
        zeroOut();
    }

    // Assignment constructor
    Buffer2D &operator=(const Buffer2D &ib)
    {
        w = ib.width();
        h = ib.height();
        setupInternal();
        for (int r = 0; r < h; r++)
        {
            for (int c = 0; c < w; c++)
            {
                grid[r][c] = ib[r][c];
            }
        }
    }

    // Set each member to zero
    void zeroOut()
    {
        for (int r = 0; r < h; r++)
        {
            for (int c = 0; c < w; c++)
            {
                grid[r][c] = 0;
            }
        }
    }

    // Width, height
    const int &width() { return w; }
    const int &height() { return h; }

    // The frequented operator for grabbing pixels
    inline T *&operator[](int i)
    {
        return grid[i];
    }
};
/****************************************************
* Matrix for transforming vertices. 
****************************************************/
class Matrix
{
  private:
    double *ptrM;
    int rows;
    int columns;
    bool initTF;

  public:
    Matrix() : rows(0), columns(0), ptrM(NULL), initTF(false){};

    // constructor that sets up the matrix size
    Matrix(int rows, int columns)
    {
        this->rows = rows;
        this->columns = columns;
        ptrM = new double[rows * columns];

        //creating the matrix
        for (int i = 0; i < (rows * columns); i++)
        {
            ptrM[i] = 0;
        }

        initTF = false;
    }

    ~Matrix()
    {
        if (ptrM != NULL)
        {
            delete[] ptrM;
            ptrM = NULL;
        }
    }
    /*************************************************************************
	* Add rotation to the current matrix.
	************************************************************************/
    void rotation(double rot1)
    {
        Matrix rotate(this->rows, this->columns);

        // initialize the rotation matrix
        rotate.ptrM[0] = cos(rot1);
        rotate.ptrM[1] = -sin(rot1);
        rotate.ptrM[1 * this->columns] = sin(rot1);
        rotate.ptrM[1 * this->columns + 1] = cos(rot1);
        rotate.ptrM[2 * this->columns + 2] = 1;
        rotate.ptrM[3 * this->columns + 3] = 1;

        if (initTF == false)
        {
            for (int i = 0; i < (this->rows * this->columns); i++)
            {
                this->ptrM[i] = rotate.ptrM[i];
            }

            initTF = true;
        }
        //Multiply to combine
        else
        {
            *this = rotate * (*this);
        }
    }

    /*************************************************************************
	* Add scaling to the current matrix.
	************************************************************************/
    void scaling(double scaleX, double scaleY, double scaleZ)
    {
        // this array is used in the for loop to make things easier.
        double scaleArray[4] = {scaleX, scaleY, scaleZ, 1};

        Matrix scale(this->rows, this->columns);

        // initialize the scaling matrix
        for (int i = 0; i < this->rows; i++)
        {
            scale.ptrM[i * this->columns + i] = scaleArray[i];
        }

        if (initTF == false)
        {
            for (int i = 0; i < (this->rows * this->columns); i++)
            {
                this->ptrM[i] = scale.ptrM[i];
            }
            initTF = true;
        }
        //multiply to combine.
        else
        {
            *this = scale * (*this);
        }
    }

    /*************************************************************************
	* Add translation to the current matrix.
	************************************************************************/
    void translation(double transX, double transY, double transZ)
    {
        Matrix translate(this->rows, this->columns);
        //initialize translation matrix.
        translate.ptrM[1 * this->columns - 1] = transX;
        translate.ptrM[2 * this->columns - 1] = transY;
        translate.ptrM[3 * this->columns - 1] = transZ;
        for (int i = 0; i < this->rows; i++)
        {
            translate.ptrM[i * this->columns + i] = 1;
        }
        // if the matrix has nothing in it make it equal
        if (initTF == false)
        {
            for (int i = 0; i < (this->rows * this->columns); i++)
            {
                this->ptrM[i] = translate.ptrM[i];
            }
            initTF = true;
        }
        //Multiply to combine.
        else
        {
            *this = translate * (*this);
        }
    }

    /*****************************************************************
	* Equals operator
	****************************************************************/
    Matrix &operator=(const Matrix &rhs)
    {
        this->rows = rhs.rows;
        this->columns = rhs.columns;
        this->initTF = rhs.initTF;
        if (this->ptrM != NULL)
        {
            delete[] ptrM;
        }
        ptrM = new double[this->rows * this->columns];
        for (int i = 0; i < (this->rows * this->columns); i++)
        {
            this->ptrM[i] = rhs.ptrM[i];
        }
        return *this;
    }

    /*************************************************************************
	* Multiplication operator
	************************************************************************/
    Matrix &operator*=(const Matrix &rhs)
    {
        if (this->columns != rhs.rows)
        {
            // the matrix cant be multiplied
            Matrix emptyMatrix;
            return emptyMatrix;
        }

        Matrix newMatrix(this->rows, rhs.columns);
        for (int i = 0; i < this->rows; i++)
        {
            for (int j = 0; j < rhs.columns; j++)
            {
                double sum = 0;
                for (int k = 0; k < rhs.rows; k++)
                {
                    sum += this->ptrM[i * this->columns + k] *
                           rhs.ptrM[k * rhs.columns + j];
                }
                newMatrix.ptrM[i * newMatrix.columns + j] = sum;
            }
        }
        newMatrix.initTF = true;
        *this = newMatrix;
        return *this;
    }

    // has to be a friend to access the private variables.
    friend Matrix operator*(const Matrix &lhs, const Matrix &rhs);
    friend Vertex operator*(const Matrix &rhs, const Vertex &lhs);
};
/****************************************************
 * BUFFER_IMAGE:
 * PIXEL (Uint32) specific Buffer2D class with .BMP 
 * loading/management features.
 ***************************************************/
class BufferImage : public Buffer2D<PIXEL>
{
  protected:
    SDL_Surface *img;                // Reference to the Surface in question
    bool ourSurfaceInstance = false; // Do we need to de-allocate?

    // Private intialization setup
    void setupInternal()
    {
        // Allocate pointers for column references
        h = img->h;
        w = img->w;
        grid = (PIXEL **)malloc(sizeof(PIXEL *) * h);

        PIXEL *row = (PIXEL *)img->pixels;
        row += (w * h);
        for (int i = 0; i < h; i++)
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
        if (ourSurfaceInstance)
        {
            SDL_FreeSurface(img);
        }
    }

    // Assignment constructor
    BufferImage &operator=(const BufferImage &ib)
    {
        img = ib.img;
        w = ib.w;
        h = ib.h;
        ourSurfaceInstance = false;
        grid = (PIXEL **)malloc(sizeof(PIXEL *) * img->h);
        for (int i = 0; i < img->w; i++)
        {
            grid[i] = ib.grid[i];
        }
    }

    // Constructor based on instantiated SDL_Surface
    BufferImage(SDL_Surface *src)
    {
        img = src;
        w = src->w;
        h = src->h;
        ourSurfaceInstance = false;
        setupInternal();
    }

    // Constructor based on reading in an image - only meant for UINT32 type
    BufferImage(const char *path)
    {
        ourSurfaceInstance = true;
        SDL_Surface *tmp = SDL_LoadBMP(path);
        SDL_PixelFormat *format = SDL_AllocFormat(SDL_PIXELFORMAT_ARGB8888);
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
    void *ptrImg;     //pointer image buffer
    double attrs[16]; //array of attributes, to make it dynamic
    Vertex *vert;
    Matrix matrixAttributes;

    // Obligatory empty constructor
    Attributes() {}

    // Needed by clipping (linearly interpolated Attributes between two others)
    Attributes(const Attributes &first, const Attributes &second, const double &valueBetween)
    {
        // Your code goes here when clipping is implemented
    }
    PIXEL color;
};

// multiplies the vertex by a matrix to get the new vertices.
void vertexShader(Vertex &vertOut, Attributes &attrOut, const Vertex &vertIn, const Attributes &vertAttr, const Attributes &uniforms)
{
    vertOut = uniforms.matrixAttributes * vertIn;
    attrOut = vertAttr;
}

/*******************************************
* ImageFragShader
*    This function is a image fragment shader
*    Which will assign the vertex of the triangle to the attribute
* ******************************************/
void ImageFragShader(PIXEL &fragment, const Attributes &vertAttr, const Attributes &uniforms)
{
    //initializing a buffer image pointer to the buffer image
    BufferImage *ptr = (BufferImage *)uniforms.ptrImg;

    //assign x and y two vertex attributes attr
    int x = vertAttr.attrs[3] * (ptr->width() - 1);
    int y = vertAttr.attrs[4] * (ptr->height() - 1);

    //Create a fragment after assigning coordinated to the triangle
    fragment = (*ptr)[y][x];
}

/*************************************************************************
* Multiplication operator
* two matrices together.
************************************************************************/
Matrix operator*(const Matrix &lhs, const Matrix &rhs)
{
    if (lhs.columns != rhs.rows)
    {
        //matrices can't be multiplied
        Matrix matrixNew;
        return matrixNew;
    }

    Matrix newMatrix(lhs.rows, rhs.columns);
    for (int i = 0; i < lhs.rows; i++)
    {
        for (int j = 0; j < rhs.columns; j++)
        {
            double sum = 0;
            for (int k = 0; k < rhs.rows; k++)
            {
                sum += lhs.ptrM[i * lhs.columns + k] *
                       rhs.ptrM[k * rhs.columns + j];
            }
            newMatrix.ptrM[i * newMatrix.columns + j] = sum;
        }
    }
    newMatrix.initTF = true;
    return newMatrix;
}

/**************************************************************
* multiplication operator to help when multiplying a vertex.
*************************************************************/
Vertex operator*(const Matrix &rhs, const Vertex &lhs)
{
    if (rhs.columns != 4 || rhs.rows != 4)
    {
        Vertex vert;
        return vert;
    }

    Vertex newVertex;
    newVertex.x = rhs.ptrM[0] * lhs.x + rhs.ptrM[1] * lhs.y + rhs.ptrM[2] * lhs.z + rhs.ptrM[3] * lhs.w;
    newVertex.y = rhs.ptrM[4] * lhs.x + rhs.ptrM[5] * lhs.y + rhs.ptrM[6] * lhs.z + rhs.ptrM[7] * lhs.w;
    newVertex.z = rhs.ptrM[8] * lhs.x + rhs.ptrM[9] * lhs.y + rhs.ptrM[10] * lhs.z + rhs.ptrM[11] * lhs.w;
    newVertex.w = rhs.ptrM[12] * lhs.x + rhs.ptrM[13] * lhs.y + rhs.ptrM[14] * lhs.z + rhs.ptrM[15] * lhs.w;

    return newVertex;
}
/********************************************
* ColorFragShader
*   This function is a fragment shader for generating the colors
*   This will set the Red, Green and Blue for the triangle
* ********************************************/
void ColorFragShader(PIXEL &fragment, const Attributes &vertAttr, const Attributes &uniforms)
{
    // assignning the rgb color attributes through shift
    // shift left 16 for red, left 8 for green, and left 0 for blue
    // assign attribute 0,1 and 2 respectively
    PIXEL color = 0xff000000;
    color += (unsigned int)(vertAttr.attrs[1] * 0xff) << 16;
    color += (unsigned int)(vertAttr.attrs[2] * 0xff) << 8;
    color += (unsigned int)(vertAttr.attrs[0] * 0xff) << 0;

    //assign fragment to color
    fragment = color;
}
// Example of a fragment shader
void DefaultFragShader(PIXEL &fragment, const Attributes &vertAttr, const Attributes &uniforms)
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
    void (*FragShader)(PIXEL &fragment, const Attributes &vertAttr, const Attributes &uniforms);

    // Assumes simple monotone RED shader
    FragmentShader()
    {
        FragShader = DefaultFragShader;
    }

    // Initialize with a fragment callback
    FragmentShader(void (*FragSdr)(PIXEL &fragment, const Attributes &vertAttr, const Attributes &uniforms))
    {
        setShader(FragSdr);
    }

    // Set the shader to a callback function
    void setShader(void (*FragSdr)(PIXEL &fragment, const Attributes &vertAttr, const Attributes &uniforms))
    {
        FragShader = FragSdr;
    }
};

// Example of a vertex shader
void DefaultVertShader(Vertex &vertOut, Attributes &attrOut, const Vertex &vertIn, const Attributes &vertAttr, const Attributes &uniforms)
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
    void (*VertShader)(Vertex &vertOut, Attributes &attrOut, const Vertex &vertIn, const Attributes &vertAttr, const Attributes &uniforms);

    // Assumes simple monotone RED shader
    VertexShader()
    {
        VertShader = DefaultVertShader;
    }

    // Initialize with a fragment callback
    VertexShader(void (*VertSdr)(Vertex &vertOut, Attributes &attrOut, const Vertex &vertIn, const Attributes &vertAttr, const Attributes &uniforms))
    {
        setShader(VertSdr);
    }

    // Set the shader to a callback function
    void setShader(void (*VertSdr)(Vertex &vertOut, Attributes &attrOut, const Vertex &vertIn, const Attributes &vertAttr, const Attributes &uniforms))
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
                   Buffer2D<PIXEL> &target,
                   const Vertex inputVerts[],
                   const Attributes inputAttrs[],
                   Attributes *const uniforms = NULL,
                   FragmentShader *const frag = NULL,
                   VertexShader *const vert = NULL,
                   Buffer2D<double> *zBuf = NULL);

#endif
