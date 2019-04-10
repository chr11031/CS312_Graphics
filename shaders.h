#ifndef SHADERS_H
#define SHADERS_H


#include "definitions.h"

void myBaryShader(PIXEL & fragment, const Attributes & vertAttr, const Attributes & uniforms)
{
    // fill in
    PIXEL color = 0xff000000;
    color += (unsigned int)(vertAttr.attrValues[0].d * 0xff) << 16;
    color += (unsigned int)(vertAttr.attrValues[1].d * 0xff) << 8;
    color += (unsigned int)(vertAttr.attrValues[2].d * 0xff) << 0;

    fragment = color;
}

void imageFragShader(PIXEL & fragment, const Attributes & vertAttr, const Attributes & uniforms)
{
    // previous BufferImage* myPointer = (BufferImage*)uniforms.pointerImg;
    BufferImage* myPointer = (BufferImage*)uniforms[0].ptr;
    int x = vertAttr[0].d * (myPointer->width() - 1);
    int y = vertAttr[1].d * (myPointer->height() - 1);

    if(x > 2000 || x < 0)
    {
        fragment = 0xff00ff00;
        return;
    }

    fragment = (*myPointer)[y][x];
}

void vertShader(Vertex & vertOut, Attributes & attrOut, const Vertex & vertIn, const Attributes & vertAttr, const Attributes & uniforms)
{
    vertOut = uniforms.matrix * vertIn;
    attrOut = vertAttr;
}

void SimpleVertexShader(Vertex & vertOut, Attributes & attrOut, const Vertex & vertIn, const Attributes & attrIn, const Attributes & uniforms)
{
    Matrix* trans = (Matrix*)uniforms[0].ptr;
    vertOut = (*trans) * vertIn;

    attrOut = attrIn;
}

void SimpleVertexShader2(Vertex & vertOut, Attributes & attrOut, const Vertex & vertIn, const Attributes & attrIn, const Attributes & uniforms)
{
    Matrix* model = (Matrix*)uniforms[1].ptr;
    Matrix* view  = (Matrix*)uniforms[2].ptr;
    Matrix* proj  = (Matrix*)uniforms[3].ptr;

    vertOut = (*proj) * (*view) * (*model) * vertIn;
    attrOut = attrIn;
}


#endif