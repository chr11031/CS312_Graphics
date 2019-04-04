#define SDL_MAIN_HANDLED
#include "/Users/ronaldmunoz/Documents/College/Winter 2019/CS 312 - Computer Graphics/SDL2-2.0.9/include/SDL.h"
#include "stdlib.h"
#include "stdio.h"
#include "math.h"
//#include "matrix.h"
#include <vector>
#include <iostream>

#define PI 3.14159265

using namespace std;

#ifndef DEFINITIONS_H
#define DEFINITIONS_H
#define MAX_DIM_SIZE_MATRIX 4

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

/****************************************************
 * X, Y, Z, handy enums
 ***************************************************/
enum DIMENSION
{
    X = 0,
    Y = 1,
    Z = 2
};

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
 * Everything needed for the view/camera transform
 ****************************************************/
struct camControls
{
	double x = 0;
	double y = 0;
	double z = 0;
	double yaw = 0;
	double roll = 0;
	double pitch = 0;
};
camControls myCam;


/******************************************************
 * Matrix class for world geometry, viewing.
 *****************************************************/
class Matrix
{
// 2D Array
private:
double mat[MAX_DIM_SIZE_MATRIX][MAX_DIM_SIZE_MATRIX];
// Update Matrix values by array
void copyValues(const Matrix & source)
    {
if(rowLen != source.rowLen || colLen != source.colLen) return;
for(int r = 0; r < rowLen; r++)
        {
for(int c = 0; c < colLen; c++)
            {
                mat[r][c] = (source.get(r))[c];
            }
        }
    }
// Update Matrix values by array
void setValues(double values[])
    {
int i = 0;
for(int r = 0; r < rowLen; r++)
        {
for(int c = 0; c < colLen; c++)
            {
                mat[r][c] = values[i++];
            }
        }
    }
// Matrix Dimensions
public:
int colLen;
int rowLen;
// Initialize size, setup identity matrix or set values to zero
Matrix(int width = MAX_DIM_SIZE_MATRIX, int height = MAX_DIM_SIZE_MATRIX)
    {
        colLen = width;
        rowLen = height;
setIdentity();
    }
// Initialize a vertex as a matrix
Matrix(const Vertex & vert)
    {
        rowLen = 4;
        colLen = 1;
        mat[0][0] = vert.x;
        mat[1][0] = vert.y;
        mat[2][0] = vert.z;
        mat[3][0] = vert.w;
    }
// Initialize matrix to suggested values
Matrix(double values[], int width = MAX_DIM_SIZE_MATRIX, int height = MAX_DIM_SIZE_MATRIX)
    {
        colLen = width;
        rowLen = height;
setValues(values);
    }
// Assignment operator 
    Matrix& operator = (const Matrix & right)
    {
        colLen = right.colLen;
        rowLen = right.rowLen;
copyValues(right);
    }
// Dereference Matrix in 'mat[r][c]' format
double* operator [](int i) { return (double*)mat[i]; }
// Const version of '[]'
const double* get(int i) const { return (double*)mat[i]; }
// Is it?
bool isQuare()
    {
return colLen == rowLen;
    }
// Set all entries to zero
void setZero()
    {
for(int r = 0; r < rowLen; r++)
        {
for(int c = 0; c < colLen; c++)
            {
                mat[r][c] = 0.0f;
            }
        }
    }
// Setup the identity matrix if square
bool setIdentity()
    {
setZero();
if(!isQuare()) return false;
for(int d = 0; d < colLen; d++)
        {
            mat[d][d] = 1.0f;
        }
    }
// Multiply two matrices together, return the result
    Matrix operator *(const Matrix & right) const
    {
        Matrix tr(right.rowLen, this->colLen);
if(colLen != right.rowLen)
        {
return tr;
        }
int runLength = rowLen;
for(int c = 0; c < right.colLen; c++)
        {
for(int r = 0; r < tr.rowLen; r++)
            {
                tr[r][c] = 0;
for(int i = 0; i < runLength; i++)
                {
                    tr[r][c] += mat[r][i] * (right.get(i))[c];
                }
            }
        }
return tr;
    } 
// Multiply a 4-component vertex by this matrix, return vertex
    Vertex operator *(const Vertex & right) const 
    {
// Convert Vertex to Matrix 
        Matrix vl(right);
// Multiply
        Matrix out = (*this) * vl;
// Return in vertex format
        Vertex rv;
        rv.x = out[0][0];
        rv.y = out[1][0];
        rv.z = out[2][0];
        rv.w = out[3][0];
return rv;
    }
};
// Rotational helper (4x4)
Matrix rotate(const DIMENSION & dim, const double & degs)
{
  Matrix tr(4, 4);
double rads = degs * M_PI / 180.0;
double cosT = cos(rads);
double sinT = sin(rads);
  tr[0][0] = 1;
  tr[0][1] = 0;
  tr[0][2] = 0;
  tr[0][3] = 0;
  tr[1][0] = 0;
  tr[1][1] = 1;
  tr[1][2] = 0;
  tr[1][3] = 0;
  tr[2][0] = 0;
  tr[2][1] = 0;
  tr[2][2] = 1;
  tr[2][3] = 0;
  tr[3][0] = 0;
  tr[3][1] = 0;
  tr[3][2] = 0;
  tr[3][3] = 1;
switch(dim)
    {
case X:
      tr[1][1] = cosT;
      tr[1][2] = -sinT;
      tr[2][1] = sinT;
      tr[2][2] = cosT;
break;
case Y:
      tr[0][0] = cosT;
      tr[0][2] = sinT;
      tr[2][0] = -sinT;
      tr[2][2] = cosT;
break;
case Z:
      tr[0][0] = cosT;
      tr[0][1] = -sinT;
      tr[1][0] = sinT;
      tr[1][1] = cosT;
break;
    }
return tr;
}
// Uniform scaling helper (4x4)
Matrix scaleMatrix(const double & scale)
{
  Matrix tr(4, 4);
  tr[0][0] = scale;
  tr[0][1] = 0;
  tr[0][2] = 0;
  tr[0][3] = 0;
  tr[1][0] = 0;
  tr[1][1] = scale;
  tr[1][2] = 0;
  tr[1][3] = 0;
  tr[2][0] = 0;
  tr[2][1] = 0;
  tr[2][2] = scale;
  tr[2][3] = 0;
  tr[3][0] = 0;
  tr[3][1] = 0;
  tr[3][2] = 0;
  tr[3][3] = 1;
return tr;
}
Matrix perspective(const double & fovYDegrees, const double & aspectRatio, 
const double & near, const double & far)
{
		Matrix rt;
double top = near * tan((fovYDegrees * M_PI) / 180.0 /2.0);
double right = aspectRatio * top;
		rt[0][0] = near / right;
		rt[0][2] = 0;
		rt[0][1] = 0;
		rt[0][3] = 0;
		rt[1][0] = 0;
		rt[1][1] = near / top;
		rt[1][2] = 0;
		rt[1][3] = 0;
		rt[2][0] = 0;
		rt[2][1] = 0;
		rt[2][2] = (far + near) / (far - near);
		rt[2][3] = (-2 * far * near) / (far - near);
		rt[3][0] = 0;
		rt[3][1] = 0;
		rt[3][2] = 1;
		rt[3][3] = 0;
return rt;		
}
// Translation helper (4x4)
Matrix translate(const double & offX, const double & offY, const double & offZ)
{
  Matrix tr(4, 4);
  tr[0][0] = 1;
  tr[0][1] = 0;
  tr[0][2] = 0;
  tr[0][3] = offX;
  tr[1][0] = 0;
  tr[1][1] = 1;
  tr[1][2] = 0;
  tr[1][3] = offY;
  tr[2][0] = 0;
  tr[2][1] = 0;
  tr[2][2] = 1;
  tr[2][3] = offZ;
  tr[3][0] = 0;
  tr[3][1] = 0;
  tr[3][2] = 0;
  tr[3][3] = 1;
return tr;
}
Matrix camera(const double & offX, const double & offY, const double & offZ,
const double & yaw, const double & pitch, const double & roll)
{
	Matrix trans = translate(-offX, -offY, -offZ);
	Matrix rotX = rotate(X, -pitch);
	Matrix rotY = rotate(Y, -yaw);
	Matrix rt = rotX * rotY * trans;
return rt;	
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
        bool baseAllocated = false;
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
            baseAllocated = true;
        }

        // Empty Constructor
        Buffer2D()
        {}

    public:
        // Free dynamic memory
        ~Buffer2D()
        {
            if(baseAllocated)
            {
                for(int r = 0; r < h; r++)
                {
                    free(grid[r]);
 		        }

 	            free(grid);
            }
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
            //SDL_LockSurface(img);
            setupInternal();
        }
};

// Interpoaltes between 3 weighted points 3 different double values for barycentric
inline double baryInterp(const double & firstWgt, const double & secndWgt, const double & thirdWgt,
			 const double & firstVal, const double & secndVal, const double & thirdVal)
{
  return ((firstWgt * thirdVal) + (secndWgt * firstVal) + (thirdWgt * secndVal));
}

// Combine two datatypes in one
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

        int count = 0;
        attrib arr[16];
        
        //Matrix transformations
        //Matrix matrix;
        //coordinates.
        double u;
        double v;

        //add RGB values.
        double r;
        double g;
        double b; 

        //pointer for image buffer
        BufferImage * ptrImg;

        //classic color pixel value
        PIXEL color;

        // Obligatory empty constructor
        Attributes() {count = 0;}

        // Interpolation Constructor
        Attributes( const double & firstWgt, const double & secndWgt, const double & thirdWgt, 
                    const Attributes & first, const Attributes & secnd, const Attributes & third,
		    const double & correctZ)
        {
            while(count < first.count)
            {
	         arr[count].d = baryInterp(firstWgt, secndWgt, thirdWgt, first.arr[count].d, secnd[count].d, third.arr[count].d);
		 arr[count].d = arr[count].d * correctZ;
		 count += 1;
            }
        }

        // Needed by clipping (linearly interpolated Attributes between two others)
        Attributes(const Attributes & first, const Attributes & second, const double & along)
        {
            // Your code goes here when clipping is implemented
            count = first.count;
 				for(int i = 0; i < count; i++)
 				{
 					arr[i].d = (first[i].d) + ((second[i].d - first[i].d) * along);
 				}
        }

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
            arr[count].d = d;
            count += 1;
        }
    
        // Insert Pointer Into Container
        void insertPtr(void * ptr)
        {
            arr[count].ptr = ptr;
            count += 1;
        }

       //Set the RGB values of pixel the attributes class using doubles.
        void setRGB(const double R,const double G,const double B)
        {
            this->r = R;
            this->g = G;
            this->b = B;
        }

        //Set the RGB values of the pixel attributes class using color codes.
        void setRGB(Uint32 R, Uint32 G, Uint32 B)
        {
            this->r = (double)R;
            this->g = (double)G;
            this->b = (double)B;
        }

        /*Vector must have values in form R G B U V*/
        void setRGBUV(vector<double> &vect)
        {
            if(vect.size() != 4)
            {
                cout << "The vector is not of a valid size, check you values" << endl;
            } 
            else
            {
                //This is not what a normalize vector does! But it helps to get an idea of how to manage vectors.
                this->setRGB(vect[0],vect[1],vect[2]);
                this->setUV(vect[3], vect[4]);
            }
            
        }

        //Set the U V values for the pixel attributes.
        void setUV(const double U, const double V)
        {
            this->u = U;
            this->v = V;
        }

        //Set Image data with a 2D Buffered Image.
        void setImageData(BufferImage myImage)
        {
            this->ptrImg = &myImage;
        }

        
};	

// Example of a fragment shader
void DefaultFragShader(PIXEL & fragment, const Attributes & vertAttr, const Attributes & uniforms)
{
    // Output our shader color value, in this case red
    fragment = 0xffff0000;
}

void GrayFragShader(PIXEL & fragment, const Attributes & vertAttr, const Attributes & uniforms){

    PIXEL avgChannel = ((vertAttr.color >> 16) && 0xff) + 
                        ((vertAttr.color >> 8) && 0xff) + 
                        ((vertAttr.color) && 0xff);
    avgChannel /= 3;
    fragment = 0xff000000 + (avgChannel << 16) + (avgChannel << 8) + avgChannel;
}

void SimpleVertexShader(Vertex & vertOut, Attributes & attrOut, const Vertex & vertIn, const Attributes & attrIn, const Attributes & uniforms)
{ 
  Matrix* trans = (Matrix*)uniforms[0].ptr;
  vertOut = (*trans) * vertIn;

  // Pass through attributes
  attrOut = attrIn;
}


void SimpleVertexShader2(Vertex & vertOut, Attributes & attrOut, const Vertex & vertIn, const Attributes & attrIn, const Attributes & uniforms)
 {
  Matrix* model = (Matrix*)uniforms[1].ptr;
  Matrix* view = (Matrix*)uniforms[2].ptr;
  Matrix* proj = (Matrix*)uniforms[3].ptr;

  vertOut = (*proj) * (*view) * (*model) * vertIn;

  // Pass through attributes
  attrOut = attrIn;
 }


// Image Fragment Shader 
 void ImageFragShader(PIXEL & fragment, const Attributes & vertAttr, const Attributes & uniforms)
 {
    BufferImage *bf = (BufferImage*)uniforms.ptrImg;
    int x = vertAttr.u * (bf->width()-1);
    int y = vertAttr.v * (bf->height()-1);
    fragment = (*bf)[y][x];
 }

  // My Fragment Shader for color interpolation
 void ColorFragShader(PIXEL & fragment, const Attributes & vertAttr, const Attributes & uniforms)
 {
    // Output our shader color value, in this case red
    PIXEL color = 0xff000000;
    color += (unsigned int)(vertAttr.r *0xff) << 16;
    color += (unsigned int)(vertAttr.g *0xff) << 8;
    color += (unsigned int)(vertAttr.b *0xff) << 0;

    fragment = color;
 }

//reciprocal function
double reciprocal(const double value)
{
	return 1 / value;
}

 //lerp function
double lerp(double v0, double v1, double t) 
{
  return v0 + ( (v1 - v0) * t);
}

 // Frag Shader for UV without image (due to SDL2 bug?)
void FragShaderUVwithoutImage(PIXEL & fragment, const Attributes & attributes, const Attributes & uniform)
{
    // Figure out which X/Y square our UV would fall on
    int xSquare = attributes.u * 8;
    int ySquare = attributes.v * 8;

	// Is the X square position even? The Y? 
    bool evenXSquare = (xSquare % 2) == 0;
    bool evenYSquare = (ySquare % 2) == 0;
   
    // Both even or both odd - red square
    if( (evenXSquare && evenYSquare) || (!evenXSquare && !evenYSquare) )
    {
        fragment = 0xffff0000;
    }
    //One even, one odd - white square
    else
    {
        fragment = 0xffffffff;
            
    }
    
}

void FragShaderUVwithoutImage2(PIXEL & fragment, const Attributes & attributes, const Attributes & uniform)
{
    // Figure out which X/Y square our UV would fall on
    int xSquare = attributes[0].d * 8;
    int ySquare = attributes[1].d * 8;

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
            //FragShader = DefaultFragShader;
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

// VERTEX SHADER FOR FULL PIPELINE
void Week08VertShader(Vertex & vertOut, Attributes & attrOut, const Vertex & vertIn, const Attributes & vertAttr, const Attributes & uniforms)
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
