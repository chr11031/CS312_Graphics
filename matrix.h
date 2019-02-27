#include "definitions.h"

#ifndef MATRIX_H
#define MATRIX_H

/***************************************************
 * MATRIX - Class to facilitate matrix operations
 **************************************************/
class Matrix {
    public:
    Matrix() { clear(); }               // initialize with identity matrix
    Matrix(double* values);             // initialize with 16 values

    double v[4][4];

    void clear(); // set back to default state (identity matrix)

    const double& operator[](const int i) const { return v[i/4][i%4]; } // array access
    double& operator[](const int i) { return v[i/4][i%4]; }             // non-const array access
};

// Constructor that takes an array of 16 values for the matrix
Matrix::Matrix(double* values) {
    for (int r = 0; r < 4; r++)
    for (int c = 0; c < 4; c++)
        this->v[r][c] = values[r*4 + c];
}

// Sets the values of the matrix to the identity matrix
void Matrix::clear() {
    for (int r = 0; r < 4; r++)
    for (int c = 0; c < 4; c++)
        this->v[r][c] = (r == c ? 1 : 0);
}

// Multiplication of 4x4 by 4x1 matrix
Vertex operator* (const Matrix& lhs, const Vertex& rhs) {
    Vertex result = {
          lhs[0] * rhs.x
        + lhs[1] * rhs.y
        + lhs[2] * rhs.z
        + lhs[3] * rhs.w,

          lhs[4] * rhs.x
        + lhs[5] * rhs.y
        + lhs[6] * rhs.z
        + lhs[7] * rhs.w,

          lhs[8] * rhs.x
        + lhs[9] * rhs.y
        + lhs[10] * rhs.z
        + lhs[11] * rhs.w,

          lhs[12] * rhs.x
        + lhs[13] * rhs.y
        + lhs[14] * rhs.z
        + lhs[15] * rhs.w,
    };
    return result;
}

// Multiplication of 4x4 by 4x4 matrix
Matrix operator* (const Matrix& lhs, const Matrix& rhs) {
    Matrix result;
    
    // Loop over every cell in result
    for (int i = 0; i < 16; i++) {
        int r = 4 * (i / 4);
        int c = i % 4;
        double sum = 0;

        // Loop 4 times (row is 4 long, col is 4 long)
        for (int j = 0; j < 4; j++) {
            sum += (lhs[r] * rhs[c]);
            r++;
            c += 4;
        }
        result[i] = sum;
    }

    return result;
}

/***************************************************
 * Matrix transformation functions - return a
 * matrix that has the specified transformation
 **************************************************/
Matrix translateMatrix(const double& x, const double& y, const double& z) {
    Matrix matrix;
    matrix[3]  = x;
    matrix[7]  = y;
    matrix[11] = z;
    return matrix;
}

Matrix scaleMatrix(const double& scale) {
    Matrix matrix;
    matrix[0]  = scale;
    matrix[5]  = scale;
    matrix[10] = scale;
    return matrix;
}

// Takes an angle in radians and the axis to rotate around
Matrix rotateMatrix(AXIS axis, const double& angle) {
    Matrix matrix;
    double sinangle = sin(angle);
    double cosangle = cos(angle);
    switch (axis) {
        case X:
        matrix[5]  = cosangle;
        matrix[6]  = -sinangle;
        matrix[9]  = sinangle;
        matrix[10] = cosangle;
        break;
        case Y:
        matrix[0]  = cosangle;
        matrix[2]  = sinangle;
        matrix[8]  = -sinangle;
        matrix[10] = cosangle;
        break;
        case Z:
        matrix[0]  = cosangle;
        matrix[1]  = -sinangle;
        matrix[4]  = sinangle;
        matrix[5]  = cosangle;
        break;
    }
    return matrix;
}

/*
// View Transform
Matrix viewTransform(const double& offX, const double& offY, const double& offZ,
                     const double& yaw, const double& pitch, const double& roll) {
    // x = pitch
    // y = yaw
    // z = roll
    Matrix matrix;

    Matrix translate = translateMatrix(-offX, -offY, -offZ);
    // First do pitch, then yaw (roll is optional)
    double pitchRad = pitch * M_PI / 180.0;
    double yawRad   = yaw   * M_PI / 180.0;
    Matrix rotX = rotateXMatrix(pitchRad);
    Matrix rotY = rotateYMatrix(yawRad);

    matrix = rotX * rotY * translate;
    return matrix;
}
*/

Matrix perspectiveTransform(const double& fovYDegrees, const double& aspectRatio,
                            const double& near, const double& far)
{
    Matrix matrix;

    double top = near * tan(fovYDegrees * M_PI / 180.0) / 2.0;
    double right = aspectRatio * top;
    matrix[0]  = near / right;
    matrix[5]  = near / top;
    matrix[10] = (far + near) / (far - near);
    matrix[11] = (-2 * far * near) / (far - near);
    matrix[14] = 1;
    matrix[15] = 0;

    return matrix;
}

#endif // MATRIX_H