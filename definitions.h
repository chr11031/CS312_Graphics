#define SDL_MAIN_HANDLED
#include "/usr/local/include/SDL2/SDL.h"
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
            return;
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
            // if having problems loading image uncomment this line
            //SDL_LockSurface(img);
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

        // Needed by clipping (linearly interpolated Attributes between two others)
        Attributes(const Attributes & first, const Attributes & second, const double & valueBetween)
        {
            // Your code goes here when clipping is implemented
        }
        PIXEL color;
        double attr[16]; // to be more dynamic, just use an array of attributes that would be of type
                         // double:
                         // color rgb, UV coordinates, normal vectors, fog coordinates, ECT.
        void* ptrImg;    // for the imgBuffer attribute, but may be re-purposed
        //Matrix matrix;
        void addAttributes(const double first, const double second = 0,
                           const double third = 0, const double fourth = 0,
                           const double fifth = 0, const double sixth = 0,
                           const double seventh = 0, const double eighth = 0,
                           const double ninth = 0, const double tenth = 0,
                           const double eleventh = 0, const double twelfth = 0,
                           const double thirteenth = 0, const double fourteenth = 0,
                           const double fifteenth = 0, const double sixteenth = 0)

        {
            attr[0] = first;
            attr[1] = second;
            attr[2] = third;
            attr[3] = fourth;
            attr[4] = fifth;
            attr[5] = sixth;
            attr[6] = seventh;
            attr[7] = eighth;
            attr[8] = ninth;
            attr[9] = tenth;
            attr[10] = eleventh;
            attr[11] = twelfth;
            attr[12] = thirteenth;
            attr[13] = fourteenth;
            attr[14] = fifteenth;
            attr[15] = sixteenth;                      
        }



};	

// Example of a fragment shader
void DefaultFragShader(PIXEL & fragment, const Attributes & vertAttr, const Attributes & uniforms)
{
    // Output our shader color value, in this case red
    fragment = 0xffff0000;
}

void ImageFragShader(PIXEL & fragment, const Attributes & vertAttr, const Attributes & uniforms)
{
   // assign buffer pointer to the buffer image
   BufferImage* bfPtr = (BufferImage*)uniforms.ptrImg;
   // assign x to attr[0] (U coordinate * (image width -1))
   int x = vertAttr.attr[0] * (bfPtr->width() - 1);
   // assign y to attr[0] (U coordinate * (image width -1))
   int y = vertAttr.attr[1] * (bfPtr->height() - 1);

   // this is the new fragment after casting the image coordinates onto the triangle
   fragment = (*bfPtr)[y][x];
}

void ColorFragShader(PIXEL & fragment, const Attributes & vertAttr, const Attributes & uniforms)
{
   // set the rgb color attributes by creating a hex color through shifts
   // shift 16 for red, 8 for green, and none for blue
   // attr[0] = red; attr[1] = green; attr[2] = blue
    PIXEL color = 0xff000000;
    color += (unsigned int) (vertAttr.attr[0] *0xff) << 16;
    color += (unsigned int) (vertAttr.attr[1] *0xff) << 8;
    color += (unsigned int) (vertAttr.attr[2] *0xff) << 0;

    fragment = color;
}

// Helper function fo do matrix multiplication
Vertex operator* (const Vertex& lhs,const Vertex * rhs)
{
    Vertex newVert;
    newVert.x = ((lhs).x * (*rhs).x)
    + ((lhs).y * (*rhs).y)
    + ((lhs).z * (*rhs).z)
    + ((lhs).w * (*rhs).w);

    newVert.y = ((lhs).x * (*(rhs + 1)).x)
    + ((lhs).y * (*(rhs + 1)).y)
    + ((lhs).z * (*(rhs + 1)).z)
    + ((lhs).w * (*(rhs + 1)).w);

    newVert.z = ((lhs).x * (*(rhs + 2)).x)
    + ((lhs).y * (*(rhs + 2)).y)
    + ((lhs).z * (*(rhs + 2)).z)
    + ((lhs).w * (*(rhs + 2)).w);

    newVert.w = ((lhs).x * (*(rhs + 3)).x)
    + ((lhs).y * (*(rhs + 3)).y)
    + ((lhs).z * (*(rhs + 3)).z)
    + ((lhs).w * (*(rhs + 3)).w);

    return newVert;
}

/*******************************************************
 * MATRIX
 * Makes use of transformation matrices to transform
 * each vertice provided
 ******************************************************/
class Matrix
{
    public:
    Vertex rotateX(Vertex vert,double angle) 
    {
        Vertex transformMatrix[4] = {  {1, 0, 0, 0},
                                       {0, cos(angle), -sin(angle), 0},
                                       {0, sin(angle), cos(angle), 0},
                                       {0, 0, 0, 1} };
        Vertex * matrix = transformMatrix;
        vert = vert * matrix;
        return vert;
    }

    Vertex rotateY(Vertex vert,double angle)
    {
        Vertex transformMatrix[4] = {  {cos(angle), 0, sin(angle), 0},
                                       {0, 1, 0, 0},
                                       {-sin(angle), 0, cos(angle), 0},
                                       {0, 0, 0, 1} };
        Vertex * matrix = transformMatrix;
        vert = vert * matrix;
        return vert;
    }

    Vertex rotateZ(Vertex vert,double angle)
    {
        Vertex transformMatrix[4] = {  {cos(angle), -sin(angle), 0, 0},
                                       {sin(angle), cos(angle), 0, 0},
                                       {0, 0, 1, 0},
                                       {0, 0, 0, 1} };
        Vertex * matrix = transformMatrix;
        vert = vert * matrix;
        return vert;
    }

    Vertex translate(Vertex vert,double xShift, double yShift, double zShift)
    {
        Vertex transformMatrix[4] = {  {1, 0, 0, xShift},
                                       {0, 1, 0, yShift},
                                       {0, 0, 1, zShift},
                                       {0, 0, 0, 1} };
        Vertex * matrix = transformMatrix;
        vert = vert * matrix;
        return vert;
    }

    Vertex scale(Vertex vert, double xScale, double yScale, double zScale)
    {
        Vertex transformMatrix[4] = {  {xScale, 0, 0, 0},
                                       {0, yScale, 0, 0},
                                       {0, 0, zScale, 0},
                                       {0, 0, 0, 1} };
        Vertex * matrix = transformMatrix;
        vert = vert * matrix;
        return vert;
    }
};


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

void TransformationVertShader(Vertex & vertOut, Attributes & attrOut, const Vertex & vertIn, const Attributes & vertAttr, const Attributes & uniforms)
{
            // Scale
            vertOut = vertIn;
            if ((uniforms).attr[4] != 0)
                vertOut = Matrix().scale(vertOut, 
                (uniforms).attr[4], (uniforms).attr[5], (uniforms).attr[6]);
            // Translate
            if ((uniforms).attr[0] != 0)
                vertOut = Matrix().translate(vertOut, 
                (uniforms).attr[0], (uniforms).attr[1], (uniforms).attr[2]);  
            // Rotate         
            if ((uniforms).attr[3] != 0)
                vertOut = Matrix().rotateZ(vertOut, 
                (uniforms).attr[3]);
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
