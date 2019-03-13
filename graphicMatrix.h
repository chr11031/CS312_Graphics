#include <cmath>
#include <iostream>
using namespace std;

#ifndef GRAPHICMATRIX_H
#define GRAPHICMATRIX_H

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
    private:
        
    public:
        double * matrixPtr;
        int row;
        int col;
        //default
        Matrix();

        //non default - Makes plain identity matrix
        Matrix(int row, int col)
        {
            this->row = row;
            this->col = col;
            matrixPtr = new double [row * col];
            
            
            for(int i = 0; i < row; i++){
                for(int j = 0; j < col; j++)
                {
                    if(i==j)
                        matrixPtr[i * col + j] = 1;
                    else
                        matrixPtr[i * col + j] = 0;
                }
                //0  1  2  3
                //4  5  6  7
                //8  9  10 11
                //12 13 14 15
            }
        }
        Matrix(Vertex vert)
        {
            this->row = 4;
            this->col = 4;
            matrixPtr = new double [row * col];
            for(int i = 0; i < row; i++){
                for(int j = 0; j < col; j++)
                {
                        this->matrixPtr[i * col + j] = 0;
                }
            }
                //0x  1  2   3
                //4   5y 6   7
                //8   9  10z 11
                //12  13 14  15w
            this->setData(0,vert.x);
            this->setData(5,vert.y);
            this->setData(10,vert.z);
            this->setData(15,vert.w);
        }
        //destructor
        ~Matrix() { delete[] matrixPtr; }


        //copy constructor
        Matrix(const Matrix & other)
        {
            if (other.row == 0 || other.col == 0)
            {
               row = col = 0;
               matrixPtr = NULL;
               return;
            }
            row = other.row;
            col = other.col;
            matrixPtr = new double[row * col];
            for (int i = 0; i < other.row * other.col; i++)
            {
                matrixPtr[i] = other.matrixPtr[i];
            }
        }
        
        // Assignment operator overload
        Matrix & operator = (const Matrix & rhs)
        {
            if (rhs.row == 0 || rhs.col == 0)
            {
               row = col = 0;
               matrixPtr = NULL;
               return *this;
            }
            row = rhs.row;
            col = rhs.col;
            matrixPtr = new double[row * col];
            for (int i = 0; i < rhs.row * rhs.col; i++)
            {
                matrixPtr[i] = rhs.matrixPtr[i];
            }
            return *this;
        }



        //Get/Set
        double getData(int i) { return matrixPtr[i];}
        void setData(int i, double data) {matrixPtr[i] = data;}
    
    //default rotation matrix manipulator
    void rotate(double xRotDeg, double yRotDeg, double zRotDeg){
        double xRad = xRotDeg * M_PI / 180;
        double yRad = yRotDeg * M_PI / 180;
        double zRad = zRotDeg * M_PI / 180;
        //RX 1 0    0     0 RY cos0  0 sin0 0 RZ cos0 -sin0 0 0
        //   0 cos0 -sin0 0    0     1 0    0    sin0 cos0  0 0
        //   0 sin0 cos0  0    -sin0 0 cos0 0    0    0     1 0
        //   0 0    0     1    0     0 0    1    0    0     0 1
        //set default identity matrex
        Matrix xRot = Matrix(4,4);
        Matrix yRot = Matrix(4,4);
        Matrix zRot = Matrix(4,4);
        if (xRotDeg !=0)//if rotation is not 0 apply rotation
        {
            xRot.setData(5,cos(xRad));
            xRot.setData(6,-1 * sin(xRad));
            xRot.setData(9,sin(xRad));
            xRot.setData(10,cos(xRad));
        }
        if (yRotDeg !=0)
        {
            yRot.setData(0,cos(yRad));
            yRot.setData(2,sin(yRad));
            yRot.setData(8,-1 * sin(yRad));
            yRot.setData(10,cos(yRad));
        }
        if (zRotDeg !=0)
        {
            zRot.setData(0,cos(zRad));
            zRot.setData(1,-1 * sin(zRad));
            zRot.setData(4,sin(zRad));
            zRot.setData(5,cos(zRad));
        }
        Matrix trueRot = xRot * yRot * zRot;
        *this = *this * trueRot;
    }

    //default scale matrix manipulator
    void scale(double xScale, double yScale, double zScale){
        //x 0 0 0
        //0 y 0 0
        //0 0 z 0
        //0 0 0 1(w)
        Matrix scaleM = Matrix(4,4);
        if (xScale != 0) {scaleM.setData(0,xScale);}
        if (yScale != 0) {scaleM.setData(5,yScale);}
        if (zScale != 0) {scaleM.setData(10,zScale);}
        *this = *this * scaleM;
    }

    void uniScale(double ratio){
        Matrix uniS = Matrix(4,4);
        uniS.scale(ratio,ratio,ratio);
        *this = *this * uniS;
    }

    //default translate matrix manipulator
    void translate(double xTrans, double yTrans, double zTrans){
        //1 0 0 x
        //0 1 0 y
        //0 0 1 z
        //0 0 0 1(w)
        Matrix transM = Matrix(4,4);
        if (xTrans != 0) {transM.setData(3,xTrans);}
        if (yTrans != 0) {transM.setData(7,yTrans);}
        if (zTrans != 0) {transM.setData(11,zTrans);}
        *this = *this * transM;
    }

    //Perspective Transformation
    void transformPerspective(const double & fovYDegrees, const double & aspectRatio, 
            const double & nearPlane, const double & farPlane){
        //n/r 0   0           0
        //0   n/t 0           0 
        //0   0   (f+n)/(f-n) (-2*f*n)/(f-n)
        //0   0   1           0(w)
        Matrix persT = Matrix(4,4);
        double top = nearPlane * tan((fovYDegrees * M_PI) / 180.0) / 2.0;
        double right = aspectRatio * top;

        persT.setData(0, (nearPlane / right));
        persT.setData(5,(nearPlane/top));
        persT.setData(10,((farPlane + nearPlane)/(farPlane - nearPlane)));
        persT.setData(11,((-2 * farPlane * nearPlane)/(farPlane - nearPlane)));
        persT.setData(14,1);
        persT.setData(15,0);   

        *this = *this * persT;
    }

/* Assume semetric
    void transformOrthographicGeneral(const double & fovYDegrees, const double & aspectRatio, 
            const double & nearPlane, const double & farPlane){
                //WARNING!!! INCOMPLETE
        // 2/(right-l) 0            0        -((right + l)/(right - l))
        // 0           2/(top - b)  0        -((t+b)/(t-b))
        // 0           0            -2/(f-n) -((f+n)/(f-n))
        // 0           0            0        1
        Matrix orthoT = Matrix(4,4);
        double top = nearPlane * tan((fovYDegrees * M_PI) / 180.0) / 2.0;
        double right = aspectRatio * top;
        //double bottom =
        //double left =

        orthoT.setData(0,(2/(right-1)));
        orthoT.setData(3,(-(right+1)/(right-1)));
        //orthoT.setData(5,(2/(top-bottom)));
        //orthoT.setData(7,(-(top+bottom)/(top-bottom)));
        orthoT.setData(10,(-2/(farPlane-nearPlane)));
        orthoT.setData(11,(-(farPlane+nearPlane)/(farPlane-nearPlane)));


        *this = *this * orthoT;
    }*/

    void transformOrthographicSymmetric(const double & fovYDegrees, const double & aspectRatio, 
            const double & nearPlane, const double & farPlane){
        // 1/r 0   0        0
        // 0   1/t 0        0
        // 0   0   -2/(f-n) -((f+n)/(f-n))
        // 0   0   0        1
        Matrix orthoT = Matrix(4,4);
        double top = nearPlane * tan((fovYDegrees * M_PI) / 180.0) / 2.0;
        double right = aspectRatio * top;

        orthoT.setData(0,(1/right));
        orthoT.setData(5,(1/top));
        orthoT.setData(10,(-2/(farPlane-nearPlane)));
        orthoT.setData(11,(-((farPlane+nearPlane)/(farPlane+nearPlane))));

        *this = *this * orthoT;

    }

    //camera transform
    void cameraTransform(double x, double y, double z, double pitch, double yaw, double roll){
        Matrix cameraT = Matrix(4,4);
        cameraT.translate(-x,-y,-z);
        cameraT.rotate(pitch,yaw,roll);

        *this = *this * cameraT;
    }


    // bracket operator overload
    double operator [] (int i) {return matrixPtr[i];}

    // Multiplication operator overload
    Matrix operator * (const Matrix & rhs) const
    {
        if (this->col != rhs.row)//should always be 4x4
        {
            // that matrices can't be multiplied so return an empty one.
            Matrix mat = Matrix(4,4);
            return mat;
        }

        Matrix newMatrix(this->row, rhs.col);
        int rSize = rhs.row * rhs.col;
        int lSize = this->row * this->col; //should always be 4x4
        
        
            newMatrix.setData(0,((this->matrixPtr[0] * rhs.matrixPtr[0]) + (this->matrixPtr[1] * rhs.matrixPtr[4])
             + (this->matrixPtr[2] * rhs.matrixPtr[8]) + (this->matrixPtr[3] * rhs.matrixPtr[12])));
            newMatrix.setData(1,((this->matrixPtr[0] * rhs.matrixPtr[1]) + (this->matrixPtr[1] * rhs.matrixPtr[5])
             + (this->matrixPtr[2] * rhs.matrixPtr[9]) + (this->matrixPtr[3] * rhs.matrixPtr[13])));
            newMatrix.setData(2,((this->matrixPtr[0] * rhs.matrixPtr[2]) + (this->matrixPtr[1] * rhs.matrixPtr[6])
             + (this->matrixPtr[2] * rhs.matrixPtr[10]) + (this->matrixPtr[3] * rhs.matrixPtr[14])));
            newMatrix.setData(3,((this->matrixPtr[0] * rhs.matrixPtr[3]) + (this->matrixPtr[1] * rhs.matrixPtr[7])
             + (this->matrixPtr[2] * rhs.matrixPtr[11]) + (this->matrixPtr[3] * rhs.matrixPtr[15])));
            
            newMatrix.setData(4,((this->matrixPtr[4] * rhs.matrixPtr[0]) + (this->matrixPtr[5] * rhs.matrixPtr[4])
             + (this->matrixPtr[6] * rhs.matrixPtr[8]) + (this->matrixPtr[7] * rhs.matrixPtr[12])));
            newMatrix.setData(5,((this->matrixPtr[4] * rhs.matrixPtr[1]) + (this->matrixPtr[5] * rhs.matrixPtr[5])
             + (this->matrixPtr[6] * rhs.matrixPtr[9]) + (this->matrixPtr[7] * rhs.matrixPtr[13])));
            newMatrix.setData(6,((this->matrixPtr[4] * rhs.matrixPtr[2]) + (this->matrixPtr[5] * rhs.matrixPtr[6])
             + (this->matrixPtr[6] * rhs.matrixPtr[10]) + (this->matrixPtr[7] * rhs.matrixPtr[14])));
            newMatrix.setData(7,((this->matrixPtr[4] * rhs.matrixPtr[3]) + (this->matrixPtr[5] * rhs.matrixPtr[7])
             + (this->matrixPtr[6] * rhs.matrixPtr[11]) + (this->matrixPtr[7] * rhs.matrixPtr[15])));
            
            newMatrix.setData(8,((this->matrixPtr[8] * rhs.matrixPtr[0]) + (this->matrixPtr[9] * rhs.matrixPtr[4])
             + (this->matrixPtr[10] * rhs.matrixPtr[8]) + (this->matrixPtr[11] * rhs.matrixPtr[12])));
            newMatrix.setData(9,((this->matrixPtr[8] * rhs.matrixPtr[1]) + (this->matrixPtr[9] * rhs.matrixPtr[5])
             + (this->matrixPtr[10] * rhs.matrixPtr[9]) + (this->matrixPtr[11] * rhs.matrixPtr[13])));
            newMatrix.setData(10,((this->matrixPtr[8] * rhs.matrixPtr[2]) + (this->matrixPtr[9] * rhs.matrixPtr[6])
             + (this->matrixPtr[10] * rhs.matrixPtr[10]) + (this->matrixPtr[11] * rhs.matrixPtr[14])));
            newMatrix.setData(11,((this->matrixPtr[8] * rhs.matrixPtr[3]) + (this->matrixPtr[9] * rhs.matrixPtr[7])
             + (this->matrixPtr[10] * rhs.matrixPtr[11]) + (this->matrixPtr[11] * rhs.matrixPtr[15])));
            
            newMatrix.setData(12,((this->matrixPtr[12] * rhs.matrixPtr[0]) + (this->matrixPtr[13] * rhs.matrixPtr[4])
             + (this->matrixPtr[14] * rhs.matrixPtr[8]) + (this->matrixPtr[15] * rhs.matrixPtr[12])));
            newMatrix.setData(13,((this->matrixPtr[12] * rhs.matrixPtr[1]) + (this->matrixPtr[13] * rhs.matrixPtr[5])
             + (this->matrixPtr[14] * rhs.matrixPtr[9]) + (this->matrixPtr[15] * rhs.matrixPtr[13])));
            newMatrix.setData(14,((this->matrixPtr[12] * rhs.matrixPtr[2]) + (this->matrixPtr[13] * rhs.matrixPtr[6])
             + (this->matrixPtr[14] * rhs.matrixPtr[10]) + (this->matrixPtr[15] * rhs.matrixPtr[14])));
            newMatrix.setData(15,((this->matrixPtr[12] * rhs.matrixPtr[3]) + (this->matrixPtr[13] * rhs.matrixPtr[7])
             + (this->matrixPtr[14] * rhs.matrixPtr[11]) + (this->matrixPtr[15] * rhs.matrixPtr[15])));
        /*}
        
        else
        {
            for(int i = 0; i < this->col; i++)
            {
                for(int j = 0; j < rhs.row; j++)
                {
                    newMatrix.matrixPtr[i * newMatrix.col + j] += 
                        this->matrixPtr[i * this->col + j] * 
                        rhs.matrixPtr[j * rhs.col + i];
                }
            }
        }*/
        return newMatrix;
    }

    // Multiply a 4-component vertex by this matrix, return vertex
    Vertex operator *(const Vertex & rhs) const 
    {
        // Convert Vertex to Matrix 
        //Matrix vl = Matrix(right);
        
        // Multiply
        //Matrix out = Matrix(4,4);
        //out = *this * vl;
        
        // Return in vertex format
        Vertex rv;
        rv.x = (this->matrixPtr[0]  * rhs.x)+(this->matrixPtr[1]  * 
            rhs.y)+(this->matrixPtr[2]  * rhs.z)+(this->matrixPtr[3]  * rhs.w);
        rv.y = (this->matrixPtr[4]  * rhs.x)+(this->matrixPtr[5]  * 
            rhs.y)+(this->matrixPtr[6]  * rhs.z)+(this->matrixPtr[7]  * rhs.w);
        rv.z = (this->matrixPtr[8]  * rhs.x)+(this->matrixPtr[9]  * 
            rhs.y)+(this->matrixPtr[10] * rhs.z)+(this->matrixPtr[11] * rhs.w);
        rv.w = (this->matrixPtr[12] * rhs.x)+(this->matrixPtr[13] * 
            rhs.y)+(this->matrixPtr[14] * rhs.z)+(this->matrixPtr[15] * rhs.w);
        return rv;
}

    //friend Vertex operator * (Matrix & rhs, Vertex & lhs);
};
/*
//matrix * vertex operator
Vertex operator * (Vertex & lhs, Matrix & rhs)
{
    if (rhs.col != 4 || rhs.row != 4)
    {
        // A vertex has 4 "rows" so the matrix must have 4 columns
        Vertex vert;
        return vert;
    }

    Vertex newVertex;
    newVertex.x = rhs.matrixPtr[0] * lhs.x + rhs.matrixPtr[1] * lhs.y + rhs.matrixPtr[2] * lhs.z + rhs.matrixPtr[3] * lhs.w;
    newVertex.y = rhs.matrixPtr[4] * lhs.x + rhs.matrixPtr[5] * lhs.y + rhs.matrixPtr[6] * lhs.z + rhs.matrixPtr[7] * lhs.w;
    newVertex.z = rhs.matrixPtr[8] * lhs.x + rhs.matrixPtr[9] * lhs.y + rhs.matrixPtr[10] * lhs.z + rhs.matrixPtr[11] * lhs.w;
    newVertex.w = rhs.matrixPtr[12] * lhs.x + rhs.matrixPtr[13] * lhs.y + rhs.matrixPtr[14] * lhs.z + rhs.matrixPtr[15] * lhs.w;

    return newVertex;
}*/


#endif