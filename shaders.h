#include "definitions.h"

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
void TranslationVertShader(Vertex & vertOut, Attributes & attrOut, const Vertex & vertIn, const Attributes & vertAttr, const Attributes & uniforms)
{

}

#endif