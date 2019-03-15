#define SDL_MAIN_HANDLED
#include "SDL2/SDL.h"
#include "stdlib.h"
#include "stdio.h"
#include <math.h>

#ifndef DEFINITIONS_H
#define DEFINITIONS_H

/******************************************************
 * DEFINES:
 * Macros for universal variables/hook-ups.
 *****************************************************/
#define WINDOW_NAME "James' Pipeline"
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

// view/camera transform
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
        Buffer2D(){}

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

        // The frequented operator for grabbing pixels
        inline T* & operator[] (int i)
        {
            return grid[i];
        }
};

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

// Interpoaltes between 3 weighted points 3 different double values for barycentric
double baryInterp(const double & firstWgt, const double & secndWgt, const double & thirdWgt,
			 const double & firstVal, const double & secndVal, const double & thirdVal)
{
    return ((firstWgt * thirdVal) + (secndWgt * firstVal) + (thirdWgt * secndVal));
}

union attrib
{
    double d;
    void *ptr;
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
        void *image;
        Vertex *vert;
        double rgb[3];
        double uv[2];
        // Matrix transforms[3];

        // New stuff
        int numMembers = 0;
        attrib arr[16];

        // Obligatory empty constructor
        Attributes() 
        {
            // transforms[0].initializeMatrix(4, 4);
            // transforms[1].initializeMatrix(4, 4);
            // transforms[2].initializeMatrix(4, 4);

            rgb[0] = 0;
            rgb[1] = 0;
            rgb[2] = 0;

            uv[0] = 0;
            uv[1] = 0;

            image = NULL;
            vert = NULL;

            color = 0xff000000;
            numMembers = 0;
        }

        // Interpolation Constructor
        Attributes( const double & firstWgt, const double & secndWgt, const double & thirdWgt, 
                    const Attributes & first, const Attributes & secnd, const Attributes & third,
		            const double & correctZ)
        {
            while(numMembers < first.numMembers)
            {
                arr[numMembers].d = baryInterp(firstWgt, secndWgt, thirdWgt, first.arr[numMembers].d, secnd[numMembers].d, third.arr[numMembers].d);
			    arr[numMembers].d = arr[numMembers].d * correctZ;
			    numMembers += 1;
            }
        }

        // Needed by clipping (linearly interpolated Attributes between two others)
        Attributes(const Attributes & first, const Attributes & second, const double & along)
        {
            // Your code goes here when clipping is implemented
            numMembers = first.numMembers;
            for (int i = 0; i < numMembers; i++)
            {
                arr[i].d = (first[i].d + ((second[i].d - first[i].d) * along));
            }
        }

        ~Attributes()
        {

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

/*******************************************************
 * CROSS_PRODUCT
 * calculates the cross product of the given vertices.
 ******************************************************/
float determinant(double v1[2], double v2[2])
{
    return ((v1[0] * v2[1]) - (v1[1] * v2[0]));
}

/****************************************
 * DETERMINANT
 * Find the determinant of a matrix with
 * components A, B, C, D from 2 vectors.
 ***************************************/
double determinant(const double & A, const double & B, const double & C, const double & D)
{
  return (A*D - B*C);
}

/***************************************************************************
 * LINEAR INTERPOLATION
 * linearly interpolates the values passed in based on the area of the
 * triangles.
 **************************************************************************/
double lerp(float area, float area1, float area2, float area3, double attr1, double attr2, double attr3)
{
    double lerpedAttr = 0.0;
    lerpedAttr = (area1/area * attr1) + (area2/area * attr2) + (area3/area * attr3);
    return lerpedAttr;
}

// Example of a fragment shader
void DefaultFragShader(PIXEL & fragment, const Attributes & vertAttr, const Attributes & uniforms)
{
    // Output our shader color value, in this case red.
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
