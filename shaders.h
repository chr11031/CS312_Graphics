#include "definitions.h"
#include "matrix.h"

#ifndef SHADERS_H
#define SHADERS_H

// Image Fragment Shader 
void ImageFragShader(PIXEL & fragment, const Attributes & vertAttr, const Attributes & uniforms)
{
    BufferImage* bf = (BufferImage*)uniforms[0].ptr;
    double u = vertAttr[0].d;
    double v = vertAttr[1].d;
    int x = u * (bf->width() - 1);
    int y = v * (bf->height() - 1);

    if(x < 0 || x >= bf->width() || y < 0 || y >= bf->height())
        return;

    fragment = (*bf)[y][x];
}

// My Fragment Shader for color interpolation
void ColorFragShader(PIXEL & fragment, const Attributes & vertAttr, const Attributes & uniforms)
{
    // Output our shader color value, in this case red
    PIXEL color = 0xff000000;
    color += (unsigned int)(vertAttr[0].d *0xff) << 16;
    color += (unsigned int)(vertAttr[1].d *0xff) << 8;
    color += (unsigned int)(vertAttr[2].d *0xff) << 0;

    fragment = color;
}

void ColorVertexShader(Vertex & vertOut, Attributes & attrOut, const Vertex & vertIn, const Attributes & vertAttr, const Attributes & uniforms)
{
    Matrix* model = (Matrix*) uniforms[0].ptr;
    vertOut = (*model) * vertIn;
    attrOut = vertAttr;
}

void SimpleVertexShader(Vertex & vertOut, Attributes & attrOut, const Vertex & vertIn, const Attributes & vertAttr, const Attributes & uniforms)
{
    Matrix* model = (Matrix*) uniforms[1].ptr;
    Matrix* view = (Matrix*) uniforms[2].ptr;
    Matrix* projection = (Matrix*) uniforms[3].ptr;

    vertOut = (*projection) * (*view) * (*model) * vertIn;
    attrOut = vertAttr;
}

#endif 