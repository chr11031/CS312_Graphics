#include "definitions.h"

/**************************************************
 * TRIANGE_COLOR_SHADER
 * Color fragment shader for triangles
 *************************************************/
void TriangleColorShader(PIXEL &frag, const Attributes &vertAttr, const Attributes &uniforms)
{
    PIXEL color = 0xff000000;
    color += (unsigned int)(0xff * vertAttr.var.at('r')) << 16;
    color += (unsigned int)(0xff * vertAttr.var.at('g')) << 8;
    color += (unsigned int)(0xff * vertAttr.var.at('b')) << 0;

    frag = color;
}

/************************************************
 * TRIANGLE_IMAGE_SHADER
 * Image fragment shader for triangles
 ***********************************************/
void TriangleImageShader(PIXEL &frag, const Attributes &vertAttr, const Attributes &uniforms)
{
     BufferImage *ptr = (BufferImage*)uniforms.image;
     int x = vertAttr.var.at('u') * (ptr->width() - 1);
     int y = vertAttr.var.at('v') * (ptr->height() - 1);

     frag = (*ptr)[y][x];
}

/**********************************************
 * GREEN_IMAGE_SHADER
 * Colors the image green
 **********************************************/
void GreenImageShader(PIXEL &frag, const Attributes &vertAttr, const Attributes &uniforms)
{
    PIXEL color = 0xff00ff00; 
    BufferImage *ptr = (BufferImage*)uniforms.image;
    int x = vertAttr.var.at('u') * (ptr->width() - 1);
    int y = vertAttr.var.at('v') * (ptr->height() - 1);

    frag = (*ptr)[y][x] & color;
}

/**********************************************
 * TRANSFORM_SHADER
 * Fragment shader to transform triangles
 *********************************************/
void transformShader(Vertex & vOut, Attributes & attrOut, const Vertex & vIn, const Attributes & vertAttr, const Attributes & uniforms)
{
    vOut = uniforms.transVertex * vIn;
    attrOut = vertAttr;
}