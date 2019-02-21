#define SDL_MAIN_HANDLED
#include "SDL2/SDL.h"
#include "stdlib.h"
#include "stdio.h"
#include "math.h"
#include <map>
#include <string>

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

// Combine two datatypes in one
union attrib
{
  double d;
  void* ptr;
};

/****************************************************
 * Matrix class
 *  This class is ment to represent a Matrix
 *  in math, this will be utalized to translate, 
 *  scale, and rotate vertices locations
 *  so that they can be manipulated appropriately
 * *****************************************************/
class Matrix
{
    public:

    //Variables
    double data[4][4];
    int rows;
    int cols;

    //D Const
    Matrix() { setRows(0); setCols(0); }
    //N Const
    Matrix(double input[][4], int r, int c) { setInfo(input, r, c); }


    //Getters
    int getRows(){return rows;}
    int getCols(){return cols;}

    //Setters
    void setRows(int r){ this->rows = r; }
    void setCols(int c){ this->cols = c; }
    void setData(double input[][4], int r, int c)
    {
        for(int x = 0; x < r; x++)
            for(int y = 0; y < c; y++)
                data[x][y] = input[x][y];
    }

    //Fucntions
    void setInfo(double input[][4], int r, int c)
    {
        setRows(r);
        setCols(c);
        setData(input, r, c);
    }

    //Overloaded Operators
    void operator =(const Matrix & rhs) 
    { 
        //setInfo(rhs.data, rhs.getRows(), rhs.getCols()); 
        for (int i = 0; i < rhs.rows; i++)
            for (int j = 0; j < rhs.cols; j++)
                this->data[i][j] = rhs.data[i][j];

        setRows(rhs.rows);
        setCols(rhs.cols);
    }
    void operator *=(const Matrix & rhs);
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
        PIXEL color;

        // Utalizing a map for flexability
        std::map<std::string, attrib> myMap;
        Matrix matrix;

        // Obligatory constructor
        Attributes()
        {
            attrib empty, iEmpty;
            empty.d = 0.0;
            myMap.insert(std::pair<std::string, attrib>("u", empty));
            myMap.insert(std::pair<std::string, attrib>("v", empty));
            myMap.insert(std::pair<std::string, attrib>("r", empty));
            myMap.insert(std::pair<std::string, attrib>("g", empty));
            myMap.insert(std::pair<std::string, attrib>("b", empty));
            myMap.insert(std::pair<std::string, attrib>("img", iEmpty));
        }

        // Needed by clipping (linearly interpolated Attributes between two others)
        Attributes(const Attributes & first, const Attributes & second, const double & valueBetween)
        {
            // Your code goes here when clipping is implemented
        }

        // Getters
        // Colors
        double getRed() const {
            return myMap.at("r").d;
        }

        double getGreen() const {
            return myMap.at("g").d;
        }

        double getBlue() const {
            return myMap.at("b").d;
        }

        // Bitmap coor
        double getBU() const {
            return myMap.at("u").d;
        }

        double getBV() const {
            return myMap.at("v").d;
        }

        // Img stuff
        void* getImg() const {
            return myMap.at("img").ptr;
        }

        // Setters
        // Colors
        void setRed(double value) {
            myMap["r"].d = value;
        }

        void setGreen(double value) {
            myMap["g"].d = value;
        }

        void setBlue(double value) {
            myMap["b"].d = value;
        }

        void setColor(double r, double g, double b) {
            setRed(r);
            setGreen(g);
            setBlue(b);
        }

        // Bitmap coor
        void setBU(double u) {
            myMap["u"].d = u;
        }

        void setBV(double v) {
            myMap["v"].d = v;
        }

        void setCoor(double u, double v) {
            setBU(u);
            setBV(v);
        }
        
        // Img stuff
        void setImg(void* img) {
            myMap["img"].ptr = img;
        }
};

/****************************************
 * The operator overloading from the
 * Matrix class are being defined
 ***************************************/
void Matrix::operator*=(const Matrix & rhs)
{
    // Initalizing variables
    double temp[4][4];
    double tempVal = 0;

    // Checks if we can do that matrix math in the first place
    if (rhs.cols != getRows())
        throw "ERROR: Invalid sizes\n";

    // Loop through the number of rows in the RHS
    for (int x = 0; x < rhs.rows; x++)
    {
        // Loop through the number of cols in LHS
        for (int y = 0; y < getCols(); y++)
        {
            double transfer[4];

            //Flatten the LHS cols into a row for maths
            for (int i = 0; i < getRows(); i++)
                transfer[i] = this->data[i][y];

            // Multiplies the rows and colloums
            for (int i = 0; i < rhs.cols; i++)
                tempVal += rhs.data[x][i] * transfer[i];

            temp[x][y] = tempVal;
        }
    }
    
    // Changes the rows for the matrix
    setRows(rhs.rows);

    // Replaces the old matrix
    for (int x = 0; x < getRows(); x++)
        for (int y = 0; y < getCols(); y++)
            this->data[x][y] = temp[x][y];
}

/******************************************************
 * MATRIX MULT OVERLOADED OPERATOR
 * Multiplies the two matricies together
 *****************************************************/ 
Vertex operator * (const Vertex & lhs, const Matrix & rhs)
{
    // Declars variables
    double tempVal = 0;
    Vertex ret = {0,0,0,0};
    double newVerts[4];
    double oldVerts[4] = {lhs.x, lhs.y, lhs.z, lhs.w};

    // Checks if we can do it
    if (rhs.cols < 1 || rhs.cols > 4)
        throw "ERROR: Invalid sizes";

    // Runs thought the rows, do math and set the new verts to thoes values
    for (int i = 0; i < rhs.rows; i++)
    {
        for (int j = 0; j < rhs.cols; j++)
                tempVal += rhs.data[i][j] * oldVerts[j];
                
        newVerts[i] = tempVal;
    }

    // Fixes the return vertex and returns it 
    ret.x = newVerts[0];
    ret.y = newVerts[1];
    ret.z = newVerts[2];
    ret.w = newVerts[3];
    return ret;
}

// Image Fragment Shader 
void ImageFragShader(PIXEL & fragment, const Attributes & vertAttr, const Attributes & uniforms)
{
    // Creats a buffer for the image
    BufferImage* bf = (BufferImage*)uniforms.getImg();

    int x = vertAttr.getBU() * (bf->width()-1);
    int y = vertAttr.getBV() * (bf->height()-1);

    fragment = (*bf)[y][x];
}

// My Fragment Shader for color interpolation
void ColorFragShader(PIXEL & fragment, const Attributes & vertAttr, const Attributes & uniforms)
{
    // Output our shader color value, in this case red
    PIXEL color = 0xff000000;


    color += (unsigned int)(vertAttr.getRed() *0xff) << 16;
    color += (unsigned int)(vertAttr.getGreen() *0xff) << 8;
    color += (unsigned int)(vertAttr.getBlue() *0xff) << 0;

    fragment = color;
}

// Example of a fragment shader 
void DefaultFragShader(PIXEL & fragment, const Attributes & vertAttr, const Attributes & uniforms)
{
    // Output our shader color value, in this case red
    fragment = 0xffff0000;
}

// Frag Shader for UV without image (due to SDL2 bug?)
void FragShaderUVwithoutImage(PIXEL & fragment, const Attributes & attributes, const Attributes & uniform)
{
    // Figure out which X/Y square our UV would fall on
    int xSquare = attributes.getBU() * 8;
    int ySquare = attributes.getBV() * 8;

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

/****************************************************** *
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

// A vertex shader that transforms based off of a matrix
void TransformVertexShader(Vertex & vertOut, Attributes & attrOut, const Vertex & vertIn, const Attributes & vertAttr, const Attributes & uniforms)
{
    vertOut = vertIn * uniforms.matrix;
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
 * Find the determinant of a matrix with
 * X, Y components from 2 vectors.
 ***************************************/
inline double determinant(const double & V1x, const double & V2x, const double & V1y, const double & V2y)
{
  return ((V1x * V2y) - (V1y * V2x));
}

/****************************************************
 * Interpolation
 *  Finds the point on the triangle
 *  Then figgures out the color value based off of the 
 *  attributes and 
 * *****************************************************/
double interp(double area, double *det, double attrs1, double attrs2, double attrs3)
{
    return ((det[0] / area) * attrs3) + ((det[1] / area) * attrs1) + ((det[2] / area) * attrs2);
}

#endif
 