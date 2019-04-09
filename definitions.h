#define SDL_MAIN_HANDLED
#include "SDL2/SDL.h"
#include "stdlib.h"
#include "stdio.h"
#include "math.h"
#include <iostream>

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
//#define crossProduct(A,B,C,D) (A * D) - (B * C)
#define X_KEY 0
#define Y_KEY 1

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

/****************************************************
 * The most descriptive name for the camera controls. 
 ****************************************************/
struct sixDoubles //Camera Controls
{
    double x = 0;
    double y = 0;
    double z = 0;

    double pitch = 0;
    double yaw = 0;
    double roll = 0;
};

sixDoubles myCam;

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

//Matrix class
class Matrix
{      
    public:
        // Default Constructor
        Matrix()
        {
            clear();
        }
  
        // Variables
        double matrix [4][4];

        // Functions
        // Sets the matrix to the identity matrix
        void clear()
        {
            for (int x = 0; x < 4; x++)
            {
                 for (int y = 0; y < 4; y++)
                 {
                    this->matrix[x][y] = (x == y ? 1 : 0);
                 }
            } 
        }

        // Operators
        // Allows access to the array
        const double& operator[] (const int i) const
        {
            return matrix[i / 4][i % 4];
        } 
        // Allows access to the array (non-const)
        double& operator[] (const int i) 
        {
            return matrix[i / 4][i % 4];
        }              
};

/***************************************************
 * Creates either a pointer or a double. 
 **************************************************/
inline double lerp(double a, double b, double lerpAmount)
{
   return a + (b - a) * lerpAmount;
}

/***************************************************
 * Creates either a pointer or a double. 
 **************************************************/
union attrib
{
    double d;

    void* ptr;
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
       // Members
       int numMembers = 0;
       attrib arr[16];

       // Obligatory empty constructor
       Attributes() {numMembers = 0;}

       // Interpolation Constructor
       Attributes(const double & areaTriangle, 
                  const double & firstDet, 
                  const double & secndDet, 
                  const double & thirdDet,
                  const Attributes & first, 
                  const Attributes & secnd, 
                  const Attributes & third, 
                  const double interpZ)
       {
           while(numMembers < first.numMembers)
           {
               arr[numMembers].d =  (firstDet/areaTriangle) * (third.arr[numMembers].d);
               arr[numMembers].d += (secndDet/areaTriangle) * (first.arr[numMembers].d);
               arr[numMembers].d += (thirdDet/areaTriangle) * (secnd.arr[numMembers].d);
               arr[numMembers].d *= interpZ;
               
               numMembers += 1;
           }
       }

       // Needed by clipping (linearly interpolated Attributes between two others)
       Attributes(const Attributes & first, const Attributes & second, const double & along)
       {
           numMembers = first.numMembers;
           for (int i = 0; i < numMembers; i++)
           {
              arr[i].d = lerp(first[i].d, second[i].d, along);
           }               
       }
       
       //Operators
       // Const Return operator
       const attrib & operator[](const int & i) const
       {
           return arr[i];
       }

       // Return operator
       attrib & operator[](const int & i)
       {
           return arr[i];
       }

       // Insert Double Into Container
       void insertDbl(const double & d)
       {
           arr[numMembers].d = d;
           numMembers += 1;
       }

       // Insert Pointer Into Container
       void insertPtr(void * ptr)
       {
           arr[numMembers].ptr = ptr;
           numMembers += 1;
       }
};

/***************************************************
 * Vertex Multiplication Operator
 **************************************************/
Vertex operator * (const Matrix& lhs, const Vertex& rhs)
{ 

    Vertex result = { lhs[0] * rhs.x +  lhs[1] * rhs.y +  lhs[2] * rhs.z +  lhs[3] * rhs.w,
                      lhs[4] * rhs.x +  lhs[5] * rhs.y +  lhs[6] * rhs.z +  lhs[7] * rhs.w,
                      lhs[8] * rhs.x +  lhs[9] * rhs.y + lhs[10] * rhs.z + lhs[11] * rhs.w,
                     lhs[12] * rhs.x + lhs[13] * rhs.y + lhs[14] * rhs.z + lhs[15] * rhs.w };
    return result;
}

/***************************************************
 * Matrix Multiplication Operator
 **************************************************/
Matrix operator * (const Matrix& lhs, const Matrix& rhs)
{
    Matrix result;

    // Loop through each cell
    for (int q = 0; q < 16; q++)
    {
       int row = 4 * (q / 4);
       int col = q % 4;
       
       double sum = 0.0;

       // Loop 4 times for each row and column
       for (int k = 0; k < 4; k++)
       {
           sum += (lhs[row] * rhs[col]);

           row++;
           col += 4;
       }

       result[q] = sum;
    }

    return result;
}

/***************************************************
 * Translation Matrix
 **************************************************/
Matrix translateMatrix(const double& x, const double& y, const double& z)
{
    Matrix matrix;

    matrix[3]  = x;
    matrix[7]  = y;
    matrix[11] = z;

    return matrix;
}

/***************************************************
 * Scale Matrix
 **************************************************/
Matrix scaleMatrix(Attributes& attr, Vertex v)
{
    Matrix matrix;

    matrix[0] = v.x;
    matrix[5] = v.y;
    matrix[10] = v.z;

    return matrix;
}

/***************************************************
 * AXIS Enumeration
 **************************************************/
enum AXIS 
{
    X,
    Y,
    Z
};

/***************************************************
 * Rotation Matrix
 **************************************************/
// Takes an angle in radians and the axis to rotate around
Matrix rotateMatrix(AXIS axis, const double& angle) 
{
    Matrix matrix;

    double sinangle = sin(angle);
    double cosangle = cos(angle);
    
    switch (axis) 
    {
        case X:
        matrix[5]  = cosangle;
        matrix[6]  = -sinangle;
        matrix[9]  = sinangle;
        matrix[10] = cosangle;
        break;
        
        case Y:
        matrix[0]  = cosangle;
        matrix[2]  = sinangle;
        matrix[8]  = -sinangle;
        matrix[10] = cosangle;
        break;
        
        case Z:
        matrix[0]  = cosangle;
        matrix[1]  = -sinangle;
        matrix[4]  = sinangle;
        matrix[5]  = cosangle;
        break;
    }
    return matrix;
}

// View Transform
Matrix viewTransform(const double& offX, const double& offY, const double& offZ,
                     const double& yaw, const double& pitch, const double& roll) {
    // x = pitch
    // y = yaw
    // z = roll
    Matrix matrix;

    Matrix translate = translateMatrix(-offX, -offY, -offZ);
    
    // First do pitch, then yaw (roll is optional)
    double pitchRad = pitch * M_PI / 180.0;
    double yawRad   = yaw   * M_PI / 180.0;
    
    Matrix rotX = rotateMatrix(X, pitchRad);
    Matrix rotY = rotateMatrix(Y, yawRad);

    matrix = rotX * rotY * translate;
    
    return matrix;
}

Matrix perspectiveTransform(const double& fovYDegrees, const double& aspectRatio,
                            const double& near, const double& far)
{
    Matrix matrix; // this starts off as an identity matrix

    double top = near * tan(fovYDegrees * M_PI / 180.0 / 2.0);
    double right = aspectRatio * top;
    
    matrix[0]  = near / right;
    matrix[5]  = near / top;
    matrix[10] = (far + near) / (far - near);
    matrix[11] = (-2 * far * near) / (far - near);
    matrix[14] = 1;
    matrix[15] = 0;

    return matrix;
}

/***************************************************
 * Static Fragment Shader 
 **************************************************/
void StaticFragShader(PIXEL & fragment, const Attributes & vertAttr, const Attributes & uniforms)
{
    // Output our shader color value, in this case red
    PIXEL color = 0xff000000;
    color += (unsigned int)(rand() *0xff) << 16;
    color += (unsigned int)(rand() *0xff) << 8;
    color += (unsigned int)(rand() *0xff) << 0;

    fragment = color;
}

/***************************************************
 * Simple Vertex Shader 
 **************************************************/
void SimpleVertexShader(Vertex & vertOut, Attributes & attrOut, const Vertex & vertIn, const Attributes & attrIn, const Attributes & uniforms)
{
   Matrix* trans = (Matrix*)uniforms[0].ptr;
   vertOut = (*trans) * vertIn;

   // Pass through attributes
   attrOut = attrIn;
}

/***************************************************
 * Another Simple Vertex Shader 
 **************************************************/
void SimpleVertexShader2(Vertex & vertOut, Attributes & attrOut, const Vertex & vertIn, const Attributes & attrIn, const Attributes & uniforms)
{
   Matrix* model = (Matrix*)uniforms[1].ptr;
   Matrix* view = (Matrix*)uniforms[2].ptr;
   Matrix* proj = (Matrix*)uniforms[3].ptr;

   vertOut = (*proj) * (*view) * (*model) * vertIn;

   // Pass through attributes
   attrOut = attrIn;
}

/***************************************************
 * Image Fragment Shader 
 **************************************************/ 
void ImageFragShader(PIXEL & fragment, const Attributes & vertAttr, const Attributes & uniforms)
{
    /*
    // Figure out which X/Y square our UV would fall on
    int xSquare = vertAttr[0].d * 8;
    int ySquare = vertAttr[1].d * 8;
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
    */
    BufferImage* bf = (BufferImage*)uniforms[0].ptr;
    int x = vertAttr[0].d * (bf->width()-1);
    int y = vertAttr[1].d * (bf->height()-1);

    fragment = (*bf)[y][x];
}

/***************************************************
 * My Fragment Shader for color interpolation
 **************************************************/ 
void ColorFragShader(PIXEL & fragment, const Attributes & vertAttr, const Attributes & uniforms)
{
    // Output our shader color value, in this case red
    PIXEL color = 0xff000000;
    color += (unsigned int)(vertAttr[0].d *0xff) << 16;
    color += (unsigned int)(vertAttr[1].d *0xff) << 8;
    color += (unsigned int)(vertAttr[2].d *0xff) << 0;
    
    fragment = color;
}

/***************************************************
 * Example of a  default fragment shader
 **************************************************/
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

/***************************************************
 * Example of a  default fragment shader
 **************************************************/
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

/****************************************
 * DETERMINANT
 * Find the determinant of a Matrix with
 * components A, B, C, D from 2 vectors.
 ***************************************/
inline double determinant(const double & A, const double & B, const double & C, const double & D)
{
  return (A*D - B*C);
}        

/****************************************
 * INTERPOLATION
 * 
 ***************************************/
 double interp(double areaTriangle, double firstDet, double secndDet, double thirdDet, double attr0, double attr1, double attr2)
 {
    return (attr2 * firstDet + attr0 * secndDet + attr1 * thirdDet) / areaTriangle;    
 }

 /***************************************************
 * THIS IS THE GREATEST FUNCTION EVER
 **************************************************//*
int getTheSinOfANumber(int num)
{
    return 0;
}
*/

#endif