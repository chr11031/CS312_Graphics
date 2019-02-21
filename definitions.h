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
#define MEMBERS_PER_ATTRIB
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
                for(int y = 0; y < h; y++)
                {
                    free(grid[y]);
                }
            }

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
	  ourSurfaceInstance = false;
	  if(!readBMP(path))
	    {
	      return;
	    }	  
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

        // Here we declare an array of vars to be interpolated however we like. 16 is more than enough.
        float vars[16];

        // Here we are declaring a variable to optomize how many times we need to interopolate our items later
        int maxVar = 0;

        // Here we have a void pointer to be used how we wish. Most likely to be used for a buffer image.
        void* ptr;
        void* ptr2;
};	

void GreenFragShader(PIXEL & fragment, const Attributes & vertAttr, const Attributes & uniforms) 
{   
    PIXEL color = ((int)vertAttr.vars[0] << 24) + ((int)vertAttr.vars[1] << 16) + ((int)vertAttr.vars[2] << 8) + (vertAttr.vars[3]);
    fragment = color & 0xff00ff00;
}

void GrayScaleFragShader(PIXEL & fragment, const Attributes & vertAttr, const Attributes & uniforms) 
{   
    PIXEL color = ((int)vertAttr.vars[0] << 24) + ((int)vertAttr.vars[1] << 16) + ((int)vertAttr.vars[2] << 8) + ((int)vertAttr.vars[3]);
    int r = (color >> 16) & 0x000000ff;
    int g = (color >> 8) & 0x000000ff;
    int b = (color) & 0x000000ff;

    int avgChannel = (r + g + b) / 3;
    fragment = 0xff00ff00 + (avgChannel << 16) + (avgChannel << 8) + avgChannel;
}

void colorFragShader(PIXEL & fragment, const Attributes & vertAttr, const Attributes & uniforms)
{
    fragment = 0xff000000 | ((int)vertAttr.vars[0] << 16) | ((int)vertAttr.vars[1] << 8) | (int)vertAttr.vars[2];
}

void checkerShader(PIXEL & fragment, const Attributes & vertAttr, const Attributes & uniforms)
{

    static BufferImage*  myImage = NULL;
    if(myImage == NULL)
        myImage = new BufferImage("./checker.bmp");
    (*myImage)[0] = (*myImage)[1];

    BufferImage* bf = (BufferImage*)uniforms.ptr;
    int x = vertAttr.vars[0] * (bf->width()-1);
    int y = vertAttr.vars[1] * (bf->height()-1);

    if(x < 0 || x > 255)
    x = 0;
    if(y < 0 || y > 255)
    y = 0;

    if (*(myImage)[y][x] == 0xffff0000)
        fragment = (*bf)[y][x];
}

void imgFragShader(PIXEL & fragment, const Attributes & vertAttr, const Attributes & uniforms)
{
    BufferImage* bf = (BufferImage*)uniforms.ptr;
    int x = vertAttr.vars[0] * (bf->width()-1);
    int y = vertAttr.vars[1] * (bf->height()-1);

    if(x < 0 || x > 255)
    x = 0;
    if(y < 0 || y > 255)
    y = 0;

    fragment = (*bf)[y][x];
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

class Matrix
{
    private:
       
    public:
        double neo[16];
        Matrix(double neo1, double neo2, double neo3, double neo4, 
               double neo5, double neo6, double neo7, double neo8,
               double neo9, double neo10, double neo11, double neo12,
               double neo13, double neo14, double neo15, double neo16)
        {
            neo[0] = neo1;
            neo[1] = neo2;
            neo[2] = neo3;
            neo[3] = neo4;
            neo[4] = neo5;
            neo[5] = neo6;
            neo[6] = neo7;
            neo[7] = neo8;
            neo[8] = neo9;
            neo[9] = neo10;
            neo[10] = neo11;
            neo[11] = neo12;
            neo[12] = neo13;
            neo[13] = neo14;
            neo[14] = neo15;
            neo[15] = neo16;
        }

        Matrix(Attributes uniforms) 
        {
            for(int i = 0; i < 16; i++)
            {
                neo[i] = uniforms.vars[i];
            }      
            if (neo[0] == 0)
                neo[0] = 1;
            if (neo[5] == 0)
                neo[5] = 1;
            if (neo[10] == 0)
                neo[10] = 1;
            if (neo[15] == 0)
                neo[15] = 1;
        }

        Matrix()
        {
            for(int i = 0; i < 16; i++)
            {
                neo[i] = 0;
            }
            if (neo[0] == 0)
                neo[0] = 1;
            if (neo[5] == 0)
                neo[5] = 1;
            if (neo[10] == 0)
                neo[10] = 1;
            if (neo[15] == 0)
                neo[15] = 1;
        }

        double const get(int index) {return neo[index];}

        void reset() 
        {
            for(int i = 0; i < 16; i++)
            {
                neo[i] = 0;
            }
            if (neo[0] == 0)
                neo[0] = 1;
            if (neo[5] == 0)
                neo[5] = 1;
            if (neo[10] == 0)
                neo[10] = 1;
            if (neo[15] == 0)
                neo[15] = 1;
        }

        void translate(double x, double y)
        {
            neo[3] = x;
            neo[7] = y;
        }

        void scale(double s)
        {
            neo[0] *= s;
            neo[5] *= s;
            //neo[10] *= s;

        }

        void rotate(double deg)
        {
            double rad = deg * .017453;
            neo[0] = cos(rad);
            neo[1] = -sin(rad);
            neo[4] = sin(rad);
            neo[5] = cos(rad);
        }

        Matrix operator = (Matrix const & R)
        {
            for(int i = 0; i < 16; i++)
            {
                this->neo[i] = R.neo[i];
            }
        }

        Matrix operator * (Matrix const & R) 
        {
            Matrix newMatr( (this->neo[0] * R.neo[0]) + (this->neo[1] * R.neo[4]) + (this->neo[2] * R.neo[8]) + (this->neo[3] * R.neo[12]),
                            (this->neo[0] * R.neo[1]) + (this->neo[1] * R.neo[5]) + (this->neo[2] * R.neo[9]) + (this->neo[3] * R.neo[13]),
                            (this->neo[0] * R.neo[2]) + (this->neo[1] * R.neo[6]) + (this->neo[2] * R.neo[10]) + (this->neo[3] * R.neo[14]),
                            (this->neo[0] * R.neo[3]) + (this->neo[1] * R.neo[7]) + (this->neo[2] * R.neo[11]) + (this->neo[3] * R.neo[15]),
                            (this->neo[4] * R.neo[0]) + (this->neo[5] * R.neo[4]) + (this->neo[6] * R.neo[8]) + (this->neo[7] * R.neo[12]),
                            (this->neo[4] * R.neo[1]) + (this->neo[5] * R.neo[5]) + (this->neo[6] * R.neo[9]) + (this->neo[7] * R.neo[13]),
                            (this->neo[4] * R.neo[2]) + (this->neo[5] * R.neo[6]) + (this->neo[6] * R.neo[10]) + (this->neo[7] * R.neo[14]),
                            (this->neo[4] * R.neo[3]) + (this->neo[5] * R.neo[7]) + (this->neo[6] * R.neo[11]) + (this->neo[7] * R.neo[15]),
                            (this->neo[8] * R.neo[0]) + (this->neo[9] * R.neo[4]) + (this->neo[10] * R.neo[8]) + (this->neo[11] * R.neo[12]),
                            (this->neo[8] * R.neo[1]) + (this->neo[9] * R.neo[5]) + (this->neo[10] * R.neo[9]) + (this->neo[11] * R.neo[13]),
                            (this->neo[8] * R.neo[2]) + (this->neo[9] * R.neo[6]) + (this->neo[10] * R.neo[10]) + (this->neo[11] * R.neo[14]),
                            (this->neo[8] * R.neo[3]) + (this->neo[9] * R.neo[7]) + (this->neo[10] * R.neo[11]) + (this->neo[11] * R.neo[15]),
                            (this->neo[12] * R.neo[0]) + (this->neo[13] * R.neo[4]) + (this->neo[14] * R.neo[8]) + (this->neo[15] * R.neo[12]),
                            (this->neo[12] * R.neo[1]) + (this->neo[13] * R.neo[5]) + (this->neo[14] * R.neo[9]) + (this->neo[15] * R.neo[13]),
                            (this->neo[12] * R.neo[2]) + (this->neo[13] * R.neo[6]) + (this->neo[14] * R.neo[10]) + (this->neo[15] * R.neo[14]),
                            (this->neo[12] * R.neo[3]) + (this->neo[13] * R.neo[7]) + (this->neo[14] * R.neo[11]) + (this->neo[15] * R.neo[15]));
            return newMatr;
        } 

        Vertex operator * (Vertex R) 
        {
            Vertex newVert;
            newVert.x = (R.x * this->neo[0]) + (R.y * this->neo[1]) + (R.z * this->neo[2]) + (R.w * this->neo[3]);
            newVert.y = (R.x * this->neo[4]) + (R.y * this->neo[5]) + (R.z * this->neo[6]) + (R.w * this->neo[7]);
            newVert.z = (R.x * this->neo[8]) + (R.y * this->neo[9]) + (R.z * this->neo[10]) + (R.w * this->neo[11]);
            newVert.w = (R.x * this->neo[12]) + (R.y * this->neo[13]) + (R.z * this->neo[14]) + (R.w * this->neo[15]);
            return newVert;
        }
};

void transformationVertShader(Vertex & vertOut, Attributes & attrOut, const Vertex & vertIn, const Attributes & vertAttr, const Attributes & uniforms)
{   
    vertOut = (* (Matrix *)uniforms.ptr2) * vertIn;
    attrOut = vertAttr;
}

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
