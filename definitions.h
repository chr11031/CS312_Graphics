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

double multiplyRowAndCol(const double row[], const double col[], const int & length)
{
    double result = 0;

    for (int i = 0; i < length; i++)
    {
        result += row[i] * col[i];
    }

    return result;
}

/******************************************************
 * Types of primitives our pipeline will render.
 *****************************************************/
enum PRIMITIVES 
{
    TRIANGLE,
    LINE,
    POINT
};

enum AXISROTATION
{
    X,
    Y,
    Z
};

union attrib
{
    double d;
    void* ptr;
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
 * Describes a geometric point in 3D space. 
 ****************************************************/
struct Vertex
{
    double x;
    double y;
    double z;
    double w;
};

/*******************************************************
*  Everything needed for the view/camera transform
********************************************************/
struct camControls
{
    double x = 0;
    double y = 0;
    double z = 0;
    double yaw = 0;
    double roll = 0;
    double pitch = 0;
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
            // De-Allocate pointers for column references
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
        const int & width()  const  { return w; }
        const int & height() const { return h; }

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
        bool ourSurfaceInstance = false;    // Do we need to de-allocate the SDL2 reference?
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

            // De-Allocate pointers for column references
            free(grid);

            // De-Allocate this image plane is necessary
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
            ourSurfaceInstance = false;
            if (!readBMP(path))
                return;
            ourBufferData = true;
        }
};

class Matrix
{
public:

    Matrix () : numRows(0), numCols(0), isInit(false) { }


    Matrix (double newData[][4], int numRows, int numCols) : numRows(numRows), numCols(numCols), isInit(true)
    {
        for (int i = 0; i < numRows; i++)
            for (int j = 0; j < numCols; j++)
                data[i][j] = newData[i][j];
    }

    void setData (const double newData[0][4], int numRows, int numCols)
    {
        for (int i = 0; i < numRows; i++)
            for (int j = 0; j < numCols; j++)
                data[i][j] = newData[i][j];

        this->numRows = numRows;
        this->numCols = numCols;
        this->isInit = true;
    }

    void operator= (const Matrix & rhs) throw (const char*)
    {
        for (int i = 0; i < rhs.numRows; i++)
            for (int j = 0; j < rhs.numCols; j++)
                this->data[i][j] = rhs.data[i][j];

        this->numRows = rhs.numRows;
        this->numCols = rhs.numCols;
        this->isInit = true;
    }

    void operator *=(const Matrix & rhs) throw (const char *);
    friend Vertex operator* (const Matrix & lhs, const Vertex & rhs) throw (const char *);
    friend Matrix operator* (const Matrix & lhs, const Matrix & rhs) throw (const char *);
    friend Matrix perspective4x4(const double & fovYDegree, const double & aspectRatio, const double & near,
                      const double & far);
    friend Matrix camera4x4(const double & offX, const double & offY, const double & offZ, 
                 const double & yaw, const double & pitch, const double & roll);
    friend Matrix orthographic4x4(const double & fovYDegree, const double & aspectRatio, const double & near,
                      const double & far);

    void addTrans(const double & x, const double & y, const double & z);
    void addRot  (AXISROTATION rot, const double & degree);
    void addScale(const double & x, const double & y, const double & z);

    double*       operator[](const int & index)        { return (double*)data[index]; }
    const double* operator[](const int & index) const  { return (double*)data[index];}

private:
    double data[4][4];
    bool isInit;
    int numRows;
    int numCols;
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

        // Attribute information
        PIXEL color; // Still here for depricated code
        //double attrValues[15]; // Still here for legacy code DEPRECATED
        int valuesToInterpolate; // Still here for legacy code DEPRECATED
        void* ptrImage; // Still here for legacy code DEPRECATED
        attrib attribs[20];
        int numMembers;

        // Matrix information for transformations
        Matrix matrix;

        // Obligatory empty constructor
        Attributes() : valuesToInterpolate(0), ptrImage(NULL), numMembers(0) {}


        // Needed by clipping (linearly interpolated Attributes between two others)
        Attributes(const Attributes & first, const Attributes & second, const double & along)
        {
            this->numMembers = first.numMembers;
            for (int i = 0; i < this->numMembers; i++)
            {
                attribs[i].d = first[i].d + ((second[i].d - first[i].d) * along);
            }
        }

        void interpolateValues(const double &area, const double &d1, const double &d2, const double &d3, Attributes* vertAttrs)
        {
            double w1 = d1 / area;
            double w2 = d2 / area;
            double w3 = 1 - w1 - w2;

            for (int i = 0; i < this->numMembers; i++)
            {
                attribs[i].d = vertAttrs[0].attribs[i].d * w2 +
                                vertAttrs[1].attribs[i].d * w3 +
                                vertAttrs[2].attribs[i].d * w1;
            }
        }

        void correctPerspective(double correctedZ)
        {
            for (int i = 0; i < this->numMembers; i++)
            {
                attribs[i].d *= correctedZ;
            }
        }

        void concatenateMatricies(const Matrix & transform) throw (const char *)
        {
            this->matrix *= transform;
        }

        void setMatrixData(const double data[0][4], int numRows, int numCols)
        {
            this->matrix.setData(data, numRows, numCols);
        }

        inline void insertDbl(const double & d) { attribs[numMembers++].d = d; }
        inline void insertPtr(void* ptr)        { attribs[numMembers++].ptr = ptr; }

        attrib       & operator[] (const int & i)        { return attribs[i]; }
        const attrib & operator[] (const int & i) const  { return attribs[i]; }
}; 

void Matrix::addTrans(const double & x = 0, const double & y = 0, const double & z = 0)
{
    Matrix temp;
    temp.numCols = 4;
    temp.numRows = 4;
    temp.data[0][0] = 1;
    temp.data[0][1] = 0;
    temp.data[0][2] = 0;
    temp.data[0][3] = x;
    temp.data[1][0] = 0;
    temp.data[1][1] = 1;
    temp.data[1][2] = 0;
    temp.data[1][3] = y;
    temp.data[2][0] = 0;
    temp.data[2][1] = 0;
    temp.data[2][2] = 1;
    temp.data[2][3] = z;
    temp.data[3][0] = 0;
    temp.data[3][1] = 0;
    temp.data[3][2] = 0;
    temp.data[3][3] = 1;

    if(this->isInit)
        *this *= temp;
    else
    {
        *this = temp;
        this->isInit = true;
    }
}

void Matrix::addScale(const double & x = 1, const double & y = 1, const double & z = 1)
{
    Matrix temp;
    temp.numCols = 4;
    temp.numRows = 4;
    temp.data[0][0] = x;
    temp.data[0][1] = 0;
    temp.data[0][2] = 0;
    temp.data[0][3] = 0;
    temp.data[1][0] = 0;
    temp.data[1][1] = y;
    temp.data[1][2] = 0;
    temp.data[1][3] = 0;
    temp.data[2][0] = 0;
    temp.data[2][1] = 0;
    temp.data[2][2] = z;
    temp.data[2][3] = 0;
    temp.data[3][0] = 0;
    temp.data[3][1] = 0;
    temp.data[3][2] = 0;
    temp.data[3][3] = 1;

    if (this->isInit)
        *this *= temp;
    else
    {
        *this = temp;
        this->isInit = true;
    }

}

void Matrix::addRot(AXISROTATION rot, const double & degrees)
{
    Matrix temp;
    temp.numCols = 4;
    temp.numRows = 4;

    double rads = degrees * M_PI / 180.0;
    double cosRot = cos(rads);
    double sinRot = sin(rads);


    temp.data[0][0] = 1;
    temp.data[0][1] = 0;
    temp.data[0][2] = 0;
    temp.data[0][3] = 0;
    temp.data[1][0] = 0;
    temp.data[1][1] = 1;
    temp.data[1][2] = 0;
    temp.data[1][3] = 0;
    temp.data[2][0] = 0;
    temp.data[2][1] = 0;
    temp.data[2][2] = 1;
    temp.data[2][3] = 0;
    temp.data[3][0] = 0;
    temp.data[3][1] = 0;
    temp.data[3][2] = 0;
    temp.data[3][3] = 1;

    switch (rot)
    {
        case X:
            temp.data[1][1] = cosRot;
            temp.data[2][2] = cosRot;
            temp.data[1][2] = -sinRot;
            temp.data[2][1] = sinRot;
            break;
        case Y:
            temp.data[0][0] = cosRot;
            temp.data[0][2] = sinRot;
            temp.data[2][0] = -sinRot;
            temp.data[2][2] = cosRot;
            break;
        case Z:
            temp.data[0][0] = cosRot;
            temp.data[0][1] = -sinRot;
            temp.data[1][0] = sinRot;
            temp.data[1][1] = cosRot;
            break;
        
    }

    if (this->isInit)
        *this *= temp;
    else 
    {
        *this = temp;
        this->isInit = true;
    }
}


/**********************************************************************
 *  
 **********************************************************************/
void Matrix::operator*=(const Matrix & rhs) throw (const char *)
{
    Matrix tempMatrix;
    if (this->isInit)
    {
        tempMatrix = *this * rhs;

        // Make sure this matrix knows it has changed
        this->numCols = rhs.numCols;

        // Overwrite the old matrix with the new one
        for (int i = 0; i < this->numRows ; i++)
            for (int j = 0; j < this->numCols; j++)
                this->data[i][j] = tempMatrix[i][j];
    }
    else
    {
        *this = rhs;
        this->isInit = true;
    }

    return;
}

Matrix perspective4x4(const double & fovYDegree, const double & aspectRatio, const double & near,
                      const double & far)
{
    Matrix rt;

    double top = near * tan((fovYDegree * M_PI) / 180.0 / 2.0);
    double right = aspectRatio * top;

    rt[0][0] = near / right;
    rt[0][1] = 0;
    rt[0][2] = 0;
    rt[0][3] = 0;

    rt[1][0] = 0;
    rt[1][1] = near / top;
    rt[1][2] = 0;
    rt[1][3] = 0;

    rt[2][0] = 0;
    rt[2][1] = 0;
    rt[2][2] = (far + near ) / (far - near);
    rt[2][3] = (-2 * far * near) / (far - near);

    rt[3][0] = 0;
    rt[3][1] = 0;
    rt[3][2] = 1;
    rt[3][3] = 0;

    rt.numCols = 4;
    rt.numRows = 4;
    rt.isInit = true;

    return rt;
}

Matrix orthographic4x4(const double & fovYDegree, const double & aspectRatio, const double & near,
                      const double & far)
{
    Matrix rt;

    double top = near * tan((fovYDegree * M_PI) / 180.0 / 2.0);
    double right = aspectRatio * top;

    rt[0][0] = 1 / right;
    rt[0][1] = 0;
    rt[0][2] = 0;
    rt[0][3] = 0;

    rt[1][0] = 0;
    rt[1][1] = 1 / top;
    rt[1][2] = 0;
    rt[1][3] = 0;

    rt[2][0] = 0;
    rt[2][1] = 0;
    rt[2][2] = -(2 / (far - near));
    rt[2][3] = -((far + near) / (far - near));

    rt[3][0] = 0;
    rt[3][1] = 0;
    rt[3][2] = 0;
    rt[3][3] = 1;

    rt.numCols = 4;
    rt.numRows = 4;
    rt.isInit = true;

    return rt;
}

Matrix camera4x4(const double & offX, const double & offY, const double & offZ, 
                 const double & yaw, const double & pitch, const double & roll)
{
    Matrix trans;

    trans.addTrans(-offX, -offY, -offZ);

    Matrix rotX;

    rotX.addRot(X, -pitch);

    Matrix rotY;

    rotY.addRot(Y, -yaw);

    Matrix rt = rotX * rotY * trans;

    return rt;
}

Matrix operator* (const Matrix & lhs, const Matrix & rhs) throw (const char *)
{
    if (rhs.numRows != lhs.numCols)
        throw "ERROR: Invalid matrix sizes for concatenate\n";

    // Temp matrix to hold results so we don't mess up data during
    // matrix multiplication
    Matrix tempMatrix;
    
    // Loop through the number of rows in the RHS
    for (int i = 0; i < lhs.numRows; i++)
    {
        // Loop through the number of cols in LHS
        for (int j = 0; j < rhs.numCols; j++)
        {
            double colToRow[4];

            //Flatten the LHS cols into a row for maths
            for (int k = 0; k < rhs.numRows; k++)
            {
                colToRow[k] = rhs.data[k][j];
            }

            // Do maths on RHS row and LHS col and assign
            tempMatrix.data[i][j] = multiplyRowAndCol(lhs.data[i], colToRow, lhs.numCols);
        }
    }

    tempMatrix.numRows = lhs.numRows;
    tempMatrix.numCols = rhs.numCols;
    tempMatrix.isInit = true;
    return tempMatrix;
}

/******************************************************
 * MATRIX MULT OVERLOADED OPERATOR
 * Multiplies the two matricies together
 *****************************************************/ 
Vertex operator * (const Matrix & lhs, const Vertex & rhs) throw (const char *)
{
    if (lhs.numCols < 1 || lhs.numCols > 4)
        throw "ERROR: Invalid matrices sizes, cannot do multiplication";

    double newVerts[4] = {rhs.x, rhs.y, rhs.z, rhs.w};
    double currentVerts[4] = {rhs.x, rhs.y, rhs.z, rhs.w};
    
    for (int i = 0; i < lhs.numRows; i++)
        newVerts[i] = multiplyRowAndCol(lhs.data[i], currentVerts, lhs.numCols);

    Vertex temp = {newVerts[0], newVerts[1], newVerts[2], newVerts[3]};
    return temp;
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
        {}

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
