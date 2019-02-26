#ifndef SHADERS_H
#define SHADERS_H


#include "definitions.h"

void myBaryShader(PIXEL & fragment, const Attributes & vertAttr, const Attributes & uniforms)
{
    // fill in
    PIXEL color = 0xff000000;
    color += (unsigned int)(vertAttr.attrValues[0] * 0xff) << 16;
    color += (unsigned int)(vertAttr.attrValues[1] * 0xff) << 8;
    color += (unsigned int)(vertAttr.attrValues[2] * 0xff) << 0;

    fragment = color;
}

void imageFragShader(PIXEL & fragment, const Attributes & vertAttr, const Attributes & uniforms)
{
    BufferImage* myPointer = (BufferImage*)uniforms.pointerImg;

    int x = vertAttr.attrValues[0] * (myPointer->width() - 1);
    int y = vertAttr.attrValues[1] * (myPointer->height() - 1);

    fragment = (*myPointer)[y][x];
}

void vertShader(Vertex & vertOut, Attributes & attrOut, const Vertex & vertIn, const Attributes & vertAttr, const Attributes & uniforms)
{
    vertOut = uniforms.matrix * vertIn;
    attrOut = vertAttr;
}

void SimpleVertexShader2(Vertex & vertOut, Attributes & attrOut, const Vertex & vertIn, const Attributes & vertAttr, const Attributes & uniforms)
{
    Matrix* model = (Matrix*)uniforms.pointerImg;
//    Matrix* view = (Matrix*)uniforms[2].ptr;

    // vertOut = (*view) * (*model) * vertIn;

    // pass through attributes
    attrOut = vertAttr;
}


#endif