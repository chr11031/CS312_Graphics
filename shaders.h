
#ifndef SHADERS_H
#define SHADERS_H

/****************************************************
 * COLORFRAGSHADER:
 * The fragment shader when drawing pixels based off
 * of individual rgb values
 ***************************************************/
void ColorFragShader(PIXEL & fragment, const Attributes & vertAttr, const Attributes & uniforms)
{
    // Output our shader color value, in this case simple colors
    // convert individual channels to hex base and shift to add to entire color
    fragment = 0xff000000;
    fragment += (unsigned int)(vertAttr.values[0] * 0xff) << 16;
    fragment += (unsigned int)(vertAttr.values[1] * 0xff) << 8;
    fragment += (unsigned int)(vertAttr.values[2] * 0xff) << 0;
}

/****************************************************
 * IMAGEFRAGSHADER:
 * The fragment shader when drawing pixels by transferring
 * them from a .bmp to the screen
 ***************************************************/
void ImageFragShader(PIXEL & fragment, const Attributes & vertAttr, const Attributes & uniforms)
{
    // create pointer to the image
    BufferImage* ptr = (BufferImage*)uniforms.ptrImg;

    // set the width to be zero-based and multiply by the percentage of the entire picture
    int x = vertAttr.values[0] * (ptr->width() - 1);
    int y = vertAttr.values[1] * (ptr->height() - 1);
    
    // set the color of that point in the picture to the pixel that we have
    fragment = (*ptr)[y][x];
}

#endif // SHADERS_H