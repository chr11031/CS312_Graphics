#include "definitions.h"

#ifndef MATRIX_H
#define MATRIX_H


/****************************************************
 * Matrix for transforming vertices. 
 ****************************************************/
class Matrix
{
  private:
    double * matrixPtr;
    int rows;
    int columns;

  public:
    Matrix(): rows(0), columns(0), matrixPtr(NULL) {};

    // constructor that sets up the matrix size
    Matrix(int rows, int columns)
    {
        this->rows = rows;
        this->columns = columns;
        matrixPtr = new double[rows * columns];
        for(int i = 0; i < rows; i++)
        {
            for(int j = 0; j < columns; j++)
            {
                if (i == j)
                    matrixPtr[i * columns + j] = 1;
                else
                    matrixPtr[i * columns + j] = 0;
            }
        }
    }

    ~Matrix()
    {
        if(matrixPtr != NULL)
        {
            delete [] matrixPtr;
            matrixPtr = NULL;
        }
    }

    void initializeMatrix(int rows, int columns)
    {
        this->rows = rows;
        this->columns = columns;
        matrixPtr = new double[rows * columns];
        for(int i = 0; i < rows; i++)
        {
            for(int j = 0; j < columns; j++)
            {
                if (i == j)
                    matrixPtr[i * columns + j] = 1;
                else
                    matrixPtr[i * columns + j] = 0;
            }
        }
    }
    
    /*************************************************************************
     * Add X Rotation
     * Add rotation to the current matrix.
     ************************************************************************/
    void addXRotation(double rot/*radians*/)
    {
        Matrix rotate(this->rows, this->columns);
        // initialize the rotation matrix
        rotate.matrixPtr[1 * this->columns + 1] = cos(rot);
        rotate.matrixPtr[1 * this->columns + 2] = -sin(rot);
        rotate.matrixPtr[2 * this->columns + 1] = sin(rot);
        rotate.matrixPtr[2 * this->columns + 2] = cos(rot);
        
        rotate *= (*this);
        (*this) = rotate;
    }

    /*************************************************************************
     * Add Y Rotation
     * Add rotation to the current matrix.
     ************************************************************************/
    void addYRotation(double rot/*radians*/)
    {
        Matrix rotate(this->rows, this->columns);
        // initialize the rotation matrix
        rotate.matrixPtr[0] = cos(rot);
        rotate.matrixPtr[2] = sin(rot);
        rotate.matrixPtr[2 * this->columns] = -sin(rot);
        rotate.matrixPtr[2 * this->columns + 2] = cos(rot);
        
        rotate *= (*this);
        (*this) = rotate;
    }

    /*************************************************************************
     * Add Z Rotation
     * Add rotation to the current matrix.
     ************************************************************************/
    void addZRotation(double rot/*radians*/)
    {
        Matrix rotate(this->rows, this->columns);
        // initialize the rotation matrix
        rotate.matrixPtr[0] = cos(rot);
        rotate.matrixPtr[1] = -sin(rot);
        rotate.matrixPtr[1 * this->columns] = sin(rot);
        rotate.matrixPtr[1 * this->columns + 1] = cos(rot);

        rotate *= (*this);
        (*this) = rotate;
    }

    /*************************************************************************
     * Add Scaling
     * Add scaling to the current matrix.
     ************************************************************************/
    void addScaling(double scaleX, double scaleY, double scaleZ)
    {
        // this array is used in the for loop to make things easier. 
        double scaleArray[4] = {scaleX, scaleY, scaleZ, 1};

        Matrix scale(this->rows, this->columns);
        // initialize the scaling matrix
        for(int i = 0; i < this->rows; i++)
        {
            scale.matrixPtr[i * this->columns + i] = scaleArray[i];
        }

        scale *= (*this);
        (*this) = scale;
    }

    /*************************************************************************
     * Add Translation
     * Add translation to the current matrix.
     ************************************************************************/
    void addTranslation(double transX, double transY, double transZ)
    {
        Matrix translate(this->rows, this->columns);
        // initialize translation matrix.
        translate.matrixPtr[1 * this->columns - 1] = transX;
        translate.matrixPtr[2 * this->columns - 1] = transY;
        translate.matrixPtr[3 * this->columns - 1] = transZ;

        translate *= (*this);
        (*this) = translate;
    }


    /**************************************************************************
     * camera4x4
     * camera tranform
     **************************************************************************/
    void camera4x4(const double & offX, const double & offY, const double & offZ,
                    const double & yaw, const double & pitch /*degrees*/, const double & roll)
    {
        Matrix tr(4, 4);

        // Matrix trans;
        tr.addTranslation(-offX, -offY, -offZ);

        // y rotation == yaw
        // x rotation == pitch
        // z rotation == roll
        double pitchRad = (pitch / 180.0) * M_PI;
        double yawRad = (yaw / 180.0) * M_PI;
        tr.addXRotation(pitchRad);
        tr.addYRotation(yawRad);

        tr *= (*this);
        (*this) = tr;
    }

    /**************************************************************************
     * perspective4x4
     * perspective transform
     *************************************************************************/
    void perspective4x4(const double & fovYDegrees, const double & aspectRation, 
                            const double & near, const double & far)
    {
        Matrix rt(4, 4);
        double top = near * tan((fovYDegrees * M_PI) / 180.0) / 2.0;
        double right = aspectRation * top;

        rt.matrixPtr[0] = near/right;
        rt.matrixPtr[5] = near/top;
        rt.matrixPtr[10] = (far + near) / (far - near);
        rt.matrixPtr[11] = (-2 * far * near) / (far - near);
        rt.matrixPtr[14] = 1;
        rt.matrixPtr[15] = 0;

        rt *= (*this);
        (*this) = rt;
    }

    /**************************************************************************
     * Orthographic4x4
     * Orthographic transform
     *************************************************************************/
    void orthographic4x4(const double & fovYDegrees, const double & aspectRation, 
                            const double & near, const double & far)
    {
        Matrix rt(4, 4);
        double top = near * tan((fovYDegrees * M_PI) / 180.0) / 2.0;
        double right = aspectRation * top;

        rt.matrixPtr[0] = 1 / right;
        rt.matrixPtr[5] = 1 / top;
        rt.matrixPtr[10] = 2 / (far - near);
        rt.matrixPtr[11] = -(far + near) / (far - near);

        rt *= (*this);
        (*this) = rt;
    }

    /*****************************************************************
     * Equals operator.
     * rows and columns can be copied, but the matrix has to
     * be set to a new matrix of the right size and then the individual
     * values can be copied over.
     ****************************************************************/
    Matrix & operator = (const Matrix & rhs)
    {
        this->rows = rhs.rows;
        this->columns = rhs.columns;
       // this->init = rhs.init;
        if(this->matrixPtr != NULL)
        {
            delete [] this->matrixPtr;
        }
        matrixPtr = new double[this->rows * this->columns];
        for(int i = 0; i < (this->rows * this->columns); i++)
        {
            this->matrixPtr[i] = rhs.matrixPtr[i];
        }
        return *this;
    }

    /*************************************************************************
     * Multiplication operator
     * A friend function that multiplies the two matrices together.
     ************************************************************************/
    Matrix & operator *= (const Matrix & rhs)
    {
        if (this->columns != rhs.rows)
        {
            // the matrices can't be multiplied so return an empty one.
            Matrix mat;
            return mat;
        }

        Matrix newMatrix(this->rows, rhs.columns);
        for(int i = 0; i < this->rows; i++)
        {
            for(int j = 0; j < rhs.columns; j++)
            {
                double sum = 0;
                for(int k = 0; k < rhs.rows; k++)
                {
                     sum += this->matrixPtr[i * this->columns + k] * 
                        rhs.matrixPtr[k * rhs.columns + j];
                }
                newMatrix.matrixPtr[i * newMatrix.columns + j] = sum;
            }
        }
       // newMatrix.init = true;
        *this = newMatrix;
        return *this;
    }

    // has to be a friend to access the private variables.
    friend Vertex operator * (const Matrix & rhs, const Vertex & lhs);
};

/**************************************************************
 * A friend multiplication operator to help when multiplying 
 * a vertex.
 *************************************************************/
Vertex operator * (const Matrix & rhs, const Vertex & lhs)
{
    if (rhs.columns != 4 || rhs.rows != 4)
    {
        // A vertex has 4 "rows" so the matrix must have 4 columns and 4 rows
        // so the output will be correct.
        Vertex vert;
        return vert;
    }

    Vertex newVertex;
    newVertex.x = rhs.matrixPtr[0] * lhs.x + rhs.matrixPtr[1] * lhs.y + rhs.matrixPtr[2] * lhs.z + rhs.matrixPtr[3] * lhs.w;
    newVertex.y = rhs.matrixPtr[4] * lhs.x + rhs.matrixPtr[5] * lhs.y + rhs.matrixPtr[6] * lhs.z + rhs.matrixPtr[7] * lhs.w;
    newVertex.z = rhs.matrixPtr[8] * lhs.x + rhs.matrixPtr[9] * lhs.y + rhs.matrixPtr[10] * lhs.z + rhs.matrixPtr[11] * lhs.w;
    newVertex.w = rhs.matrixPtr[12] * lhs.x + rhs.matrixPtr[13] * lhs.y + rhs.matrixPtr[14] * lhs.z + rhs.matrixPtr[15] * lhs.w;

    return newVertex;
}


#endif