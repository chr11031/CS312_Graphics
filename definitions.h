#define SDL_MAIN_HANDLED
#include "SDL2/SDL.h"
#include "stdlib.h"
#include "stdio.h"
#include "math.h"
#include <vector>

#ifndef DEFINITIONS_H
#define DEFINITIONS_H

using namespace std;

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

/***************************************************
 * ATTRIBUTES (shadows OpenGL VAO, VBO)
 * The attributes associated with a rendered 
 * primitive as a whole OR per-vertex. Will be 
 * designed/implemented by the programmer. 
 **************************************************/
template <class T>
class Attributes
{      
        private:
        //public:
        double u; //for image
        double v; // ^ ^ ^ ^
        double r; // red
        double g; // green
        double b; // blue
        double a; // alpha component

        public:
        //if someone wants to just initialize rgba
        //check my draw 
        Attributes(T red, T grn, T blu, T alp) 
        {
            r = red;
            g = grn;
            b = blu;
            a = alp;
        }
        // Obligatory empty constructor
        Attributes() {}
        //Setters
        setR(double red) { r = red; };
        setG(double grn) { g = grn; };
        setB(double blu) { b = blu; };
        setA(double alp) { a = alp; };
        setU(double uuu) { u = uuu; };
        setV(double vvv) { v = vvv; };
        //Getters
        double getR() const { return r; };
        double getG() const { return g; };
        double getB() const { return b; };
        double getA() const { return a; };
        double getU() const { return u; };
        double getV() const { return v; };

        void* ptrImg;
        PIXEL color;

        vector<double> doubleVars; //Vector for additional doubles the user might need
        vector<T> otherVars; //Vector for additional datatypes

        // Needed by clipping (linearly interpolated Attributes between two others)
        Attributes(const Attributes & first, const Attributes & second, const double & valueBetween)
        {
            // Your code goes here when clipping is implemented
        }
};	

template <class T>
void ImageFragShader(PIXEL & fragment, const Attributes<T> & vertAttr, const Attributes<T> & uniforms)
{
    //PIXEL color;
    BufferImage* bf = (BufferImage*)uniforms.ptrImg;

    int x = vertAttr.getU() * (bf->width()-1);
    int y = vertAttr.getV() * (bf->height()-1);

    fragment = (*bf)[y][x] * 0x0000ff00;
}

template <class T>
void ColorFragShader(PIXEL & fragment, const Attributes<T> & vertAttr, const Attributes<T> & uniforms)
{
    PIXEL color = 0xff000000;
    color += (unsigned int)(vertAttr.getA() *0xff) << 24;
    color += (unsigned int)(vertAttr.getR() *0xff) << 16; 
    color += (unsigned int)(vertAttr.getG() *0xff) << 8; 
    color += (unsigned int)(vertAttr.getB() *0xff) << 0; 

    fragment = color;
}

// Example of a fragment shader
template <class T>
void DefaultFragShader(PIXEL & fragment, const Attributes<T> & vertAttr, const Attributes<T> & uniforms)
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
template <class T>
class FragmentShader
{
    public:
 
        // Get, Set implicit
        void (*FragShader)(PIXEL & fragment, const Attributes<T> & vertAttr, const Attributes<T> & uniforms);

        // Assumes simple monotone RED shader
        FragmentShader()
        {
            FragShader = DefaultFragShader;
        }

        // Initialize with a fragment callback
        FragmentShader(void (*FragSdr)(PIXEL & fragment, const Attributes<T> & vertAttr, const Attributes<T> & uniforms))
        {
            setShader(FragSdr);
        }

        // Set the shader to a callback function
        void setShader(void (*FragSdr)(PIXEL & fragment, const Attributes<T> & vertAttr, const Attributes<T> & uniforms))
        {
            FragShader = FragSdr;
        }
};

// Example of a vertex shader
template <class T>
void DefaultVertShader(Vertex & vertOut, Attributes<T> & attrOut, const Vertex & vertIn, const Attributes<T> & vertAttr, const Attributes<T> & uniforms)
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
template <class T>
class VertexShader
{
    public:
        // Get, Set implicit
        void (*VertShader)(Vertex & vertOut, Attributes<T> & attrOut, const Vertex & vertIn, const Attributes<T> & vertAttr, const Attributes<T> & uniforms);

        // Assumes simple monotone RED shader
        VertexShader()
        {
            VertShader = DefaultVertShader;
        }

        // Initialize with a fragment callback
        VertexShader(void (*VertSdr)(Vertex & vertOut, Attributes<T> & attrOut, const Vertex & vertIn, const Attributes<T> & vertAttr, const Attributes<T> & uniforms))
        {
            setShader(VertSdr);
        }

        // Set the shader to a callback function
        void setShader(void (*VertSdr)(Vertex & vertOut, Attributes<T> & attrOut, const Vertex & vertIn, const Attributes<T> & vertAttr, const Attributes<T> & uniforms))
        {
            VertShader = VertSdr;
        }
};

// Stub for Primitive Drawing function
/****************************************
 * DRAW_PRIMITIVE
 * Prototype for main drawing function.
 ***************************************/
template <class T>
void DrawPrimitive(PRIMITIVES prim, 
                   Buffer2D<PIXEL>& target,
                   const Vertex inputVerts[], 
                   const Attributes<T> inputAttrs[],
                   Attributes<T>* const uniforms = NULL,
                   FragmentShader<T>* const frag = NULL,
                   VertexShader<T>* const vert = NULL,
                   Buffer2D<double>* zBuf = NULL);      

/****************************************
 * Determinant: Returns the cross product
 ***************************************/       
inline double determinant(const double & a, const double & b, const double & c, const double & d)
{
    return (a * d - b * c);
}

/*****************************************
 * Linear Interpolation
 ****************************************/
template <class T>
T interp(const T & area,
             const T & det1,
             const T & det2,
             const T & det3,
             const T & attrs0, 
             const T & attrs1, 
             const T & attrs2)
{
    // Individual percentages as template types
    T componentR = (det1 / area) * attrs2;
    T componentG = (det2 / area) * attrs0;
    T componentB = (det3 / area) * attrs1;

    return (componentR + componentG + componentB);
};
#endif
