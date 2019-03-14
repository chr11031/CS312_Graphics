
#ifndef SHADERS_H
#define SHADERS_H

void SimpleVertexShader2(Vertex & vertOut, Attributes & attrOut, const ... ) // TODO something else goes here
{
    Transform* model = (Transform*)uniforms[1].ptr;
    Transform* view = (Transform*)uniforms[2].ptr;
    Transform* proj = (Transform*)uniforms[3].ptr;

    vertOut = (*proj) * (*proj) * (*proj); // TODO something else goes here
}

/****************************************************
 * MYVERTSHADER:
 * Vertex shader for transformations, transfers inputting vertices,
 * attributes, and matrices, then combines them appropriately
 ***************************************************/
void MyVertShader(Vertex & vertOut, Attributes & attrOut, const Vertex & vertIn, const Attributes & vertAttr, const Attributes & uniforms)
{
    vertOut = vertIn;

    vertOut *= uniforms.matrix;

    attrOut = vertAttr;
}

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

/****************************************************
 * FRAGSHADERUVWITHOUTIMAGE:
 * The fragment shader that draws a red and white
 * checkerboard
 ***************************************************/
void FragShaderUVwithoutImage(PIXEL & fragment, const Attributes & attributes, const Attributes & uniform)
{
    // these static ints and key listeners change the number of rows and columns
    // on the triangle
    static int row = 8;
    static int column = 8;
    SDL_Event e;
    while(SDL_PollEvent(&e))
    {
        // add a row
        if(e.key.keysym.sym == 'w' && e.type == SDL_KEYDOWN) 
        {
            row++;
        }
        // subtract a row
        if(e.key.keysym.sym == 's' && e.type == SDL_KEYDOWN) 
        {
            row--;
        }
        // add a column
        if(e.key.keysym.sym == 'd' && e.type == SDL_KEYDOWN) 
        {
            column++;
        }
        // subtract a column
        if(e.key.keysym.sym == 'a' && e.type == SDL_KEYDOWN) 
        {
            column--;
        }   
    }

    // Figure out which X/Y square our UV would fall on
    int xSquare = attributes.values[0] * row;
    int ySquare = attributes.values[1] * column;

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
}

#endif // SHADERS_H