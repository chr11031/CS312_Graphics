#define SDL_MAIN_HANDLED
#include "/Users/ronaldmunoz/Documents/College/Winter 2019/CS 312 - Computer Graphics/SDL2-2.0.9/include/SDL.h"
#include "stdlib.h"
#include "stdio.h"
#include "math.h"
#include <vector>
#include <iostream>

#define PI 3.14159265

using namespace std;

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
* My Matrix class
*****************************************************/
class Matrix {

public:

   int height = 0;
   int width = 0;
// Create a Generic Matrix, this will be useful later
   double matrix[4][4] = { {1,0,0,0},
                           {0,1,0,0},
                           {0,0,1,0},
                           {0,0,0,1} };

// Some constructors
   Matrix(){
   }

   Matrix(double fbf[4][4]){
      height = 4;
      width = 4;
      
      for (int i = 0; i < 4; i++)
      {
         for (int j = 0; j < 4; j++)
         {
            matrix[i][j] = fbf[i][j];
         }        
      }
         
   }

// Initialise with size only.
   Matrix(int h, int w){
      this->height = h;
      this->width = w;
   }

// Multiplication operator overload for matrix operations
   Matrix operator*(const Matrix &rhs)
   {
      //check if you can actually multiply them
      if(width == rhs.height)
      { 
         Matrix res(rhs.height,rhs.width);

          int i, j, k; 
            for (i = 0; i < height; i++) 
            { 
               for (j = 0; j < rhs.width; j++) 
               { 
                     res.matrix[i][j] = 0; 
                     for (k = 0; k < rhs.height; k++) 
                        res.matrix[i][j] += this->matrix[i][k] *  
                                    rhs.matrix[k][j]; 
               } 
            }
         return res;
      }
   }

// For shader operations
   Matrix operator*(const Vertex &rhs)
 	{
      Matrix res(4,1);

      if(width == 4)
      { 
         double vertex[4][1] = { {rhs.x}, 
                                 {rhs.y}, 
                                 {rhs.z}, 
                                 {rhs.w} };

         for(int i = 0; i < height; i++){	
            for(int j = 0; j < 1; j++){ 
               for( int k = 0; k < 4; k++){  
                  res.matrix[i][j] += this->matrix[i][k] * vertex[k][j];
                  }
               }
            }
            return res;
         }
 	}

    Matrix operator*(const Vertex &rhs) const
 	{
      Matrix res(4,1);

      if(width == 4)
      { 
         double vertex[4][1] = { {rhs.x}, 
                                 {rhs.y}, 
                                 {rhs.z}, 
                                 {rhs.w} };

         for(int i = 0; i < height; i++){	
            for(int j = 0; j < 1; j++){ 
               for( int k = 0; k < 4; k++){  
                  res.matrix[i][j] += this->matrix[i][k] * vertex[k][j];
                  }
               }
            }
            return res;
         }
 	}

    void print()
    {
        for(int i=0;i < height;i++) 
        {
            for(int j=0;j < width;j++) 
            {
               cout << matrix[i][j] << ' ';
            }
            cout << endl;
        }
        cout << endl;
    }

    Matrix rotate(int angle){

        double sin1 = sin(angle * PI/180.0);
        double cos1 = cos(angle * PI/180.0);
        
        Matrix temp(4,4);
        temp.matrix[0][0] = cos1;
        temp.matrix[0][1] = -sin1;
        temp.matrix[1][0] = sin1;
        temp.matrix[1][1] = cos1;
        temp.matrix[2][2] = temp.matrix[3][3] = 1;

        return *this * temp;
    }

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
            //SDL_LockSurface(img);
            setupInternal();
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

        //Matrix transformations
        Matrix matrix;

        //coordinates.
        double u;
        double v;

        //add RGB values.
        double r;
        double g;
        double b; 

        //pointer for image buffer
        BufferImage * ptrImg;

        //classic color pixel value
        PIXEL color;

        // Obligatory empty constructor
        Attributes() {}

        // Needed by clipping (linearly interpolated Attributes between two others)
        Attributes(const Attributes & first, const Attributes & second, const double & valueBetween)
        {
            // Your code goes here when clipping is implemented
        }

        //Set the RGB values of pixel the attributes class using doubles.
        void setRGB(const double R,const double G,const double B)
        {
            this->r = R;
            this->g = G;
            this->b = B;
        }

        //Set the RGB values of the pixel attributes class using color codes.
        void setRGB(Uint32 R, Uint32 G, Uint32 B)
        {
            this->r = (double)R;
            this->g = (double)G;
            this->b = (double)B;
        }

        /*Vector must have values in form R G B U V*/
        void setRGBUV(vector<double> &vect)
        {
            if(vect.size() != 4)
            {
                cout << "The vector is not of a valid size, check you values" << endl;
            } 
            else
            {
                //This is not what a normalize vector does! But it helps to get an idea of how to manage vectors.
                this->setRGB(vect[0],vect[1],vect[2]);
                this->setUV(vect[3], vect[4]);
            }
            
        }

        //Set the U V values for the pixel attributes.
        void setUV(const double U, const double V)
        {
            this->u = U;
            this->v = V;
        }

        //Set Image data with a 2D Buffered Image.
        void setImageData(BufferImage myImage)
        {
            this->ptrImg = &myImage;
        }

        void setFogCoord(double fogCoord)
        {
            //Your code goes here
        }

        void setCustomAttributes()
        {
            //Your code goes here
        }

        void setTransofmations(vector<double> &transformationMatrix)
        {
            //Your code goes here
        }

        void setLightSourceInfo(double light)
        {
            //Your code goes here
        }

        void mouseInfo(int x, int y)
        {
            //Your code goes here
        }

        void time(double time)
        {
            //Your code goes here
        }
        
};	

// Example of a fragment shader
void DefaultFragShader(PIXEL & fragment, const Attributes & vertAttr, const Attributes & uniforms)
{
    // Output our shader color value, in this case red
    fragment = 0xffff0000;
}

void GrayFragShader(PIXEL & fragment, const Attributes & vertAttr, const Attributes & uniforms){

    PIXEL avgChannel = ((vertAttr.color >> 16) && 0xff) + 
                        ((vertAttr.color >> 8) && 0xff) + 
                        ((vertAttr.color) && 0xff);
    avgChannel /= 3;
    fragment = 0xff000000 + (avgChannel << 16) + (avgChannel << 8) + avgChannel;
}

// Image Fragment Shader 
 void ImageFragShader(PIXEL & fragment, const Attributes & vertAttr, const Attributes & uniforms)
 {
    BufferImage *bf = (BufferImage*)uniforms.ptrImg;
    int x = vertAttr.u * (bf->width()-1);
    int y = vertAttr.v * (bf->height()-1);
    fragment = (*bf)[y][x];
 }

  // My Fragment Shader for color interpolation
 void ColorFragShader(PIXEL & fragment, const Attributes & vertAttr, const Attributes & uniforms)
 {
    // Output our shader color value, in this case red
    PIXEL color = 0xff000000;
    color += (unsigned int)(vertAttr.r *0xff) << 16;
    color += (unsigned int)(vertAttr.g *0xff) << 8;
    color += (unsigned int)(vertAttr.b *0xff) << 0;

    fragment = color;
 }

//reciprocal function
double reciprocal(const double value)
{
	return 1 / value;
}

 //lerp function
double lerp(double v0, double v1, double t) 
{
  return v0 + ( (v1 - v0) * t);
}

 // Frag Shader for UV without image (due to SDL2 bug?)
void FragShaderUVwithoutImage(PIXEL & fragment, const Attributes & attributes, const Attributes & uniform)
{
    // Figure out which X/Y square our UV would fall on
    int xSquare = attributes.u * 8;
    int ySquare = attributes.v * 8;

	// Is the X square position even? The Y? 
    bool evenXSquare = (xSquare % 2) == 0;
    bool evenYSquare = (ySquare % 2) == 0;
   
    // Both even or both odd - red square
    if( (evenXSquare && evenYSquare) || (!evenXSquare && !evenYSquare) )
    {
        fragment = 0xffff0000;
    }
    //One even, one odd - white square
    else
    {
        fragment = 0xffffffff;
            
    }
    
}

void FragShaderUVwithoutImage2(PIXEL & fragment, const Attributes & attributes, const Attributes & uniform)
{
    // Figure out which X/Y square our UV would fall on
    int xSquare = attributes.u * 8;
    int ySquare = attributes.v * 8;

	// Is the X square position even? The Y? 
    bool evenXSquare = (xSquare % 2) == 0;
    bool evenYSquare = (ySquare % 2) == 0;
   
    // Both even or both odd - red square
    if( (evenXSquare && evenYSquare) || (!evenXSquare && !evenYSquare) )
    {
        fragment = 0xffffff00;
    }
    //One even, one odd - white square
    else
    {
        fragment = 0xffffffff;
            
    }
    
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
            //FragShader = DefaultFragShader;
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

// My vertex shader
void transformShader(Vertex & vertOut, Attributes & attrOut, const Vertex & vertIn, const Attributes & attrIn, const Attributes & uniforms)
 {
    Matrix transform(uniforms.matrix * vertIn);
    //transform.print();
    vertOut = vertIn;
    vertOut.x = transform.matrix[0][0];
    vertOut.y = transform.matrix[1][0];
    vertOut.z = transform.matrix[2][0];
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
