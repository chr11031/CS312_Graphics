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


inline bool operator==(const Vertex& lhs, const Vertex& rhs)
{ 
    return lhs.x == rhs.x &&
           lhs.y == rhs.y &&
           lhs.z == rhs.z &&
           lhs.w == rhs.w;
}


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

/****************************************************
 * Describes a matrix 
 ****************************************************/
class Matrix
{
    // 2D Array
    private:
        double mat[4][4];

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
    Matrix(int width = 4, int height = 4)
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
    Matrix(double values[], int width = 4, int height = 4)
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
        return *this;
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
Matrix rotateMatrix(const int & dim, const double & degs)
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
    case 0:
      tr[1][1] = cosT;
      tr[1][2] = -sinT;
      tr[2][1] = sinT;
      tr[2][2] = cosT;
      break;
    case 1:
      tr[0][0] = cosT;
      tr[0][2] = sinT;
      tr[2][0] = -sinT;
      tr[2][2] = cosT;
      break;
    case 2:
      tr[0][0] = cosT;
      tr[0][1] = -sinT;
      tr[1][0] = sinT;
      tr[1][1] = cosT;
      break;
    default:
      tr[1][1] = cosT;
      tr[1][2] = -sinT;
      tr[2][1] = sinT;
      tr[2][2] = cosT;
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

// Translation helper (4x4)
Matrix translateMatrix(const double & offX, const double & offY, const double & offZ)
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

Matrix perspectiveMatrix(	const double & fovYDegrees, const double & aspectRatio, 
							const double & near, const double & far)
{
		Matrix rt;

		double top = near * tan((fovYDegrees * M_PI) / 180.0)/ 2.0;
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

Matrix cameraMatrix(const double & offX, const double & offY, const double & offZ,
					 const double & yaw, const double & pitch, const double & roll)
{

	Matrix trans = translateMatrix(-offX, -offY, -offZ);
	Matrix rotX = rotateMatrix(0, -pitch);
	Matrix rotY = rotateMatrix(1, -yaw);

	Matrix rt = rotX * rotY * trans;
	return rt;	
}

// Struct used for reading in RGB values from a bitmap file
struct bmpRGB
{
  unsigned char b;
  unsigned char g; 
  unsigned char r;
};

// The portion of the Bitmap header we want to read
struct bmpLayout
{
  int offset;
  int headerSize;
  int width;
  int height;
  unsigned short colorPlanes;
  unsigned short bpp;
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
        bool baseAllocated = false;

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
            // De-Allocate pointers for column references
     //       for(int r = 0; r < h; r++)
       //     {
         //       free(grid[r]);
           // }
            //free(grid);
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
    private:

        // Non-SDL2 24BPP, 2^N dimensions BMP reader
        bool readBMP(const char* fileName)
        {
            // Read in Header - check signature
            FILE * fp = fopen(fileName, "rb");	    
            char signature[2];
            fread(signature, 1, 2, fp);
            if(!(signature[0] == 'B' && signature[1] == 'M'))
            {
                printf("Invalid header for file: \"%c%c\"", signature[0], signature[1]);
                return 1;
            }

            // Read in BMP formatting - verify type constraints
            bmpLayout layout;
            fseek(fp, 8, SEEK_CUR);
            fread(&layout, sizeof(layout), 1, fp);
            if(layout.width % 2 != 0 || layout.width <= 4)
            {
                printf("Size Width MUST be a power of 2 larger than 4; not %d\n", w);
                return false;		
            }
            if(layout.bpp != 24)
            {
                printf("Bits per pixel of image must be 24; not %d\n", layout.bpp);
                return false;
            }

            // Copy W+H information
            w = layout.width;
            h = layout.height;

            // Initialize internal pointers/memory
            grid = (PIXEL**)malloc(sizeof(PIXEL*) * h);
            for(int y = 0; y < h; y++) grid[y] = (PIXEL*)malloc(sizeof(PIXEL) * w);

            // Advance to beginning of pixel data, read values in
            bmpRGB* pixel = (bmpRGB*)malloc(sizeof(bmpRGB)*w*h);
            fseek(fp, layout.offset, SEEK_SET);  	
            fread(pixel, sizeof(bmpRGB), w*h, fp);

            // Convert 24-bit RGB to 32-bit ARGB
            bmpRGB* pixelPtr = pixel;
            PIXEL* out = (PIXEL*)malloc(sizeof(PIXEL)*w*h);
            for(int y = 0; y < h; y++)
            {
                for(int x = 0; x < w; x++)
                {
                    grid[y][x] =    0xff000000 + 
                                    ((pixelPtr->r) << 16) +
                                    ((pixelPtr->g) << 8) +
                                    ((pixelPtr->b));
                                    ++pixelPtr;
                }
            }

            // Release 24-Bit buffer, release file
            free(pixel);
            fclose(fp); 
            return true;
        }
    protected:       
        SDL_Surface* img;                   // Reference to the Surface in question
        bool ourSurfaceInstance = false;    // Do we need to de-allocate?
        bool ourBufferData = false;         // Are we using the non-SDL2 allocated memory

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
            // De-Allocate non-SDL2 image data
            if(ourBufferData)
            {
                free(grid);
	     	return;
            }
            

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
            return *this;
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
            ourSurfaceInstance = false;
	        if(!readBMP(path))
	        {
	            return;
	        }
            ourBufferData = true;
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
        Attributes(const Attributes & first, const Attributes & second, const double & along)
        {
            numValues = first.numValues;
				for(int i = 0; i < numValues; i++)
				{
					values[i] = (first.values[i]) + ((second.values[i] - first.values[i]) * along);
				}
        }
    PIXEL color;

    //array of doubles for value and the number in the instance.
    double values[5];
    int numValues;
    //Void pointer that can be cast to an image or other value.
    void * pointer;
    Matrix matrix;
    Matrix matrix2;
    Matrix matrix3;
    Matrix matrix4;
};	

//Creates a color with the attributes sent to it
void avgColorFragShader(PIXEL & fragment, const Attributes & vertAttr, const Attributes & uniforms)
{
    //color base
    PIXEL color = 0xFF000000;
    //Adds the individual parts of the color value together.
    color += (unsigned int)(vertAttr.values[0] * 0xFF) << 16;
    color += (unsigned int)(vertAttr.values[1] * 0xFF) << 8;
    color += (unsigned int)(vertAttr.values[2] * 0xFF) << 0;

    fragment = color;
}

void flatColorFragShader(PIXEL & fragment, const Attributes & vertAttr, const Attributes & uniforms)
{

    fragment = vertAttr.color;
}

//Finds the color corresponding to the u/v values.
void imageFragShader(PIXEL & fragment, const Attributes & vertAttr, const Attributes & uniforms)
{
    BufferImage* buffer = (BufferImage*)uniforms.pointer;
    int x = vertAttr.values[0] * (buffer->width() - 1);
    int y = vertAttr.values[1] * (buffer->height() - 1);

    if (y < S_HEIGHT && y >= 0 && x < S_WIDTH && x >= 0)
        fragment = (*buffer)[y][x];
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

void TransformVertexShader(Vertex & vertOut, Attributes & attrOut, const Vertex & vertIn, const Attributes & attrIn, const Attributes & uniforms)
{
    //multiplies the values with the matrix
    Matrix trans = (Matrix)uniforms.matrix;
    vertOut = trans * vertIn;

    // Pass through attributes
    attrOut = attrIn;
}

void SimpleVertexShader2(Vertex & vertOut, Attributes & attrOut, const Vertex & vertIn, const Attributes & attrIn, const Attributes & uniforms)
{
  Matrix model = uniforms.matrix;
  Matrix view = uniforms.matrix2;
  Matrix proj = uniforms.matrix3;

  vertOut = proj * view * model * vertIn;

  // Pass through attributes
  attrOut = attrIn;
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
