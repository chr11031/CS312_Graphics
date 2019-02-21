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
}

void TransformShader (Vertex & vertOut, Attributes & attrOut, const Vertex & vertIn, const Attributes & vertAttr, const Attributes & uniforms)
{
    vertOut = vertIn * uniforms.matrix;
    attrOut = vertAttr;
}
#endif