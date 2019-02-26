#ifndef MAT4
#define MAT4

#include <math.h>
#include "matrix.h"

#define PI 3.14159265

using namespace std;

Matrix createIdentityMatrix(){

    double array[] = {1.0, 0.0, 0.0, 0.0,
                      0.0, 1.0, 0.0, 0.0,
                      0.0, 0.0, 1.0, 0.0,
                      0.0, 0.0, 0.0, 1.0};


    Matrix returnMatrix = Matrix(4,4,array);

    return returnMatrix;
    
}

Matrix createTranslationMatrix(double xTranslation, double yTranslation, double zTranslation){
    
    Matrix translationMatrix(4,4);
    translationMatrix = createIdentityMatrix();

    translationMatrix.setElement(0,3,xTranslation);
    translationMatrix.setElement(1,3,yTranslation);
    translationMatrix.setElement(2,3,zTranslation);

    return translationMatrix;
}

Matrix createScaleMatrix(double xScale, double yScale, double zScale){
    
    Matrix scaleMatrix(4,4);
    scaleMatrix = createIdentityMatrix();

    scaleMatrix.setElement(0,0,xScale);
    scaleMatrix.setElement(1,1,yScale);
    scaleMatrix.setElement(2,2,zScale);

    return scaleMatrix;
}

Matrix createXRotationMatrix(double xRotation){
    Matrix rotationMatrix(4,4);
    rotationMatrix = createIdentityMatrix();

    rotationMatrix.setElement(1,1,cos (xRotation*PI/180));  
    rotationMatrix.setElement(1,2,-sin (xRotation*PI/180));
    rotationMatrix.setElement(2,1,sin (xRotation*PI/180));
    rotationMatrix.setElement(2,2,cos (xRotation*PI/180)); 

    return rotationMatrix; 
}

Matrix createYRotationMatrix(double yRotation){
    Matrix rotationMatrix(4,4);
    rotationMatrix = createIdentityMatrix();

    rotationMatrix.setElement(0,0,cos (yRotation*PI/180));  
    rotationMatrix.setElement(0,2,sin (yRotation*PI/180));
    rotationMatrix.setElement(2,0,-sin (yRotation*PI/180));
    rotationMatrix.setElement(2,2,cos (yRotation*PI/180));

    return rotationMatrix;

}

Matrix createZRotationMatrix(double zRotation){
    Matrix rotationMatrix(4,4);
    rotationMatrix = createIdentityMatrix();

    rotationMatrix.setElement(0,0,cos (zRotation*PI/180));  
    rotationMatrix.setElement(0,1,-sin (zRotation*PI/180));
    rotationMatrix.setElement(1,0,sin (zRotation*PI/180));
    rotationMatrix.setElement(1,1,cos (zRotation*PI/180));

    return rotationMatrix;

}

Matrix createRotationMatrix(double xRotation, double yRotation, double zRotation){

    return
         (createXRotationMatrix(xRotation) * createYRotationMatrix(yRotation))
         * createZRotationMatrix(zRotation);
}

Matrix camera(const double &offx, const double & offy, const double & offz,
                const double & yaw, const double & pitch, const double & roll)
{
    Matrix tr = Matrix(4,4);

    Matrix trans = createTranslationMatrix(-offx, -offy, -offz);

    Matrix rotX = createXRotationMatrix(pitch);
    Matrix rotY = createXRotationMatrix(yaw);

    tr = rotX * rotY * trans;

    // rotX x, pitch
    // rotY y, yaw

    // rt = rotX * rotY * trans
    
}

Matrix perspective(const double & fovy, const double & aspectRatio, const double & near, const double far)
{
    Matrix tr = Matrix(4,4);

    double top = near * tan((fovy * PI) / 180.0) / 2.0;

    double right = aspectRatio * top;

    
    tr.setElement(0,0,near / right);

    tr.setElement(1,1,near / top);

    tr.setElement(2,2,(far + near) / (far - near));
    tr.setElement(2,3,(-2 * far * near) / (far - near));

    tr.setElement(3,2,1);

    return tr;

}

#endif
