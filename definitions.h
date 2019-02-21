#define SDL_MAIN_HANDLED
#include "SDL2/SDL.h"
#include "stdlib.h"
#include "stdio.h"
#include "math.h"
#include <map>

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

/****************************************************
 * Matrix class
 ***************************************************/
class Matrix
{
private:
    double *neo;
    int row;
    int col;
    bool init;

public:

    Matrix(): row(0), col(0), neo(NULL), init(false) {};

    //non default constructor
    Matrix(int row, int col)
    {
        this->row = row;
        this->col = col;
        neo = new double[row * col];

        for(int i =  0; i < (row * col); i++)
        {
            neo[i] = 0;
        }
        init = false;
    }
    //destructor
    ~Matrix()
    {
        if(neo!=NULL)
        {
            delete [] neo;
            neo = NULL;
        }
    }

    /****************************************************
    * Scales the matrix by what ever is passed in for 
    * x y and z
    ***************************************************/
    void scale(double x, double y, double z)
    {
        //acts as vert to multiply matrix by
        double temp[4] = {x,y,z,1};

        Matrix scaler(this->row, this->col);

        for(int i = 0; i < this->row; i++)
        {
            scaler.neo[i * this->col + i] = temp[i];
        }
        if(init == false)
        {
            for(int i = 0; i < (this->row * this->col); i++)
            {
                this->neo[i] = scaler.neo[i];
            }

            init = true;
        }
        else{
            *this *= scaler;
        }
    }

    /****************************************************
    * translates the matrix
    ***************************************************/
    void translate(double x, double y, double z)
    {
        Matrix translator(this->row, this->col);
        translator.neo[1 * this->col - 1] = x;
        translator.neo[2 * this->col - 1] = y;
        translator.neo[3 * this->col - 1] = z;

        for(int i = 0; i < this->row; i++)
        {
            translator.neo[i * this->col + i] = 1;
        }

        if(init == false)
        {
            for(int i = 0; i < (this->row * this->col); i++)
            {
                this->neo[i] = translator.neo[i];
            }

            init = true;
        }
        else{
            *this *= translator;
        }

    }

    /****************************************************
    * Rotates the matrix based on the "degree" passed in
    ***************************************************/
    void rotate(double r)
    {
        Matrix rotator(this->row, this->col);
        double rsin = sin(r * M_PI/180.0);
        double rcos = cos(r * M_PI/180.0);

        rotator.neo[0] = rcos;
        rotator.neo[1] = -rsin;
        rotator.neo[1 * this->col] = rsin;
        rotator.neo[1 * this->col + 1] = rcos;
        rotator.neo[2 * this->col + 2] = 1;
        rotator.neo[3 * this->col + 3] = 1;


        if(init == false)
        {
            for(int i = 0; i < (this->row * this->col); i++)
            {
                this->neo[i] = rotator.neo[i];
            }

            init = true;
        }
        else{
            *this *= rotator;
        }

    }

    //overloaded assignmed operator
    Matrix & operator = (const Matrix & rhs)
    {
        this->row = rhs.row;
        this->col = rhs.col;
        this->init = rhs.init;
        if(this->neo != NULL)
        {
            delete [] neo;
        }

        neo = new double[this->row * this->col];

        for(int i = 0; i < (this->row * this->col); i++)
        {
            this->neo[i] = rhs.neo[i];
        }
        return *this;
    }

    //overloaded operator for matrix multiplication
    Matrix & operator *= (const Matrix & rhs)
    {
        Matrix temp(this->row, rhs.col);

        for(int i = 0; i < this->row; i++)
        {
            for(int j = 0; j < rhs.col; j++)
            {
                double total = 0;

                for(int k = 0; k < rhs.row; k++)
                {
                     total += this->neo[i * this->col + k] * rhs.neo[k * rhs.col + j];
                }
                temp.neo[i * temp.col + j] = total;
            }
        }


        temp.init = true;
        *this = temp;
        return *this;
    }
    //allows access to private variables
    friend Vertex operator * (const Matrix & rhs, const Vertex & lhs);
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

/***************************************************
 * ATTRIBUTES (shadows OpenGL VAO, VBO)
 * The attributes associated with a rendered 
 * primitive as a whole OR per-vertex. Will be 
 * designed/implemented by the programmer. 
 **************************************************/
//template class T
class Attributes
{     
    private:
        std::map<std::string, double> mapper; 
    public:
        
        double getR() const {return mapper.at("r");}
        void setR(double r) {mapper["r"] = r;}

        double getG() const {return mapper.at("g");}
        void setG(double g) {mapper["g"] = g;}

        double getB() const {return mapper.at("b");}
        void setB(double b) {mapper["b"] = b;}

        double getU() const {return mapper.at("u");}
        void setU(double u) {mapper["u"] = u;}

        double getV() const {return mapper.at("v");}
        void setV(double v) {mapper["v"] = v;}


        void* ptrImg;
        PIXEL color;
        Matrix atMat;
        // Obligatory empty constructor
        Attributes() 
        {
            mapper.insert(std::pair<std::string, double>("r",0.0));
            mapper.insert(std::pair<std::string, double>("g",0.0));
            mapper.insert(std::pair<std::string, double>("b",0.0));
            mapper.insert(std::pair<std::string, double>("u",0.0));
            mapper.insert(std::pair<std::string, double>("v",0.0));

        }

        // Needed by clipping (linearly interpolated Attributes between two others)
        Attributes(const Attributes & first, const Attributes & second, const double & valueBetween)
        {
            // Your code goes here when clipping is implemented
        }
};	
void ImageFragShader (PIXEL & fragment, const Attributes & vertAttr, const Attributes & uniforms)
{
    BufferImage* bf = (BufferImage*)uniforms.ptrImg;
    int x = vertAttr.getU() * (bf->width()-1);
    int y = vertAttr.getV() * (bf->height()-1);

    //PIXEL color = 0xff00ff00;

    fragment = (*bf)[y][x]; //& color

}

void ColorFragShader(PIXEL & fragment, const Attributes & vertAttr, const Attributes & uniforms)
{
    PIXEL color = 0xff000000;
    color += (unsigned int)(vertAttr.getR() *0xff) << 16;
    color += (unsigned int)(vertAttr.getG() *0xff) << 8;
    color += (unsigned int)(vertAttr.getB() *0xff) << 0;

    fragment = color;
}
// Example of a fragment shader
void DefaultFragShader(PIXEL & fragment, const Attributes & vertAttr, const Attributes & uniforms)
{
    // Output our shader color value, in this case red
    fragment = 0xffff0000;
}

void GreenShader(PIXEL & fragment, const Attributes & vertAttr, const Attributes & uniforms)
{
    BufferImage* bf = (BufferImage*)uniforms.ptrImg;
    int x = vertAttr.getU() * (bf->width()-1);
    int y = vertAttr.getV() * (bf->height()-1);

    // int a = 0;
    // a += vertAttr.getR();
    // a += vertAttr.getB();
    // a += vertAttr.getG();

    // a /= 3;

    // PIXEL color1 = 0xff000000;

    // color1 += a << 8;


    

    PIXEL color = 0xff00ff00;

    fragment = ((*bf)[y][x]) & color; //& color

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

void changerShader(Vertex & vertOut, Attributes & attrOut, const Vertex & vertIn, const Attributes & vertAttr, const Attributes & uniforms)
        {
            vertOut = uniforms.atMat * vertIn;
            attrOut = vertAttr;
        }




/**************************************************************
 * overloaded * operator
 *************************************************************/
Vertex operator * (const Matrix & rhs, const Vertex & lhs)
{
    Vertex vert;
    if(rhs.col !=4 || rhs.row !=4)
    {
        
        return vert;
    }
    vert.x = rhs.neo[0] * lhs.x + rhs.neo[1] * lhs.y + rhs.neo[2] * lhs.z + rhs.neo[3] * lhs.w;
    vert.y = rhs.neo[4] * lhs.x + rhs.neo[5] * lhs.y + rhs.neo[6] * lhs.z + rhs.neo[7] * lhs.w;
    vert.z = rhs.neo[8] * lhs.x + rhs.neo[9] * lhs.y + rhs.neo[10] * lhs.z + rhs.neo[11] * lhs.w;
    vert.w = rhs.neo[12] * lhs.x + rhs.neo[13] * lhs.y + rhs.neo[14] * lhs.z + rhs.neo[15] * lhs.w;

    return vert;
}


/****************************************
 * CROSS_PRODUCT
 * returns the determinant 
 ***************************************/
double crossProduct(double v1x, double v1y, double v2x, double v2y)
{
    
    return (v1x*v2y - v1y*v2x);
}

/****************************************
 * INTERP
 * Linear interpolation between three points
 ***************************************/
double interp(double area, double d1, double d2, double d3, double att1, double att2, double att3)
{
    

    //mix in the attribute with the how much it makes up of the triangle. 
    return ((d1 * att3) + (d2 * att1) + (d3 * att2)) / area;
}


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
