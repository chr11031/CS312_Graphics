#pragma once

#include "definitions.h"

/*******************************************************
 * COLOR_FRAGMENT_SHADER
 * Fragment shader for argb values.
 ******************************************************/
void ColorFragmentShader(PIXEL & fragment, const Attributes & vertAttr, const Attributes & uniforms)
{
    // Change the attribute color from doubles to an unsigned int
    PIXEL color = 0xff000000;
    color += (unsigned int)(vertAttr.argb[1] * 0xff) << 16;
    color += (unsigned int)(vertAttr.argb[2] * 0xff) << 8;
    color += (unsigned int)(vertAttr.argb[3] * 0xff) << 0;

    fragment = color;
}

/*******************************************************
 * IMAGE_FRAGMENT_SHADER
 * Fragment shader for images.
 ******************************************************/
void ImageFragmentShader(PIXEL & fragment, const Attributes & vertAttr, const Attributes & uniforms)
{
    BufferImage* ptr = (BufferImage*)uniforms.ptr;

    // Set the fragment equal to the color of the "same point" at the image using the u, v coordinates
    int x = vertAttr.argb[0] * (ptr->width() - 1);
    int y = vertAttr.argb[1] * (ptr->height() - 1);

    fragment = (*ptr)[y][x];
}

/******************************************************* 
 * TRANSFORMATION_VERTEX_SHADER
 * Applies the transformations to the vertex
 ******************************************************/
void TransformationVertexShader(Vertex & vertOut, Attributes & attrOut, const Vertex & vertIn, const Attributes & attrIn, const Attributes & uniforms)
{
    vertOut = (*uniforms.matrix).multiplyByVertex(vertIn);
    attrOut = attrIn;
}