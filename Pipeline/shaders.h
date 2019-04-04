#include "definitions.h"

#ifndef SHADERS_H
#define SHADERS_H

// Simple Vertex Shader
void SimpleVertexShader(Vertex & vertOut, Attributes & attrOut, const Vertex & vertIn, const Attributes & attrIn, const Attributes & uniforms)
{
  Transform* trans = (Transform*)uniforms[0].ptr;
  vertOut = (*trans) * vertIn;

  // Pass through attributes
  attrOut = attrIn;
}

void SimpleVertexShader2(Vertex & vertOut, Attributes & attrOut, const Vertex & vertIn, const Attributes & attrIn, const Attributes & uniforms)
{
  Transform* model = (Transform*)uniforms[1].ptr;
  Transform* view = (Transform*)uniforms[2].ptr;
  Transform* proj = (Transform*)uniforms[3].ptr;

  vertOut = (*proj) * (*view) * (*model) * vertIn;

  // Pass through attributes
  attrOut = attrIn;
}

// Image Fragment Shader 
void ImagePersonalProjectShader(PIXEL & fragment, const Attributes & vertAttr, const Attributes & uniforms)
{
    BufferImage* bf = (BufferImage*)uniforms[0].ptr;
    int x = vertAttr[0].d * (bf->width()-1);
    int y = vertAttr[1].d * (bf->height()-1);

    fragment = (*bf)[y][x];
}

// Image Fragment Shader 
void ImagePersonalProjectShader1(PIXEL & fragment, const Attributes & vertAttr, const Attributes & uniforms)
{
    BufferImage* bf = (BufferImage*)uniforms[0].ptr;
    int x = vertAttr[0].d * (bf->width()-1);
    int y = vertAttr[1].d * (bf->height()-1);

    fragment = (*bf)[y][x] * 0xbb << 0;
}

// Image Fragment Shader 
void ImagePersonalProjectShader2(PIXEL & fragment, const Attributes & vertAttr, const Attributes & uniforms)
{
    BufferImage* bf = (BufferImage*)uniforms[0].ptr;
    int x = vertAttr[0].d * (bf->width()-1);
    int y = vertAttr[1].d * (bf->height()-1);

    fragment = (*bf)[y][x] * 0xff << 8;

}

// Image Fragment Shader 
void ImageFragShader(PIXEL & fragment, const Attributes & vertAttr, const Attributes & uniforms)
{

    BufferImage* bf = (BufferImage*)uniforms[0].ptr;
    int x = vertAttr[0].d * (bf->width()-1);
    int y = vertAttr[1].d * (bf->height()-1);

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

#endif 