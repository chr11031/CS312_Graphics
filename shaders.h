#include "SDL2/SDL.h"
#include "definitions.h"
#include <iostream>
#include <cmath>

/**************************************************
 * TRIANGE_COLOR_SHADER
 * Color fragment shader for triangles
 *************************************************/
void TriangleColorShader(PIXEL &frag, const Attributes &vertAttr, const Attributes &uniforms)
{
    // Barycentric Coordinates
    /* https://codeplea.com/triangular-interpolation */
    float w1, w2, w3 = 0;
    w1 = ((uniforms.vertPoints[1].y - uniforms.vertPoints[2].y) * (vertAttr.vertPoint.x - uniforms.vertPoints[2].x) + (uniforms.vertPoints[2].x - uniforms.vertPoints[1].x) * (vertAttr.vertPoint.y - uniforms.vertPoints[2].y)) / 
         ((uniforms.vertPoints[1].y - uniforms.vertPoints[2].y) * (uniforms.vertPoints[0].x - uniforms.vertPoints[2].x) + (uniforms.vertPoints[2].x - uniforms.vertPoints[1].x) * (uniforms.vertPoints[0].y - uniforms.vertPoints[2].y));

    w2 = ((uniforms.vertPoints[2].y - uniforms.vertPoints[0].y) * (vertAttr.vertPoint.x - uniforms.vertPoints[2].x) + (uniforms.vertPoints[0].x - uniforms.vertPoints[2].x) * (vertAttr.vertPoint.y - uniforms.vertPoints[2].y)) /
         ((uniforms.vertPoints[1].y - uniforms.vertPoints[2].y) * (uniforms.vertPoints[0].x - uniforms.vertPoints[2].x) + (uniforms.vertPoints[2].x - uniforms.vertPoints[1].x) * (uniforms.vertPoints[0].y - uniforms.vertPoints[2].y));
    w3 = 1 - w1 - w2;

    PIXEL color = 0xff000000;
    color += (unsigned int)(0xff * w1) << 16;
    color += (unsigned int)(0xff * w2) << 8;
    color += (unsigned int)(0xff * w3) << 0;

    frag = color;
}

/************************************************
 * TRIANGLE_IMAGE_SHADER
 * Image fragment shader for triangles
 ***********************************************/
void TriangleImageShader(PIXEL &frag, const Attributes &vertAttr, const Attributes &uniforms)
{
    frag = getImgPixel(uniforms.image->img, 
           vertAttr.vertPoint.x - fmin(fmin(uniforms.vertPoints[0].x, 
           uniforms.vertPoints[1].x), uniforms.vertPoints[2].x),
           abs(vertAttr.vertPoint.y - fmax(fmax(uniforms.vertPoints[0].y, 
           uniforms.vertPoints[1].y), uniforms.vertPoints[2].y)));
}