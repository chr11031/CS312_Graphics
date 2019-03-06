#define SDL_MAIN_HANDLED
#include "C:/MinGW/include/SDL2/SDL.h"
#include "stdlib.h"
#include "stdio.h"
#include "math.h"
#include "shaders.h"

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

// Getting size of any array
#define ARRAY_SIZE(array) (sizeof((array))/sizeof((array[0])))

// Max # of vertices after clipping
#define MAX_VERTICES 8 

// TransformationMatrix Types
#define ROTATE 0
#define SCALE 1
#define TRANSLATE 2

/******************************************************
 * Camera Struct
 *****************************************************/
struct CameraControls {
    double x = 0;
    double y = 0;
    double z = 0;
    double yaw = 0;
    double roll = 0;
    double pitch = 0;
};

CameraControls myCam;

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


/**************************************************************
 * INTERP
 * Interpolates the attributes.
 *************************************************************/
double interp(double area, double det1, double det2, double det3, double attr1, double attr2, double attr3) 
{
    // Calculate the weights of each point (percentage)
    double weight1 = det1 / area;
    double weight2 = det2 / area;
    double weight3 = 1 - weight1 - weight2;
    
    // Apply the weights and return the final value
    return ((weight2 * attr1) + (weight3 * attr2) + (weight1 * attr3));
}

/****************************************************
 * A matrix used for 3D transformations
 ****************************************************/
class TransformationMatrix 
{
    private:
        double base[4][4];

        // Multiply a 1x4 and 4x1 (for easier matrix multiplication) choosing which rown and column to multiply
        double multiplyRowColumn(const double row[4], const double column[4][4], const int columnNum) const
        {
            switch (columnNum)
            {
                case 0:
                    return row[0] * column[0][0]
                         + row[1] * column[1][0]
                         + row[2] * column[2][0]
                         + row[3] * column[3][0];
                    break;
                case 1:
                    return row[0] * column[0][1]
                         + row[1] * column[1][1]
                         + row[2] * column[2][1]
                         + row[3] * column[3][1];
                    break;
                case 2:
                    return row[0] * column[0][2]
                         + row[1] * column[1][2]
                         + row[2] * column[2][2]
                         + row[3] * column[3][2];
                    break;
                case 3:
                    return row[0] * column[0][3]
                         + row[1] * column[1][3]
                         + row[2] * column[2][3]
                         + row[3] * column[3][3];
                    break;
                default:
                    return row[0] * column[0][0]
                         + row[1] * column[1][0]
                         + row[2] * column[2][0]
                         + row[3] * column[3][0];
                    break;
            }
        }

        // Multiply a row by a Vertex onject (for easier 4x4 times 4x1)
        double multiplyRowVertex(const double row[4], const Vertex vert) const
        {
            return row[0] * vert.x
                 + row[1] * vert.y
                 + row[2] * vert.z
                 + row[3] * vert.w;
        }

        // Helper for making a rotation matrix (Roatates X, Y, then Z)
        void createRotationMatrix(double x, double y, double z)
        {
            TransformationMatrix xMat;
            TransformationMatrix yMat;
            TransformationMatrix zMat;

            // Rotation about X axis
            (xMat).base[0][0] = 1;
            (xMat).base[1][1] = cos(x * M_PI / 180.0);
            (xMat).base[1][2] = -sin(x * M_PI / 180.0);
            (xMat).base[2][1] = sin(x * M_PI / 180.0);
            (xMat).base[2][2] = cos(x * M_PI / 180.0);

            // Rotation about Y axis
            (yMat).base[0][0] = cos(y * M_PI / 180.0);
            (yMat).base[0][2] = sin(y * M_PI / 180.0);
            (yMat).base[1][1] = 1;
            (yMat).base[2][0] = -sin(y * M_PI / 180.0);
            (yMat).base[2][2] = cos(y * M_PI / 180.0);

            // Rotation about Z axis
            (zMat).base[0][0] = cos(z * M_PI / 180.0);
            (zMat).base[0][1] = -sin(z * M_PI / 180.0);
            (zMat).base[1][0] = sin(z * M_PI / 180.0);
            (zMat).base[1][1] = cos(z * M_PI / 180.0);
            (zMat).base[2][2] = 1;

            // Yaw -y Pitch -x Roll -z
            // Pitch, then Yaw, then Roll
            *this = (xMat) * (yMat) * (zMat);
        }

    public:
        //Default constuctor makes an Identity TransformationMatrix
        TransformationMatrix()
        {
            // Base TransformationMatrix (4x4 Identity matrix)
            base[0][0] = 1;
            base[0][1] = 0;
            base[0][2] = 0;
            base[0][3] = 0;
            
            base[1][0] = 0;
            base[1][1] = 1;
            base[1][2] = 0;
            base[1][3] = 0;
            
            base[2][0] = 0;
            base[2][1] = 0;
            base[2][2] = 1;
            base[2][3] = 0;
            
            base[3][0] = 0;
            base[3][1] = 0;
            base[3][2] = 0;
            base[3][3] = 1;
        }

        // Constructor type:translate,rotate or scale and x,y,z values (degrees for rotation)
        TransformationMatrix(const int type, const double x, const double y, const double z)
        {
            // Base TransformationMatrix (4x4 Identity Matrix)
            base[0][0] = 1;
            base[0][1] = 0;
            base[0][2] = 0;
            base[0][3] = 0;
            
            base[1][0] = 0;
            base[1][1] = 1;
            base[1][2] = 0;
            base[1][3] = 0;
            
            base[2][0] = 0;
            base[2][1] = 0;
            base[2][2] = 1;
            base[2][3] = 0;
            
            base[3][0] = 0;
            base[3][1] = 0;
            base[3][2] = 0;
            base[3][3] = 1;

            // Create a matrix based on the type given
            switch (type)
            {
                case ROTATE:
                    createRotationMatrix(x, y, z);
                    break;
            
                case TRANSLATE:
                    base[0][3] = x;
                    base[1][3] = y;
                    base[2][3] = z;
                    break;
            
                case SCALE:
                    base[0][0] = x;
                    base[1][1] = y;
                    base[2][2] = z;
                    break;
            
                default:
                    break;
            }
        }

        // Multiply two 4X4 matrices
        TransformationMatrix operator*(const TransformationMatrix& multiplier) const
        {
            TransformationMatrix temp;
            // First Row
            temp.base[0][0] = multiplyRowColumn(base[0], multiplier.base, 0);
            temp.base[0][1] = multiplyRowColumn(base[0], multiplier.base, 1);
            temp.base[0][2] = multiplyRowColumn(base[0], multiplier.base, 2);
            temp.base[0][3] = multiplyRowColumn(base[0], multiplier.base, 3);
            // Second Row
            temp.base[1][0] = multiplyRowColumn(base[1], multiplier.base, 0);
            temp.base[1][1] = multiplyRowColumn(base[1], multiplier.base, 1);
            temp.base[1][2] = multiplyRowColumn(base[1], multiplier.base, 2);
            temp.base[1][3] = multiplyRowColumn(base[1], multiplier.base, 3);
            // Third Row
            temp.base[2][0] = multiplyRowColumn(base[2], multiplier.base, 0);
            temp.base[2][1] = multiplyRowColumn(base[2], multiplier.base, 1);
            temp.base[2][2] = multiplyRowColumn(base[2], multiplier.base, 2);
            temp.base[2][3] = multiplyRowColumn(base[2], multiplier.base, 3);
            // Fourth Row
            temp.base[3][0] = multiplyRowColumn(base[3], multiplier.base, 0);
            temp.base[3][1] = multiplyRowColumn(base[3], multiplier.base, 1);
            temp.base[3][2] = multiplyRowColumn(base[3], multiplier.base, 2);
            temp.base[3][3] = multiplyRowColumn(base[3], multiplier.base, 3);
            return temp;
        }

        // Multiply a 4x4 by a 4x1
        Vertex multiplyByVertex(const Vertex& vert) const 
        {
            Vertex temp;
            temp.x = multiplyRowVertex(base[0], vert);
            temp.y = multiplyRowVertex(base[1], vert);
            temp.z = multiplyRowVertex(base[2], vert);
            temp.w = multiplyRowVertex(base[3], vert);
            return temp;
        }

        void toPerspectiveMatrix(const double &fovYDeg, const double &aspectRatio,
                        const double &near, const double &far)
        {
            double top = near * tan((fovYDeg * M_PI) / 180.0 / 2.0);
            double right = aspectRatio * top;

            base[0][0] = near/right;
            base[0][1] = 0;
            base[0][2] = 0;
            base[0][3] = 0;
            
            base[1][0] = 0;
            base[1][1] = near/top;
            base[1][2] = 0;
            base[1][3] = 0;
            
            base[2][0] = 0;
            base[2][1] = 0;
            base[2][2] = (far + near) / (far - near);
            base[2][3] = (-2 * far * near) / (far - near);
            
            base[3][0] = 0;
            base[3][1] = 0;
            base[3][2] = 1;
            base[3][3] = 0;
            }


        void toCameraMatrix(const double &offx, const double &offy, const double &offz,
                            const double &yaw, const double &pitch, const double &roll)
        {
            TransformationMatrix rt;
            TransformationMatrix trans(TRANSLATE, -offx, -offy, -offz);
            TransformationMatrix rotation(ROTATE, -pitch, -yaw, -roll);

            rt = rotation * trans;
            *this = rt;
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

// Union of two types of attributes used in Attributes
union attribute
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
        int numMembers = 0;
        attribute attr[16];

        // Obligatory empty constructor
        Attributes() { numMembers = 0; }

        // Needed by clipping (linearly interpolated Attributes between two others)
        Attributes(const Attributes & first, const Attributes & second, const double & along)
        {
            numMembers = first.numMembers;
            for(int i = 0; i < numMembers; i++)
            {
                attr[i].d = (first[i].d) + ((second[i].d - first[i].d) * along);
            }
        }

        // interpolate all doubles
        Attributes(const double & firstDet, const double & secondDet, const double & thirdDet, 
                   const Attributes & first, const Attributes & secnd, const Attributes & third,
                   const double & correctZ, const double& area)
        {
            numMembers = 0;
            while(numMembers < first.numMembers)
            {
                attr[numMembers].d = interp(area, firstDet, secondDet, thirdDet, first[numMembers].d, secnd[numMembers].d, third[numMembers].d);
                attr[numMembers].d = attr[numMembers].d * correctZ;
                numMembers += 1;
            }
        }

        // Const return operator
        const attribute & operator[](const int & i) const
        {
            return attr[i];
        }

        // Return operator
        attribute & operator[](const int & i) 
        {
            return attr[i];
        }

        // Add Double
        void addDouble(const double & d)
        {
            attr[numMembers].d = d;
            numMembers += 1;
        }
    
        // Add Pointer
        void addPtr(void * ptr)
        {
            attr[numMembers].ptr = ptr;
            numMembers += 1;
        }

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
