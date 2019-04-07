#ifndef MATRIX_H
#define MATRIX_H

#include "definitions.h"

/**********************************************************
 * MATRIX
 * Encapsulates a transformation matrix
 *********************************************************/
class Matrix
{
    private:
        double matrix[4][4];

    public:
        Matrix() 
        {
            matrix[0][0] = 1;
            matrix[0][1] = 0;
            matrix[0][2] = 0;
            matrix[0][3] = 0;

            matrix[1][0] = 0;
            matrix[1][1] = 1;
            matrix[1][2] = 0;
            matrix[1][3] = 0;

            matrix[2][0] = 0;
            matrix[2][1] = 0;
            matrix[2][2] = 1;
            matrix[2][3] = 0;

            matrix[3][0] = 0;
            matrix[3][1] = 0;
            matrix[3][2] = 0;
            matrix[3][3] = 1;
        }

        Matrix(double matrix[4][4])
        {
            this->matrix[0][0] = matrix[0][0];
            this->matrix[0][1] = matrix[0][1];
            this->matrix[0][2] = matrix[0][2];
            this->matrix[0][3] = matrix[0][3];

            this->matrix[1][0] = matrix[1][0];
            this->matrix[1][1] = matrix[1][1];
            this->matrix[1][2] = matrix[1][2];
            this->matrix[1][3] = matrix[1][3];

            this->matrix[2][0] = matrix[2][0];
            this->matrix[2][1] = matrix[2][1];
            this->matrix[2][2] = matrix[2][2];
            this->matrix[2][3] = matrix[2][3];

            this->matrix[3][0] = matrix[3][0];
            this->matrix[3][1] = matrix[3][1];
            this->matrix[3][2] = matrix[3][2];
            this->matrix[3][3] = matrix[3][3];
        }

        double* operator[](int i)
        {
            if(i < 0 || i >= 4)
                throw(false);
            return matrix[i];
        }

        const double* operator[](int i) const
        {
            if(i < 0 || i >= 4)
                throw(false);
            return matrix[i];
        }
};

// Multiplies two matrices
Matrix operator*(const Matrix &lhs, const Matrix &rhs)
{
    Matrix result;
    for(int y = 0; y < 4; y++)
    {
        for(int x = 0; x < 4; x++)
        {
            result[y][x] = lhs[y][0] * rhs[0][x] + lhs[y][1] * rhs[1][x] + lhs[y][2] * rhs[2][x] + lhs[y][3] * rhs[3][x];
        }
    }
    return result;
}

// Muliplies a matrix by a vertex
Vertex operator*(const Matrix &lhs, const Vertex &rhs)
{
    Vertex result;
    result.x = rhs.x * lhs[0][0] + rhs.y * lhs[0][1] + rhs.z * lhs[0][2] + rhs.w * lhs[0][3];
    result.y = rhs.x * lhs[1][0] + rhs.y * lhs[1][1] + rhs.z * lhs[1][2] + rhs.w * lhs[1][3];
    result.z = rhs.x * lhs[2][0] + rhs.y * lhs[2][1] + rhs.z * lhs[2][2] + rhs.w * lhs[2][3];
    result.w = rhs.x * lhs[3][0] + rhs.y * lhs[3][1] + rhs.z * lhs[3][2] + rhs.w * lhs[3][3];
    return result;
}

// The identity matrix
Matrix IdentityMatrix()
{
    double data[4][4] = {
        {1.0, 0.0, 0.0, 0.0}, 
        {0.0, 1.0, 0.0, 0.0}, 
        {0.0, 0.0, 1.0, 0.0}, 
        {0.0, 0.0, 0.0, 1.0}};
    return Matrix(data);
}

// Transformation matrix that translates
Matrix TranslationMatrix(double dx, double dy, double dz)
{
    double data[4][4] = {
        {1.0, 0.0, 0.0, dx}, 
        {0.0, 1.0, 0.0, dy}, 
        {0.0, 0.0, 1.0, dz}, 
        {0.0, 0.0, 0.0, 1.0}};
    return Matrix(data);
}

// Transformation matrix that scales
Matrix ScaleMatrix(double sx, double sy, double sz)
{
    double data[4][4] = {
        { sx, 0.0, 0.0, 0.0}, 
        {0.0,  sy, 0.0, 0.0}, 
        {0.0, 0.0,  sz, 0.0}, 
        {0.0, 0.0, 0.0, 1.0}};
    return Matrix(data);
}

// Transformation matrix that rotates ccw around the x-axis
Matrix XRotationMatrix(double rad)
{
    double data[4][4] = {
        {1.0,       0.0,       0.0, 0.0}, 
        {0.0,  cos(rad), -sin(rad), 0.0}, 
        {0.0,  sin(rad),  cos(rad), 0.0}, 
        {0.0,       0.0,       0.0, 1.0}};
    return Matrix(data);
}

// Transformation matrix that rotates ccw around the y-axis
Matrix YRotationMatrix(double rad)
{
    double data[4][4] = {
        { cos(rad), 0.0, sin(rad), 0.0}, 
        {      0.0, 1.0,      0.0, 0.0}, 
        {-sin(rad), 0.0, cos(rad), 0.0}, 
        {      0.0, 0.0,      0.0, 1.0}};
    return Matrix(data);
}

// Transformation matrix that rotates ccw around the z-axis
Matrix ZRotationMatrix(double rad)
{
    double data[4][4] = {
        {cos(rad), -sin(rad), 0.0, 0.0}, 
        {sin(rad),  cos(rad), 0.0, 0.0}, 
        {     0.0,       0.0, 1.0, 0.0}, 
        {     0.0,       0.0, 0.0, 1.0}};
    return Matrix(data);
}

Matrix CameraMatrix(const double &offX, const double &offY, const double &offZ, const double &yaw, const double &pitch, const double &roll)
{
    Matrix trans = TranslationMatrix(-offX, -offY, -offZ);
    Matrix rotX = XRotationMatrix(-pitch * DEG_TO_RAD);
    Matrix rotY = YRotationMatrix(-yaw * DEG_TO_RAD);

    return rotX * rotY * trans;
}

Matrix PerspectiveMatrix(const double &fovY, const double &aspectRatio, const double &near, const double &far)
{
    double top = near * tan(fovY * DEG_TO_RAD / 2.0);
    double right = aspectRatio * top;
    Matrix rt;

    rt[0][0] = near / right;
    rt[0][1] = 0;
    rt[0][2] = 0;
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

Matrix OrthographicMatrix(const double &top, const double &bottom, const double &left, const double &right, const double &near, const double &far)
{
    Matrix rt;

    rt[0][0] = 2 / (right - left);
    rt[0][1] = 0;
    rt[0][2] = 0;
    rt[0][3] = -(right + left) / (right - left);

    rt[1][0] = 0;
    rt[1][1] = 2 / (top - bottom);
    rt[1][2] = 0;
    rt[1][3] = -(top + bottom) / (top - bottom);

    rt[2][0] = 0;
    rt[2][1] = 0;
    rt[2][2] = 2 / (far - near);
    rt[2][3] = -(far + near) / (far - near);

    rt[3][0] = 0;
    rt[3][1] = 0;
    rt[3][2] = 0;
    rt[3][3] = 1;

    return rt;
}
       
#endif