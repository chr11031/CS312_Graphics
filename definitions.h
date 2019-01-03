#define SDL_MAIN_HANDLED
#include "SDL2/SDL.h"
#include "stdlib.h"
#include "stdio.h"
#include "math.h"

#ifndef DEFINITIONS_H
#define DEFINITIONS_H

/******************************************************
 * DEFINES:
 *  -Macros for universal variables/hook-ups
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

// Camera values
#define CAM_INCREMENT 0.01
#define STEP_INCREMENT 1
double camX = 0;
double camY = 0;
double camZ = 0;
double camYaw = 0;
double camRoll = 0;
double camPitch = 0;

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
            for(int i = 0; i < h; i++)
            {
                grid[i] = row;
                row += w;                    
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
 * VARIABLE_BUFFER_OBJECT (shadows OpenGL VBO)
 * The attributes associated with a rendered 
 * primitive OR per-vertex attributes. Will be 
 * designed/implemented by the programmer. 
 **************************************************/
class VBO
{      
    public:
        // Obligatory empty constructor
        VBO() {}

        // Needed by clipping (linearly interpolated VBO between two others)
        VBO(const VBO & first, const VBO & second, const double & valueBetween)
        {
            // Your code goes here when clipping is implemented
        }
};	

// Example of a fragment shader
void DefaultFragShader(PIXEL & fragment, const VBO & attr, const VBO & uniforms)
{
    // Output our shader color value
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
        void (*FragShader)(PIXEL & fragment, const VBO & attr, const VBO & uniforms);

        // Assumes simple monotone RED shader
        FragmentShader()
        {
            FragShader = DefaultFragShader;
        }

        // Initialize with a fragment callback
        FragmentShader(void (*FragSdr)(PIXEL & fragment, const VBO & attr, const VBO & uniforms))
        {
            setShader(FragSdr);
        }

        // Set the shader to a callback function
        void setShader(void (*FragSdr)(PIXEL & fragment, const VBO & attr, const VBO & uniforms))
        {
            FragShader = FragSdr;
        }
};

// Example of a vertex shader
void DefaultVertShader(Vertex & vertOut, VBO & attrOut, const Vertex & vertIn, const VBO & attrIn, const VBO & uniforms)
{
    // Nothing happens with this vertex, attribute
    vertOut = vertIn;
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
        void (*VertShader)(Vertex & vertOut, VBO & attrOut, const Vertex & vertIn, const VBO & attrIn, const VBO & uniforms);

        // Assumes simple monotone RED shader
        VertexShader()
        {
            VertShader = DefaultVertShader;
        }

        // Initialize with a fragment callback
        VertexShader(void (*VertSdr)(Vertex & vertOut, VBO & attrOut, const Vertex & vertIn, const VBO & attrIn, const VBO & uniforms))
        {
            setShader(VertSdr);
        }

        // Set the shader to a callback function
        void setShader(void (*VertSdr)(Vertex & vertOut, VBO & attrOut, const Vertex & vertIn, const VBO & attrIn, const VBO & uniforms))
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
                   const VBO inputAttrs[],
                   VBO* const uniforms = NULL,
                   FragmentShader* const frag = NULL,
                   VertexShader* const vert = NULL,
                   Buffer2D<double>* zBuf = NULL);             












/*********************************************************************************************************
 * NOTE: Everything that follows below will be used last for the 
 * software renderer. You are encouraged to review/study it but
 * you do not need work on this code.
********************************************************************************************************/











/**************************************************************
 * INTERSECT_AGAINST_Y_LIMIT
 * Compares a limit for a given plane treated as Y but used for
 * W limit to avoid W division by zero 
 *************************************************************/
void intersectAgainstYLimit(double & along, const double & yLimit, const double & segStartY, const double & segEndY)
{
    // Segment Parametric equation
    along = -1;
    double segDiffY = (segEndY - segStartY);

    along = (yLimit - segStartY) / (segDiffY);
}

/***************************************************************
 * INTERSECT_AT_POSITIVE_LINE
 * Compares the input line segment against lines X=Y
 * limits and clips
 **************************************************************/ 
void intersectAtPositiveLine(double & along, 
                  const double & segStartX, const double & segStartY, 
                  const double & segEndX, const double & segEndY)
{
    // Segment Parametric equation
    along = -1;
    double segDiffX = (segEndX - segStartX);
    double segDiffY = (segEndY - segStartY);

    // Solve for segT
    along = (segStartY - segStartX) / (segDiffX - segDiffY);
}


/***************************************************************
 * INTERSECT_AT_NEGATIVE_LINE
 * Compares the input line segment against lines X=Y
 * limits and clips
 **************************************************************/ 
void intersectAtNegativeLine(double & along, 
                  const double & segStartX, const double & segStartY, 
                  const double & segEndX, const double & segEndY)
{
    // Segment Parametric equation
    along = -1;
    double segDiffX = (segEndX - segStartX);
    double segDiffY = (segEndY - segStartY);

    // Solve for segT
    along = (segStartY + segStartX) / (-segDiffX - segDiffY);
 }


/*************************************************************
 * INTERP_VERTS
 * Returns a vertex "along" amount between the two vertices.
 * Used by clipping. 
 ************************************************************/
Vertex vertBetweenVerts(const Vertex & first, const Vertex & last, const double & along)
{
    Vertex rv;
    rv.x = first.x + (along * (last.x - first.x));
    rv.y = first.y + (along * (last.y - first.y));
    rv.z = first.z + (along * (last.z - first.z));
    rv.w = first.w + (along * (last.w - first.w));

    double startZ = first.z / first.w;
    double endZ = last.z / last.w;
    rv.z = (startZ + (along * (endZ - startZ)));
    rv.z *= rv.w;

    return rv;
}


#define MAX_VERTICES 8 // Max # of vertices after clipping
/*************************************************************
 * CLIP_VERTICES:
 * Depending on our view type - clip to the frustrum that 
 * maps to our screen's aspect ratio.  
 ************************************************************/
void ClipVertices(Vertex const inputVerts[], VBO const inputAttrs[], const int & numIn, 
                  Vertex clippedVerts[], VBO clippedAttrs[], int & numClipped)
{
    // TMP Clip buffers
    int num;
    int numOut;
    bool inBounds[MAX_VERTICES];
    Vertex tmpVertA[MAX_VERTICES];
    Vertex tmpVertB[MAX_VERTICES];
    VBO tmpAttrA[MAX_VERTICES];
    VBO tmpAttrB[MAX_VERTICES];
    Vertex const * srcVerts;
    Vertex* sinkVerts;
    VBO const * srcAttrs;
    VBO* sinkAttrs;

    // Setup pointers
    srcVerts = inputVerts;
    srcAttrs = inputAttrs;
    sinkVerts = tmpVertA;
    sinkAttrs = tmpAttrA;
    num = numIn;
    numOut = 0;

    // Clip on each side against wLimit
    double wLimit = 0.001;
    for(int i = 0; i < num; i++)
    {
        inBounds[i] = (srcVerts[i].w > wLimit);    
    }
    for(int i = 0; i < num; i++)
    {
        int cur = i;
        int next = (i+1) % num;
        bool curVertIn = inBounds[cur];
        bool nextVertIn = inBounds[next];

        if(curVertIn && nextVertIn)
        {
            sinkVerts[numOut] = srcVerts[next];
            sinkAttrs[numOut++] = srcAttrs[next];
        }
        else if(curVertIn && !nextVertIn)
        {
            double along;
            intersectAgainstYLimit(along, wLimit, srcVerts[cur].w, srcVerts[next].w);

            sinkVerts[numOut] = vertBetweenVerts(srcVerts[cur], srcVerts[next], along);
            sinkAttrs[numOut++] = VBO(srcAttrs[cur], srcAttrs[next], along);
        }
        else if(!curVertIn && nextVertIn)
        {
            double along;
            intersectAgainstYLimit(along, wLimit, srcVerts[cur].w, srcVerts[next].w);

            sinkVerts[numOut] = vertBetweenVerts(srcVerts[cur], srcVerts[next], along);
            sinkAttrs[numOut++] = VBO(srcAttrs[cur], srcAttrs[next], along);

            sinkVerts[numOut] = srcVerts[next];
            sinkAttrs[numOut++] = srcAttrs[next];
        }
        else // (!curVertIn && !nextVertIn)
        {
            ; //Do nothing... 
        }
    }

    // Clip against X

    // Setup pointers
    srcVerts = tmpVertA;
    srcAttrs = tmpAttrA;
    sinkVerts = tmpVertB;
    sinkAttrs = tmpAttrB;
    num = numOut;
    numOut = 0;

    // Clip X=W
    for(int i = 0; i < num; i++)
    {
        inBounds[i] = (srcVerts[i].x < srcVerts[i].w);    
    }
    for(int i = 0; i < num; i++)
    {
        int cur = i;
        int next = (i+1) % num;
        bool curVertIn = inBounds[cur];
        bool nextVertIn = inBounds[next];

        if(curVertIn && nextVertIn)
        {
            sinkVerts[numOut] = srcVerts[next];
            sinkAttrs[numOut++] = srcAttrs[next];
        }
        else if(curVertIn && !nextVertIn)
        {
            double along;
            intersectAtPositiveLine(along, srcVerts[cur].x, srcVerts[cur].w, srcVerts[next].x, srcVerts[next].w);

            sinkVerts[numOut] = vertBetweenVerts(srcVerts[cur], srcVerts[next], along);
            sinkAttrs[numOut++] = VBO(srcAttrs[cur], srcAttrs[next], along);
        }
        else if(!curVertIn && nextVertIn)
        {
            double along;
            intersectAtPositiveLine(along, srcVerts[cur].x, srcVerts[cur].w, srcVerts[next].x, srcVerts[next].w);

            sinkVerts[numOut] = vertBetweenVerts(srcVerts[cur], srcVerts[next], along);
            sinkAttrs[numOut++] = VBO(srcAttrs[cur], srcAttrs[next], along);

            

            sinkVerts[numOut] = srcVerts[next];
            sinkAttrs[numOut++] = srcAttrs[next];
        }
        else // (!curVertIn && !nextVertIn)
        {
            ; //Do nothing... 
        }
    }

    // Setup Pointers
    srcVerts = tmpVertB;
    srcAttrs = tmpAttrB;
    sinkVerts = tmpVertA;
    sinkAttrs = tmpAttrA;
    num = numOut;
    numOut = 0;

    // Clip -X=W
    for(int i = 0; i < num; i++)
    {
        inBounds[i] = (-srcVerts[i].x < srcVerts[i].w);    
    }
    for(int i = 0; i < num; i++)
    {
        int cur = i;
        int next = (i+1) % num;
        bool curVertIn = inBounds[cur];
        bool nextVertIn = inBounds[next];

        if(curVertIn && nextVertIn)
        {
            sinkVerts[numOut] = srcVerts[next];
            sinkAttrs[numOut++] = srcAttrs[next];
        }
        else if(curVertIn && !nextVertIn)
        {
            double along;
            intersectAtNegativeLine(along, srcVerts[cur].x, srcVerts[cur].w, srcVerts[next].x, srcVerts[next].w);

            sinkVerts[numOut] = vertBetweenVerts(srcVerts[cur], srcVerts[next], along);
            sinkAttrs[numOut++] = VBO(srcAttrs[cur], srcAttrs[next], along);
        }
        else if(!curVertIn && nextVertIn)
        {
            double along;
            intersectAtNegativeLine(along, srcVerts[cur].x, srcVerts[cur].w, srcVerts[next].x, srcVerts[next].w);

            sinkVerts[numOut] = vertBetweenVerts(srcVerts[cur], srcVerts[next], along);
            sinkAttrs[numOut++] = VBO(srcAttrs[cur], srcAttrs[next], along);

            sinkVerts[numOut] = srcVerts[next];
            sinkAttrs[numOut++] = srcAttrs[next];
        }
        else // (!curVertIn && !nextVertIn)
        {
            ; //Do nothing... 
        }
    }



    // Clip against Y

    // Setup pointers
    srcVerts = tmpVertA;
    srcAttrs = tmpAttrA;
    sinkVerts = tmpVertB;
    sinkAttrs = tmpAttrB;
    num = numOut;
    numOut = 0;

    // Clip Y=W
    for(int i = 0; i < num; i++)
    {
        inBounds[i] = (srcVerts[i].y < srcVerts[i].w);    
    }
    for(int i = 0; i < num; i++)
    {
        int cur = i;
        int next = (i+1) % num;
        bool curVertIn = inBounds[cur];
        bool nextVertIn = inBounds[next];

        if(curVertIn && nextVertIn)
        {
            sinkVerts[numOut] = srcVerts[next];
            sinkAttrs[numOut++] = srcAttrs[next];
        }
        else if(curVertIn && !nextVertIn)
        {
            double along;
            intersectAtPositiveLine(along, srcVerts[cur].y, srcVerts[cur].w, srcVerts[next].y, srcVerts[next].w);

            sinkVerts[numOut] = vertBetweenVerts(srcVerts[cur], srcVerts[next], along);
            sinkAttrs[numOut++] = VBO(srcAttrs[cur], srcAttrs[next], along);
        }
        else if(!curVertIn && nextVertIn)
        {
            double along;
            intersectAtPositiveLine(along, srcVerts[cur].y, srcVerts[cur].w, srcVerts[next].y, srcVerts[next].w);

            sinkVerts[numOut] = vertBetweenVerts(srcVerts[cur], srcVerts[next], along);
            sinkAttrs[numOut++] = VBO(srcAttrs[cur], srcAttrs[next], along);

            sinkVerts[numOut] = srcVerts[next];
            sinkAttrs[numOut++] = srcAttrs[next];
        }
        else // (!curVertIn && !nextVertIn)
        {
            ; //Do nothing... 
        }
    }

    // Setup Pointers
    srcVerts = tmpVertB;
    srcAttrs = tmpAttrB;
    sinkVerts = tmpVertA;
    sinkAttrs = tmpAttrA;
    num = numOut;
    numOut = 0;

    // Clip -Y=W
    for(int i = 0; i < num; i++)
    {
        inBounds[i] = (-srcVerts[i].y < srcVerts[i].w);    
    }
    for(int i = 0; i < num; i++)
    {
        int cur = i;
        int next = (i+1) % num;
        bool curVertIn = inBounds[cur];
        bool nextVertIn = inBounds[next];

        if(curVertIn && nextVertIn)
        {
            sinkVerts[numOut] = srcVerts[next];
            sinkAttrs[numOut++] = srcAttrs[next];
        }
        else if(curVertIn && !nextVertIn)
        {
            double along;
            intersectAtNegativeLine(along, srcVerts[cur].y, srcVerts[cur].w, srcVerts[next].y, srcVerts[next].w);

            sinkVerts[numOut] = vertBetweenVerts(srcVerts[cur], srcVerts[next], along);
            sinkAttrs[numOut++] = VBO(srcAttrs[cur], srcAttrs[next], along);
        }
        else if(!curVertIn && nextVertIn)
        {
            double along;
            intersectAtNegativeLine(along, srcVerts[cur].y, srcVerts[cur].w, srcVerts[next].y, srcVerts[next].w);

            sinkVerts[numOut] = vertBetweenVerts(srcVerts[cur], srcVerts[next], along);
            sinkAttrs[numOut++] = VBO(srcAttrs[cur], srcAttrs[next], along);

            sinkVerts[numOut] = srcVerts[next];
            sinkAttrs[numOut++] = srcAttrs[next];
        }
        else // (!curVertIn && !nextVertIn)
        {
            ; //Do nothing... 
        }
    }

    // Clip against Z

    // Setup pointers
    srcVerts = tmpVertA;
    srcAttrs = tmpAttrA;
    sinkVerts = tmpVertB;
    sinkAttrs = tmpAttrB;
    num = numOut;
    numOut = 0;

    // Clip Z=W
    for(int i = 0; i < num; i++)
    {
        inBounds[i] = (srcVerts[i].z < srcVerts[i].w);    
    }
    for(int i = 0; i < num; i++)
    {
        int cur = i;
        int next = (i+1) % num;
        bool curVertIn = inBounds[cur];
        bool nextVertIn = inBounds[next];

        if(curVertIn && nextVertIn)
        {
            sinkVerts[numOut] = srcVerts[next];
            sinkAttrs[numOut++] = srcAttrs[next];
        }
        else if(curVertIn && !nextVertIn)
        {
            double along;
            intersectAtPositiveLine(along, srcVerts[cur].z, srcVerts[cur].w, srcVerts[next].z, srcVerts[next].w);

            sinkVerts[numOut] = vertBetweenVerts(srcVerts[cur], srcVerts[next], along);
            sinkAttrs[numOut++] = VBO(srcAttrs[cur], srcAttrs[next], along);
        }
        else if(!curVertIn && nextVertIn)
        {
            double along;
            intersectAtPositiveLine(along, srcVerts[cur].z, srcVerts[cur].w, srcVerts[next].z, srcVerts[next].w);

            sinkVerts[numOut] = vertBetweenVerts(srcVerts[cur], srcVerts[next], along);
            sinkAttrs[numOut++] = VBO(srcAttrs[cur], srcAttrs[next], along);

            sinkVerts[numOut] = srcVerts[next];
            sinkAttrs[numOut++] = srcAttrs[next];
        }
        else // (!curVertIn && !nextVertIn)
        {
            ; //Do nothing... 
        }
    }

    // Setup Pointers
    srcVerts = tmpVertB;
    srcAttrs = tmpAttrB;
    sinkVerts = clippedVerts;
    sinkAttrs = clippedAttrs;
    num = numOut;
    numOut = 0;

    // Clip -Z=W
    for(int i = 0; i < num; i++)
    {
        inBounds[i] = (-srcVerts[i].z < srcVerts[i].w);    
    }
    for(int i = 0; i < num; i++)
    {
        int cur = i;
        int next = (i+1) % num;
        bool curVertIn = inBounds[cur];
        bool nextVertIn = inBounds[next];

        if(curVertIn && nextVertIn)
        {
            sinkVerts[numOut] = srcVerts[next];
            sinkAttrs[numOut++] = srcAttrs[next];
        }
        else if(curVertIn && !nextVertIn)
        {
            double along;
            intersectAtNegativeLine(along, srcVerts[cur].z, srcVerts[cur].w, srcVerts[next].z, srcVerts[next].w);

            sinkVerts[numOut] = vertBetweenVerts(srcVerts[cur], srcVerts[next], along);
            sinkAttrs[numOut++] = VBO(srcAttrs[cur], srcAttrs[next], along);
        }
        else if(!curVertIn && nextVertIn)
        {
            double along;
            intersectAtNegativeLine(along, srcVerts[cur].z, srcVerts[cur].w, srcVerts[next].z, srcVerts[next].w);

            sinkVerts[numOut] = vertBetweenVerts(srcVerts[cur], srcVerts[next], along);
            sinkAttrs[numOut++] = VBO(srcAttrs[cur], srcAttrs[next], along);

            sinkVerts[numOut] = srcVerts[next];
            sinkAttrs[numOut++] = srcAttrs[next];
        }
        else // (!curVertIn && !nextVertIn)
        {
            ; //Do nothing... 
        }
    }

    // Final number of output vertices
    numClipped = numOut;  
}



/*************************************************************
 * NORMALIZE VERTICES
 * Homogeneous coordinates need to be normalized, implicitly
 * performing the perspective divide for us. 
 ************************************************************/
void NormalizeVertices(Vertex clippedVertices[], const int & num)
{
    for(int i = 0; i < num; i++)
    {
        clippedVertices[i].x /= clippedVertices[i].w;
        clippedVertices[i].y /= clippedVertices[i].w;
        //clippedVertices[i].z Carries Z-Buffer 'Z'
        //clippedVertices[i].w Carries True 'Z'
    }
}
       
#endif
