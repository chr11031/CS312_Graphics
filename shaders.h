#pragma once

#include "definitions.h"

/*******************************************************
 * COLOR_FRAGMENT_SHADER
 * Fragment shader for doubles values.
 ******************************************************/
void ColorFragmentShader(PIXEL & fragment, const Attributes & vertAttr, const Attributes & uniforms)
{
    // Change the attribute color from doubles to an unsigned int
    PIXEL color = 0xff000000;
    color += (unsigned int)(vertAttr[1].d * 0xff) << 16;
    color += (unsigned int)(vertAttr[2].d * 0xff) << 8;
    color += (unsigned int)(vertAttr[3].d * 0xff) << 0;

    fragment = color;
}

/*******************************************************
 * IMAGE_FRAGMENT_SHADER
 * Fragment shader for images.
 ******************************************************/
void ImageFragmentShader(PIXEL & fragment, const Attributes & vertAttr, const Attributes & uniforms)
{
    BufferImage* ptr = (BufferImage*)uniforms[0].ptr;

    // Set the fragment equal to the color of the "same point" at the image using the u, v coordinates
    int x = vertAttr[0].d * (ptr->width() - 1);
    int y = vertAttr[1].d * (ptr->height() - 1);

    fragment = (*ptr)[y][x];
}

/******************************************************* 
 * TRANSFORMATION_VERTEX_SHADER
 * Applies the transformations to the vertex
 ******************************************************/
void TransformationVertexShader(Vertex & vertOut, Attributes & attrOut, const Vertex & vertIn, const Attributes & attrIn, const Attributes & uniforms)
{
    vertOut = (*(TransformationMatrix*)uniforms[0].ptr).multiplyByVertex(vertIn);
    attrOut = attrIn;
}

/******************************************************* 
 * SIPMLE_VERTEX_SHADER
 * A simple vertex shader
 ******************************************************/
void SimpleVertShader(Vertex & vertOut, Attributes & attrOut, const Vertex & vertIn, const Attributes & attrIn, const Attributes & uniforms)
{
    TransformationMatrix *model = (TransformationMatrix*)uniforms[1].ptr;
    TransformationMatrix *view = (TransformationMatrix*)uniforms[2].ptr;
    TransformationMatrix *proj = (TransformationMatrix*)uniforms[3].ptr;

    vertOut =  ((*proj) * (*view) * (*model)).multiplyByVertex(vertIn);
    attrOut = attrIn;
}