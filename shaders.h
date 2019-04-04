#include "definitions.h"
#include "matrix.h"
#include <iostream>

#ifndef SHADERS_H
#define SHADERS_H

// Image Fragment Shader 
void ImageFragShader(PIXEL & fragment, const Attributes & vertAttr, const Attributes & uniforms)
{
    BufferImage* bf = (BufferImage*)uniforms[0].ptr;
    double u = vertAttr[0].d;
    double v = vertAttr[1].d;
    int x = u * bf->width();
    int y = v * bf->height();

    if(x >= bf->width())
        x = 255;
    else if(x < 0)
        x = 0;

    if(y >= bf->height())
        y= 255;
    else if(y < 0)
        y = 0;

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

// My Fragment Shader for color interpolation
void UniformColorFragShader(PIXEL & fragment, const Attributes & vertAttr, const Attributes & uniforms)
{
    // Output our shader color value, in this case red
    PIXEL color = 0xff000000;
    color += (unsigned int)(uniforms[4].d *0xff) << 16;
    color += (unsigned int)(uniforms[5].d *0xff) << 8;
    color += (unsigned int)(uniforms[6].d *0xff) << 0;

    fragment = color;
}

void ParticleFragShader(PIXEL & fragment, const Attributes & vertAttr, const Attributes & uniforms)
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

void ParticleVertexShader(Vertex & vertOut, Attributes & attrOut, const Vertex & vertIn, const Attributes & vertAttr, const Attributes & uniforms)
{
    int currentFrame = (int) uniforms[0].d;
    Matrix* view = (Matrix*) uniforms[1].ptr;
    Matrix* projection = (Matrix*) uniforms[2].ptr;

    double dx = vertAttr[3].d;
    double dy = vertAttr[4].d;
    double dz = vertAttr[5].d;
    int deltaFrame = currentFrame - ((int) vertAttr[6].d);

    vertOut.x = dx * deltaFrame + vertIn.x;
    vertOut.y = ((dy * deltaFrame) - (0.0005 * deltaFrame * deltaFrame)) + vertIn.y;
    vertOut.z = dz * deltaFrame + vertIn.z;
    vertOut.w = vertIn.w;

    vertOut = (*projection) * (*view) * vertOut;
    attrOut = vertAttr;
}

#endif 