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
#define X_KEY 0
#define Y_KEY 1
// Max # of vertices after clipping
#define MAX_VERTICES 8
#define PI 3.14159 
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
 * Matrix class 
 ****************************************************/
class Matrix
{
private:
public:

    double matrix[4][4]  = {{1,0,0,0},
                            {0,1,0,0},
                            {0,0,1,0},
                            {0,0,0,1}};

    Matrix() {}

    //setting the matrices
    Matrix(double newMatrix[4][4])
    {
        for(int y = 0; y < 3; y++)
        {
            for(int x = 0; x < 3; x++)
            { 
                 this->matrix[y][x] = newMatrix[y][x];
            }
       }
    }       

    double * operator[](int i)
    {
        if(i == 1 || i == 2 || i == 3 || i ==4)
        {
            return matrix[i];
        }
    }

    const double * operator[](int i) const
    {
        if(i == 1 || i == 2 || i == 3 || i ==4)
        {
            return matrix[i];
        }
    }

};

/****************************************************
 * Multiplies any two 4x4 matrices 
 ****************************************************/
Matrix operator * (const Matrix & first, const Matrix & second)
{
    double sum = 0;
    double resultMatrix[4][4];
    for(int x = 0; x < 4; x++)
    {
        for(int k = 0; k < 4; k++)
        {
            sum = 0;
            for(int y = 0; y < 4; y++)
            {
                sum = sum + (first[x][y] * second[y][k]);  
            }
            resultMatrix[x][k] = sum;      
        }
    }
    return resultMatrix;
}
     

/****************************************************
 * Multiply the vertices
 ****************************************************/
Vertex operator * (const Matrix & first, const Vertex & second)
{
    Vertex resultVertex;

    resultVertex.x = second.x * first[0][0] + second.y * first[0][1] + second.z * first[0][2] + second.w * first[0][3];
    resultVertex.y = second.x * first[1][0] + second.y * first[1][1] + second.z * first[1][2] + second.w * first[1][3];
    resultVertex.z = second.x * first[2][0] + second.y * first[2][1] + second.z * first[2][2] + second.w * first[2][3];
    resultVertex.w = second.x * first[3][0] + second.y * first[3][1] + second.z * first[3][2] + second.w * first[3][3];

    return resultVertex;
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
            setupInternal();
        }
};
/***************************************************
 * ATTRIBUTES (shadows OpenGL VAO, VBO)
 * The attributes associated with a rendered 
 * primitive as a whole OR per-vertex. Will be 
 * designed/implemented by the programmer. 
 **************************************************/
//template <class T>
class Attributes
{      
    private: 
        // double r;
        // double g;
        // double b;
        // double u;
        // double v;  

    public:
        void* ptrImg;
        Matrix aMatrix;

        // Obligatory empty constructor
        //Attributes() : r(0), g(0), b(0), u(0), v(0){}
        Attributes() {}
        double allAttributes[5];

        // double getR() const {return this->r;}0
        // double getG() const {return this->g;}1
        // double getB() const {return this->b;}2
        // double getU() const {return this->u;}3
        // double getV() const {return this->v;}4

        // void setR(double r){ this->r = r;}
        // void setG(double g){ this->g = g;}
        // void setB(double b){ this->b = b;}
        // void setU(double u){ this->u = u;}
        // void setV(double v){ this->v = v;}

        // Needed by clipping (linearly interpolated Attributes between two others)
        Attributes(const Attributes & first, const Attributes & second, const double & valueBetween)
        {
            // Your code goes here when clipping is implemented
        }
        PIXEL color;
};  
// Image Fragment Shader 
void ImageFragShader(PIXEL & fragment, const Attributes & vertAttr, const Attributes & uniforms)
{
    BufferImage* bf = (BufferImage*)uniforms.ptrImg;
    int x = vertAttr.allAttributes[3] * (bf->width()-1);
    int y = vertAttr.allAttributes[4] * (bf->height()-1);

    fragment = (*bf)[y][x];
}

void ImageFragShaderGreen(PIXEL & fragment, const Attributes & vertAttr, const Attributes & uniforms)
{
    BufferImage* bf = (BufferImage*)uniforms.ptrImg;
    int x = vertAttr.allAttributes[3] * (bf->width()-1);
    int y = vertAttr.allAttributes[4] * (bf->height()-1);

    

    fragment = (*bf)[y][x];
}
// My Fragment Shader for color interpolation
void ColorFragShader(PIXEL & fragment, const Attributes & vertAttr, const Attributes & uniforms)
{
    // Output our shader color value, in this case red
    PIXEL color = 0xff000000;

    color += (unsigned int)(vertAttr.allAttributes[0] *0xff) << 16;
    color += (unsigned int)(vertAttr.allAttributes[1] *0xff) << 8;
    color += (unsigned int)(vertAttr.allAttributes[2] *0xff) << 0;

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

void RealVertShader(Vertex & vertOut, Attributes & attrOut, const Vertex & vertIn, const Attributes & vertAttr, const Attributes & uniforms)
{
    Matrix vertMatrix = uniforms.aMatrix;
    
    vertOut = uniforms.aMatrix * vertIn;
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
 * INTERP
 * Trying to perdict each the color at a certain point
 * You get the determinate of the each area and divide it by the area, then multiply it
 * by the color attribute. Then you add all of those together to find the interpolation.
 * **************************************/
double interp(double & areaTriangle, double & firstDet, double & secondDet, double & thirdDet, double a1, double a2, double a3)
{
    double w1 = (firstDet/areaTriangle) * a3;
    double w2 = (secondDet/areaTriangle) * a1;
    double w3 = (thirdDet/areaTriangle) * a2;
    return (w1 + w2 + w3);
}   
       
#endif