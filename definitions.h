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
 * ENUM SO THAT ROTATION IS EASIER
 ****************************************************/
enum AXIS_ROTATION
{
    X,
    Y,
    Z
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
 * Everything needed for the view/transform
 ****************************************************/
struct camControls
{
    double x = 0;
    double y = 0;
    double z = 0;
    double yaw   = 0;
    double pitch = 0;
    double roll  = 0;
};

// Global var
camControls myCam;

/****************************************************
 * For the new Image Loader
 ****************************************************/
struct bmpRGB
{
    unsigned char b;
    unsigned char g;
    unsigned char r;
};

/****************************************************
 * For the new Image Loader
 ****************************************************/
struct bmpLayout
{
    int offset;
    int headerSize;
    int width;
    int height;
    unsigned short colorPlanes;
    unsigned short bpp;
};

class Quad
{
    public:
        Vertex verts[4];


};

class Node
{
    public:
        // variables
        Quad myQuad;
        Vertex normal;
        Node * parent;
        Node * backChild;
        Node * frontChild;

        // methods
        Vertex * isIntersected(); // only x and z are relevant - y is always 39
};

class BSPTree
{
    public:
};

/****************************************************
 * This is the matrix class which holds the number of
 * rows, the number of columns, and the data in a 2D
 * double array. It has an overloaded *= operator.
 ****************************************************/
class Matrix
{
    public:
        int    numRows;
        int    numCols;
        bool   isInit;
        double matrixData[4][4]; // row col


        /****************************************************
         * CONSTRUCTORS
         * *************************************************/

        Matrix() : numRows(0), numCols(0), isInit(false) {} // default
        Matrix(double newData[][4], int newRows, int newCols) : isInit(true) // non-default
        {
            this->numRows = newRows;
            this->numCols = newCols;

            for (int i = 0; i < numRows; i++)
            {
                for (int j = 0; j < numCols; j++)
                    matrixData[i][j] = newData[i][j];
            }
        }

        /****************************************************
         * MEMBER FUNCTIONS
         * *************************************************/
        void addTranslate(const double &x, const double &y, const double &z);
        void addRotate(AXIS_ROTATION rot, const double &degrees);
        void addScale(const double &x = 1, const double &y = 1, const double &z = 1);

        /****************************************************
         * OVERLOADED OPERATORS
         * *************************************************/

              double* operator [] (const int &index)       { return (double*)matrixData[index]; }
        const double* operator [] (const int &index) const { return (double*)matrixData[index]; }
        void operator *= (const Matrix & rhs);
        void operator =  (const Matrix & rhs)
        {
            this->numRows = rhs.numRows;
            this->numCols = rhs.numCols;

            for (int i = 0; i < numRows; i++)
            {
                for (int j = 0; j < numCols; j++)
                    matrixData[i][j] = rhs.matrixData[i][j];
            }
        }

        /****************************************************
         * FRIEND OPERATORS
         * *************************************************/
        friend Vertex operator * (const Matrix &lhs, const Vertex &rhs);
        friend Matrix operator * (const Matrix &lhs, const Matrix &rhs);
};

/******************************************************
 * Helper function for the overloaded multiply operation
 *****************************************************/
double multRowCol(const double row[], const double col[], const int & size)
{
    double result = 0;

    for (int i = 0; i < size; i++)
        result += row[i] * col[i];

    return result;
}

/******************************************************
 * Overloading the * operator - Friend to Matrix
 *****************************************************/
Vertex operator * (const Matrix & lhs, const Vertex & rhs)
{
    if (lhs.numCols > 4 || lhs.numCols < 1)
        throw "ERROR: Cannot multiply matrices of invalid sizes.";

    double tempVerts[4]    = {rhs.x, rhs.y, rhs.z, rhs.w};
    double currentVerts[4] = {rhs.x, rhs.y, rhs.z, rhs.w};

    for (int i = 0; i < lhs.numRows; i++)
        tempVerts[i] = multRowCol(lhs.matrixData[i], currentVerts, lhs.numCols);

    Vertex resultVerts = {tempVerts[0], tempVerts[1], tempVerts[2], tempVerts[3]};
    return resultVerts;
}

/********************************************************************
 * Overloading the * operator for matrix x matrix - Friend to Matrix
 * *****************************************************************/
Matrix operator * (const Matrix &lhs, const Matrix &rhs)
{
    // check to see if the "lhs" rows is equal to the "rhs" cols
    if (lhs.numCols != rhs.numRows)
        throw "ERROR: Cannot multiply matrices of invalid sizes.";

    // we need a matrix that we can change without messing up the multiplaction
    Matrix tempMatrix;

    // this loop goes through the rows of the "rhs"
    for (int i = 0; i < lhs.numRows; i++)
    {
        // this loop goes through the cols of the "lhs"
        for (int j = 0; j < rhs.numCols; j++)
        {
            // this is a 1x4 "matrix" which is pretty much just the column turned sideways to resemble a row
            double colToRow[4];

            // loop through the rows of the specified column and put it into our 1x4 matrix
            for (int k = 0; k < rhs.numRows; k++)
                colToRow[k] = rhs.matrixData[k][j];

            // use helper funciton to multiply and add the current rows and columns
            tempMatrix.matrixData[i][j] = multRowCol(lhs.matrixData[i], colToRow, lhs.numCols);
        }
    }

    tempMatrix.numRows = lhs.numRows;
    tempMatrix.numCols = rhs.numCols;
    return tempMatrix;
}

/******************************************************
 * Helper function for the overloaded multiply equals operation
 *****************************************************/
void Matrix::operator*=(const Matrix & rhs)
{
    if (this->isInit)
    {
        // Take advantage of the * operator    
        Matrix tempMatrix = *this * rhs;

        // make sure we update the rows
        this->numCols = rhs.numCols;
        
        // loop through the matrix and overwrite it with the new matrix
        for (int i = 0; i < this->numRows; i++)
        {
            for (int j = 0; j < this->numCols; j++)
                this->matrixData[i][j] = tempMatrix[i][j];
        }
    }
    else
        *this = rhs;
    
    return;
}

/*****************************************
 * ADD TRANSLATE
 * Adds a translation matrix
 * **************************************/
void Matrix::addTranslate(const double &x, const double &y, const double &z)
{
    Matrix identityTrans;
    identityTrans.matrixData[0][0] = 1;
    identityTrans.matrixData[0][1] = 0;
    identityTrans.matrixData[0][2] = 0;
    identityTrans.matrixData[0][3] = x;

    identityTrans.matrixData[1][0] = 0;
    identityTrans.matrixData[1][1] = 1;
    identityTrans.matrixData[1][2] = 0;
    identityTrans.matrixData[1][3] = y;

    identityTrans.matrixData[2][0] = 0;
    identityTrans.matrixData[2][1] = 0;
    identityTrans.matrixData[2][2] = 1;
    identityTrans.matrixData[2][3] = z;

    identityTrans.matrixData[3][0] = 0;
    identityTrans.matrixData[3][1] = 0;
    identityTrans.matrixData[3][2] = 0;
    identityTrans.matrixData[3][3] = 1;

    identityTrans.numRows = 4;
    identityTrans.numCols = 4;

    if (this->isInit)
        *this *= identityTrans;
    else
        *this = identityTrans;
}

/*****************************************
 * ADD ROTATE
 * Adds a rotation matrix
 * **************************************/
void Matrix::addRotate(AXIS_ROTATION rot, const double &degrees)
{
    Matrix identityRot;
    double rads = (degrees * M_PI / 180.0);
    double cosRot = cos(rads);
    double sinRot = sin(rads);

    identityRot.matrixData[0][0] = 1;
    identityRot.matrixData[0][1] = 0;
    identityRot.matrixData[0][2] = 0;
    identityRot.matrixData[0][3] = 0;

    identityRot.matrixData[1][0] = 0;
    identityRot.matrixData[1][1] = 1;
    identityRot.matrixData[1][2] = 0;
    identityRot.matrixData[1][3] = 0;

    identityRot.matrixData[2][0] = 0;
    identityRot.matrixData[2][1] = 0;
    identityRot.matrixData[2][2] = 1;
    identityRot.matrixData[2][3] = 0;

    identityRot.matrixData[3][0] = 0;
    identityRot.matrixData[3][1] = 0;
    identityRot.matrixData[3][2] = 0;
    identityRot.matrixData[3][3] = 1;

    identityRot.numRows = 4;
    identityRot.numCols = 4;

    switch(rot)
    {
        case X:
            identityRot.matrixData[1][1] = cosRot;
            identityRot.matrixData[2][2] = cosRot;
            identityRot.matrixData[1][2] = -sinRot;
            identityRot.matrixData[2][1] = sinRot;
            break;
        case Y:
            identityRot.matrixData[0][0] = cosRot;
            identityRot.matrixData[2][2] = cosRot;
            identityRot.matrixData[0][2] = sinRot;
            identityRot.matrixData[2][0] = -sinRot;
            break;
        case Z:
            identityRot.matrixData[0][0] = cosRot;
            identityRot.matrixData[1][1] = cosRot;
            identityRot.matrixData[0][1] = -sinRot;
            identityRot.matrixData[1][0] = sinRot;
            break;
    }

    if (this->isInit)
        *this *= identityRot;
    else
        *this = identityRot;
}

/*****************************************
 * ADD SCALE
 * Adds a scaling matrix
 * **************************************/
void Matrix::addScale(const double &x, const double &y, const double &z)
{
    Matrix identityScale;
    identityScale.matrixData[0][0] = x;
    identityScale.matrixData[0][1] = 0;
    identityScale.matrixData[0][2] = 0;
    identityScale.matrixData[0][3] = 0;

    identityScale.matrixData[1][0] = 0;
    identityScale.matrixData[1][1] = y;
    identityScale.matrixData[1][2] = 0;
    identityScale.matrixData[1][3] = 0;

    identityScale.matrixData[2][0] = 0;
    identityScale.matrixData[2][1] = 0;
    identityScale.matrixData[2][2] = z;
    identityScale.matrixData[2][3] = 0;

    identityScale.matrixData[3][0] = 0;
    identityScale.matrixData[3][1] = 0;
    identityScale.matrixData[3][2] = 0;
    identityScale.matrixData[3][3] = 1;
    
    identityScale.numRows = 4;
    identityScale.numCols = 4;

    if (this->isInit)
        *this *= identityScale;
    else
        *this = identityScale;
}

/*****************************************
 * Matrix helper functions
 * **************************************/
Matrix perspective4x4(const double &fovYDeg, const double &aspectRatio, const double &near, const double &far)
{
    Matrix rt;
    rt.numRows = 4;
    rt.numCols = 4;

    double top   = near * tan((fovYDeg * M_PI) / 180 / 2.0); // previous parenthesis after 180
    double right = aspectRatio * top;

    rt[0][0] = near / right;
    rt[0][1] = 0;
    rt[0][2] = 0;
    rt[0][3] = 0;

    rt[1][0] = 0;
    rt[1][1] = near / top;
    rt[1][2] = 0;
    rt[1][3] = 0;
    // rt[1][4] = 0; // previously un commented

    rt[2][0] = 0;
    rt[2][1] = 0;
    rt[2][2] = (far + near) / (far - near);
    rt[2][3] = (-2 * far * near) / (far - near);

    rt[3][0] = 0;
    rt[3][1] = 0;
    rt[3][2] = 1;
    rt[3][3] - 0;

    return rt;
}

/*****************************************
 * Matrix helper functions
 * **************************************/
Matrix orthogonal4x4(const double &fovYDeg, const double &aspectRatio, const double &near, const double &far)
{
    Matrix rt;
    rt.numRows = 4;
    rt.numCols = 4;

    double top   = near * tan((fovYDeg * M_PI) / 180 / 2.0); // previous parenthesis after 180
    double right = aspectRatio * top;

    rt[0][0] = 1 / right;
    rt[0][1] = 0;
    rt[0][2] = 0;
    rt[0][3] = 0;

    rt[1][0] = 0;
    rt[1][1] = 1 / top;
    rt[1][2] = 0;
    rt[1][3] = 0;
    // rt[1][4] = 0; // previously un commented

    rt[2][0] = 0;
    rt[2][1] = 0;
    rt[2][2] = (-2) / (far - near);
    rt[2][3] = - (far + near) / (far - near);

    rt[3][0] = 0;
    rt[3][1] = 0;
    rt[3][2] = 1;
    rt[3][3] - 0;

    return rt;
}

/*****************************************
 * Matrix helper functions
 * **************************************/
Matrix camera4x4(const double & offX, const double &offY, const double &offZ, const double &yaw, const double &pitch, const double &roll)
{
    Matrix trans;
    trans.addTranslate(-offX, -offY, -offZ); // 4x4 from add translate

    Matrix rotX;
    Matrix rotY;
    rotX.addRotate(X, -pitch); // 4x4 from add rotate // previous positive pitch
    rotY.addRotate(Y, -yaw);   // 4x4 from add rotate // previous positive yaw

    Matrix rt = rotX * rotY * trans;

    // Matrix rt;
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
                // for(int y = 0; y < h; y++)
                // {
                //     free(grid[y]);
                // }
                free(grid);
                return;
            }

            // // De-Allocate pointers for column references
            // free(grid);

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
            ourSurfaceInstance = false;
            if(!readBMP(path))
                return;

            ourBufferData = true;
        }
};

/****************************************************
 * Combine two datatypes into one
 ***************************************************/
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

        /*******************************************************
         * PUBLIC MEMBER VARIABLES
         * ****************************************************/
        PIXEL  color; 
        attrib attrValues[16];
        int    numValues; // number of values to interpolate (3 for rgb, 2 for UV, etc.)
        void*  pointerImg; // address -> pointer without a base type kept because of DEPRECATED CODE!!!
        // double attrValues[5]; // according to the slides, we will likely have at most 5 attribute values

        Matrix matrix;

        /*******************************************************
         * CONSTRUCTORS
         * ****************************************************/
        Attributes() : numValues(0), pointerImg(NULL) {} // Obligatory empty constructor

        // Needed by clipping (linearly interpolated Attributes between two others)
        Attributes(const Attributes & first, const Attributes & second, const double & along)
        {
            // Your code goes here when clipping is implemented
            numValues = first.numValues;

            for (int i = 0; i < this->numValues; i++)
                attrValues[i].d = (first[i].d) + ((second[i].d - first[i].d) * along);
        }

        /*******************************************************
         * MEMBER FUNCTIONS
         * ****************************************************/
        void interpolateValues(const double & det1, const double & det2, const double & area, Attributes* vertAttrs, const Vertex vertices[3])
        {
            double w1 = det1 / area;
            double w2 = det2 / area;
            double w3 = 1 - w2 - w1;
            double z  = 1.0 / (vertices[0].w * w2 + vertices[1].w * w3 + vertices[2].w * w1);

            for (int i = 0; i < numValues; i++)
            {
                attrValues[i].d = (vertAttrs[0].attrValues[i].d * w2 +
                                   vertAttrs[1].attrValues[i].d * w3 +
                                   vertAttrs[2].attrValues[i].d * w1) * z;
            }
        }

        void concatMatrices    (const Matrix & transform) { this->matrix *= transform; }
        void setMatrix(double newData[][4], int numRows, int numCols)
        {
            matrix.numRows = numRows;
            matrix.numCols = numCols;

            for (int i = 0; i < numRows; i++)
            {
                for (int j = 0; j < numCols; j++)
                    matrix.matrixData[i][j] = newData[i][j];
            }
        }

        void insertDbl(const double & d) { attrValues[numValues++].d =   d;   }
        void insertPtr(void * ptr      ) { attrValues[numValues++].ptr = ptr; }

        /*******************************************************
         * OVERLOADED OPERATORS
         * ****************************************************/

              attrib & operator[](const int & i)       { return attrValues[i]; }
        const attrib & operator[](const int & i) const { return attrValues[i]; }
};	

// Example of a fragment shader
void DefaultFragShader(PIXEL & fragment, const Attributes & vertAttr, const Attributes & uniforms)
{
    // Output our shader color value, in this case red
    fragment = 0xffff0000;
}

void GrayFragShader(PIXEL & fragment, const Attributes & vertAttr, const Attributes & uniforms)
{
    PIXEL avgChannel = ((vertAttr.color >> 16) && 0xff) + ((vertAttr.color >> 8) && 0xff) + ((vertAttr.color) && 0xff);
    
    avgChannel /= 3;
    fragment    = 0xff000000 + (avgChannel << 16) + (avgChannel << 8) + avgChannel;
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
