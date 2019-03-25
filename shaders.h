#include "definitions.h"

#ifndef SHADERS_H
#define SHADERS_H

void RealVertShader(Vertex & vertOut, Attributes & attrOut, const Vertex & vertIn, const Attributes & attrIn, const Attributes & uniforms)
{
  Transform* trans = (Transform*)uniforms[0].ptr;
  vertOut = (*trans) * vertIn;
  // Pass through attributes
  attrOut = attrIn;
}

void RealVertShader2(Vertex & vertOut, Attributes & attrOut, const Vertex & vertIn, const Attributes & attrIn, const Attributes & uniforms)
{
  Transform* model = (Transform*)uniforms[1].ptr;
  Transform* view = (Transform*)uniforms[2].ptr;
  Transform* proj = (Transform*)uniforms[3].ptr;

  vertOut = (*proj) * (*view) * (*model) * vertIn;

  // Pass through attributes
  attrOut = attrIn;
}

// Image Fragment Shader 
void ImageFragShader(PIXEL & fragment, const Attributes & vertAttr, const Attributes & uniforms)
{
        /*
    // Figure out which X/Y square our UV would fall on
    int xSquare = vertAttr[0].d * 8;
    int ySquare = vertAttr[1].d * 8;
	// Is the X square position even? The Y? 
    bool evenXSquare = (xSquare % 2) == 0;
    bool evenYSquare = (ySquare % 2) == 0;
    // Both even or both odd - red square
    if( (evenXSquare && evenYSquare) || (!evenXSquare && !evenYSquare) )
    {
        fragment = 0xffff0000;
    }
    // One even, one odd - white square
    else
    {
        fragment = 0xffffffff;
    }
    */

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

