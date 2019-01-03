#include "definitions.h"

#ifndef COURSE_FUNCTIONS_H
#define COURSE_FUNCTIONS_H

/***************************************************
 * Team Activity for week #1.
 * When working on this activity be sure to 
 * comment out the following function calls in 
 * pipeline.cpp:main():
 *      1) processUserInputs(running);
 *      2) clearScreen(frame);
 *      3) Any draw calls that are being made there
 * 
 * When you finish this activity be sure to 
 * uncomment these functions again!!!
 **************************************************/
void GameOfLife(Buffer2D<PIXEL> & target)
{
        // 'Static's are initialized exactly once
        static bool isSetup = true;
        static bool holdDown = false;
        static int w = target.width();
        static int h = target.height();
        static int scaleFactor = 8;
        static int gridW = 64;
        static int gridH = 64; 
        static int grid[64][64];
        static int gridTmp[64][64];

        // Setup small grid, temporary grid from previous iteration
        for(int y = 0; y < gridH; y++)
        {
                for(int x = 0; x < gridW; x++)
                {
                        grid[y][x] = (target[y*scaleFactor][x*scaleFactor] == 0xffff0000) ? 1 : 0;
                        gridTmp[y][x] = grid[y][x];
                }
        }

        //Parse for inputs
        SDL_Event e;
        while(SDL_PollEvent(&e)) 
        {
                int mouseX;
                int mouseY;
                if(e.type == SDL_MOUSEBUTTONDOWN)
                {
                        holdDown = true;
                }
                if(e.type == SDL_MOUSEBUTTONUP)
                {
                        holdDown = false;
                }
                if(e.key.keysym.sym == 'g' && e.type == SDL_KEYDOWN) 
                {
                        isSetup = !isSetup;
                }
                if(holdDown && isSetup)
                {
                        // Clicking the mouse changes a pixel's color
                        SDL_GetMouseState(&mouseX, &mouseY);
                        int gridX = mouseX / scaleFactor;
                        int gridY = mouseY / scaleFactor;
                        if(grid[gridY][gridX] == 1)
                        {
                                // Dead
                                grid[gridY][gridX] = 0;
                        }
                        else
                        {
                                // Alive
                                grid[gridY][gridX] = 1;
                        }
                }
        }

        // Advance the simulation after pressing 'g'
        if(!isSetup)
        {
                // Your Code goes here

                // Wait a half-second between iterations
                SDL_Delay(500);
        }



        // Upscale/match to screen
        for(int y = 0; y < h; y++)
        {
                for(int x = 0; x < w; x++)
                {
                        int yScal = y/scaleFactor;
                        int xScal = x/scaleFactor;
                        if(grid[yScal][xScal] == 0)
                        {
                                // Dead Color
                                target[y][x] = 0xff000000;
                        }
                        else
                        {
                                // Alive color
                                target[y][x] = 0xffff0000;
                        }
                }
        }
}

/***************************************************
 * Create a 3D View like in a CAD program
 * NOTE: Assumes that the resolution is an even 
 * value in both dimensions.
 **************************************************/
void CADView(Buffer2D<PIXEL> & target)
{
        // Each CAD Quadrant
        static int halfWid = target.width()/2;
        static int halfHgt = target.height()/2;
        static Buffer2D<PIXEL> topLeft(halfWid, halfHgt);
        static Buffer2D<PIXEL> topRight(halfWid, halfHgt);
        static Buffer2D<PIXEL> botLeft(halfWid, halfHgt);
        static Buffer2D<PIXEL> botRight(halfWid, halfHgt);


        // Your code goes here 
        // Feel free to copy from other test functions to get started!


        // Blit four panels to target
        int yStartSrc = 0;
        int xStartSrc = 0;
        int yLimitSrc = topLeft.height();
        int xLimitSrc = topLeft.width();
        for(int ySrc = yStartSrc; ySrc < yLimitSrc; ySrc++)
        {
                for(int xSrc = xStartSrc; xSrc < xLimitSrc; xSrc++)
                {
                        target[ySrc][xSrc]                 = topLeft[ySrc][xSrc];
                        target[ySrc][xSrc+halfWid]         = topRight[ySrc][xSrc];
                        target[ySrc+halfHgt][xSrc]         = botLeft[ySrc][xSrc];
                        target[ySrc+halfHgt][xSrc+halfWid] = botRight[ySrc][xSrc];
                }
        }
}

/***************************************************
 * Demonstrate pixel drawing for project 01.
 **************************************************/
void TestDrawPixel(Buffer2D<PIXEL> & target)
{
        Vertex vert = {10, 10, 1, 1};
        VBO pointAttributes;
        PIXEL color = 0xffff0000;
        // Your Code goes here for 'VBO pointAttributes'


        DrawPrimitive(POINT, target, &vert, &pointAttributes);
}

/***********************************************
 * Demonstrate Triangle Drawing for Project 02. 
 **********************************************/
void TestDrawTriangle(Buffer2D<PIXEL> & target)
{
        /**************************************************
        * 6 Flat color triangles below
        *************************************************/
        Vertex verts[3];
        VBO attributes[3];
        verts[0] = {100, 150, 1, 1};
        verts[1] = {150, 60, 1, 1};
        verts[2] = {50, 60, 1, 1};
        PIXEL colors1[3] = {0xffff0000, 0xffff0000, 0xffff0000};
        // Your color code goes here for 'VBO attributes'

        DrawPrimitive(TRIANGLE, target, verts, attributes);


        verts[0] = {300, 110, 1, 1};
        verts[1] = {250, 60, 1, 1};
        verts[2] = {250, 150, 1, 1};
        PIXEL colors2[3] = {0xffff0000, 0xffff0000, 0xffff0000};
        // Your color code goes here for 'VBO attributes'

        DrawPrimitive(TRIANGLE, target, verts, attributes);

        verts[0] = {450, 150, 1, 1};
        verts[1] = {450, 60, 1, 1};
        verts[2] = {350, 110, 1, 1};
        PIXEL colors3[3] = {0xff00ff00, 0xff00ff00, 0xff00ff00};
        // Your color code goes here for 'VBO attributes'

        DrawPrimitive(TRIANGLE, target, verts, attributes);
        
        verts[0] = {110, 250, 1, 1};
        verts[1] = {60, 350, 1, 1};
        verts[2] = {150, 350, 1, 1};
        PIXEL colors4[3] = {0xff00ff00, 0xff00ff00, 0xff00ff00};
        // Your color code goes here for 'VBO attributes'

        DrawPrimitive(TRIANGLE, target, verts, attributes);

        verts[0] = {210, 260, 1, 1};
        verts[1] = {260, 340, 1, 1};
        verts[2] = {310, 310, 1, 1};
        PIXEL colors5[3] = {0xff00ff00, 0xff00ff00, 0xff00ff00};
        // Your color code goes here for 'VBO attributes'

        DrawPrimitive(TRIANGLE, target, verts, attributes);
        
        verts[0] = {370, 310, 1, 1};
        verts[1] = {430, 350, 1, 1};
        verts[2] = {470, 260, 1, 1};
        PIXEL colors6[3] = {0xff00ff00, 0xff00ff00, 0xff00ff00};
        // Your color code goes here for 'VBO attributes'

        DrawPrimitive(TRIANGLE, target, verts, attributes);
}


/***********************************************
 * Demonstrate Fragment Shader, linear VBO 
 * interpolation for Project 03. 
 **********************************************/
void TestDrawFragments(Buffer2D<PIXEL> & target)
{
        /**************************************************
        * 1. Interpolated color triangle
        *************************************************/
        Vertex colorTriangle[3];
        VBO colorAttributes[3];
        colorTriangle[0] = {250, 400, 1, 1};
        colorTriangle[1] = {450, 60, 1, 1};
        colorTriangle[2] = {50, 60, 1, 1};
        PIXEL colors[3] = {0xffff0000, 0xff00ff00, 0xff0000ff}; // Or {{1.0,0.0,0.0}, {0.0,1.0,0.0}, {0.0,0.0,1.0}}
        // Your color code goes here for 'VBO colorAttributes'

        FragmentShader myColorFragShader;
        // Your code for the color fragment shader goes here

        VBO colorUniforms;
        // Your code for the uniform goes here, if any (don't pass NULL here)

        DrawPrimitive(TRIANGLE, target, colorTriangle, colorAttributes, &colorUniforms, &myColorFragShader);

        /****************************************************
         * 2. Interpolated image triangle
        ****************************************************/
        Vertex imageTriangle[3];
        VBO imageAttributes[3];
        imageTriangle[0] = {425, 400, 1, 1};
        imageTriangle[1] = {500, 260, 1, 1};
        imageTriangle[2] = {350, 260, 1, 1};
        double coordinates[3][2] = { {1,1}, {1,0}, {0,0} };
        // Your texture coordinate code goes here for 'VBO imageAttributes'

        BufferImage myImage("image.bmp");
        // Provide an image in this directory that you would like to use (powers of 2 dimensions)

        VBO imageUniforms;
        // Your code for the uniform goes here

        FragmentShader myImageFragShader;
        // Your code for the image fragment shader goes here

        DrawPrimitive(TRIANGLE, target, imageTriangle, imageAttributes, &imageUniforms, &myImageFragShader);
}

/************************************************
 * Demonstrate Perspective correct interpolation 
 * for Project 04. 
 ***********************************************/
void TestDrawPerspectiveCorrect(Buffer2D<PIXEL> & target)
{
        /**************************************************
        * 1. Image quad (2 TRIs) Code (texture interpolated)
        **************************************************/
        // Artificially projected, viewport transformed
        double divA = 40;
        double divB = 6;
        Vertex quad[] = {{(-1200 / divA) + 256, (-1500 / divA) + 256, divA, 1.0/divA },
                         {(-1200 / divB) + 256, (1500  / divB) + 256, divB, 1.0/divB },
                         {(1200  / divB) + 256, (1500  / divB) + 256, divB, 1.0/divB },
                         {(1200  / divA) + 256, (-1500 / divA) + 256, divA, 1.0/divA }};

        Vertex verticesImgA[3];
        VBO imageAttributesA[3];
        verticesImgA[0] = quad[0];
        verticesImgA[1] = quad[1];
        verticesImgA[2] = quad[2];

        Vertex verticesImgB[3];        
        VBO imageAttributesB[3];
        verticesImgB[0] = quad[2];
        verticesImgB[1] = quad[3];
        verticesImgB[2] = quad[0];

        double coordinates[4][2] = { {0/divA,1/divA}, {0/divB,0/divB}, {1/divB,0/divB}, {1/divA,1/divA} };
        // Your texture coordinate code goes here for 'VBO imageAttributesA, imageAttributesB'

        BufferImage myImage("checker.bmp");
        // Ensure the checkboard image is in this directory

        VBO imageUniforms;
        // Your code for the uniform goes here

        FragmentShader fragImg;
        // Your code for the image fragment shader goes here
                
        // Draw image triangle 
        DrawPrimitive(TRIANGLE, target, verticesImgA, imageAttributesA, &imageUniforms, &fragImg);
        DrawPrimitive(TRIANGLE, target, verticesImgB, imageAttributesB, &imageUniforms, &fragImg);
}

/************************************************
 * Demonstrate simple transformations for  
 * Project 05 in the vertex shader callback. 
 ***********************************************/
void TestVertexShader(Buffer2D<PIXEL> & target)
{
        /**************************************************
        * 1. Interpolated color triangle
        *************************************************/
        Vertex colorTriangle[3];
        VBO colorAttributes[3];
        colorTriangle[0] = { 250, 400, 1, 1};
        colorTriangle[1] = { 450, 60, 1, 1};
        colorTriangle[2] = { 50, 60, 1, 1};

        PIXEL colors[3] = {0xffff0000, 0xff00ff00, 0xff0000ff};
        // Your code for VBO 'colorAttributes' goes here

        FragmentShader myColorFragShader;

        VBO colorUniforms;
        // Your code for the uniform goes here, if any (don't pass NULL here)
        
        VertexShader myColorVertexShader;
        // Your code for the vertex shader goes here 

        /******************************************************************
         * TRANSLATE (move +150 in the X direction, -50 in the Y direction)
         *****************************************************************/
        // Your scaling code that is stored in 'colorUniforms', used by 'myColorVertexShader' goes here

        // Draw image triangle as normal
        DrawPrimitive(TRIANGLE, target, colorTriangle, colorAttributes, &colorUniforms, &myColorFragShader, &myColorVertexShader);

        /***********************************
         * SCALE (scale by a factor of 0.5)
         ***********************************/
        // Your scaling code that is stored in 'colorUniforms', used by 'myColorVertexShader' goes here

        DrawPrimitive(TRIANGLE, target, colorTriangle, colorAttributes, &colorUniforms, &myColorFragShader, &myColorVertexShader);

        /**********************************************
         * ROTATE 30 degrees in the X-Y plane around Z
         *********************************************/
        // Your scaling code that is stored in 'colorUniforms', used by 'myColorVertexShader' goes here

        DrawPrimitive(TRIANGLE, target, colorTriangle, colorAttributes, &colorUniforms, &myColorFragShader, &myColorVertexShader);

        /*************************************************
         * SCALE-TRANSLATE-ROTATE in left-to-right order
         * the previous transformations concatenated.
         ************************************************/
        // Your scaling code that is stored in 'colorUniforms', used by 'myColorVertexShader' goes here

        DrawPrimitive(TRIANGLE, target, colorTriangle, colorAttributes, &colorUniforms, &myColorFragShader, &myColorVertexShader);
}

/********************************************
 * Verify that the whole pipeline works. By
 * the end of week 07 you should be able to
 * run this code successfully.
 *******************************************/
void TestPipeline(Buffer2D<PIXEL> & target)
{
        // This is similar to TestDrawPerspectiveCorrect 
        // except that:
        //      1) perspective projection is expected from
        //         the programmer in the vertex shader.
        //      2) Clipping/normalization must be turned on.
        //      3) The ViewPort Transform must be applied.
        //      4) The Z-Buffer is incorporated into drawing.
        //      5) You may want to involve camera variables:
        //              i)   camYaw
        //              ii)  camPitch
        //              iii) camRoll, 
        //              iv)  camX
        //              v)   camY
        //              vi)  camZ
        //      To incorporate a view transform (add movement)
        
        static Buffer2D<double> zBuf(target.width(), target.height());
        // Will need to be cleared every frame, like the screen

        /**************************************************
        * 1. Image quad (2 TRIs) Code (texture interpolated)
        **************************************************/
        Vertex quad[] = { {-20,20, 50, 1},
                          {20, 20, 50, 1},
                          {20, 20, 50, 1},
                          {-20,20, 50, 1}};

        Vertex verticesImgA[3];
        VBO imageAttributesA[3];
        verticesImgA[0] = quad[0];
        verticesImgA[1] = quad[1];
        verticesImgA[2] = quad[2];

        Vertex verticesImgB[3];        
        VBO imageAttributesB[3];
        verticesImgB[0] = quad[2];
        verticesImgB[1] = quad[3];
        verticesImgB[2] = quad[0];

        double coordinates[4][2] = { {0,1}, {1,1}, {1,0}, {0,0} };
        // Your texture coordinate code goes here for 'VBO imageAttributesA, imageAttributesB'

        BufferImage myImage("checker.bmp");
        // Ensure the checkboard image is in this directory, you can use another image though

        VBO imageUniforms;
        // Your code for the uniform goes here

        FragmentShader fragImg;
        // Your code for the image fragment shader goes here

        VertexShader vertImg;
        // Your code for the image vertex shader goes here
        // NOTE: This must include the at least the 
        // projection matrix if not more transformations 
                
        // Draw image triangle 
        DrawPrimitive(TRIANGLE, target, verticesImgA, imageAttributesA, &imageUniforms, &fragImg, &vertImg, &zBuf);
        DrawPrimitive(TRIANGLE, target, verticesImgB, imageAttributesB, &imageUniforms, &fragImg, &vertImg, &zBuf);

        // NOTE: To test the Z-Buffer additinonal draw calls/geometry need to be called into this scene
}



#endif