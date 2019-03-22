#include "graphicMatrix.h"
#include "definitions.h"

#ifndef SHADERS_H
#define SHADERS_H

//DefaultFragShader
void DefaultFragShader(PIXEL & fragment, const Attributes & vertAttr, const Attributes & uniforms)
{
    // Output our shader color value, in this case red
    fragment = 0xffff0000;
}

/*******************************************************
 * FRAGMENT_SHADER
 * Encapsulates a programmer-specified callback
 * function for shading pixels. See 'DefaultFragShader'
 * for an example. 
 ******************************************************/
class FragmentShader
{
    public:
 
        // Get, Set implicit
        void (*FragShader)(PIXEL & fragment, const Attributes & vertAttr, const Attributes & uniforms);

        // Assumes simple monotone RED shader
        FragmentShader()
        {
            FragShader = DefaultFragShader;
        }

        // Initialize with a fragment callback
        FragmentShader(void (*FragSdr)(PIXEL & fragment, const Attributes & vertAttr, const Attributes & uniforms))
        {
            setShader(FragSdr);
        }

        // Set the shader to a callback function
        void setShader(void (*FragSdr)(PIXEL & fragment, const Attributes & vertAttr, const Attributes & uniforms))
        {
            FragShader = FragSdr;
        }
};


/*******************************
 * Image Frag Shader
 * Takes image data and returns fragment
 ******************************/
void ImageFragShader(PIXEL & fragment, const Attributes & vertAttr, const Attributes & uniforms){
    BufferImage* ptr;
    if(uniforms.ptrImg != NULL){
        ptr = (BufferImage*)uniforms.ptrImg;
    }
    else
    {
        ptr = (BufferImage*)uniforms.att[0].ptr;
    }
    
    int wid = ptr->width();
    int hgth = ptr->height();

    int x = vertAttr.att[0].d * (wid - 1);// U
    int y = vertAttr.att[1].d * (hgth - 1);// V

    //Error check to avoid out of bounds draw
    if (x < 0 || x >= ptr->width() || y < 0 || y >= ptr->height()){ return; }

    fragment = (*ptr)[y][x];

}

/*******************************
 * Image Frag Shader 2
 * Takes image data and returns fragment
 ******************************
void ImageFragShader2(PIXEL & fragment, const Attributes & vertAttr, const Attributes & uniforms){
    BufferImage* ptr = (BufferImage*)uniforms.ptrImg;

    int wid = ptr->width();
    int hgth = ptr->height();

    int x = vertAttr.att[0].d * (wid - 1);// U
    int y = vertAttr.att[1].d * (hgth - 1);// V

    fragment = (*ptr)[y][x];
}*/

/*******************************
 * color Frag Shader
 * Takes color data and returns fragment
 ******************************/
 void ColorFragShader(PIXEL & fragment, const Attributes & vertAttr, const Attributes & uniforms){
    PIXEL color = 0xff000000;

    color += (unsigned int)(vertAttr.att[0].d * 0xff) << 16;//16 R
    color += (unsigned int)(vertAttr.att[1].d * 0xff) << 8;//8 G
    color += (unsigned int)(vertAttr.att[2].d * 0xff) << 0;//0 B

    fragment = color;
}

void ColorFragShader2(PIXEL & fragment, const Attributes & vertAttr, const Attributes & uniforms){
    PIXEL color = 0xffff0000;


    fragment = color;
}

// Example of a vertex shader
void DefaultVertShader(Vertex & vertOut, Attributes & attrOut, const Vertex & vertIn, const Attributes & vertAttr, const Attributes & uniforms)
{
    // Nothing happens with this vertex, attribute
    vertOut = vertIn;
    attrOut = vertAttr;
}

/**********************************************************
 * VERTEX_SHADER
 * Encapsulates a programmer-specified callback
 * function for transforming vertices and per-vertex
 * attributes. See 'DefaultVertShader' for a pass-through
 * shader example.
 *********************************************************/
class VertexShader
{
    public:
        // Get, Set implicit
        void (*VertShader)(Vertex & vertOut, Attributes & attrOut, const Vertex & vertIn, const Attributes & vertAttr, const Attributes & uniforms);

        // Assumes simple monotone RED shader
        VertexShader()
        {
            VertShader = DefaultVertShader;
        }

        // Initialize with a fragment callback
        VertexShader(void (*VertSdr)(Vertex & vertOut, Attributes & attrOut, const Vertex & vertIn, const Attributes & vertAttr, const Attributes & uniforms))
        {
            setShader(VertSdr);
        }

        // Set the shader to a callback function
        void setShader(void (*VertSdr)(Vertex & vertOut, Attributes & attrOut, const Vertex & vertIn, const Attributes & vertAttr, const Attributes & uniforms))
        {
            VertShader = VertSdr;
        }
};

void SimpleVertexShader(Vertex & vertOut, Attributes & attrOut, const Vertex & vertIn, const Attributes & attrIn, const Attributes & uniforms)
{
    //simple matrix connection
    Matrix* trans = (Matrix*)uniforms.att[0].ptr;
    // borrowed for example vertOut = (*(TransformationMatrix*)uniforms[0].ptr).multiplyByVertex(vertIn);
    vertOut = (*trans) * vertIn;

    // Pass through attributes
    attrOut = attrIn;
}

void SimpleVertexShader2(Vertex & vertOut, Attributes & attrOut, const Vertex & vertIn, const Attributes & attrIn, const Attributes & uniforms)
{
    //perspective transform matrix connections
    Matrix* model = (Matrix*)uniforms.att[1].ptr;
    Matrix* view = (Matrix*)uniforms.att[2].ptr;
    Matrix* proj = (Matrix*)uniforms.att[3].ptr;

    vertOut = ((*proj) * ((*view) * ((*model) * vertIn)));

    // Pass through attributes
    attrOut = attrIn;
}

#endif