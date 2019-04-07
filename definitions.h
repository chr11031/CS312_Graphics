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
        Buffer2D() {}

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
            h = img -> h;
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
            //SDL_LockSurface(img);
            setupInternal();
        }
};

class Matrix
{
    public: 
        double matrix4[4][4] = {{1,0,0,0}, {0,1,0,0}, {0,0,1,0}, {0,0,0,1}};
        
        // Obligatory empty constructor
        Matrix(){}

        Matrix(const double matrix[4][4])
        {
            for(int x=0;x<4;x++) for(int y=0;y<4;y++) matrix4[x][y] = matrix[x][y];
        }
        Matrix(double x, double y, double z)
        {
            matrix4[0][0] = x;
            matrix4[1][1] = y;
            matrix4[2][2] = z;
        }

        Matrix trans(double x, double y, double z)
        {
            double trans[4][4] = {{1,0,0,x}, {0,1,0,y}, {0,0,1,z}, {0,0,0,1}};
            return Matrix(trans);
        }
        Matrix scale(double x, double y, double z) { return Matrix(x,y,z); }
        Matrix rotate(double x, double y, double z)
        {
            double cosX = cos(x * M_PI/180); double cosY = cos(y * M_PI/180); double cosZ = cos(z * M_PI/180);
            double sinX = sin(x * M_PI/180); double sinY = sin(y * M_PI/180); double sinZ = sin(z * M_PI/180);
            double xAxis[4][4] = {{1,0,0,0}, {0,cosX,-sinX,0}, {0,sinX,cosX,0}, {0,0,0,1}};
            double yAxis[4][4] = {{cosY,0,sinY,0}, {0,1,0,0}, {-sinY,0,cosY,0}, {0,0,0,1}};
            double zAxis[4][4] = {{cosZ,-sinZ,0,0}, {sinZ,cosZ,0,0}, {0,0,1,0}, {0,0,0,1}};
            Matrix xMatr = Matrix(xAxis);
            return xMatr.multi(yAxis).multi(zAxis);
        }
        Matrix multi(double matrix[4][4])
        {
            double multi[4][4];
            double sum = 0;
            for (int i = 0; i < 4; i++)
            {
                for (int j = 0; j < 4; j++)
                {
                    for (int k = 0; k < 4; k++) sum += matrix4[i][k] * matrix[k][j];
                    multi[i][j] = sum;
                    sum = 0;
                }
            }
            return Matrix(multi);
        }
        Vertex multi(Vertex vertIn) 
        {
            double x = (matrix4[0][0] * vertIn.x) + (matrix4[0][1] * vertIn.y)
                        + (matrix4[0][2] * vertIn.z) + (matrix4[0][3] * vertIn.w);
            double y = (matrix4[1][0] * vertIn.x) + (matrix4[1][1] * vertIn.y)
                        + (matrix4[1][2] * vertIn.z) + (matrix4[1][3] * vertIn.w);
            double z = (matrix4[2][0] * vertIn.x) + (matrix4[2][1] * vertIn.y)
                        + (matrix4[2][2] * vertIn.z) + (matrix4[2][3] * vertIn.w);
            double w = (matrix4[3][0] * vertIn.x) + (matrix4[3][1] * vertIn.y)
                        + (matrix4[3][2] * vertIn.z) + (matrix4[3][3] * vertIn.w);
            Vertex vertOut = {x,y,z,w};
            return vertOut;
        }
};

/***************************************************
 * ATTRIBUTES (shadows OpenGL VAO, VBO)
 * The attributes associated with a rendered 
 * primitive as a whole OR per-vertex. Will be 
 * designed/implemented by the programmer. 
 **************************************************/
union doublePointer
{
    double d;
    void* ptr;
};
class Attributes
{   
    public:
        //PIXEL color;
        Matrix matrix;
        vector<doublePointer> colorAttr;

        // Obligatory empty constructor
        Attributes() {}

        // Needed by clipping (linearly interpolated Attributes between two others)
        Attributes(const Attributes & first, const Attributes & second, const double & valueBetween)
        {
            // Your code goes here when clipping is implemented
            
        }
        Attributes(Attributes* const attrs, const double & area0, const double & area1, const double & area2, const double & Z)
        {
            for(int i = 0; i < attrs[0].colorAttr.size(); i++)
            {
                //determine attribute by adding each portion of vertex given by the corresponding area
                addDouble(((attrs[0][i].d * area0) + (attrs[1][i].d * area1) + (attrs[2][i].d * area2))*Z);
            }
        }
        void addDouble (const double & d)
        {
            doublePointer newD;
            newD.d = d;
            colorAttr.push_back(newD);
        }
        void addDouble (const double & d0, const double & d1, const double & d2)
        {
            doublePointer newD;
            newD.d = d0;
            colorAttr.push_back(newD);
            newD.d = d1;
            colorAttr.push_back(newD);
            newD.d = d2;
            colorAttr.push_back(newD);
        }
        void addDouble (const double* & d, const int length)
        {
            for(int x = 0; x < length; x++){
                doublePointer newD;
                newD.d = d[x];
                colorAttr.push_back(newD);
            }
        }
        void addPtr (void * ptr)
        {
            doublePointer newPtr;
            newPtr.ptr = ptr;
            colorAttr.push_back(newPtr);
        }
        void clear() {colorAttr.clear();}
        // Const Return operator
        const doublePointer & operator[](const int & i) const {return colorAttr[i];}
        // Return operator
        doublePointer & operator[](const int & i) {return colorAttr[i];}
};

// Example of a fragment shader
void DefaultFragShader(PIXEL & fragment, const Attributes & vertAttr, const Attributes & uniforms)
{
    PIXEL color = 0xff000000;
    color += (unsigned int) (vertAttr[0].d *0xff) << 16;
    color += (unsigned int) (vertAttr[1].d *0xff) << 8;
    color += (unsigned int) (vertAttr[2].d *0xff) << 0;
    fragment = color;
    // Output our shader color value, in this case red
    //fragment = 0xffff0000;
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
                   VertexShader* const vertIn = NULL,
                   Buffer2D<double>* zBuf = NULL);

#endif