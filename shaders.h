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

  myLight.point = (*view) * (*model) * myLight.point;
  Vertex pos = { attrIn[2].d, attrIn[3].d, attrIn[4].d, 1 };
  Vertex norm ={ attrIn[5].d, attrIn[6].d, attrIn[7].d, 1 };

  pos = (*view) * (*model) * pos;
  norm = (*view) * (*model) * norm;

  vertOut = (*proj) * (*view) * (*model) * vertIn;

  // Pass through attributes
    //   attrOut.insertDbl(attrIn[0].d);
    //   attrOut.insertDbl(attrIn[1].d);
    //   attrOut.insertDbl(pos.x);
    //   attrOut.insertDbl(pos.y);
    //   attrOut.insertDbl(pos.z);
    //   attrOut.insertDbl(norm.x);
    //   attrOut.insertDbl(norm.y);
    //   attrOut.insertDbl(norm.z);

  attrOut = attrIn;
}


// Image Fragment Shader 
void ImageFragShader(PIXEL & fragment, const Attributes & vertAttr, const Attributes & uniforms)
{
    /*
    // FOR CALCULATED CHECKERBOARD
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

    // normalized light vector
    Vertex lightV = lightVec(vertAttr[2].d, vertAttr[3].d, vertAttr[4].d);

    // find the cosine of the angle between world and light vectors by finding the
    // dot product of normalized/interpolated world coordinates and normalized light vector
    double dot = dotProduct(vertAttr[5].d, vertAttr[6].d, vertAttr[7].d,
                            lightV.x, lightV.y, lightV.z);

    // dot product of light diffuse value, or the final diffuse values
    double rD = (dot * myLight.diffuse[0]);
    double gD = (dot * myLight.diffuse[1]);
    double bD = (dot * myLight.diffuse[0]);

    // light intensity simplified
    double intensity = myLight.intensity;

    BufferImage* bf = (BufferImage*)uniforms[0].ptr;
    int x = vertAttr[0].d * (bf->width()-1);
    int y = vertAttr[1].d * (bf->height()-1);

    fragment = (*bf)[y][x];

    // the amazing diffuse calculation for PHONG
    PIXEL r = (unsigned int)(((fragment & 0xff0000) >> 16) * (MIN((myLight.ambient[0] + rD) * intensity, 1.0))) << 16;
    PIXEL g = (unsigned int)(((fragment & 0xff0000) >> 8)  * (MIN((myLight.ambient[1] + gD) * intensity, 1.0))) << 8;
    PIXEL b = (unsigned int)( (fragment & 0xff0000)        * (MIN((myLight.ambient[2] + bD) * intensity, 1.0)));

    fragment = r + g + b;
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