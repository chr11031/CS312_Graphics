#include "definitions.h"
#include "matrix.h"

#ifndef SHADERS_H
#define SHADERS_H


//
//  Fragment Shaders
//
//

void grayFragShader(PIXEL & fragment, const Attributes & vertAttr, const Attributes & uniforms)
{
    fragment = 0xff808080;
}

void greenFragShader(PIXEL & fragment, const Attributes & vertAttr, const Attributes & uniforms)
{
    fragment = 0xff00ff00;
}

/********************************************************************
 * COLOR FRAG SHADER
 * adds the red, green, and blue values to the color of the pixel.
 *******************************************************************/
void colorFragShader(PIXEL & fragment, const Attributes & vertAttr, const Attributes & uniforms)
{
    PIXEL color = 0xff000000;
    color += (unsigned int)(vertAttr[0].d *0xff) << 16;
    color += (unsigned int)(vertAttr[1].d *0xff) << 8;
    color += (unsigned int)(vertAttr[2].d *0xff) << 0;
    
    fragment = color;
}

/*****************************************************************
 * IMAGE FRAG SHADER 
 * prints the image to the screen according to the UV coords.
 *****************************************************************/
void imageFragShader(PIXEL & fragment, const Attributes & vertAttr, const Attributes & uniforms)
{
    BufferImage* bf = (BufferImage*)uniforms[0].ptr;
    int x = vertAttr[0].d * (bf->width()-1);
    int y = vertAttr[1].d * (bf->height()-1);

    fragment = (*bf)[y][x];
}

//
//  Vertex Shaders
//
//

// multiplies the vertex by a matrix to get the new vertices.
void vertexShader(Vertex & vertOut, Attributes & attrOut, const Vertex & vertIn, const Attributes & vertAttr, const Attributes & uniforms)
{
    vertOut = *((Matrix*)uniforms[0].ptr) * vertIn;
    attrOut = vertAttr;
}

void vertexShader2(Vertex & vertOut, Attributes & attrOut, const Vertex & vertIn, const Attributes & vertAttr, const Attributes & uniforms)
{
    Matrix* model = (Matrix*)uniforms[1].ptr;
    Matrix* view = (Matrix*)uniforms[2].ptr;
    Matrix* proj = (Matrix*)uniforms[3].ptr;

    Vertex transVert = (*model) * vertIn;
    Vertex viewVert = (*view) * transVert;
    vertOut = (*proj) * viewVert;

    // Pass through attributes
    attrOut = vertAttr;
}

#endif