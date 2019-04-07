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
#define PI 3.14159265

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
            // return;
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

/****************************************************
 * MATRIX
 ***************************************************/
class Matrix
{
    private:
        int row;
        int col;
        double * m;

    public:
        // Default constructor (not used)
        Matrix() {}

        // Non-def constructor
        Matrix(int r, int c)
        {
            m = new double[r * c];
            row = r;
            col = c;

            for(int i = 0; i < row; i++){
                for(int j = 0; j < col; j++)
                {
                    if(i==j)
                        m[i * col + j] = 1;
                    else
                        m[i * col + j] = 0;
                }
            }
        }

        // Destructor
        ~Matrix() { delete [] m; }

        // Assignment operator overload
        Matrix & operator =(const Matrix & rhs)
        {
            for (int i = 0; i < row * col; i++)
                this->m[i] = rhs.m[i];
        }

        // Multiplication overload
        Matrix operator *(const Matrix & rhs) const
        {
            if (this->col != rhs.row)
            {
                printf("ERROR! Wrong Matrix Multiplication!");
                exit;
            }
            
            Matrix ret = Matrix(this->row, rhs.col);
            for (int i = 0; i < ret.row * ret.col; i++)
            {
                if (ret.m[i] != 0.0)
                    ret.m[i] = 0.0;
            }

            for (int r = 0; r < this->row; r++)
                for (int c = 0; c < rhs.col; c++)
                    for (int k = 0; k < rhs.row; k++)
                    {
                        ret.m[(r * ret.col) + c] += this->m[(r * this->col) + k] * rhs.m[(k * rhs.col) + c];
                    }
            
            return ret;
        }
        // Sets a value to the specified coordinates.
        void setValue(const int r, const int c, const double val)
        {
            this->m[(r * this->col) + c] = val;
        }

        // Returns a value from the specified coordinates.
        double getValue(const int r, const int c)
        {
            return this->m[(r * this->col) + c];
        }

        // Resets the matrix to the identity matrix.
        void reset()
        {
            for(int i = 0; i < this->row; i++)
            {
                for(int j = 0; j < this->col; j++)
                {
                    if(i==j)
                        m[i * this->col + j] = 1;
                    else
                        m[i * this->col + j] = 0;
                }
            }
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
        // Individual red, green, blue, u and v variables to easier handle interpolation.
        double r;
        double g;
        double b;
        
        double u;
        double v;

        // void pointer to hold an image for texture-related stuff.
        void * ptrImg;

        // For mouse coordinates. Will possibly be changed later.
        double mouseX;
        double mouseY;

        // For matrix tranformations.
        double translate[3];
        double scale[3];
        double rotate;

        Matrix tMatrix = Matrix(4, 4);

        // Obligatory empty constructor
        Attributes() {
            r, g, b, u, v, translate[0], translate[1], translate[2], scale[0], scale[1], scale[2], rotate = 0.0;
        }

        // Needed by clipping (linearly interpolated Attributes between two others)
        Attributes(const Attributes & first, const Attributes & second, const double & valueBetween)
        {
            // Your code goes here when clipping is implemented
        }

        // Constructor with passed in red, green, and blue values.
        Attributes(double & red, double & green, double & blue)
        {
            this->r = red;
            this->g = green;
            this->b = blue;
        }

        // Constructor with passed in u and v values.
        Attributes(double & u, double & v)
        {
            this->u = u;
            this->v = v;
        }
};	

// My interpolation function. The determinants of each vertex is divided by the area and multiplied
// with the specific color weights of each vertex.
double interp(double area, double d1, double d2, double d3, double t1, double t2, double t3)
{
    d1 /= area;
    d2 /= area;
    d3 /= area;

    return (d1 * t3) + (d2 * t1) + (d3 * t2);
}

// Example of a fragment shader
void DefaultFragShader(PIXEL & fragment, const Attributes & vertAttr, const Attributes & uniforms)
{
    // Output our shader color value, in this case red
    fragment = 0xffff0000;
}

// Color fragment shader, which assigns a color to a pixel based on the color weights passed in.
void colorFragShader(PIXEL & fragment, const Attributes & vertAttr, const Attributes & uniforms)
{
    PIXEL color = 0xff000000;
    color += (unsigned int)(vertAttr.r * 0xff) << 16;
    color += (unsigned int)(vertAttr.g * 0xff) << 8;
    color += (unsigned int)(vertAttr.b * 0xff) << 0;

    fragment = color;
}

// Image fragment shader, which draws an image over a rendered triangle.
void imageFragShader(PIXEL & fragment, const Attributes & vertAttr, const Attributes & uniforms)
{
    BufferImage* bf = (BufferImage*)uniforms.ptrImg;
    int x = vertAttr.u * (bf->width()-1);
    int y = vertAttr.v * (bf->height()-1);

    fragment = (*bf)[y][x];
}

// Custom Shader
void tvStaticFragShader(PIXEL & fragment, const Attributes & vertAttr, const Attributes & uniforms)
{
    PIXEL color = 0xff000000;
    // double c = (rand() % 11) / 10;
    int c = rand() % 2;
    color += (unsigned int)(c * 0xff) << 16;
    color += (unsigned int)(c * 0xff) << 8;
    color += (unsigned int)(c * 0xff) << 0;

    fragment = color;
}

// Another custom shader
void colorStaticFragShader(PIXEL & fragment, const Attributes & vertAttr, const Attributes & uniforms)
{
    PIXEL color = 0xff000000;
    int r = (rand() % 11) / 10;
    int g = (rand() % 11) / 10;
    int b = (rand() % 11) / 10;
    color += (unsigned int)(r * 0xff) << 16;
    color += (unsigned int)(g * 0xff) << 8;
    color += (unsigned int)(b * 0xff) << 0;

    fragment = color;
}

// Another another custom shader
void greenFragShader(PIXEL & fragment, const Attributes & vertAttr, const Attributes & uniforms)
{
    BufferImage* bf = (BufferImage*)uniforms.ptrImg;
    int x = vertAttr.u * (bf->width()-1);
    int y = vertAttr.v * (bf->height()-1);

    PIXEL color = 0xff00ff00;

    fragment = ((*bf)[y][x]) & color;
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

void MatrixVertShader(Vertex & vertOut, Attributes & attrOut, const Vertex & vertIn, const Attributes & vertAttr, const Attributes & uniforms)
{
    Matrix temp = Matrix(4, 1);

    temp.setValue(0, 0, vertIn.x);
    temp.setValue(1, 0, vertIn.y);
    temp.setValue(2, 0, vertIn.z);
    temp.setValue(3, 0, vertIn.w);

    temp = uniforms.tMatrix * temp;

    vertOut.x = temp.getValue(0, 0);
    vertOut.y = temp.getValue(1, 0);
    vertOut.z = temp.getValue(2, 0);
    vertOut.w = temp.getValue(3, 0);
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
