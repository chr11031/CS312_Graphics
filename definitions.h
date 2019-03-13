#define SDL_MAIN_HANDLED
#include "SDL2\SDL.h"
#include "stdlib.h"
#include "stdio.h"
#include "math.h"
#include "graphicMatrix.h"
//#include "shaders.h"

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

//set aside numbers
#define X_Key 0
#define y_Key 1

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
 * X, Y, Z enums
 ***************************************************/
enum DIMENSION
{
    X = 0,
    Y = 1,
    Z = 2
};

/***************************************************
 * camera controls
 ***************************************************/
struct camControls
{
    double x = 0;
    double y = 0;
    double z = 0;
    double pitch = 0;
    double yaw = 0;
    double roll = 0;
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
        bool ourSurfaceInstance = false;    // Do we need to de-allocate? (2dl)
        bool ourBufferData = false;

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
            /*ourSurfaceInstance = true;
            SDL_Surface* tmp = SDL_LoadBMP(path);      
            SDL_PixelFormat* format = SDL_AllocFormat(SDL_PIXELFORMAT_ARGB8888);
            img = SDL_ConvertSurface(tmp, format, 0);
            SDL_FreeSurface(tmp);
            SDL_FreeFormat(format);
            //May need
            //SDL_LockSurface(img);
            setupInternal();*/
            ourSurfaceInstance = false;
	        if(!readBMP(path))
	        {
	            return;
	        }	  
        }
};

//Attribute struct union
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
        int numMembers = 0;
        attrib att[16];//0R 1G 2B 3U 4V --

        //Matrix trans = Matrix(4,4);

        //for image
        void* ptrImg;
        double size = 3;
        PIXEL color = 0;

        // Obligatory empty constructor
        Attributes() { /*numMembers = 0;*/ }


        // Needed by clipping (linearly interpolated Attributes between two others)
        Attributes(const Attributes & first, const Attributes & second, const double & along)
        {
            numMembers = first.numMembers;
            for (int i = 0; i < numMembers; i++)
            {
                att[i].d = (first.att[i].d) + ((second.att[i].d - first.att[i].d) * along); 
            }
        }

        // Const Return operator
        const double & operator[](const int & i) const
        {
            return att[i].d;
        }

        // Return operator
        double & operator[](const int & i) 
        {
            return att[i].d;
        }

        // Insert Double Into Container
        void insertDbl(const double & d)
        {
            att[numMembers].d = d;
            numMembers += 1;
        }
    
        // Insert Pointer Into Container
        void insertPtr(void * ptr)
        {
            att[numMembers].ptr = ptr;
            numMembers += 1;
        }

};	
       
#endif
