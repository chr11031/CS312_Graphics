#include "definitions.h"

#ifndef SHADERS_H
#define SHADERS_H

void ColorFragShader(PIXEL & fragment, const Attributes & attr, const Attributes & uniforms)
{
    PIXEL color = 0xff000000;
    color += (unsigned int)(attr.attrValues[0] *0xff) << 16;
    color += (unsigned int)(attr.attrValues[1] *0xff) << 8;
    color += (unsigned int)(attr.attrValues[2] *0xff) << 0;

    fragment = color;
}

void ImageFragShader(PIXEL & fragment, const Attributes & attr, const Attributes & uniforms)
{
BufferImage* myPointer = (BufferImage*)uniforms.ptrImg;

    int x = attr.attrValues[0] * (myPointer->width() - 1);
    int y = attr.attrValues[1] * (myPointer->height() - 1);

    fragment = (*myPointer)[y][x];

    // PIXEL color;
    // BufferImage* ptr = (BufferImage*)uniforms.ptrImg;

    // int wid = ptr->width();
    // int hgt = ptr->height();

    // int x = attr.attrValues[0] * wid;
    // int y = attr.attrValues[1] * hgt;

    // color = (*ptr)[y][x];
    // fragment = color;
}

#endif