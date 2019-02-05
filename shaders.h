#include "C:/MinGW/include/SDL2/SDL.h"
#include "definitions.h"
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
    //std::cout << "X: " << vertAttr.vertexPoint.x - MinNum(MinNum(uniforms.vertexPoints[0].x, uniforms.vertexPoints[1].x), uniforms.vertexPoints[2].x); std::cout << std::endl; std::cout << "Y: " << abs(vertAttr.vertexPoint.y - MaxNum(MaxNum(uniforms.vertexPoints[0].y, uniforms.vertexPoints[1].y), uniforms.vertexPoints[2].y)); std::cout << std::endl; std::cout << std::hex << getpixel(uniforms.image->img, vertAttr.vertexPoint.x - MinNum(MinNum(uniforms.vertexPoints[0].x, uniforms.vertexPoints[1].x), uniforms.vertexPoints[2].x), abs(vertAttr.vertexPoint.y - MaxNum(MaxNum(uniforms.vertexPoints[0].y, uniforms.vertexPoints[1].y), uniforms.vertexPoints[2].y))); std::cout << std::endl << std::endl;

    fragment = getpixel(uniforms.image->img, 
    vertAttr.vertexPoint.x - MinNum(MinNum(uniforms.vertexPoints[0].x, uniforms.vertexPoints[1].x), uniforms.vertexPoints[2].x), 
    abs(vertAttr.vertexPoint.y - MaxNum(MaxNum(uniforms.vertexPoints[0].y, uniforms.vertexPoints[1].y), uniforms.vertexPoints[2].y)));
}