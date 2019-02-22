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
 * Calculate the area of a triangle with X and Y
 * coordinates by using two points on a graph.
 * This equation is similar to the one we discused
 * in class on finding the area of a parallelogram.
 * ****************************************************/
double calcArea(const double & A, const double & B, const double & C, const double & D)
{
    return (A * D - B * C);
}

/******************************************************
 * Interpolation Function
 ******************************************************/
double interpolate(double area, double firstDet, double secondDet, double thirdDet, double A, double B, double C)
{
    double w1 = C * (firstDet / area);
    double w2 = A * (secondDet / area);
    double w3 = B * (thirdDet / area);

    return w1 + w2 + w3;
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

        // Needed by clipping (linearly interpolated Attributes between two others)
        Attributes(const Attributes & first, const Attributes & second, const double & valueBetween)
        {
            // Your code goes here when clipping is implemented
        }

        /*Vertex Attributes*/
        //Color
        PIXEL color;
        double r;
        double g;
        double b;
        //Texture Coordinates
        double u;
        double v;
        //Image Reference
        void* ptrImage;
        //Other Attributes
        double other[16];
};	

/******************************************************
 * MATRIX
 ******************************************************/
class Matrix
{
    public:

        //Setup the matrix and a single vertex
        Matrix() 
        {
            //Matrix of 4x4
            for (int i = 0; i < 4; i++)
            {
                for (int j = 0; j < 4; j++)
                {
                    matrix[i][j] = 0;
                }
            }
            matrix[0][0] = 1;
            matrix[1][1] = 1;
            matrix[2][2] = 1;
            matrix[3][3] = 1;

            //Single Vertex   
            result.x = 0;
            result.y = 0;
            result.z = 0;
            result.w = 0;
        }

        //Setup the matrix and a single vertex and call the
        //transformation required
        Matrix(const Vertex & vertIn, const Attributes & uniforms)
        {
            //Matrix of 4x4
            for (int i = 0; i < 4; i++)
            {
                for (int j = 0; j < 4; j++)
                {
                    matrix[i][j] = 0;
                }
            }
            matrix[0][0] = 1;
            matrix[1][1] = 1;
            matrix[2][2] = 1;
            matrix[3][3] = 1;

            //Single vertex
            result.x = 0;
            result.y = 0;
            result.z = 0;
            result.w = 0;

            //Based on the first number in the uniforms
            //the wanted transformation is performed
            switch((int)uniforms.other[0])
            {
                case 0:
                translate(vertIn, uniforms);
                break;
                case 1:
                scale(vertIn, uniforms);
                break;
                case 2:
                rotate(vertIn, uniforms);
                break;
                default:
                STR(vertIn, uniforms);
            }
        }

        //Translate the vertex by the uniform using matrix multiplication
        void translate(const Vertex & vertIn, const Attributes & uniforms)
        {
            double sum[4] = {0, 0, 0, 0};
            double vert[4] = {vertIn.x, vertIn.y, vertIn.z, vertIn.w};

            //Setup the matrix
            matrix[0][2] = uniforms.other[1];
            matrix[1][2] = uniforms.other[2];

            //Multiply and add the result
            for(int i = 0; i < 4; i++)
            {
                for (int j = 0; j < 4; j++)
                {
                    sum[i] += matrix[i][j] * vert[j];
                }
            }

            //Save the result
            result.x = sum[0];
            result.y = sum[1];
            result.z = sum[2];
            result.w = sum[3];
        }

        //Rotate the vertex by the uniform using matrix multiplication
        void rotate(const Vertex & vertIn, const Attributes & uniforms)
        {
            //Convert degrees to radians
            double angle = (uniforms.other[1] * M_PI) / 180;
            double sum[4] = {0, 0, 0, 0};
            double vert[4] = {vertIn.x, vertIn.y, vertIn.z, vertIn.w};

            //Setup the matrix
            matrix[0][0] = cos(angle);
            matrix[0][1] = -sin(angle);
            matrix[1][0] = sin(angle);
            matrix[1][1] = cos(angle);

            //Multiply and add the result
            for(int i = 0; i < 4; i++)
            {
                for (int j = 0; j < 4; j++)
                {
                    sum[i] += matrix[i][j] * vert[j];
                }
            }

            //Save the result
            result.x = sum[0];
            result.y = sum[1];
            result.z = sum[2];
            result.w = sum[3];
        }

        //Scale the vertex by the uniform using matrix multiplication
        void scale(const Vertex & vertIn, const Attributes & uniforms)
        {
            double sum[4] = {0, 0, 0, 0};
            double vert[4] = {vertIn.x, vertIn.y, vertIn.z, vertIn.w};

            //Setup the matrix
            matrix[0][0] = uniforms.other[1];
            matrix[1][1] = uniforms.other[1];

            //Multiply and add the result
            for(int i = 0; i < 4; i++)
            {
                for (int j = 0; j < 4; j++)
                {
                    sum[i] += matrix[i][j] * vert[j];
                }
            }

            //Save the result
            result.x = sum[0];
            result.y = sum[1];
            result.z = sum[2];
            result.w = sum[3];
        }

        void STR(const Vertex & vertIn, const Attributes & uniforms)
        {
            //Convert degrees to radians
            double angle = (uniforms.other[4] * M_PI) / 180;

            //Holds the translation results
            double transSum[4] = {0, 0, 0, 0};

            //Holds the rotation results
            double rotateSum[4] = {0, 0, 0, 0};

            //Holds the scaled results
            double scaledSum[4] = {0, 0, 0, 0};

            //Converts the vertex into an array
            double vert[4] = {vertIn.x, vertIn.y, vertIn.z, vertIn.w};

            //Setup the matrix
            matrix[0][0] = uniforms.other[1];
            matrix[1][1] = uniforms.other[1];

            //Multiply and add the result
            for(int i = 0; i < 4; i++)
            {
                for (int j = 0; j < 4; j++)
                {
                    scaledSum[i] += matrix[i][j] * vert[j];
                }
            }
            
            //Setup the matrix
            resetMatrix();
            matrix[0][2] = uniforms.other[2];
            matrix[1][2] = uniforms.other[3];

            //Multiply and add the result
            for(int i = 0; i < 4; i++)
            {
                for (int j = 0; j < 4; j++)
                {
                    transSum[i] += matrix[i][j] * scaledSum[j];
                }
            }

            //Setup the matrix
            resetMatrix();
            matrix[0][0] = cos(angle);
            matrix[0][1] = -sin(angle);
            matrix[1][0] = sin(angle);
            matrix[1][1] = cos(angle);

            //Multiply and add the result
            for(int i = 0; i < 4; i++)
            {
                for (int j = 0; j < 4; j++)
                {
                    rotateSum[i] += matrix[i][j] * transSum[j];
                }
            }

            //Save the result
            result.x = rotateSum[0];
            result.y = rotateSum[1];
            result.z = rotateSum[2];
            result.w = rotateSum[3];
        }

        Vertex getResult()
        {
            return result;
        }

        void resetMatrix()
        {
            //Matrix of 4x4
            for (int i = 0; i < 4; i++)
            {
                for (int j = 0; j < 4; j++)
                {
                    matrix[i][j] = 0;
                }
            }
            matrix[0][0] = 1;
            matrix[1][1] = 1;
            matrix[2][2] = 1;
            matrix[3][3] = 1;
        }

    private:
        double matrix[4][4];
        Vertex result;
};

/******************************************************
 * Fragment Shader List
 * ****************************************************/
// Example of a fragment shader
void DefaultFragShader(PIXEL & fragment, const Attributes & vertAttr, const Attributes & uniforms)
{
    // Output our shader color value, in this case red
    fragment = 0xffff0000;
}

// This Shader takes three colors and assigns them to each corner of a triangle
// It then uses linear interpolation to fill the triangle
void gradiantColorShader(PIXEL & fragment, const Attributes & vertAttr, const Attributes & uniforms)
{
    PIXEL color = 0xff000000;
    color += (unsigned int)(vertAttr.r * 0xff) << 16;
    color += (unsigned int)(vertAttr.g * 0xff) << 8;
    color += (unsigned int)(vertAttr.b * 0xff) << 0;

    fragment = color;
}

//This shader draws a trianlge with a given image
void imageFragShader(PIXEL & fragment, const Attributes & vertAttr, const Attributes & uniforms)
{
    BufferImage* bf = (BufferImage*)uniforms.ptrImage;
    int x = vertAttr.u * (bf->width()-1);
    int y = vertAttr.v * (bf->height()-1);

    fragment = (*bf)[y][x];
}

//This shader draws a picture in green
void limeFragShader(PIXEL & fragment, const Attributes & vertAttr, const Attributes & uniforms)
{
    BufferImage* bf = (BufferImage*)uniforms.ptrImage;
    int x = vertAttr.u * (bf->width()-1);
    int y = vertAttr.v * (bf->height()-1);

    PIXEL color = 0xff000000;
    color += (unsigned int)(x * 0xff) << 8;
    color += (unsigned int)(y * 0xff) << 8;

    fragment = color;
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

// Translation Shader
void transVertShader(Vertex & vertOut, Attributes & attrOut, const Vertex & vertIn, const Attributes & vertAttr, const Attributes & uniforms)
{
    Matrix matrix(vertIn, uniforms);
    vertOut = matrix.getResult();
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
