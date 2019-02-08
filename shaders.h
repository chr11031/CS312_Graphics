#include "C:/MinGW/include/SDL2/SDL.h"
#include "./definitions.h"
#include <iostream>

void TriangleColorShader(PIXEL & fragment, const Attributes & vertAttr, const Attributes & uniforms)
{
    float w1, w2, w3 = 0;
    w1 = ((uniforms.vertexPoints[1].y - uniforms.vertexPoints[2].y) * (vertAttr.vertexPoint.x - uniforms.vertexPoints[2].x)     + (uniforms.vertexPoints[2].x - uniforms.vertexPoints[1].x) * (vertAttr.vertexPoint.y - uniforms.vertexPoints[2].y)) /
         ((uniforms.vertexPoints[1].y - uniforms.vertexPoints[2].y) * (uniforms.vertexPoints[0].x - uniforms.vertexPoints[2].x) + (uniforms.vertexPoints[2].x - uniforms.vertexPoints[1].x) * (uniforms.vertexPoints[0].y - uniforms.vertexPoints[2].y));
    w2 = ((uniforms.vertexPoints[2].y - uniforms.vertexPoints[0].y) * (vertAttr.vertexPoint.x - uniforms.vertexPoints[2].x)     + (uniforms.vertexPoints[0].x - uniforms.vertexPoints[2].x) * (vertAttr.vertexPoint.y - uniforms.vertexPoints[2].y)) /
         ((uniforms.vertexPoints[1].y - uniforms.vertexPoints[2].y) * (uniforms.vertexPoints[0].x - uniforms.vertexPoints[2].x) + (uniforms.vertexPoints[2].x - uniforms.vertexPoints[1].x) * (uniforms.vertexPoints[0].y - uniforms.vertexPoints[2].y));
    w3 = 1 - (w1 + w2);
    //Every 4 << equals one F shift
    // Output our shader color value, in this case red
    fragment = 0xff000000 | ((int)(0xff * w1) << 16);
    fragment = fragment | ((int)(0xff * w2) << 8);
    fragment = fragment | ((int)(0xff * w3));
}

void TriangleImageShader(PIXEL & fragment, const Attributes & vertAttr, const Attributes & uniforms)
{
    // Output our shader color value, in this case red
   BufferImage* bf = (BufferImage*)uniforms.image;
    int x = vertAttr.vertexPoint.x - (MIN3(uniforms.vertexPoints[0].x, uniforms.vertexPoints[1].x, uniforms.vertexPoints[2].x));
    int y = vertAttr.vertexPoint.y - (MIN3(uniforms.vertexPoints[0].y, uniforms.vertexPoints[1].y, uniforms.vertexPoints[2].y));
        fragment = (*bf)[y][x];

}

void TriangleInterpolatedImageShader(PIXEL & fragment, const Attributes & vertAttr, const Attributes & uniforms)
{
    BufferImage* bf = (BufferImage*)uniforms.image;
    //std::cout << ((vertAttr.vertexPoint.y - 6.0)/(uniforms.quadHeight/(bf->height()-1)) << std::endl;//1.122 = 287/256  1.563 = 400/256
     fragment = (*bf)[(int)( ((vertAttr.vertexPoint.y - uniforms.minY)/(uniforms.quadHeight/(bf->height()-1))) /((1-((vertAttr.vertexPoint.y-uniforms.minY)/(uniforms.maxY - uniforms.minY)))*(uniforms.quadLengthBottom/uniforms.quadLengthTop-1)+1))]
    [(int)((vertAttr.vertexPoint.x - ((vertAttr.vertexPoint.y - uniforms.minY) / (uniforms.quadHeight/(abs(uniforms.quadLengthTop - uniforms.quadLengthBottom)/2)) + uniforms.minX) ) * bf->height()/((vertAttr.vertexPoint.y-uniforms.minY)*((uniforms.quadLengthTop - uniforms.quadLengthBottom)/uniforms.quadHeight)+uniforms.quadLengthBottom))];
}