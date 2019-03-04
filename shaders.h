#include "definitions.h"
#include "matrix.h"

#ifndef SHADERS_H
#define SHADERS_H

/***************************************************
 * Fragment shaders used in week 03 project
 **************************************************/
void ColorFragShader(PIXEL & fragment, const Attributes & vertAttr, const Attributes & uniforms) {
    fragment = 0xff000000
        | (((unsigned int)(vertAttr[0].d * 0xff)) << 16)
        | (((unsigned int)(vertAttr[1].d * 0xff)) << 8)
        |  ((unsigned int)(vertAttr[2].d * 0xff));
}

void ImageFragShader(PIXEL & fragment, const Attributes & vertAttr, const Attributes & uniforms) {
    BufferImage* ptr = (BufferImage*)uniforms[0].ptr;

    int width = ptr->width();
    int height = ptr->height();
    int x = vertAttr[0].d * (width - 1);
    int y = vertAttr[1].d * (height - 1);
    fragment = (*ptr)[y][x];
}

// This makes the grid look like static
void StaticShader(PIXEL & fragment, const Attributes & vertAttr, const Attributes & uniforms) {
    fragment = 0xff000000
        | (((unsigned int)(rand() * 0xff)) << 16)
        | (((unsigned int)(rand() * 0xff)) << 8)
        |  ((unsigned int)(rand() * 0xff));
}

/***************************************************
 * Vertex shaders used in week 05 project
 **************************************************/
// Vertex shader that supports transformations of a set of verticies

// Storing the matrix in a ptr of the uniforms object is smarter
// than placing another variable in Attributes class to store matrix.
// https://github.com/danebear/CS312_Graphics-1/blob/dchristensen_project_05_vertex_shaders/shaders.h
void TransformVertShader(Vertex & vertOut, Attributes & attrOut, const Vertex & vertIn, const Attributes & vertAttr, const Attributes & uniforms)
{
    vertOut = *(Matrix*)uniforms[0].ptr * vertIn;
    attrOut = vertAttr;
}

// Another vertex shader
void VertexShaderV2(Vertex & vertOut, Attributes & attrOut, const Vertex & vertIn, const Attributes & vertAttr, const Attributes & uniforms)
{
    Matrix* model = (Matrix*)uniforms[1].ptr;
    Matrix* view = (Matrix*)uniforms[2].ptr;
    Matrix* proj = (Matrix*)uniforms[3].ptr;

    vertOut = (*proj) * (*view) * (*model) * vertIn;

    attrOut = vertAttr;
}

#endif