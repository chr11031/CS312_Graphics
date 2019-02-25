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
  double** mat;


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

  // Dynamic memory 
  void alloc()
  {
    // Allocate pointers for column references
    mat = (double**)malloc(sizeof(double*) * rowLen);

    // Allocate pointer to each row
    for(int i = 0; i < rowLen; i++)
      {
	mat[i] = (double*)malloc(sizeof(double) * colLen);
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
      alloc();
      setIdentity();
    }

  // Initialize a vertex as a matrix
  Transform(const Vertex & vert)
    {
      rowLen = 4;
      colLen = 1;
      alloc();
      mat[0][0] = vert.x;
      mat[1][0] = vert.y;
      mat[2][0] = vert.z;
      mat[3][0] = vert.w;
    }

  // De-allocate memory
  ~Transform()
    {
      // De-Allocate pointer to each row
      for(int i = 0; i < rowLen; i++)
        {
	  free(mat[i]);
        }

      // De-Allocate pointers for column references
      free(mat);
      mat = NULL;
    }

  // Initialize matrix to suggested values
  Transform(double values[], int width = MAX_DIM_SIZE_MATRIX, int height = MAX_DIM_SIZE_MATRIX)
    {
      colLen = width;
      rowLen = height;
      alloc();
      setValues(values);
    }

  // Assignment operator 
  Transform& operator = (const Transform & right)
    {
      colLen = right.colLen;
      rowLen = right.rowLen;
      alloc();
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
      tr[0][2] = -sinT;
      tr[2][0] = sinT;
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

// Projection matrix helper (4x4)
Transform perspectiveProjection4x4(const double & fovYDegs, const double & aspectRatio, const double & near, const double & far)
{
  Transform tr(4, 4);
    
  double top = near * tan((fovYDegs * M_PI / 180.0)/ 2) ;
  double right = aspectRatio * top;

  tr[0][0] = near / right;
  tr[0][1] = 0;
  tr[0][2] = 0;
  tr[0][3] = 0;

  tr[1][0] = 0;
  tr[1][1] = near / top;
  tr[1][2] = 0;
  tr[1][3] = 0;

  tr[2][0] = 0;
  tr[2][1] = 0;
  tr[2][2] = (far + near) / (far - near);
  tr[2][3] = (-2 * far * near) / (far - near);

  tr[3][0] = 0;
  tr[3][1] = 0;
  tr[3][2] = 1;
  tr[3][3] = 0;

  return tr;
}

// View or Camera transform - everything is done in reverse here 
Transform viewTransform4x4(const double & yaw, const double & pitch, const double & roll,
                           const double & xPos, const double & yPos, const double & zPos)
{
  // Angles
  double yawRad = (yaw / 180.0) * M_PI;
  double yawPitch = (pitch / 180.0) * M_PI;

  // Original vectors
  double rightX = 1.0;
  double rightY = 0.0;
  double rightZ = 0.0;
  double forwardX = 0.0;
  double forwardY = 0.0;
  double forwardZ = 1.0;
  double upX = 0.0;
  double upY = 1.0;
  double upZ = 0.0;

  // Rotate around Z-Axis 
  // Goes here....

  // Rotate around X-Axis
  double rightZRot = rightZ * cos(yawPitch) - rightY * sin(yawPitch);
  double rightXRot = rightX;
  double rightYRot = rightZ * sin(yawPitch) + rightY * cos(yawPitch);
  double upZRot = upZ * cos(yawPitch) - upY * sin(yawPitch);
  double upXRot = upX;
  double upYRot = upZ * sin(yawPitch) + upY * cos(yawPitch);
  double forwardZRot = forwardZ * cos(yawPitch) - forwardY * sin(yawPitch);
  double forwardXRot = forwardX;
  double forwardYRot = forwardZ * sin(yawPitch) + forwardY * cos(yawPitch);

  // Rotate around Y-Axis
  double rightXRotRot = rightXRot * cos(yawRad) - rightZRot * sin(yawRad);
  double rightYRotRot = rightYRot;
  double rightZRotRot = rightXRot * sin(yawRad) + rightZRot * cos(yawRad);
  double upXRotRot = upXRot * cos(yawRad) - upZRot * sin(yawRad);
  double upYRotRot = upYRot;
  double upZRotRot = upXRot * sin(yawRad) + upZRot * cos(yawRad);
  double forwardXRotRot = forwardXRot * cos(yawRad) - forwardZRot * sin(yawRad);
  double forwardYRotRot = forwardYRot;
  double forwardZRotRot = forwardXRot * sin(yawRad) + forwardZRot * cos(yawRad);

  // Plug everything in as rotation transform
  Transform rot;
  rot[0][0] = rightXRotRot;
  rot[0][1] = rightYRotRot;
  rot[0][2] = rightZRotRot;
  rot[0][3] = 0;
  rot[1][0] = upXRotRot;
  rot[1][1] = upYRotRot;
  rot[1][2] = upZRotRot;
  rot[1][3] = 0;
  rot[2][0] = forwardXRotRot;
  rot[2][1] = forwardYRotRot;
  rot[2][2] = forwardZRotRot;
  rot[2][3] = 0;       
  rot[3][0] = 0;
  rot[3][1] = 0;
  rot[3][2] = 0;
  rot[3][3] = 1;
    
  // Translation component
  Transform trans = translate4x4(-xPos, -yPos, -zPos);
  Transform tr = rot * trans;
  return tr;
}


#endif
