#define SDL_MAIN_HANDLED
#include "SDL2/SDL.h"
#include "stdlib.h"
#include "stdio.h"
#include "math.h"
#include "trans.h"

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
#define MEMBERS_PER_ATTRIB
#define X_KEY 0
#define Y_KEY 1
#define MAX_DIM_SIZE_MATRIX 4

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
 
/******************************************************
 * Transformation class for world geometry, viewing.
 *****************************************************/
class Transform
{ 
    // 2D Array 
    private: 
        double mat[MAX_DIM_SIZE_MATRIX][MAX_DIM_SIZE_MATRIX];
 
    // Update Matrix values by array 
    void copyValues(const Transform & source) 
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
    Transform(int width = MAX_DIM_SIZE_MATRIX, int height = MAX_DIM_SIZE_MATRIX) 
    { 
        colLen = width; 
        rowLen = height; 
        setIdentity(); 
    }
 
    // Initialize a vertex as a matrix 
    Transform(const Vertex & vert) 
    { 
        rowLen = 4; 
        colLen = 1; 
        mat[0][0] = vert.x; 
        mat[1][0] = vert.y; 
        mat[2][0] = vert.z; 
        mat[3][0] = vert.w; 
    }
 
    // Initialize matrix to suggested values 
    Transform(double values[], int width = MAX_DIM_SIZE_MATRIX, int height = MAX_DIM_SIZE_MATRIX)
    { 
        colLen = width; 
        rowLen = height; 
        setValues(values); 
    }
 
    // Assignment operator  
    Transform& operator = (const Transform & right) 
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
    Transform operator *(const Transform & right) const
    {
        Transform tr(right.rowLen, this->colLen);

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
        Transform vl(right);

        // Multiply 
        Transform out = (*this) * vl;

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
Transform rotate4x4(const DIMENSION & dim, const double & degs)
{
  Transform tr(4, 4);
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

Transform perspective4x4(const double & fovYDegrees, const double & aspectRatio,
			 const double & near, const double & far)
{
    Transform rt;

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

// Uniform scaling helper (4x4) 
Transform scale4x4(const double & scale)
{
  Transform tr(4, 4);
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
Transform translate4x4(const double & offX, const double & offY, const double & offZ)
{
  Transform tr(4, 4);
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

Transform camera4x4(const double & offX, const double & offY, const double & offZ,
					 const double & yaw, const double & pitch, const double & roll)
{
	Transform trans = translate4x4(-offX, -offY, -offZ);
	Transform rotX = rotate4x4(X, -pitch);
	Transform rotY = rotate4x4(Y, -yaw);
	
	Transform rt = rotX * rotY * trans;
	return rt;
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
        Buffer2D() {}

    public:
        // Free dynamic memory
        ~Buffer2D()
        {
            // De-Allocate pointers for column references
            if (baseAllocated)
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
        bool ourBufferData = false;         // Are we using the non-SDL2 allocated memory?

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
                    grid[y][x] = 0xff000000 + 
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

    public:
        // Free dynamic memory
        ~BufferImage()
        {
            // De-Allocate non-SDL2 image data
            if (ourBufferData)
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
            /*ourSurfaceInstance = true;
            SDL_Surface* tmp = SDL_LoadBMP(path);      
            SDL_PixelFormat* format = SDL_AllocFormat(SDL_PIXELFORMAT_ARGB8888);
            img = SDL_ConvertSurface(tmp, format, 0);
            SDL_FreeSurface(tmp);
            SDL_FreeFormat(format);
            SDL_LockSurface(img);// For Project04
            setupInternal();
            */
           ourSurfaceInstance = false;
           if(!readBMP(path))
           {
               return;
           }	  
           ourBufferData = true;
        }
};

// Interpoaltes between 3 weighted points 3 different double values for barycentric
inline double baryInterp(const double & firstWgt, const double & secndWgt, const double & thirdWgt,
			 const double & firstVal, const double & secndVal, const double & thirdVal)
{
    return ((firstWgt * thirdVal) + (secndWgt * firstVal) + (thirdWgt * secndVal));
}

class Matrix
{
    public:
        double cell[4][4];
        int numRows;
        int numCols;

    // Default constructor
    Matrix ()
    {
        numRows = 0;
        numCols = 0;
    }
    // Non-default constructor
    Matrix(double data[][4], int rows, int cols)
    {
        this->numRows = rows;
        this->numCols = cols;
        for (int i = 0; i < numRows; i++)
        {
            for (int j = 0; j < numCols; j++)
            {
                cell[i][j] = data[i][j];
            }
        }
    }
    //  Operators
    void operator *= (const Matrix & rhs);
    void operator = (const Matrix & rhs);
};

/**************************************************
 * Assignment operator
 ***************************************************/
void Matrix :: operator = (const Matrix & rhs)
{
        for (int i = 0; i < rhs.numRows; i++)
        {
            for (int j = 0; j < rhs.numCols; j++)
            {
                this->cell[i][j] = rhs.cell[i][j];
            }
        }
        this->numRows = rhs.numRows;
        this->numCols = rhs.numCols;
}

/***************************************
 * Override *= operator for matrices
 * *************************************/
void Matrix :: operator *= (const Matrix & rhs)
{
    if (this->numRows != rhs.numCols)
    {
        throw "ERROR: Cannot concatenate matrices\n";
    }

    // Temporary matrix to help with multiplication
    double tempMatrix[4][4];

    for (int i = 0; i < rhs.numRows; i++)
    {
        for (int j = 0; j < this->numCols; j++)
        {
            double colToRow[4];
            for (int k = 0; k < this->numRows; k++)
            {
                colToRow[k] = this->cell[k][j];
            }
            tempMatrix[i][j] = 0;
            // multiplies rows and columns
            for (int n = 0; n < rhs.numCols; n++)
            {
                tempMatrix[i][j] += (rhs.cell[i][n] * colToRow[n]);
            }
        }
    }
    this->numRows = rhs.numRows;
    // Write onto the matrix for assignment
    for (int i = 0; i < this->numRows; i++)
    {
        for (int j = 0; j < this->numCols; j++)
        {
            this->cell[i][j] = tempMatrix[i][j];
        }
    }
}

/*****************************************************
 * Override multiplication operator for vertices and matrices
 * ****************************************************/
Vertex operator * (const Vertex & lhs, const Matrix & rhs)
{
    if (rhs.numCols < 1 || rhs.numCols > 4)
    {
        throw "ERROR: Cannot multiply matrices\n";
    }
    double currentVerts[4] = {lhs.x, lhs.y, lhs.z, lhs.w};
    double tempVerts[4] = {lhs.x, lhs.y, lhs.z, lhs.w};
    for (int i = 0; i < rhs.numRows; i++)
    {
        // multiplies rows and columns
        tempVerts[i] = 0;
        for (int n = 0; n < rhs.numCols; n++)
        {
            tempVerts[i] += (rhs.cell[i][n] * currentVerts[n]);
        }
    }
    return {tempVerts[0], tempVerts[1], tempVerts[2], tempVerts[3]};
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
        // value[] is used for r, g, b, u, and v values
        //double value[16];//
        int numValues = 0;
        // Used for image fragment shader
        void * ptrImg;
        void * valuePtr[16];
        attrib value[16];//
        PIXEL color;

        // For matrices
        Matrix matrix;

        // Obligatory empty constructor
        Attributes() {numValues = 0;}

        // Interpolation Constructor
        Attributes( const double & firstWgt, const double & secndWgt, const double & thirdWgt, 
                    const Attributes & first, const Attributes & secnd, const Attributes & third,
		    const double & correctZ)
        {
            while(numValues < first.numValues)
            {
                value[numValues].d = baryInterp(firstWgt, secndWgt, thirdWgt, first.value[numValues].d, secnd.value[numValues].d, third.value[numValues].d);
                value[numValues].d = value[numValues].d * correctZ;
                numValues += 1;
            }
        }

        // Needed by clipping (linearly interpolated Attributes between two others)
        Attributes(const Attributes & first, const Attributes & second, const double & along)
        {
            numValues = first.numValues;
            for(int i = 0; i < numValues; i++)
            {
                //arr[i].d = (first[i].d) + ((second[i].d - first[i].d) * along);
                value[i].d = (first.value[i].d) + ((second.value[i].d - first.value[i].d) * along);
            }
        }

        void insertDbl(const double d)
        {
            value[numValues].d = d;
            numValues += 1;
        }

        void insertPtr(void * ptr)
        {
            //arr[numMembers].d = d;
            //valuePtr[numValues].ptr = ptr;
            value[numValues].ptr = ptr;
            numValues += 1;
        }
};

void ColorFragShader(PIXEL & fragment, const Attributes & vertAttr, const Attributes & uniforms)
{
    // Output the shader color value
    PIXEL color = 0xff000000;
    color += (unsigned int)(vertAttr.value[0].d *0xff) << 16;
    color += (unsigned int)(vertAttr.value[1].d *0xff) << 8;
    color += (unsigned int)(vertAttr.value[2].d *0xff) << 0;
    fragment = color;
}

void SineShader(PIXEL & fragment, const Attributes & vertAttr, const Attributes & uniforms)
{
    BufferImage* buffer = (BufferImage*)uniforms.ptrImg;
    //int x = vertAttr.value[0] = (buffer -> width()); <--
    //int y = vertAttr.value[1] = (buffer -> height()); <--
    int x = (buffer -> width());
    int y = (buffer -> height());
    if (y > sin(x / 10.0) * 10 + 20)
    {
        fragment = 0xFFFFFFFF;
    }
    else
    {
        fragment = 0xFF00FF00;
    }
}

void StaticNoiseShader(PIXEL & fragment, const Attributes & vertAttr, const Attributes & uniforms)
{
    if (rand() % 2)
       fragment = 0xFFFFFFFF;
    else
       fragment = 0xFF000000;
}

//extern //What is this??

void SimpleVertexShader(Vertex & vertOut, Attributes & attrOut, const Vertex & vertIn, const Attributes & attrIn, const Attributes & uniforms)
{
    /*Transform * model = (Transform *) uniforms[1].ptr;
    Transform * view = (Transform *) uniforms[2].ptr;
    Transform * proj = (Transform *) uniforms[3].ptr;

    vertOut = (*proj) * (*view);
    attrOut = attrIn;
    */
   Transform* trans = (Transform*) uniforms.value[0].ptr;
   vertOut = (*trans) * vertIn;
   
   // Pass through attributes
   attrOut = attrIn;
}

void SimpleVertexShader2(Vertex & vertOut, Attributes & attrOut, const Vertex & vertIn, const Attributes & attrIn, const Attributes & uniforms)
{
    Transform* model = (Transform*) uniforms.value[1].ptr;
    Transform* view = (Transform*) uniforms.value[2].ptr;
    Transform* proj = (Transform*) uniforms.value[3].ptr;
    
    vertOut = (*proj) * (*view) * (*model) * vertIn;
    
    // Pass through attributes
    attrOut = attrIn;
}

void ImageFragShader(PIXEL & fragment, const Attributes & vertAttr, const Attributes & uniforms)
{
    // Output a bitmap image
    /*BufferImage * ptr = (BufferImage *)uniforms.ptrImg;
    int wid = ptr->width();
    int hgt = ptr->height();
    int x = vertAttr.value[0] * (wid - 1);
    int y = vertAttr.value[1] * (hgt - 1);
    fragment = (*ptr)[y][x];
    */
    BufferImage* bf = (BufferImage*)uniforms.value[0].ptr;
    int x = vertAttr.value[0].d * (bf->width()-1);
    int y = vertAttr.value[1].d * (bf->height()-1);

    fragment = (*bf)[y][x];
}

// Frag Shader for UV without image (due to SDL2 bug?)
void FragShaderUVwithoutImage(PIXEL & fragment, const Attributes & attributes, const Attributes & uniform)
{
    // Figure out which X/Y square our UV would fall on
    int xSquare = attributes.value[0].d * 8;
    int ySquare = attributes.value[1].d * 8;

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

// Example of a fragment shader
void DefaultFragShader(PIXEL & fragment, const Attributes & vertAttr, const Attributes & uniforms)
{
    // Output our shader color value, in this case red
    fragment = 0xffff0000;
}

void transparencyShader(PIXEL & fragment, const Attributes & vertAttr, const Attributes & uniforms)
{
    /*
    uniform sampler2D myTexture;
    varying vec2 TexCoord;
    if(texture2D(myTexture, TexCoord).a != 1.0f)
    {
        discard;
    }
    gl_FragColor = texture2D(myTexture, TexCoord);
    */

    //fragment = texture2D(myTexture,TexCoord); //
}

void asciiShader() // "Maybe"
{
    ;
}

void lightNShadowShader()
{
    ;
}

void blurringShader() // "Maybe"
{
    ;
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

// Transformation Vertex Shader
void transVertShader(Vertex & vertOut, Attributes & attrOut, const Vertex & vertIn, const Attributes & vertAttr, const Attributes & uniforms)
{
    vertOut = (vertIn * uniforms.matrix);
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

/*Transform perspective4x4(const double & fovY, const double & aspectRatio,
                        const double & near, const double & far)
{
    Transform rt;

    double top = near * tan((fovYDegrees * M_PI) / 180.0) / 2.0;
    double right = aspectRatio * top;
    tr[0][0] = near / right;
    tr[0][1] = 0;
    tr[0][2] = 0;
    tr[0][3] = 0;

    tr[1][0] = 0
    tr[1][1] = near / top;
    tr[1][2] = 0;
    tr[1][3] = 0;
    tr[1][4] = 0;
}

Transform camera4x4(const double & offX, const double & offY, const double & offZ,
                    const double & yaw, const double & pitch, const double & roll)
{
    Transform tr;
    Transform trans = translate4x4(-offX, -offY, -offZ);
    Transform rotX = rotate4x4(X, pitch);
    Transform rotY = rotate4x4(Y, yaw);
    Transform rt = rotX * rotY * trans;
    return rt;
}
*/

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
