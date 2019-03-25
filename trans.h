#ifndef TRANS_H
#define TRANS_H
#define MAX_DIM_SIZE_MATRIX 4

#include "definitions.h"

/******************************************************
 * Transformation class for world geometry, viewing.
 *****************************************************/
class Transform
{
    // 2D Array
    private:
        double mat[MAX_DIM_SIZE_MATRIX][MAX_DIM_SIZE_MATRIX];

    // Update Matrix values by array
    void copyValues(const Transform & source)
    {
        if(rowLen != source.rowLen || colLen != source.colLen) return;
        for(int r = 0; r < rowLen; r++)
        {
            for(int c = 0; c < colLen; c++)
            {
                mat[r][c] = (source.get(r))[c];
            }
        }
    }

    // Update Matrix values by array
    void setValues(double values[])
    {
        int i = 0;
        for(int r = 0; r < rowLen; r++)
        {
            for(int c = 0; c < colLen; c++)
            {
                mat[r][c] = values[i++];
            }
        }
    }

    // Matrix Dimensions
    public:
        int colLen;
        int rowLen;

    // Initialize size, setup identity matrix or set values to zero
    Transform(int width = MAX_DIM_SIZE_MATRIX, int height = MAX_DIM_SIZE_MATRIX)
    {
        colLen = width;
        rowLen = height;
        setIdentity();
    }

    // Initialize a vertex as a matrix
    Transform(const Vertex & vert)
    {
        rowLen = 4;
        colLen = 1;
        mat[0][0] = vert.x;
        mat[1][0] = vert.y;
        mat[2][0] = vert.z;
        mat[3][0] = vert.w;
    }

    // Initialize matrix to suggested values
    Transform(double values[], int width = MAX_DIM_SIZE_MATRIX, int height = MAX_DIM_SIZE_MATRIX)
    {
        colLen = width;
        rowLen = height;
        setValues(values);
    }

    // Assignment operator 
    Transform& operator = (const Transform & right)
    {
        colLen = right.colLen;
        rowLen = right.rowLen;
        copyValues(right);
    }
    
    // Dereference Matrix in 'mat[r][c]' format
    double* operator [](int i) { return (double*)mat[i]; }

    // Const version of '[]'
    const double* get(int i) const { return (double*)mat[i]; }

    // Is it?
    bool isQuare()
    {
        return colLen == rowLen;
    }

    // Set all entries to zero
    void setZero()
    {
        for(int r = 0; r < rowLen; r++)
        {
            for(int c = 0; c < colLen; c++)
            {
                mat[r][c] = 0.0f;
            }
        }
    }

    // Setup the identity matrix if square
    bool setIdentity()
    {
        setZero();
        if(!isQuare()) return false;

        for(int d = 0; d < colLen; d++)
        {
            mat[d][d] = 1.0f;
        }
    }

    // Multiply two matrices together, return the result
    Transform operator *(const Transform & right) const
    {
        Transform tr(right.rowLen, this->colLen);

        if(colLen != right.rowLen)
        {
            return tr;
        }

        int runLength = rowLen;
        for(int c = 0; c < right.colLen; c++)
        {
            for(int r = 0; r < tr.rowLen; r++)
            {
                tr[r][c] = 0;
                for(int i = 0; i < runLength; i++)
                {
                    tr[r][c] += mat[r][i] * (right.get(i))[c];
                }
            }
        }
        return tr;
    } 

    // Multiply a 4-component vertex by this matrix, return vertex
    Vertex operator *(const Vertex & right) const 
    {
        // Convert Vertex to Matrix 
        Transform vl(right);
        
        // Multiply
        Transform out = (*this) * vl;
        
        // Return in vertex format
        Vertex rv;
        rv.x = out[0][0];
        rv.y = out[1][0];
        rv.z = out[2][0];
        rv.w = out[3][0];
        return rv;
    }
};

// Rotational helper (4x4)
Transform rotate4x4(const DIMENSION & dim, const double & degs)
{
  Transform tr(4, 4);
  double rads = degs * M_PI / 180.0;
  double cosT = cos(rads);
  double sinT = sin(rads);

  tr[0][0] = 1;
  tr[0][1] = 0;
  tr[0][2] = 0;
  tr[0][3] = 0;
  tr[1][0] = 0;
  tr[1][1] = 1;
  tr[1][2] = 0;
  tr[1][3] = 0;
  tr[2][0] = 0;
  tr[2][1] = 0;
  tr[2][2] = 1;
  tr[2][3] = 0;
  tr[3][0] = 0;
  tr[3][1] = 0;
  tr[3][2] = 0;
  tr[3][3] = 1;

  switch(dim)
    {
    case X:
      tr[1][1] = cosT;
      tr[1][2] = -sinT;
      tr[2][1] = sinT;
      tr[2][2] = cosT;
      break;
    case Y:
      tr[0][0] = cosT;
      tr[0][2] = sinT;
      tr[2][0] = -sinT;
      tr[2][2] = cosT;
      break;
    case Z:
      tr[0][0] = cosT;
      tr[0][1] = -sinT;
      tr[1][0] = sinT;
      tr[1][1] = cosT;
      break;
    }

  return tr;
}

// Uniform scaling helper (4x4)
Transform scale4x4(const double & scale)
{
  Transform tr(4, 4);
  tr[0][0] = scale;
  tr[0][1] = 0;
  tr[0][2] = 0;
  tr[0][3] = 0;
  tr[1][0] = 0;
  tr[1][1] = scale;
  tr[1][2] = 0;
  tr[1][3] = 0;
  tr[2][0] = 0;
  tr[2][1] = 0;
  tr[2][2] = scale;
  tr[2][3] = 0;
  tr[3][0] = 0;
  tr[3][1] = 0;
  tr[3][2] = 0;
  tr[3][3] = 1;

  return tr;
}


Transform perspective4x4(const double & fovYDegrees, const double & aspectRatio, 
			 const double & near, const double & far)
{
		Transform rt;
		
		double top = near * tan((fovYDegrees * M_PI) / 180.0 /2.0);
		double right = aspectRatio * top;
		
		rt[0][0] = near / right;
		rt[0][2] = 0;
		rt[0][1] = 0;
		rt[0][3] = 0;
		
		rt[1][0] = 0;
		rt[1][1] = near / top;
		rt[1][2] = 0;
		rt[1][3] = 0;
		
		rt[2][0] = 0;
		rt[2][1] = 0;
		rt[2][2] = (far + near) / (far - near);
		rt[2][3] = (-2 * far * near) / (far - near);
		
		rt[3][0] = 0;
		rt[3][1] = 0;
		rt[3][2] = 1;
		rt[3][3] = 0;
		
		return rt;		
}


// Translation helper (4x4)
Transform translate4x4(const double & offX, const double & offY, const double & offZ)
{
  Transform tr(4, 4);
  tr[0][0] = 1;
  tr[0][1] = 0;
  tr[0][2] = 0;
  tr[0][3] = offX;
  tr[1][0] = 0;
  tr[1][1] = 1;
  tr[1][2] = 0;
  tr[1][3] = offY;
  tr[2][0] = 0;
  tr[2][1] = 0;
  tr[2][2] = 1;
  tr[2][3] = offZ;
  tr[3][0] = 0;
  tr[3][1] = 0;
  tr[3][2] = 0;
  tr[3][3] = 1;

  return tr;
}




Transform camera4x4(const double & offX, const double & offY, const double & offZ,
					 const double & yaw, const double & pitch, const double & roll)
{
	Transform trans = translate4x4(-offX, -offY, -offZ);
	Transform rotX = rotate4x4(X, -pitch);
	Transform rotY = rotate4x4(Y, -yaw);
	
	Transform rt = rotX * rotY * trans;
	return rt;	
}



#endif