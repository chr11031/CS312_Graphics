#define SDL_MAIN_HANDLED
#include "SDL2/SDL.h"
#include "stdlib.h"
#include "stdio.h"
#include "math.h"
#include <vector>

#include <iostream>

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

// My definitions
#define DEG_TO_RAD M_PI/180

/******************************************************
 * Types of primitives our pipeline will render.
 *****************************************************/
enum PRIMITIVES 
{
    TRIANGLE,
    LINE,
    POINT
};

/***************************************************
 * MATRIX
 * Holds a matrix and functions to manipulate it
 **************************************************/
class Matrix
{

public:
    double cell[4][4];

    Matrix() {
        for (int i = 0; i < 4; i++)
            for (int j = 0; j < 4; j++)
            {
                if (i == j)
                    cell[i][j] = 1;
                else
                    cell[i][j] = 0;
            }
    }

    // *= operator overload for another 4x4 matrix
    Matrix operator *= (const Matrix rhs) {
        Matrix temp;
        temp.clear();
        for (int i = 0; i < 4; i++)
            for (int j = 0; j < 4; j++)
                for (int k = 0; k < 4; k++)
                {
                    temp.cell[i][j] += this->cell[i][k] * rhs.cell[k][j];
                }
        for (int i = 0; i < 4; i++)
            for (int j = 0; j < 4; j++)
            {
                this->cell[i][j]  = temp.cell[i][j];
            }
        // I think I have to return *this... but if I didn't have to, couldn't I get rid of the last for loop?
        return *this;
    }

    // assignment operator overload for another 4x4 matrix
    Matrix& operator = (const Matrix rhs) {
        for (int i = 0; i < 4; i++)
            for (int j = 0; j < 4; j++)
            {
                this->cell[i][j] = rhs.cell[i][j];
            }
        return *this;
    }

    // translates the triangle on the x, y, and/or z axis
    void translate(double x, double y, double z)
    {
        Matrix temp;
        temp.cell[0][3] += x;
        temp.cell[1][3] += y;
        temp.cell[2][3] += z;

        *this *= temp;
    }

    // scales the triangle in the x, y, and/or z direction
    void scale(double x, double y, double z)
    {
        Matrix temp;
        temp.cell[0][0] = x;
        temp.cell[1][1] = y;
        temp.cell[2][2] = z;

        *this *= temp;
    }

    // rotates the vertices around a user-selected axis
    void rotate(double angle, const char axis)
    {
        angle *= DEG_TO_RAD;
        Matrix temp;
        switch (axis)
        {
            case 'x':
            temp.cell[1][1] = cos(angle);
            temp.cell[1][2] = -sin(angle);
            temp.cell[2][1] = sin(angle);
            temp.cell[2][2] = cos(angle);   
            break;
            case 'y':
            temp.cell[0][0] = cos(angle);
            temp.cell[0][2] = sin(angle);
            temp.cell[2][0] = -sin(angle);
            temp.cell[2][2] = cos(angle);
            break;
            case 'z':
            temp.cell[0][0] = cos(angle);
            temp.cell[0][1] = -sin(angle);
            temp.cell[1][0] = sin(angle);
            temp.cell[1][1] = cos(angle);
            break;
        }

        *this *= temp;
    }

    // sets the matrix identity back to all 1s
    void clear()
    {
        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                cell[i][j] = 0;
            }
        }
    }

    // sets the matrix identity back to all 1s
    void reset()
    {
        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                    if (i == j)
                            cell[i][j] = 1;
                    else
                            cell[i][j] = 0;
            }
        }
    }
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

    // the only time I need to multiply a 4x4 by a 4x1 is with a vertex, so I have overloaded the vertex *= operator
    Vertex& operator *= (const Matrix &rhs);

    // in case the user wants to use this version instead
    Vertex& operator * (const Matrix &rhs) {
        *this *= rhs;
        return *this;
    }

};

// the full operator function
Vertex& Vertex::operator *= (const Matrix &rhs) {
    double copy[4] = {x, y, z, w};
    double temp[4] = {0, 0, 0, 0};

    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
        {
            temp[i] += rhs.cell[i][j] * copy[j];
        }
    this->x = temp[0];
    this->y = temp[1];
    this->z = temp[2];
    this->w = temp[3];

    std::cout << std::endl;

    return *this;
}

/******************************************************
 * BUFFER_2D:
 * Used for 2D buffers including render targets, images
 * and depth buffers. Can be described as frames or 
 * 2D arrays of type 'T' encapsulated in an object.
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

        // Obligatory empty constructor
        Attributes() {}

        // Needed by clipping (linearly interpolated Attributes between two others)
        Attributes(const Attributes & first, const Attributes & second, const double & valueBetween)
        {
            // Your code goes here when clipping is implemented
        }

        /// My Code
        PIXEL color;
        
        void * ptrImg;  // points to the .bmp

        std::vector<double> values; // vector to allow for as many as the user would like
        // std::vector<void*> ptrImgs; // vector to allow for as many textures as the user would like

        Matrix matrix;

        // easier to add another value to the vector
        void add(const double value) { values.push_back(value); }

        // clears the vector for future use
        void reset() { values.clear(); }
};

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
