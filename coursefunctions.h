#include "definitions.h"
#include "shaders.h"

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
                // Your Code goes here
                for(int y = 0; y < gridH; y++)
                {
                        for(int x = 0; x < gridW; x++)
                        {
                                // Check for number of alive neighbors
                                int numNeighbors = 0;
                                int xtemp = -1;
                                int ytemp = -2;
                                for(int i = 0; i < 9; i++)
                                {
                                        if(i % 3 == 0)
                                                ytemp += 1;
                                        if(y + ytemp >= 0 && x + xtemp >= 0)
                                        {
                                                if(grid[y + ytemp][x + xtemp] == 1)
                                                {
                                                        numNeighbors++;
                                                        if(xtemp == 0 && ytemp == 0)
                                                                numNeighbors--;
                                                }
                                        }
                                        else
                                        {
                                                int tempy = y + ytemp;
                                                int tempx = x + xtemp;
                                                if(tempy < 0)
                                                {
                                                        tempy = gridH - 1;
                                                }
                                                else if(tempx < 0)
                                                {
                                                        tempx = gridW - 1;
                                                }

                                                if(grid[tempy][tempx] == 1)
                                                {
                                                        numNeighbors++;
                                                        if(xtemp == 0 && ytemp == 0)
                                                                numNeighbors--;
                                                }
                                        }
                                        
                                        if(xtemp >= 1)
                                                xtemp = -1;
                                        else
                                                xtemp += 1;
                                }
                                if(grid[y][x] == 1) // Alive at current location
                                {
                                        if(numNeighbors < 2 || numNeighbors >= 4)
                                                gridTmp[y][x] = 0;
                                        else
                                                gridTmp[y][x] = 1;
                                }
                                else // Dead at current location
                                {
                                        if(numNeighbors == 3)
                                                gridTmp[y][x] = 1;
                                        else
                                                gridTmp[y][x] = 0;
                                }
                        }
                }

                for(int y = 0; y < gridH; y++)
                        for(int x = 0; x < gridW; x++)
                                grid[y][x] = gridTmp[y][x];

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
        pointAttributes.color = color; 

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
        PIXEL colors1[3] = {0xffff0000, 0xff00ff00, 0x0000ff};
        // Your color code goes here for 'attr'
        for(int i = 0; i < 3; i++)
                attr[i].add(colors1[i]);

        // setting the first attribute's color to the first color in the PIXEL color just defined
        // attr[0].color = colors1[0];

        DrawPrimitive(TRIANGLE, target, verts, attr);

        verts[0] = {300, 402, 1, 1};
        verts[1] = {250, 452, 1, 1};
        verts[2] = {250, 362, 1, 1};
        PIXEL colors2[3] = {0xff00ff00, 0xff00ff00, 0xff00ff00};
        // Your color code goes here for 'attr'
        for(int i = 0; i < 3; i++)
                attr[i].add(colors2[i]);

        // attr[0].color = colors2[1];

        DrawPrimitive(TRIANGLE, target, verts, attr);

        verts[0] = {450, 362, 1, 1};
        verts[1] = {450, 452, 1, 1};
        verts[2] = {350, 402, 1, 1};
        PIXEL colors3[3] = {0xff0000ff, 0xff0000ff, 0xff0000ff};
        // Your color code goes here for 'attr'
        for(int i = 0; i < 3; i++)
                attr[i].add(colors3[i]);

        // attr[0].color = colors3[2];

        DrawPrimitive(TRIANGLE, target, verts, attr);
        
        verts[0] = {110, 262, 1, 1};
        verts[1] = {60, 162, 1, 1};
        verts[2] = {150, 162, 1, 1};
        PIXEL colors4[3] = {0xffff0000, 0xffff0000, 0xffff0000};
        // Your color code goes here for 'attr'
        for(int i = 0; i < 3; i++)
                attr[i].add(colors4[i]);

        // attr[0].color = colors4[0];

        DrawPrimitive(TRIANGLE, target, verts, attr);

        verts[0] = {210, 252, 1, 1};
        verts[1] = {260, 172, 1, 1};
        verts[2] = {310, 202, 1, 1};
        PIXEL colors5[3] = {0xffff0000, 0xff00ff00, 0x0000ff};
        // Your color code goes here for 'attr'
        for(int i = 0; i < 3; i++)
                attr[i].add(colors5[i]);

        // attr[0].color = colors5[1];

        DrawPrimitive(TRIANGLE, target, verts, attr);
        
        verts[0] = {370, 202, 1, 1};
        verts[1] = {430, 162, 1, 1};
        verts[2] = {470, 252, 1, 1};
        PIXEL colors6[3] = {0xff0000ff, 0xff0000ff, 0xff0000ff};
        // Your color code goes here for 'attr'
        for(int i = 0; i < 3; i++)
                attr[i].add(colors6[i]);

        // attr[0].color = colors6[2];

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
        Vertex myTriangle[3];
        Attributes myAttributes[3];
        myTriangle[0] = {250, 112, 1, 1};
        myTriangle[1] = {450, 452, 1, 1};
        myTriangle[2] = {50, 452, 1, 1};
        PIXEL colors[3] = {0xffff0000, 0xff00ff00, 0xff0000ff}; // Or {{1.0,0.0,0.0}, {0.0,1.0,0.0}, {0.0,0.0,1.0}}
        // Your color code goes here for 'colorAttributes'
        myAttributes[0].add(1.0);
        myAttributes[0].add(0.0);
        myAttributes[0].add(0.0);
        myAttributes[1].add(0.0);
        myAttributes[1].add(1.0);
        myAttributes[1].add(0.0);
        myAttributes[2].add(0.0);
        myAttributes[2].add(0.0);
        myAttributes[2].add(1.0);

        FragmentShader myFragShader;
        // Your code for the color fragment shader goes here
        myFragShader.setShader(ColorFragShader);

        Attributes colorUniforms;
        // Your code for the uniform goes here, if any (don't pass NULL here)

        DrawPrimitive(TRIANGLE, target, myTriangle, myAttributes, &colorUniforms, &myFragShader);

        myAttributes[0].reset();
        myAttributes[1].reset();
        myAttributes[2].reset();

        /****************************************************
         * 2. Interpolated image triangle
        ****************************************************/
        myTriangle[0] = {425, 112, 1, 1};
        myTriangle[1] = {500, 252, 1, 1};
        myTriangle[2] = {350, 252, 1, 1};
        double coordinates[3][2] = { {1,0}, {1,1}, {0,1} };
        // Your texture coordinate code goes here for 'imageAttributes'
        // r and g are used for the x and y
        myAttributes[0].add(coordinates[0][0]);
        myAttributes[0].add(coordinates[0][1]);
        myAttributes[1].add(coordinates[1][0]);
        myAttributes[1].add(coordinates[1][1]);
        myAttributes[2].add(coordinates[2][0]);
        myAttributes[2].add(coordinates[2][1]);

        static BufferImage myImage("baboon.bmp");
        // Provide an image in this directory that you would like to use (powers of 2 dimensions)

        Attributes imageUniforms;
        // Your code for the uniform goes here
        imageUniforms.ptrImg = &myImage;

        // Your code for the image fragment shader goes here
        // IMPLEMENT CALLBACK BETWEEN THESE TWO THINGS
        myFragShader.setShader(ImageFragShader);

        DrawPrimitive(TRIANGLE, target, myTriangle, myAttributes, &imageUniforms, &myFragShader);
}

/************************************************
 * Demonstrate Perspective correct interpolation 
 * for Project 04. 
 * 
 * If I had more time, I would refine this
 * function to use a cube of vertices instead of
 * a quad. This isn't as efficient but it is what
 * I came up with so far
 ***********************************************/
void TestDrawPerspectiveCorrect(Buffer2D<PIXEL> & target)
{
        /**************************************************
        * 1. Image quad (2 TRIs) Code (texture interpolated)
        **************************************************/
        // Artificially projected, viewport transformed
        double divA = 6;
        double divB = 40;
                                                                                        // this last value is w, which is 1/z
        Vertex quad[] = {{(-1200 / divA) + (S_WIDTH/2), (-1500 / divA) + (S_HEIGHT/2), divA, 1.0/divA },  // {56, 6, 6, 0.1666}
                         {(1200  / divA) + (S_WIDTH/2), (-1500 / divA) + (S_HEIGHT/2), divA, 1.0/divA },  // {456, 6, 6, 0.1666}
                         {(1200  / divB) + (S_WIDTH/2), (1500  / divB) + (S_HEIGHT/2), divB, 1.0/divB },  // {286, 293.5, 40, 0.025}
                         {(-1200 / divB) + (S_WIDTH/2), (1500  / divB) + (S_HEIGHT/2), divB, 1.0/divB }}; // {226, 293.5, 40, 0.025}

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

        double coordinates[4][2] = { {0/divA,0/divA}, {1/divA,0/divA}, {1/divB,1/divB}, {0/divB,1/divB} }; // {0,0}{0.1666, 0}{0.025, 0.025}{0, 0.025}
        // Your texture coordinate code goes here for 'imageAttributesA, imageAttributesB'
        
        imageAttributesA[0].add(coordinates[0][0]);
        imageAttributesA[0].add(coordinates[0][1]);
        imageAttributesA[1].add(coordinates[1][0]);
        imageAttributesA[1].add(coordinates[1][1]);
        imageAttributesA[2].add(coordinates[2][0]);
        imageAttributesA[2].add(coordinates[2][1]);

        imageAttributesB[0].add(coordinates[2][0]);
        imageAttributesB[0].add(coordinates[2][1]);
        imageAttributesB[1].add(coordinates[3][0]);
        imageAttributesB[1].add(coordinates[3][1]);
        imageAttributesB[2].add(coordinates[0][0]);
        imageAttributesB[2].add(coordinates[0][1]);

        /**************************************************
        * 2. Second image quad for back wall
        **************************************************/
        double divC = 40;
        Vertex wall[] = {{(-1200 / divC) + (S_WIDTH/2), (1500 / divC) + (S_HEIGHT/2), divC, 1.0/divC },
                         {(1200  / divC) + (S_WIDTH/2), (1500 / divC) + (S_HEIGHT/2), divC, 1.0/divC },
                         {(1200  / divC) + (S_WIDTH/2), (4500  / divC) + (S_HEIGHT/2), divC, 1.0/divC },  
                         {(-1200 / divC) + (S_WIDTH/2), (4500  / divC) + (S_HEIGHT/2), divC, 1.0/divC }};

        Vertex verticesImgC[3];
        Attributes imageAttributesC[3];
        verticesImgC[0] = wall[0];
        verticesImgC[1] = wall[1];
        verticesImgC[2] = wall[2];

        Vertex verticesImgD[3];
        Attributes imageAttributesD[3];
        verticesImgD[0] = wall[2];
        verticesImgD[1] = wall[3];
        verticesImgD[2] = wall[0];

        double wallCoordinates[4][2] = { {0/divC,0/divC}, {1/divC,0/divC}, {1/divC,1/divC}, {0/divC,1/divC} };

        imageAttributesC[0].add(wallCoordinates[0][0]);
        imageAttributesC[0].add(wallCoordinates[0][1]);
        imageAttributesC[1].add(wallCoordinates[1][0]);
        imageAttributesC[1].add(wallCoordinates[1][1]);
        imageAttributesC[2].add(wallCoordinates[2][0]);
        imageAttributesC[2].add(wallCoordinates[2][1]);
        
        imageAttributesD[0].add(wallCoordinates[2][0]);
        imageAttributesD[0].add(wallCoordinates[2][1]);
        imageAttributesD[1].add(wallCoordinates[3][0]);
        imageAttributesD[1].add(wallCoordinates[3][1]);
        imageAttributesD[2].add(wallCoordinates[0][0]);
        imageAttributesD[2].add(wallCoordinates[0][1]);

        /**************************************************
        * 3. Third image quad for left wall
        **************************************************/
        Vertex left[] = {{(-1200 / divA) + (S_WIDTH/2), (-1500 / divA) + (S_HEIGHT/2), divA, 1.0/divA },
                         {(-1200 / divB) + (S_WIDTH/2), (1500 / divB) + (S_HEIGHT/2), divB, 1.0/divB },
                         {(-1200 / divB) + (S_WIDTH/2), (4500 / divB) + (S_HEIGHT/2), divB, 1.0/divB },  
                         {(-1200 / divA) + (S_WIDTH/2), (1500 / divA) + (S_HEIGHT/2), divA, 1.0/divA }};

        Vertex verticesImgE[3];
        Attributes imageAttributesE[3];
        verticesImgE[0] = left[0];
        verticesImgE[1] = left[1];
        verticesImgE[2] = left[2];

        Vertex verticesImgF[3];
        Attributes imageAttributesF[3];
        verticesImgF[0] = left[2];
        verticesImgF[1] = left[3];
        verticesImgF[2] = left[0];

        double leftCoordinates[4][2] = { {0/divA,0/divA}, {1/divB,0/divB}, {1/divB,1/divB}, {0/divA,1/divA} };

        imageAttributesE[0].add(leftCoordinates[0][0]);
        imageAttributesE[0].add(leftCoordinates[0][1]);
        imageAttributesE[1].add(leftCoordinates[1][0]);
        imageAttributesE[1].add(leftCoordinates[1][1]);
        imageAttributesE[2].add(leftCoordinates[2][0]);
        imageAttributesE[2].add(leftCoordinates[2][1]);

        imageAttributesF[0].add(leftCoordinates[2][0]);
        imageAttributesF[0].add(leftCoordinates[2][1]);
        imageAttributesF[1].add(leftCoordinates[3][0]);
        imageAttributesF[1].add(leftCoordinates[3][1]);
        imageAttributesF[2].add(leftCoordinates[0][0]);
        imageAttributesF[2].add(leftCoordinates[0][1]);      

        /**************************************************
        * 3. Fourth image quad for right wall
        **************************************************/
        Vertex right[] = {{(1200 / divB) + (S_WIDTH/2), (1500 / divB) + (S_HEIGHT/2), divB, 1.0/divB },
                         {(1200 / divA) + (S_WIDTH/2), (-1500 / divA) + (S_HEIGHT/2), divA, 1.0/divA },
                         {(1200 / divA) + (S_WIDTH/2), (1500 / divA) + (S_HEIGHT/2), divA, 1.0/divA },  
                         {(1200 / divB) + (S_WIDTH/2), (4500 / divB) + (S_HEIGHT/2), divB, 1.0/divB }};

        Vertex verticesImgG[3];
        Attributes imageAttributesG[3];
        verticesImgG[0] = right[0];
        verticesImgG[1] = right[1];
        verticesImgG[2] = right[2];

        Vertex verticesImgH[3];
        Attributes imageAttributesH[3];
        verticesImgH[0] = right[2];
        verticesImgH[1] = right[3];
        verticesImgH[2] = right[0];

        double rightCoordinates[4][2] = { {0/divB,0/divB}, {1/divA,0/divA}, {1/divA,1/divA}, {0/divB,1/divB} };

        imageAttributesG[0].add(rightCoordinates[0][0]);
        imageAttributesG[0].add(rightCoordinates[0][1]);
        imageAttributesG[1].add(rightCoordinates[1][0]);
        imageAttributesG[1].add(rightCoordinates[1][1]);
        imageAttributesG[2].add(rightCoordinates[2][0]);
        imageAttributesG[2].add(rightCoordinates[2][1]);

        imageAttributesH[0].add(rightCoordinates[2][0]);
        imageAttributesH[0].add(rightCoordinates[2][1]);
        imageAttributesH[1].add(rightCoordinates[3][0]);
        imageAttributesH[1].add(rightCoordinates[3][1]);
        imageAttributesH[2].add(rightCoordinates[0][0]);
        imageAttributesH[2].add(rightCoordinates[0][1]); 

        static BufferImage myImage("checker.bmp");
        // Ensure the checkboard image is in this directory
        static BufferImage baboon("baboon.bmp");

        Attributes imageUniforms;
        // Your code for the uniform goes here
        imageUniforms.ptrImg = &myImage;

        FragmentShader fragImg;
        // Your code for the image fragment shader goes here
        fragImg.setShader(FragShaderUVwithoutImage);
                
        // Draw image triangle 
        DrawPrimitive(TRIANGLE, target, verticesImgA, imageAttributesA, &imageUniforms, &fragImg);
        DrawPrimitive(TRIANGLE, target, verticesImgB, imageAttributesB, &imageUniforms, &fragImg);
        // temporarily draw a .bmp
        imageUniforms.ptrImg = &baboon;
        fragImg.setShader(ImageFragShader);
        DrawPrimitive(TRIANGLE, target, verticesImgC, imageAttributesC, &imageUniforms, &fragImg);
        DrawPrimitive(TRIANGLE, target, verticesImgD, imageAttributesD, &imageUniforms, &fragImg);
        imageUniforms.ptrImg = &myImage;
        fragImg.setShader(FragShaderUVwithoutImage);
        DrawPrimitive(TRIANGLE, target, verticesImgE, imageAttributesE, &imageUniforms, &fragImg);
        DrawPrimitive(TRIANGLE, target, verticesImgF, imageAttributesF, &imageUniforms, &fragImg);
        DrawPrimitive(TRIANGLE, target, verticesImgG, imageAttributesG, &imageUniforms, &fragImg);
        DrawPrimitive(TRIANGLE, target, verticesImgH, imageAttributesH, &imageUniforms, &fragImg);
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
        colorAttributes[0].add(1.0);
        colorAttributes[0].add(0.0);
        colorAttributes[0].add(0.0);

        colorAttributes[1].add(0.0);
        colorAttributes[1].add(1.0);
        colorAttributes[1].add(0.0);

        colorAttributes[2].add(0.0);
        colorAttributes[2].add(0.0);
        colorAttributes[2].add(1.0);

        FragmentShader myColorFragShader;
        myColorFragShader.setShader(ColorFragShader);

        Attributes colorUniforms;
        // Your code for the uniform goes here, if any (don't pass NULL here)

        VertexShader myColorVertexShader;
        // Your code for the vertex shader goes here
        // myColorVertexShader.setShader(MyVertShader);
        myColorVertexShader.VertShader = MyVertShader;

        /******************************************************************
		 * TRANSLATE (move +100 in the X direction, +50 in the Y direction)
         *****************************************************************/
        // Your translating code that integrates with 'colorUniforms', used by 'myColorVertexShader' goes here
        colorUniforms.matrix.translate(100, 50, 0);
        DrawPrimitive(TRIANGLE, target, colorTriangle, colorAttributes, &colorUniforms, &myColorFragShader, &myColorVertexShader);

        /***********************************
         * SCALE (scale by a factor of 0.5)
         ***********************************/
        // Your scaling code that integrates with 'colorUniforms', used by 'myColorVertexShader' goes here
        colorUniforms.matrix.reset();
        colorUniforms.matrix.scale(0.5, 0.5, 0.5);

        DrawPrimitive(TRIANGLE, target, colorTriangle, colorAttributes, &colorUniforms, &myColorFragShader, &myColorVertexShader);

        /**********************************************
         * ROTATE 45 degrees in the X-Y plane around Z
         *********************************************/
        // Your rotation code that integrates with 'colorUniforms', used by 'myColorVertexShader' goes here
        colorUniforms.matrix.reset();
        colorUniforms.matrix.rotate(30, 'z');

        DrawPrimitive(TRIANGLE, target, colorTriangle, colorAttributes, &colorUniforms, &myColorFragShader, &myColorVertexShader);

        /*************************************************
         * SCALE-TRANSLATE-ROTATE in left-to-right order
         * the previous transformations concatenated.
         ************************************************/
	// Your scale-translate-rotation code that integrates with 'colorUniforms', used by 'myColorVertexShader' goes here
        colorUniforms.matrix.reset();
        colorUniforms.matrix.rotate(30, 'z');
        colorUniforms.matrix.translate(100, 50, 0);
        colorUniforms.matrix.scale(0.5, 0.5, 0.5);

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

/************************************************
 * DETERMINANT of two matrices
 ***********************************************/
float Determinant(float A, float B, float C, float D)
{
        return ((A * D) - (B * C));
}

/************************************************
 * INTERP interpolates values between points based
 * on the percentage each determinant area is of the whole
 ***********************************************/
Attributes Interpolate(const double totalArea, const double firsD, const double seconD, const double thirD, const Attributes * attrs, const Vertex* triangle)
{
        Attributes interpolated;
        // each determinant of the triangle is a percentage of the total area, so multiply that times the color
        // at the opposite end of the triangle from that vertex, which tells you how much color to apply
        for (int i = 0; i < attrs[0].values.size(); i++)
        {
                // interpolation for the image
                double interp = ((firsD * attrs[2].values[i] + seconD * attrs[0].values[i] + thirD * attrs[1].values[i]) / totalArea);

                // interpolation for the depth
                double depth = ((firsD * triangle[2].w + seconD * triangle[0].w + thirD * triangle[1].w) / totalArea);

                // finally reciprocate and combine the interpolation of the two
                interpolated.values.push_back(interp * 1/depth);
        }

        return interpolated;
}

#endif