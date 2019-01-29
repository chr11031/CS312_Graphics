#ifndef SHADERS_H
#define SHADERS_H

#include "definitions.h"

/***************************************************
 * Fragment shaders used in week 03 project
 **************************************************/
/*
void ColorFragShader(PIXEL & fragment, const Attributes & vertAttr, const Attributes & uniforms) {
    fragment = 0xff000000
        | (((unsigned int)(vertAttr.r * 0xff)) << 16)
        | (((unsigned int)(vertAttr.g * 0xff)) << 8)
        |  ((unsigned int)(vertAttr.b * 0xff));
}

void ImageFragShader(PIXEL & fragment, const Attributes & vertAttr, const Attributes & uniforms) {
    BufferImage* ptr = (BufferImage*)uniforms.ptrImg;

    int width = ptr->width();
    int height = ptr->height();
    int x = vertAttr.u * (width - 1);
    int y = vertAttr.v * (height - 1);
    fragment = (*ptr)[y][x];
}
*/

#endif