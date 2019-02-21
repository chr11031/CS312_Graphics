#ifndef MAT4
#define MAT4

#include <math.h>
// #include "definitions.h"

#define PI 3.14159265

using namespace std;

Matrix createIdentityMatrix(){

    double array[] = {1.0, 0.0, 0.0, 0.0,
                      0.0, 1.0, 0.0, 0.0,
                      0.0, 0.0, 1.0, 0.0,
                      0.0, 0.0, 0.0, 1.0};


    Matrix returnMatrix = Matrix(4,4,array);

    double a = returnMatrix.getElement(0,0);
    double aa = returnMatrix.getElement(0,1);
    double aaa = returnMatrix.getElement(0,2);
    double aaaa = returnMatrix.getElement(0,3);
    double aaaaa = returnMatrix.getElement(1,0);
    double aaaaaa = returnMatrix.getElement(1,1);
    double aaaaaaa = returnMatrix.getElement(1,2);
    double aaaaaaaa = returnMatrix.getElement(1,3);
    double aaaaaaaaa = returnMatrix.getElement(2,0);
    double aaaaaaaaaa = returnMatrix.getElement(2,1);
    double aaaaaaaaaaa = returnMatrix.getElement(2,2);
    double aaaaaaaaaaaa = returnMatrix.getElement(2,3);
    double aaaaaaaaaaaaa = returnMatrix.getElement(3,0);
    double aaaaaaaaaaaaaa = returnMatrix.getElement(3,1);
    double aaaaaaaaaaaaaaa = returnMatrix.getElement(3,2);
    double aaaaaaaaaaaaaaaa = returnMatrix.getElement(3,3);

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

    double a = scaleMatrix.getElement(0,0);
    double aa = scaleMatrix.getElement(0,1);
    double aaa = scaleMatrix.getElement(0,2);
    double aaaa = scaleMatrix.getElement(0,3);
    double aaaaa = scaleMatrix.getElement(1,0);
    double aaaaaa = scaleMatrix.getElement(1,1);
    double aaaaaaa = scaleMatrix.getElement(1,2);
    double aaaaaaaa = scaleMatrix.getElement(1,3);
    double aaaaaaaaa = scaleMatrix.getElement(2,0);
    double aaaaaaaaaa = scaleMatrix.getElement(2,1);
    double aaaaaaaaaaa = scaleMatrix.getElement(2,2);
    double aaaaaaaaaaaa = scaleMatrix.getElement(2,3);
    double aaaaaaaaaaaaa = scaleMatrix.getElement(3,0);
    double aaaaaaaaaaaaaa = scaleMatrix.getElement(3,1);
    double aaaaaaaaaaaaaaa = scaleMatrix.getElement(3,2);
    double aaaaaaaaaaaaaaaa = scaleMatrix.getElement(3,3);

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

Matrix createYRotationMatrix(double xRotation){
    Matrix rotationMatrix(4,4);
    rotationMatrix = createIdentityMatrix();

    rotationMatrix.setElement(0,0,cos (xRotation*PI/180));  
    rotationMatrix.setElement(0,2,sin (xRotation*PI/180));
    rotationMatrix.setElement(2,0,-sin (xRotation*PI/180));
    rotationMatrix.setElement(2,2,cos (xRotation*PI/180));

    return rotationMatrix;

}

Matrix createZRotationMatrix(double xRotation){
    Matrix rotationMatrix(4,4);
    rotationMatrix = createIdentityMatrix();

    rotationMatrix.setElement(0,0,cos (xRotation*PI/180));  
    rotationMatrix.setElement(0,1,-sin (xRotation*PI/180));
    rotationMatrix.setElement(1,0,sin (xRotation*PI/180));
    rotationMatrix.setElement(1,1,cos (xRotation*PI/180));

    return rotationMatrix;

}

Matrix createRotationMatrix(double xRotation, double yRotation, double zRotation){

    return
        (createXRotationMatrix(xRotation) * createYRotationMatrix(yRotation))
        * createZRotationMatrix(zRotation);
}

#endif
