#include "definitions.h"

#ifndef SHADERS_H
#define SHADERS_H

void imageFragShader(PIXEL & fragment, const Attributes & vertAttr, const Attributes & uniforms)
{
    BufferImage* imagePtr = (BufferImage*)uniforms.ptrImage;

    int x = vertAttr.attrValues[0] * (imagePtr->width()  - 1);
    int y = vertAttr.attrValues[1] * (imagePtr->height() - 1);

    fragment = (*imagePtr)[y][x];
}

void baryInterpolationShader(PIXEL & fragment, const Attributes & vertAttr, const Attributes & uniforms)
{
    PIXEL color = 0xff000000;
    color += (unsigned int)(vertAttr.attrValues[0] * 0xff) << 16;
    color += (unsigned int)(vertAttr.attrValues[1] * 0xff) << 8;
    color += (unsigned int)(vertAttr.attrValues[2] * 0xff);

    fragment = color;
}

// Example of a fragment shader
void DefaultFragShader(PIXEL & fragment, const Attributes & vertAttr, const Attributes & uniforms)
{
    // Output our shader color value, in this case red
    fragment = vertAttr.color;
}

void RedScaleShader(PIXEL & fragment, const Attributes & vertAttr, const Attributes & uniforms)
{
    fragment = 0xff000000 + (0x00ff0000 & vertAttr.color);
}

void BlueScaleShader(PIXEL & fragment, const Attributes & vertAttr, const Attributes & uniforms)
{
    fragment = 0xff000000 + (0x000000ff & vertAttr.color);
}

void GreenScaleShader(PIXEL & fragment, const Attributes & vertAttr, const Attributes & uniforms)
{
    fragment = 0xff000000 + (0x0000ff00 & vertAttr.color);
}

void GrayScaleShader(PIXEL & fragment, const Attributes & vertAttr, const Attributes & uniforms)
{
    PIXEL avgScale = ((vertAttr.color >> 16) & 0xff) +
                     ((vertAttr.color >>  8) & 0xff) +
                     ((vertAttr.color      ) & 0xff);
    avgScale /= 3;

    fragment = 0xff000000 + (avgScale << 16) + (avgScale << 8) + avgScale;
}

void TransformVertexShader(Vertex & vertOut, Attributes & attrOut, const Vertex & vertIn, const Attributes & vertAttr, const Attributes & uniforms)
{
    vertOut = vertIn * uniforms.matrix;
    attrOut = vertAttr;
}

#endif