#include "definitions.h"

#ifndef MY_FUNCTIONS_H
#define MY_FUNCTIONS_H

// Image Fragment Shader 
void ImageFragShader(PIXEL & fragment, const Attributes & vertAttr, const Attributes & uniforms)
{
    BufferImage* ptr = (BufferImage*)uniforms[0].ptr;
    int x = vertAttr[0].d * (ptr -> width()-1);
    int y = vertAttr[1].d * (ptr -> height()-1);
    fragment = (*ptr)[y][x];
}

// My Fragment Shader for color interpolation
void ColorFragShader(PIXEL & fragment, const Attributes & vertAttr, const Attributes & uniforms)
{
    // creating a full color from rgb parts
    PIXEL color = 0xff000000;
    color += (unsigned int)(vertAttr[0].d *0xff) << 16;
    color += (unsigned int)(vertAttr[1].d *0xff) << 8;
    color += (unsigned int)(vertAttr[2].d *0xff) << 0;
    fragment = color;
}

//My Vertex Shader
void BasicVertexShader(Vertex & vertOut, Attributes & attrOut, const Vertex & vertIn, const Attributes & vertAttr, const Attributes & uniforms)
{
    Matrix mat = uniforms.matrix;
    vertOut = mat.multi(vertIn);
    attrOut = vertAttr;
}

/****************************************
 * DETERMINANT
 * Find the determinant of a matrix with
 * components A, B, C, D from 2 vectors.
 ***************************************/
inline double determinant(const double & A, const double & B, const double & C, const double & D)
{ return (A*D - B*C); }
/*********************************************************************
 * CROSS_PRODUCT
 * helper functions to calculate the cross product of two vertecies
 ********************************************************************/
inline double crossProduct(double* v1, double* v2) { return (v1[0] * v2[1]) - (v1[1] * v2[0]); }

#endif