#define SDL_MAIN_HANDLED
#include "SDL2/SDL.h"
#include "stdlib.h"
#include "stdio.h"
#include "math.h"
#include "shaders.h"
#include <iostream>
#include <iomanip>


#ifndef DEFINITIONS_H
#define DEFINITIONS_H

using namespace std;

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
 * Describes a geometric point in 3D space. 
 ****************************************************/
struct Vertex
{
    double x;
    double y;
    double z;
    double w;
};

class Matrix
{
    public:
		Matrix();
        Matrix(int c, int r);
        void insert(int x, int y, double data);
        double returnVal(int x, int y);
		int getColumnSize();
		int getRowSize();
		int getSize();
		//Matrix matrixCopy(Matrix & m);
		//Have to write 4 different const types
		Matrix operator*(const Matrix & m) const;
		//Matrix operator*(const Vertex & m) const;
		Vertex operator*(const Vertex & m) const;
		Matrix& operator=(Matrix const& m);
		Matrix & operator=(std::initializer_list<double> m);
		Matrix & operator=(std::initializer_list<std::initializer_list<double>> m);

    private:
        int column;
        int row;
		int size;
        double matrixData[16];
};
Matrix::Matrix()
{
	column = 0;
    row = 0;
	size = 0;
}

Matrix::Matrix (int c, int r)
{
    column = c;
    row = r;
	size = row * column;
	for(int i = 0; i < size; i++)
	{
		matrixData[i] = 0;
	}
}

void Matrix::insert(int x, int y, double data)
{
    matrixData[(x * column) + y] = data;
}

double Matrix::returnVal(int x, int y)
{
    return matrixData[(x * column) + y];
}

int Matrix::getColumnSize()
{
	return column;
}

int Matrix::getRowSize()
{
	return row;
}

int Matrix::getSize()
{
	return size;
}

Matrix computeDimensions(int c1, int r1, int c2, int r2)
{
	if (c1 < c2 && r1 == r2)
	{
		Matrix result = Matrix(c1, r1);
		return result;
	}
	else if (c1 == c2 && r1 < r2)
	{
		Matrix result = Matrix(c1, r1);
		return result;
	}

	else if (c2 < c1 && r2 == r1)
	{
		Matrix result = Matrix(c2, r2);
		return result;
	}
	else if (c2 == c1 && r2 < r1)
	{
		Matrix result = Matrix(c2, r2);
		return result;
	}
	else if (c1 == c2 && r1 == r2)
	{
		Matrix result = Matrix(c1, r1);
		return result;
	}
	else
	{
		std::cerr << "WARNING: DIMENSIONAL MISMATCH ABORTING PROGRAM\n";
		exit(1);
	}
}

/********************************************************
* Matrix Copy
*
* Overloading the equal operator doesn't work I don't
* know why so I'm making this.
********************************************************/
/*
Matrix Matrix::matrixCopy(Matrix & m)
{
	delete [] matrixData;
	matrixData = NULL;
	this->~Matrix();
	
	
	column = m.column;
	row = m.row;
	size = m.size;
	
	matrixData = new double[size];
	
	for (int i = 0; i < size; i++)
	{
		//cout << "entering in " << m.matrixData[i] << " at " << i << endl;
		matrixData[i] =  m.matrixData[i];
		//cout << "matrixData[" << i << "] is " << this->matrixData[i] << endl;
	}
	
	
	
	return *this;
}
*/


/******************************************************
* * Multiplication Operator Overload
*
* 
******************************************************/
Matrix Matrix::operator*(const Matrix & m) const
{
	Matrix result = computeDimensions(column, row, m.column, m.row);
	//Matrix result = Matrix(1, 3);

	/*result.insert(0, 0, 5);
	cout << "inserted 5\n";
	result.insert(0, 1, 5);
	cout << "inserted 5\n";
	result.insert(0, 2, 6);*/


	for (int x = 0; x < row; x++)
	{
		for (int y = 0; y < m.column; y++)
		{
			for (int k = 0; k < column; k++)
			{
				result.insert(x, y, (result.returnVal(x, y) + (matrixData[(x * column)+k] * m.matrixData[(k * m.column) + y])));
			}
		}
	}

	return result;
}

/*************************************************************
* Overloaded * Operator for vertex
*
*************************************************************/

/*Matrix Matrix::operator*(const Vertex & m) const
{
	Matrix result = computeDimensions(4, 4, 1, 4);
	
	result.insert(0, 0, m.x);
	result.insert(0, 1, m.y);
	result.insert(0, 2, m.z);
	result.insert(0, 3, m.w);
	
	result = *this * result;
	
	return result;
}*/

Vertex Matrix::operator*(const Vertex & m) const
{
	Vertex v;

	Matrix result = computeDimensions(4, 4, 1, 4);

	result.insert(0, 0, m.x);
	result.insert(0, 1, m.y);
	result.insert(0, 2, m.z);
	result.insert(0, 3, m.w);

	result = *this * result;

	v.x = result.returnVal(0, 0);
	v.y = result.returnVal(0, 1);
	v.z = result.returnVal(0, 2);
	v.w = result.returnVal(0, 3);

	return v;
}

/***********************************************************
* operator = Overload
*
* Need to do deep copy operations. 
***********************************************************/

Matrix& Matrix::operator = (Matrix const& m)
{
	this->~Matrix();


	column = m.column;
	row = m.row;
	size = m.size;



	for (int i = 0; i < size; i++)
	{
		//cout << "entering in " << m.matrixData[i] << " at " << i << endl;
		matrixData[i] =  m.matrixData[i];
		//cout << "matrixData[" << i << "] is " << this->matrixData[i] << endl;
	}



	return *this;
}

/***********************************************************
* = operator Overload
* 
* If we are entering in a 1 dimensional array it will be
* entered as 1 x 4, 1 x 6. whatever column will always 
* be one.
************************************************************/

Matrix & Matrix::operator = (std::initializer_list<double> m)
{
	//delete old stuff
	column = 1;
	row = m.size();
	size = (column * row);



	int i = 0;
	std::initializer_list<double>::iterator it;
	for (it = m.begin(); it != m.end(); ++it)
	{
		insert(0, i, *it);
		i++;
	}
	return *this;
}

Matrix & Matrix::operator=(std::initializer_list<std::initializer_list<double>> m)
{
	//debug
	//cout << "Length of array = ";
	//cout << m.size() << endl;
	//Have a nested initializer list so multiple iterators are required
	std::initializer_list<std::initializer_list<double>>::iterator it;
	std::initializer_list<double>::iterator it2;
	it = m.begin();
	it2 = it->begin();

	//Set column value and build new array
	column = it->size();
	//cout << "Length of second dimension is = " << it->size();
	//cout << endl;
	row = m.size();
	size = (column * row);

	//delete old stuff



	int i = 0;
	for (it = m.begin(); it != m.end(); ++it)
	{
		for (it2 = it->begin(); it2 != it->end(); ++it2)
		{
			insert(0, i, *it2);
			//cout << "inserting " << *it2 << " at " << i << endl;
			i++;
		}
	}
	//debug
	/*cout << "Values in matrixData are \n";
	for (int j = 0; j < 8; j++)
	{
		cout << "at " << j << " = " << matrixData[j] << endl;
	}*/


	return *this;
}



void displayMatrix(Matrix m)
{
	for (int x = 0; x < m.getRowSize(); x++)
	{
		for (int y = 0; y < m.getColumnSize(); y++)
		{
			cout << setw(4) << m.returnVal(x, y) << ",";
		}
		cout << endl;
	}
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
class Attributes
{      
    public:
		

        // Obligatory empty constructor
        Attributes() {}
         // Used for the color of the point or u=a v=r
        double argb[4];
        // For now points to the image, but can later point to other assets
        void* ptr;

        //Now we need to hold a matrix for our transofrmations
        Matrix transforms;

        Attributes(const Attributes & first, const Attributes & second, const double & valueBetween)
        {
            // Your code goes here when clipping is implemented
        }
};	

// Example of a fragment shader
void DefaultFragShader(PIXEL & fragment, const Attributes & vertAttr, const Attributes & uniforms)
{
     BufferImage* bf = (BufferImage*)uniforms.ptrImg;
    int x = vertAttr.getU() * (bf->width()-1);
    int y = vertAttr.getV() * (bf->height()-1);
    // Output our shader color value, in this case red
    fragment = (*bf)[y][x]; //& color
}

void GreenShader(PIXEL & fragment, const Attributes & vertAttr, const Attributes & uniforms)
{
    BufferImage* bf = (BufferImage*)uniforms.ptrImg;
    int x = vertAttr.getU() * (bf->width()-1);
    int y = vertAttr.getV() * (bf->height()-1);


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
void myVertexShader(Vertex & vertOut, Attributes & attrOut, const Vertex & vertIn, const Attributes & vertAttr, const Attributes & uniforms)
{
    vertOut = uniforms.transforms * vertIn;
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
