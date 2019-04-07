#include "definitions.h"
#include <iostream>

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
                        mouseY = S_HEIGHT - mouseY;
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
                // Implement the rules of the Game Of Life!
                for (int i = 0; i < gridW; ++i)
                {
                        for (int j = 0; j < gridH; ++j)
                        {
                                int count = 0;
                                for (int x = i - 1; x <= i + 1; ++x)
                                {
                                        for (int y = j - 1; y <= j + 1; ++y)
                                        {
                                                // 1. If current coordinates are not the center point,
                                                // 2. AND if it doesn't go out of the 64x64 boundaries
                                                // 3. AND if the outer coordinate is alive.
                                                if (!(x == i && y == j) 
                                                && (x >= 0 && x < gridW && y >= 0 && y < gridH)
                                                && grid[x][y])
                                                {
                                                        count++; 
                                                }
                                        }
                                }

                                // Mortality check!
                                if (grid[i][j]) // If it is alive...
                                {
                                        gridTmp[i][j] = (!(count < 2 || count > 3)); // ...does it stay alive?
                                }
                                else if (!grid[i][j] && count == 8) // PERFORM A RITUAL!!!
                                {
                                        gridTmp[i][j] = true;
                                }
                                else // Otherwise if it's not alive...
                                {
                                        gridTmp[i][j] = (count == 3); // ...does it stay dead?
                                }
                                
                        }
                }

                // Update the grid with results from gridTmp
                for (int i = 0; i < gridW; ++i)
                {
                        for (int j = 0; j < gridH; ++j)
                        {
                                grid[i][j] = gridTmp[i][j];
                        }
                }

                // Wait a half-second between iterations
                SDL_Delay(500);
        }


        // Upscale/blit to screen
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
                        target[ySrc][xSrc]                 = botLeft[ySrc][xSrc];
                        target[ySrc][xSrc+halfWid]         = botRight[ySrc][xSrc];
                        target[ySrc+halfHgt][xSrc]         = topLeft[ySrc][xSrc];
                        target[ySrc+halfHgt][xSrc+halfWid] = topRight[ySrc][xSrc];
                }
        }
}

/***************************************************
 * Demonstrate pixel drawing for project 01.
 **************************************************/
void TestDrawPixel(Buffer2D<PIXEL> & target)
{
        Vertex vert = {10, 502, 1, 1};
        Attributes pointAttributes;
        PIXEL color = 0xffff0000;
        pointAttributes.var['c'] = color;       

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
        Attributes attr[3];
        verts[0] = {100, 362, 1, 1};
        verts[1] = {150, 452, 1, 1};
        verts[2] = {50, 452, 1, 1};
        PIXEL colors1[3] = {0xffff0000, 0xffff0000, 0xffff0000};
        // Your color code goes here for 'attr'
        // Loop to add colors to attr
        for (int  i = 0; i < 3; ++i)
        {
                // The first time we need to insert it.
                attr[i].var.insert(std::pair<char, double>('c', colors1[i])); 
        }

        DrawPrimitive(TRIANGLE, target, verts, attr);

        verts[0] = {300, 402, 1, 1};
        verts[1] = {250, 452, 1, 1};
        verts[2] = {250, 362, 1, 1};
        PIXEL colors2[3] = {0xff00ff00, 0xff00ff00, 0xff00ff00};
        // Your color code goes here for 'attr'
        // Loop to add colors to attr
        for (int  i = 0; i < 3; ++i)
        {
                attr[i].var['c'] = colors2[i]; // Every other time we just overwrite it.
        }

        DrawPrimitive(TRIANGLE, target, verts, attr);

        verts[0] = {450, 362, 1, 1};
        verts[1] = {450, 452, 1, 1};
        verts[2] = {350, 402, 1, 1};
        PIXEL colors3[3] = {0xff0000ff, 0xff0000ff, 0xff0000ff};
        // Your color code goes here for 'attr'
        // Loop to add colors to attr
        for (int  i = 0; i < 3; ++i)
        {
                attr[i].var['c'] = colors3[i];
        }

        DrawPrimitive(TRIANGLE, target, verts, attr);
        
        verts[0] = {110, 262, 1, 1};
        verts[1] = {60, 162, 1, 1};
        verts[2] = {150, 162, 1, 1};
        PIXEL colors4[3] = {0xffff0000, 0xffff0000, 0xffff0000};
        // Your color code goes here for 'attr'
        // Loop to add colors to attr
        for (int  i = 0; i < 3; ++i)
        {
                attr[i].var['c'] = colors4[i];
        }

        DrawPrimitive(TRIANGLE, target, verts, attr);

        verts[0] = {210, 252, 1, 1};
        verts[1] = {260, 172, 1, 1};
        verts[2] = {310, 202, 1, 1};
        PIXEL colors5[3] = {0xff00ff00, 0xff00ff00, 0xff00ff00};
        // Your color code goes here for 'attr'
        // Loop to add colors to attr
        for (int  i = 0; i < 3; ++i)
        {
                attr[i].var['c'] = colors5[i];
        }

        DrawPrimitive(TRIANGLE, target, verts, attr);
        
        verts[0] = {370, 202, 1, 1};
        verts[1] = {430, 162, 1, 1};
        verts[2] = {470, 252, 1, 1};
        PIXEL colors6[3] = {0xff0000ff, 0xff0000ff, 0xff0000ff};
        // Your color code goes here for 'attr'
        // Loop to add colors to attr
        for (int  i = 0; i < 3; ++i)
        {
                attr[i].var['c'] = colors6[i];
        }

        DrawPrimitive(TRIANGLE, target, verts, attr);
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
        Attributes colorAttributes[3];
        colorTriangle[0] = {250, 112, 1, 1};
        colorTriangle[1] = {450, 452, 1, 1};
        colorTriangle[2] = {50, 452, 1, 1};
        PIXEL colors[3] = {0xffff0000, 0xff00ff00, 0xff0000ff}; // Or {{1.0,0.0,0.0}, {0.0,1.0,0.0}, {0.0,0.0,1.0}}
        // Your color code goes here for 'colorAttributes'
        colorAttributes[0].var.insert(std::pair<char, double>('r', 1.0));
        colorAttributes[0].var.insert(std::pair<char, double>('g', 0.0));
        colorAttributes[0].var.insert(std::pair<char, double>('b', 0.0));
        colorAttributes[1].var.insert(std::pair<char, double>('r', 0.0));
        colorAttributes[1].var.insert(std::pair<char, double>('g', 1.0));
        colorAttributes[1].var.insert(std::pair<char, double>('b', 0.0));
        colorAttributes[2].var.insert(std::pair<char, double>('r', 0.0));
        colorAttributes[2].var.insert(std::pair<char, double>('g', 0.0));
        colorAttributes[2].var.insert(std::pair<char, double>('b', 1.0));
        

        FragmentShader myColorFragShader;
        myColorFragShader.FragShader = ColorFragShader;

        Attributes colorUniforms;
        // Your code for the uniform goes here, if any (don't pass NULL here)

        DrawPrimitive(TRIANGLE, target, colorTriangle, colorAttributes, &colorUniforms, &myColorFragShader);
        
        /****************************************************
         * 2. Interpolated image triangle
        ****************************************************/
        Vertex imageTriangle[3];
        Attributes imageAttributes[3];
        imageTriangle[0] = {425, 112, 1, 1};
        imageTriangle[1] = {500, 252, 1, 1};
        imageTriangle[2] = {350, 252, 1, 1};
        double coordinates[3][2] = { {1,0}, {1,1}, {0,1} };
        
        imageAttributes[0].var.insert(std::pair<char, double>('u', coordinates[0][0]));
        imageAttributes[0].var.insert(std::pair<char, double>('v', coordinates[0][1]));
        imageAttributes[1].var.insert(std::pair<char, double>('u', coordinates[1][0]));
        imageAttributes[1].var.insert(std::pair<char, double>('v', coordinates[1][1]));
        imageAttributes[2].var.insert(std::pair<char, double>('u', coordinates[2][0]));
        imageAttributes[2].var.insert(std::pair<char, double>('v', coordinates[2][1]));

        static BufferImage myImage("firelinkarmor.bmp");
        // Provide an image in this directory that you would like to use (powers of 2 dimensions)

        Attributes imageUniforms;
        imageUniforms.ptrImg = &myImage;

        // Your code for the uniform goes here

        FragmentShader myImageFragShader;
        myImageFragShader.FragShader = ImageFragShader;

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
        double divA = 6;
        double divB = 40;
        Vertex quad[] = {{(-1200 / divA) + (S_WIDTH/2), (-1500 / divA) + (S_HEIGHT/2), divA, 1.0/divA },
                         {(1200  / divA) + (S_WIDTH/2), (-1500 / divA) + (S_HEIGHT/2), divA, 1.0/divA },
                         {(1200  / divB) + (S_WIDTH/2), (1500  / divB) + (S_HEIGHT/2), divB, 1.0/divB },
                         {(-1200 / divB) + (S_WIDTH/2), (1500  / divB) + (S_HEIGHT/2), divB, 1.0/divB }};

        Vertex verticesImgA[3];
        Attributes imageAttributesA[3];
        verticesImgA[0] = quad[0];
        verticesImgA[1] = quad[1];
        verticesImgA[2] = quad[2];

        Vertex verticesImgB[3];        
        Attributes imageAttributesB[3];
        verticesImgB[0] = quad[2];
        verticesImgB[1] = quad[3];
        verticesImgB[2] = quad[0];

        double coordinates[4][2] = { {0/divA,0/divA}, {1/divA,0/divA}, {1/divB,1/divB}, {0/divB,1/divB} };
        // Your texture coordinate code goes here for 'imageAttributesA, imageAttributesB'
        imageAttributesA[0].var.insert(std::pair<char, double>('u', coordinates[0][0]));
        imageAttributesA[0].var.insert(std::pair<char, double>('v', coordinates[0][1]));
        
        imageAttributesA[1].var.insert(std::pair<char, double>('u', coordinates[1][0]));
        imageAttributesA[1].var.insert(std::pair<char, double>('v', coordinates[1][1]));
        
        imageAttributesA[2].var.insert(std::pair<char, double>('u', coordinates[2][0]));
        imageAttributesA[2].var.insert(std::pair<char, double>('v', coordinates[2][1]));
        
        imageAttributesB[0].var.insert(std::pair<char, double>('u', coordinates[2][0]));
        imageAttributesB[0].var.insert(std::pair<char, double>('v', coordinates[2][1]));
        
        imageAttributesB[1].var.insert(std::pair<char, double>('u', coordinates[3][0]));
        imageAttributesB[1].var.insert(std::pair<char, double>('v', coordinates[3][1]));
        
        imageAttributesB[2].var.insert(std::pair<char, double>('u', coordinates[0][0]));
        imageAttributesB[2].var.insert(std::pair<char, double>('v', coordinates[0][1]));

        BufferImage myImage("checker.bmp");
        // Ensure the checkboard image is in this directory

        Attributes imageUniforms;
        imageUniforms.ptrImg = &myImage;

        FragmentShader fragImg(ImageFragShader);
                
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
        Attributes colorAttributes[3];
        colorTriangle[0] = { 350, 112, 1, 1};
        colorTriangle[1] = { 400, 200, 1, 1};
        colorTriangle[2] = { 300, 200, 1, 1};

        PIXEL colors[3] = {0xffff0000, 0xff00ff00, 0xff0000ff};
        // Your code for 'colorAttributes' goes here
        colorAttributes[0].var.insert(std::pair<char, double>('r', 1.0));
        colorAttributes[0].var.insert(std::pair<char, double>('g', 0.0));
        colorAttributes[0].var.insert(std::pair<char, double>('b', 0.0));
        colorAttributes[1].var.insert(std::pair<char, double>('r', 0.0));
        colorAttributes[1].var.insert(std::pair<char, double>('g', 1.0));
        colorAttributes[1].var.insert(std::pair<char, double>('b', 0.0));
        colorAttributes[2].var.insert(std::pair<char, double>('r', 0.0));
        colorAttributes[2].var.insert(std::pair<char, double>('g', 0.0));
        colorAttributes[2].var.insert(std::pair<char, double>('b', 1.0));

        FragmentShader myColorFragShader;
        myColorFragShader.FragShader = ColorFragShader;

        Attributes colorUniforms;
        // Your code for the uniform goes here, if any (don't pass NULL here)
        
        VertexShader myColorVertexShader(transformShader);

        /******************************************************************
	 * TRANSLATE (move +100 in the X direction, +50 in the Y direction)
         *****************************************************************/
        // Your translating code that integrates with 'colorUniforms', used by 'myColorVertexShader' goes here
        Matrix transMatrix(4, 4);
        transMatrix.translate(100, 50, 0);
        colorUniforms.transVertex = transMatrix;

	DrawPrimitive(TRIANGLE, target, colorTriangle, colorAttributes, &colorUniforms, &myColorFragShader, &myColorVertexShader);

        /***********************************
         * SCALE (scale by a factor of 0.5)
         ***********************************/
        // Your scaling code that integrates with 'colorUniforms', used by 'myColorVertexShader' goes here
        Matrix scaleMatrix(4, 4);
        scaleMatrix.scale(0.5, 0.5, 1);
        colorUniforms.transVertex = scaleMatrix;

        DrawPrimitive(TRIANGLE, target, colorTriangle, colorAttributes, &colorUniforms, &myColorFragShader, &myColorVertexShader);

        /**********************************************
         * ROTATE 45 degrees in the X-Y plane around Z
         *********************************************/
        // Your rotation code that integrates with 'colorUniforms', used by 'myColorVertexShader' goes here
        Matrix rotMatrix(4, 4);
        rotMatrix.rotate(29.5);
        colorUniforms.transVertex = rotMatrix;

        DrawPrimitive(TRIANGLE, target, colorTriangle, colorAttributes, &colorUniforms, &myColorFragShader, &myColorVertexShader);

        /*************************************************
         * SCALE-TRANSLATE-ROTATE in left-to-right order
         * the previous transformations concatenated.
         ************************************************/
	// Your scale-translate-rotation code that integrates with 'colorUniforms', used by 'myColorVertexShader' goes here
	colorUniforms.transVertex = rotMatrix;
        colorUniforms.transVertex *= transMatrix;
        colorUniforms.transVertex *= scaleMatrix;
        
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
        Vertex quad[] = { {-20,-20, 50, 1},
                          {20, -20, 50, 1},
                          {20, 20, 50, 1},
                          {-20,20, 50, 1}};

        Vertex verticesImgA[3];
        Attributes imageAttributesA[3];
        verticesImgA[0] = quad[0];
        verticesImgA[1] = quad[1];
        verticesImgA[2] = quad[2];

        Vertex verticesImgB[3];        
        Attributes imageAttributesB[3];
        verticesImgB[0] = quad[2];
        verticesImgB[1] = quad[3];
        verticesImgB[2] = quad[0];

        double coordinates[4][2] = { {0,0}, {1,0}, {1,1}, {0,1} };
        // Your texture coordinate code goes here for 'imageAttributesA, imageAttributesB'

        BufferImage myImage("checker.bmp");
        // Ensure the checkboard image is in this directory, you can use another image though

        Attributes imageUniforms;
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