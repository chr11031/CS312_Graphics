#define SDL_MAIN_HANDLED
#include "SDL2/SDL.h"
#include "stdlib.h"
#include "stdio.h"
#include "math.h"
#include "stdexcept"


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

struct Vertex;
typedef Vertex Matrix[4];
typedef Vertex *MatrixPtr;


/******************************************************
 * Types of primitives our pipeline will render.
 *****************************************************/
enum PRIMITIVES 
{
    TRIANGLE,
    LINE,
    POINT
};

// Retrieves a vertex value given an index
// Using a template allows for using the function
// to define both const and non-const versions
template<typename T>
T & getVertI(int i, T & x, T & y, T & z, T & w) {
  switch(i)
  {
    case 0:
      return x;
      break;
    case 1:
      return y;
      break;
    case 2:
      return z;
      break;
    case 3:
      return w;
      break;
    default:
      throw std::invalid_argument("Index out of bounds");
  }
}

struct Vertex
{
    double x;
    double y;
    double z;
    double w;

    // access like an array
    const double &operator[](int i) const
    {
      return getVertI<const double>(i, x, y, z, w);
    }

    double &operator[](int i)
    {
      return getVertI<double>(i, x, y, z, w);
    }
};


/*************************************************
 * Implements methods to multiply 4x4 and 4x1 matrixs
 * Uses the vertex data type to represent matrixes
 *************************************************/
class MatrixMath
{
  public:
    const Vertex *matrix;
    Vertex *res;

    // For 4x1 results
    MatrixMath(const Vertex *m, Vertex &result)
    {
        matrix = m;
        res = &result;
    }

    // For 4x4 results (Vertex pointer can be interpretted as array)
    MatrixMath(const Vertex *m, Vertex *result)
    {
        matrix = m;
        res = result;
    }

    MatrixMath operator*(const Vertex &rhs)
    {
        multiply(rhs);
        return *this;
    }

    MatrixMath operator*(const Matrix rhs)
    {
        multiply(rhs);
        return *this;
    }

    // For 4x1 matrix math
    void resolveMult(const Vertex &vert, int i)
    {
        (*res)[i] = 0;
        for (int k = 0; k < 4; k++)
        {
            (*res)[i] += matrix[i][k] * vert[k];
        }
    }

    // For 4x4 matrix math
    void resolveMult(const Matrix matrix2, int i)
    {
        for (int j = 0; j < 4; j++)
        {
            res[i][j] = 0;
            for (int k = 0; k < 4; k++)
            {
                res[i][j] += matrix[i][k] * matrix2[k][j];
            }
        }
    }

    // Template used to select the correct revoleMult
    template <class T>
    void multiply(T &matrix2)
    {
        for (int i = 0; i < 4; i++)
            resolveMult(matrix2, i);
    }
};

/*****************************************************
 * Implements methods for translation, rotation, and
 * scaling a 4x1 matrix. Methods are for x and y
 * plane, but adding functionality for z is trivial
 *****************************************************/
class Transform
{
  public:

    Transform() : matrix(nullptr)
    {
        matrix = (Matrix*) new Vertex[4];
        isAllocated = true;

        for (int i = 0; i < 4; i++)
            for (int j = 0; j < 4; j++)
                (*matrix)[i][j] = i == j ? 1 : 0;
    }

    Transform(Matrix * m)
    {
        matrix = m;
        isAllocated = false;
    }

    ~Transform()
    {
        if (matrix && isAllocated)
            delete [] matrix;
    }

    // Positive X moves to the left, positive y moves down
    // as a result of pipeline implementation
    Transform & translate(double x = 1, double y = 1)
    {
      (*matrix)[0][3] = x;
      (*matrix)[1][3] = y;

      return *this;
    }

    Transform & scale(double x, double y)
    {
        if ((*matrix)[0][0] != 1 || (*matrix)[1][1] != 1)
        {
            Matrix m2 =
                {
                    {x, 0, 0, 0},
                    {0, y, 0, 0},
                    {0, 0, 1, 0},
                    {0, 0, 0, 1}};

            transformHelper(m2);
        }
        else // if we don't need to allocate memory then don't do it
        {    // Another transformation involving these blocks hasn't been done
            (*matrix)[0][0] = x;
            (*matrix)[1][1] = y;
        }

        return *this;
    }

    // Rotate matrix counter clockwise (as a result of pipeline implementation)
    Transform & rotate(double degrees)
    {
        // have to convert to radians
        const double PI  =3.141592653589793238463;
        double rad = degrees / 180.0 * PI;
        double s = sin(rad);
        double c = cos(rad);

        // check if other transformations have happened to check
        // if we have to multiply matrices which involves allocating memory
        if ((*matrix)[0][0] != 1 || (*matrix)[1][1] != 1)
        {
            Matrix m2 =
                {
                    {c, -s, 0, 0},
                    {s, c, 0, 0},
                    {0, 0, 1, 0},
                    {0, 0, 0, 1}
        };

            transformHelper(m2);
        }
        else // if we don't need to allocate memory then don't do it
        {    
            (*matrix)[0][0] = c;
            (*matrix)[0][1] = -s;
            (*matrix)[1][0] = s;
            (*matrix)[1][1] = c;
        }

        return *this;
    }

    void apply(const Vertex & vert, Vertex & res)
    {
      MatrixMath(*matrix, res) * vert;
    }

    private:
      Matrix *matrix;
      bool isAllocated;

      // For methods that require multiplying two matrices
      void transformHelper(Matrix & m2)
      {
        // The two matrices being multiplied must stay constant
        // so a new matrix must be allocated for the new values
        Matrix * tmp = matrix;
        matrix = (Matrix*) new Vertex[4];
        MatrixMath(m2, *matrix) * *tmp;
        delete [] tmp;
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
        void * data;
        int dLen; // Know how much data is in an array

        // No need for an empty constructor as this constructor will
        // be called when no parameters are provided
        Attributes(void * data = nullptr, int dLen = -1) 
        {
            this->data = data;
            this->dLen = dLen;
        }

        // Needed by clipping (linearly interpolated Attributes between two others)
        Attributes(const Attributes & first, const Attributes & second, const double & valueBetween)
        {
            // Your code goes here when clipping is implemented
        }
};

/*******************************************************
 * Contains values for RgbAttributes
 ******************************************************/
class RgbAttr : public Attributes
{
    public:
        RgbAttr(double red = 0, double green = 0, double blue = 0) 
        {
            rgb[0] = red;
            rgb[1] = green;
            rgb[2] = blue;
            this->data = rgb;
            this->dLen = 3;
        }

    private:
        double rgb[3];
};

/*******************************************************
 * Contains values for ImageAttributes
 ******************************************************/
class ImageAttr : public Attributes
{
    public:
        ImageAttr(double u = 0, double v = 0) 
        {
            uv[0] = u;
            uv[1] = v;
            this->data = uv;
            this->dLen = 2;
        }

    private:
        double uv[2];
};

/*******************************************************
 * Contains values for Transform Uniforms
 ******************************************************/
class TransUni : public Attributes
{
    public:
        TransUni() : trans(Transform())
        {
            this->data = &trans;
            this->dLen = -1;
        }

        Transform & get() { return trans; }

    private:
        Transform trans;
};

// Example of a fragment shader
void DefaultFragShader(PIXEL & fragment, const Attributes & vertAttr, const Attributes & uniforms)
{
    // Output our shader color value, in this case red
    fragment = 0xffff0000;
}

// Image Fragment Shader 
void ImageFragShader(PIXEL & fragment, const Attributes & vertAttr, const Attributes & uniforms)
{
    BufferImage* bf = (BufferImage*)uniforms.data;
    double * data = (double *)vertAttr.data;
    int x = data[0] * (bf->width()-1);
    int y = data[1] * (bf->height()-1);

    fragment = (*bf)[y][x];
}

void ColorFragShader(PIXEL & fragment, const Attributes & vertAttr, const Attributes & uniforms)
{
    PIXEL color = 0xff000000;
    double * data = (double*) vertAttr.data;
    color += (unsigned int)(data[0] *0xff) << 16;
    color += (unsigned int)(data[1] *0xff) << 8;
    color += (unsigned int)(data[2] *0xff) << 0;

    fragment = color;
}

void FragShaderSin(PIXEL & fragment, const Attributes & attributes, const Attributes & uniform)
{
    
}

// Frag Shader for UV without image (due to SDL2 bug?)
void FragShaderUVwithoutImage(PIXEL & fragment, const Attributes & attributes, const Attributes & uniform)
{
    // Figure out which X/Y square our UV would fall on
    int xSquare = ((double*)(attributes.data))[0] * 8;
    int ySquare = ((double*)(attributes.data))[1] * 8;

    // Is the X square position even? The Y? 
    bool evenXSquare = (xSquare % 2) == 0;
    bool evenYSquare = (ySquare % 2) == 0;

    // Both even or both odd - red square
    if( (evenXSquare && evenYSquare) || (!evenXSquare && !evenYSquare) )
    {
        fragment = 0xffff0000;
    }
    // One even, one odd - white square
    else
    {
        fragment = 0xffffffff;
    }
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

void TransformVertShader(Vertex & vertOut, Attributes & attrOut, const Vertex & vertIn, const Attributes & vertAttr, const Attributes & uniforms)
{
    Transform * trans = (Transform *) uniforms.data;
    trans->apply(vertIn, vertOut);
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
