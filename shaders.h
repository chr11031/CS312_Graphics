#include "definitions.h"

#ifndef SHADERS_H
#define SHADERS_H

void SimpleFragShader(PIXEL & fragment, const Attributes & attr, const Attributes & uniforms)
{
    // Color the pixel to be the attribute's color
    fragment = attr.color;
}

#endif