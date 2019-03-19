#include "definitions.h"
#include "shaders.h"
#include "math.h"

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
            int pixelNeighbors = 0;

            // Your Code goes here
            for (int gridY = 0; gridY < gridH; gridY++)
            {
                for (int gridX = 0; gridX < gridW; gridX++)
                {
                    if(gridX > 0 && gridY > 0)
                        pixelNeighbors = grid[gridY + 1][gridX - 1] +
                                            grid[gridY + 1][gridX]     +
                                            grid[gridY + 1][gridX + 1] +
                                            grid[gridY][gridX - 1]     +
                                            grid[gridY][gridX + 1]     +
                                            grid[gridY - 1][gridX - 1] +
                                            grid[gridY - 1][gridX]     +
                                            grid[gridY - 1][gridX + 1];
                    else if (gridX == 0 && gridY > 0)
                        pixelNeighbors = grid[gridY + 1][gridX]     +
                                            grid[gridY + 1][gridX + 1] +
                                            grid[gridY][gridX + 1]     +
                                            grid[gridY - 1][gridX]     +
                                            grid[gridY - 1][gridX + 1];
                    else if (gridX == gridW && gridY > 0)
                        pixelNeighbors = grid[gridY + 1][gridX - 1] +
                                            grid[gridY + 1][gridX]     +
                                            grid[gridY][gridX - 1]     +
                                            grid[gridY - 1][gridX - 1] +
                                            grid[gridY - 1][gridX];
                    else if (gridX > 0 && gridY == 0)
                        pixelNeighbors = grid[gridY + 1][gridX - 1] +
                                            grid[gridY + 1][gridX]     +
                                            grid[gridY + 1][gridX + 1] +
                                            grid[gridY][gridX - 1]     +
                                            grid[gridY][gridX + 1];
                    else if (gridX > 0 && gridY == gridH)
                        pixelNeighbors = grid[gridY][gridX - 1]     +
                                            grid[gridY][gridX + 1]     +
                                            grid[gridY - 1][gridX - 1] +
                                            grid[gridY - 1][gridX]     +
                                            grid[gridY - 1][gridX + 1];


                    if (grid[gridY][gridX] == 1 && pixelNeighbors <= 1)
                        grid[gridY][gridX] = 0;
                    else if (grid[gridY][gridX] == 1 && pixelNeighbors >= 4)
                        grid[gridY][gridX] = 0;
                    else if (grid[gridY][gridX] == 0 && pixelNeighbors == 3)
                        grid[gridY][gridX] = 1;
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
        botRight.zeroOut();
        botLeft.zeroOut();
        topRight.zeroOut();
        topLeft.zeroOut();


        // Your code goes here 
        // Feel free to copy from other test functions to get started!

        static Buffer2D<double> zBuf(target.width(), target.height());
        // Will need to be cleared every frame, like the screen

        /**************************************************
        * 1. Image quad (2 TRIs) Code (texture interpolated)
        **************************************************/
        Vertex quad[] = { {-20,-20, 50, 1},  //0
                          {20, -20, 50, 1},  //1
                          {20, 20, 50, 1},   //2
                          {-20, 20, 50, 1},  //3
                          {-20,-20, 90, 1},  //4
                          {20, -20, 90, 1},  //5
                          {20, 20, 90, 1},   //6
                          {-20, 20, 90, 1}}; //7

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

        Vertex verticesImgC[3];
        Attributes imageAttributesC[3];
        verticesImgC[0] = quad[2];
        verticesImgC[1] = quad[6];
        verticesImgC[2] = quad[7];

        Vertex verticesImgD[3];
        Attributes imageAttributesD[3];
        verticesImgD[0] = quad[2];
        verticesImgD[1] = quad[7];
        verticesImgD[2] = quad[3];

        Vertex verticesImgE[3];
        Attributes imageAttributesE[3];
        verticesImgE[0] = quad[1];
        verticesImgE[1] = quad[5];
        verticesImgE[2] = quad[2];

        Vertex verticesImgF[3];
        Attributes imageAttributesF[3];
        verticesImgF[0] = quad[5];
        verticesImgF[1] = quad[6];
        verticesImgF[2] = quad[2];

        Vertex verticesImgG[3];
        Attributes imageAttributesG[3];
        verticesImgG[0] = quad[4];
        verticesImgG[1] = quad[0];
        verticesImgG[2] = quad[3];

        Vertex verticesImgH[3];
        Attributes imageAttributesH[3];
        verticesImgH[0] = quad[3];
        verticesImgH[1] = quad[7];
        verticesImgH[2] = quad[4];

        Vertex verticesImgI[3];
        Attributes imageAttributesI[3];
        verticesImgI[0] = quad[5];
        verticesImgI[1] = quad[4];
        verticesImgI[2] = quad[7];

        Vertex verticesImgJ[3];
        Attributes imageAttributesJ[3];
        verticesImgJ[0] = quad[7];
        verticesImgJ[1] = quad[6];
        verticesImgJ[2] = quad[5];

        double coordinates[4][2] = { {0,0}, {1,0}, {1,1}, {0,1} };
        // Your texture coordinate code goes here for 'imageAttributesA, imageAttributesB'
        
        //First group of attributes
        imageAttributesA[0].insertDbl(coordinates[0][0]);
        imageAttributesA[0].insertDbl(coordinates[0][1]);

        imageAttributesA[1].insertDbl(coordinates[1][0]);
        imageAttributesA[1].insertDbl(coordinates[1][1]);

        imageAttributesA[2].insertDbl(coordinates[2][0]);
        imageAttributesA[2].insertDbl(coordinates[2][1]);

        // Second group of attributes
        imageAttributesB[0].insertDbl(coordinates[2][0]);
        imageAttributesB[0].insertDbl(coordinates[2][1]);

        imageAttributesB[1].insertDbl(coordinates[3][0]);
        imageAttributesB[1].insertDbl(coordinates[3][1]);

        imageAttributesB[2].insertDbl(coordinates[0][0]);
        imageAttributesB[2].insertDbl(coordinates[0][1]);

        // Third group of attributes
        imageAttributesC[0].insertDbl(coordinates[1][0]);
        imageAttributesC[0].insertDbl(coordinates[1][1]);

        imageAttributesC[1].insertDbl(coordinates[2][0]);
        imageAttributesC[1].insertDbl(coordinates[2][1]);

        imageAttributesC[2].insertDbl(coordinates[3][0]);
        imageAttributesC[2].insertDbl(coordinates[3][1]);

        // Fourth group of attributes
        imageAttributesD[0].insertDbl(coordinates[1][0]);
        imageAttributesD[0].insertDbl(coordinates[1][1]);

        imageAttributesD[1].insertDbl(coordinates[3][0]);
        imageAttributesD[1].insertDbl(coordinates[3][1]);

        imageAttributesD[2].insertDbl(coordinates[0][0]);
        imageAttributesD[2].insertDbl(coordinates[0][1]);

        // Fifth group of attributes
        imageAttributesE[0].insertDbl(coordinates[0][0]);
        imageAttributesE[0].insertDbl(coordinates[0][1]);

        imageAttributesE[1].insertDbl(coordinates[1][0]);
        imageAttributesE[1].insertDbl(coordinates[1][1]);

        imageAttributesE[2].insertDbl(coordinates[3][0]);
        imageAttributesE[2].insertDbl(coordinates[3][1]);

        // Sixth group of attributes
        imageAttributesF[0].insertDbl(coordinates[1][0]);
        imageAttributesF[0].insertDbl(coordinates[1][1]);

        imageAttributesF[1].insertDbl(coordinates[2][0]);
        imageAttributesF[1].insertDbl(coordinates[2][1]);

        imageAttributesF[2].insertDbl(coordinates[3][0]);
        imageAttributesF[2].insertDbl(coordinates[3][1]);

        // Seventh group of attributes
        imageAttributesG[0].insertDbl(coordinates[0][0]);
        imageAttributesG[0].insertDbl(coordinates[0][1]);

        imageAttributesG[1].insertDbl(coordinates[1][0]);
        imageAttributesG[1].insertDbl(coordinates[1][1]);

        imageAttributesG[2].insertDbl(coordinates[2][0]);
        imageAttributesG[2].insertDbl(coordinates[2][1]);

        // Eighth group of attributes
        imageAttributesH[0].insertDbl(coordinates[2][0]);
        imageAttributesH[0].insertDbl(coordinates[2][1]);

        imageAttributesH[1].insertDbl(coordinates[3][0]);
        imageAttributesH[1].insertDbl(coordinates[3][1]);

        imageAttributesH[2].insertDbl(coordinates[0][0]);
        imageAttributesH[2].insertDbl(coordinates[0][1]);

        // Ninth group of attributes
        imageAttributesI[0].insertDbl(coordinates[0][0]);
        imageAttributesI[0].insertDbl(coordinates[0][1]);

        imageAttributesI[1].insertDbl(coordinates[1][0]);
        imageAttributesI[1].insertDbl(coordinates[1][1]);

        imageAttributesI[2].insertDbl(coordinates[2][0]);
        imageAttributesI[2].insertDbl(coordinates[2][1]);

        // Tenth group of attributes
        imageAttributesJ[0].insertDbl(coordinates[2][0]);
        imageAttributesJ[0].insertDbl(coordinates[2][1]);

        imageAttributesJ[1].insertDbl(coordinates[3][0]);
        imageAttributesJ[1].insertDbl(coordinates[3][1]);

        imageAttributesJ[2].insertDbl(coordinates[0][0]);
        imageAttributesJ[2].insertDbl(coordinates[0][1]);

        

        static BufferImage myImage("images/doggie.bmp");
        static BufferImage huskyImg("images/husky.bmp");
        static BufferImage foxieImg("images/fox-dog.bmp");
        static BufferImage labImage("images/lab.bmp");
        static BufferImage smartImg("images/smart-dog.bmp");

        //Ensure the checkboard image is in this directory, you can use another image though

        Attributes imageUniforms;

        //Your code for the uniform goes here
        Matrix model;
        model.addTrans(0, 0, 0);
        Matrix view = camera4x4(myCam.x, myCam.y, myCam.z,
                               myCam.yaw, myCam.pitch, myCam.roll);
        Matrix proj = perspective4x4(60, 1.0, 1, 200); // FOV, Aspect, Near, Far
        //Matrix proj = orthographic4x4(30, 30, 1, 200);

        imageUniforms.insertPtr((void*)&myImage);
        imageUniforms.insertPtr((void*)&model);
        imageUniforms.insertPtr((void*)&view);
        imageUniforms.insertPtr((void*)&proj);

        

        FragmentShader fragImg;
        fragImg.setShader(imageFragShader);
        // Your code for the image fragment shader goes here

        VertexShader vertImg;
        // Your code for the image vertex shader goes here
        // NOTE: This must include the at least the 
        // projection matrix if not more transformations 
        vertImg.setShader(SimpleVertexShader2);
                
        // Draw image triangle 
        DrawPrimitive(TRIANGLE, topLeft, verticesImgA, imageAttributesA, &imageUniforms, &fragImg, &vertImg, &zBuf);
        DrawPrimitive(TRIANGLE, topLeft, verticesImgB, imageAttributesB, &imageUniforms, &fragImg, &vertImg, &zBuf);

        
        imageUniforms[0].ptr = (void*)&huskyImg;
        DrawPrimitive(TRIANGLE, topLeft, verticesImgC, imageAttributesC, &imageUniforms, &fragImg, &vertImg, &zBuf);
        DrawPrimitive(TRIANGLE, topLeft, verticesImgD, imageAttributesD, &imageUniforms, &fragImg, &vertImg, &zBuf);

        imageUniforms[0].ptr = (void*)&foxieImg;
        DrawPrimitive(TRIANGLE, topLeft, verticesImgE, imageAttributesE, &imageUniforms, &fragImg, &vertImg, &zBuf);
        DrawPrimitive(TRIANGLE, topLeft, verticesImgF, imageAttributesF, &imageUniforms, &fragImg, &vertImg, &zBuf);

        imageUniforms[0].ptr = (void*)&labImage;
        DrawPrimitive(TRIANGLE, topLeft, verticesImgG, imageAttributesG, &imageUniforms, &fragImg, &vertImg, &zBuf);
        DrawPrimitive(TRIANGLE, topLeft, verticesImgH, imageAttributesH, &imageUniforms, &fragImg, &vertImg, &zBuf);

        imageUniforms[0].ptr = (void*)&smartImg;
        DrawPrimitive(TRIANGLE, topLeft, verticesImgI, imageAttributesI, &imageUniforms, &fragImg, &vertImg, &zBuf);
        DrawPrimitive(TRIANGLE, topLeft, verticesImgJ, imageAttributesJ, &imageUniforms, &fragImg, &vertImg, &zBuf);

        


        imageUniforms[0].ptr = (void*)&myImage;
        view = camera4x4(topCam.x, topCam.y, topCam.z,
                        topCam.yaw, topCam.pitch, topCam.roll);
        imageUniforms[2].ptr = (void*)&view;

        proj = orthographic4x4(25, 25, 1, 200);
        imageUniforms[3].ptr = (void*)&proj;

        DrawPrimitive(TRIANGLE, topRight, verticesImgA, imageAttributesA, &imageUniforms, &fragImg, &vertImg, &zBuf);
        DrawPrimitive(TRIANGLE, topRight, verticesImgB, imageAttributesB, &imageUniforms, &fragImg, &vertImg, &zBuf);

        imageUniforms[0].ptr = (void*)&huskyImg;
        DrawPrimitive(TRIANGLE, topRight, verticesImgC, imageAttributesC, &imageUniforms, &fragImg, &vertImg, &zBuf);
        DrawPrimitive(TRIANGLE, topRight, verticesImgD, imageAttributesD, &imageUniforms, &fragImg, &vertImg, &zBuf);

        imageUniforms[0].ptr = (void*)&myImage;
        view = camera4x4(frontCam.x, frontCam.y, frontCam.z,
                        frontCam.yaw, frontCam.pitch, frontCam.roll);
        imageUniforms[2].ptr = (void*)&view;

        DrawPrimitive(TRIANGLE, botLeft, verticesImgA, imageAttributesA, &imageUniforms, &fragImg, &vertImg, &zBuf);
        DrawPrimitive(TRIANGLE, botLeft, verticesImgB, imageAttributesB, &imageUniforms, &fragImg, &vertImg, &zBuf);

        view = camera4x4(sideCam.x, sideCam.y, sideCam.z,
                        sideCam.yaw, sideCam.pitch, sideCam.roll);
        imageUniforms[0].ptr = &foxieImg;
        imageUniforms[2].ptr = (void*)&view;

        DrawPrimitive(TRIANGLE, botRight, verticesImgE, imageAttributesE, &imageUniforms, &fragImg, &vertImg, &zBuf);
        DrawPrimitive(TRIANGLE, botRight, verticesImgF, imageAttributesF, &imageUniforms, &fragImg, &vertImg, &zBuf);



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
        // Your Code goes here for 'pointAttributes'
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

        // Prepare the attributes for triangle 1
        verts[0] = {100, 362, 1, 1};
        verts[1] = {150, 452, 1, 1};
        verts[2] = {50, 452, 1, 1};
        PIXEL colors1[3] = {0xffff0000, 0xffff0000, 0xffff0000};
        
        // Assign each vertex the color it will be
        attr[0].color = colors1[0];
        attr[1].color = colors1[1];
        attr[2].color = colors1[2];

        // Draw triangle 1
        DrawPrimitive(TRIANGLE, target, verts, attr);

        // Preapre the attributes for triangle 2
        verts[0] = {300, 402, 1, 1};
        verts[1] = {250, 452, 1, 1};
        verts[2] = {250, 362, 1, 1};
        PIXEL colors2[3] = {0xff00ff00, 0xff00ff00, 0xff00ff00};
        
        // Assign each vertex the color it will be
        attr[0].color = colors2[0];
        attr[1].color = colors2[1];
        attr[2].color = colors2[2];

        // Draw triangle 2
        DrawPrimitive(TRIANGLE, target, verts, attr);

        // Prepare the attributes for triangle 3
        verts[0] = {450, 362, 1, 1};
        verts[1] = {450, 452, 1, 1};
        verts[2] = {350, 402, 1, 1};
        PIXEL colors3[3] = {0xff0000ff, 0xff0000ff, 0xff0000ff};
        
        // Assign each vertex the color it will be
        attr[0].color = colors3[0];
        attr[1].color = colors3[1];
        attr[2].color = colors3[2];

        // Draw triangle 3
        DrawPrimitive(TRIANGLE, target, verts, attr);
        
        // Prepare the attributes for triangle 4
        verts[0] = {110, 262, 1, 1};
        verts[1] = {60, 162, 1, 1};
        verts[2] = {150, 162, 1, 1};
        PIXEL colors4[3] = {0xffff0000, 0xffff0000, 0xffff0000};
        
        // Assign each vertex the color it will be
        attr[0].color = colors4[0];
        attr[1].color = colors4[1];
        attr[2].color = colors4[2];

        // Draw triangle 4
        DrawPrimitive(TRIANGLE, target, verts, attr);

        // Prepare the attributes for triangle 5
        verts[0] = {210, 252, 1, 1};
        verts[1] = {260, 172, 1, 1};
        verts[2] = {310, 202, 1, 1};
        PIXEL colors5[3] = {0xff00ff00, 0xff00ff00, 0xff00ff00};
        
        // Assign each vertex the color it will be
        attr[0].color = colors5[0];
        attr[1].color = colors5[1];
        attr[2].color = colors5[2];

        // Draw triangle 5
        DrawPrimitive(TRIANGLE, target, verts, attr);
        
        // Prepare the attributes for triangle 6
        verts[0] = {370, 202, 1, 1};
        verts[1] = {430, 162, 1, 1};
        verts[2] = {470, 252, 1, 1};
        PIXEL colors6[3] = {0xff0000ff, 0xff0000ff, 0xff0000ff};
        
        // Assign each vertex the color it will be
        attr[0].color = colors6[0];
        attr[1].color = colors6[1];
        attr[2].color = colors6[2];

        // Draw triangle 6
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
        colorAttributes[0].insertDbl(1.0);
        colorAttributes[0].insertDbl(0.0);
        colorAttributes[0].insertDbl(0.0);

        colorAttributes[1].insertDbl(0.0);
        colorAttributes[1].insertDbl(1.0);
        colorAttributes[1].insertDbl(0.0);

        colorAttributes[2].insertDbl(0.0);
        colorAttributes[2].insertDbl(0.0);
        colorAttributes[2].insertDbl(1.0);

        FragmentShader myColorFragShader;
        myColorFragShader.setShader(baryInterpolationShader);

        Attributes colorUniforms;
        // Your code for the uniform goes here, if any (don't pass NULL here)
        // No uniforms today

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
        // Your texture coordinate code goes here for 'imageAttributes'
        imageAttributes[0].insertDbl(1.0);
        imageAttributes[0].insertDbl(0.0);

        imageAttributes[1].insertDbl(1.0);
        imageAttributes[1].insertDbl(1.0);

        imageAttributes[2].insertDbl(0.0);
        imageAttributes[2].insertDbl(1.0);

        static BufferImage myImage("c.bmp");
        // Provide an image in this directory that you would like to use (powers of 2 dimensions)

        Attributes imageUniforms;
        // Your code for the uniform goes here
        imageUniforms.ptrImage = &myImage;

        FragmentShader myImageFragShader;
        // Your code for the image fragment shader goes here
        myImageFragShader.setShader(imageFragShader);

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
        imageAttributesA[0].insertDbl(coordinates[0][0]);
        imageAttributesA[0].insertDbl(coordinates[0][1]);

        imageAttributesA[1].insertDbl(coordinates[1][0]);
        imageAttributesA[1].insertDbl(coordinates[1][1]);

        imageAttributesA[2].insertDbl(coordinates[2][0]);
        imageAttributesA[2].insertDbl(coordinates[2][1]);

        imageAttributesB[0].insertDbl(coordinates[2][0]);
        imageAttributesB[0].insertDbl(coordinates[2][1]);

        imageAttributesB[1].insertDbl(coordinates[3][0]);
        imageAttributesB[1].insertDbl(coordinates[3][1]);

        imageAttributesB[2].insertDbl(coordinates[0][0]);
        imageAttributesB[2].insertDbl(coordinates[0][1]);

        BufferImage myImage("checker.bmp");
        // Ensure the checkboard image is in this directory

        Attributes imageUniforms;
        // Your code for the uniform goes here
        imageUniforms.ptrImage = &myImage;


        FragmentShader fragImg;
        // Your code for the image fragment shader goes here
        fragImg.setShader(imageFragShader);
                
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
        colorAttributes[0].insertDbl(1.0);
        colorAttributes[0].insertDbl(0.0);
        colorAttributes[0].insertDbl(0.0);

        colorAttributes[1].insertDbl(0.0);
        colorAttributes[1].insertDbl(1.0);
        colorAttributes[1].insertDbl(0.0);

        colorAttributes[2].insertDbl(0.0);
        colorAttributes[2].insertDbl(0.0);
        colorAttributes[2].insertDbl(1.0);

        FragmentShader myColorFragShader;
        myColorFragShader.setShader(baryInterpolationShader);

        Attributes colorUniforms;
        // Your code for the uniform goes here, if any (don't pass NULL here)
        
        VertexShader myColorVertexShader;
        // Your code for the vertex shader goes here 
        myColorVertexShader.setShader(TransformVertexShader);

        /******************************************************************
	 * TRANSLATE (move +100 in the X direction, +50 in the Y direction)
         *****************************************************************/
        // Your translating code that integrates with 'colorUniforms', used by 'myColorVertexShader' goes here
        Matrix transMatrix;
        transMatrix.addTrans(100, 50, 0);
        colorUniforms.matrix = transMatrix;

	DrawPrimitive(TRIANGLE, target, colorTriangle, colorAttributes, &colorUniforms, &myColorFragShader, &myColorVertexShader);

        /***********************************
         * SCALE (scale by a factor of 0.5)
         ***********************************/
        // Your scaling code that integrates with 'colorUniforms', used by 'myColorVertexShader' goes here
        Matrix scaleMatrix;
        scaleMatrix.addScale(0.5, 0.5, 0.5);
        colorUniforms.matrix = scaleMatrix;

        DrawPrimitive(TRIANGLE, target, colorTriangle, colorAttributes, &colorUniforms, &myColorFragShader, &myColorVertexShader);

        /**********************************************
         * ROTATE 45 degrees in the X-Y plane around Z
         *********************************************/
        // Your rotation code that integrates with 'colorUniforms', used by 'myColorVertexShader' goes here
        Matrix rotMatrix;
        rotMatrix.addRot(Z, 30);
        colorUniforms.matrix = rotMatrix;

        DrawPrimitive(TRIANGLE, target, colorTriangle, colorAttributes, &colorUniforms, &myColorFragShader, &myColorVertexShader);

        /*************************************************
         * SCALE-TRANSLATE-ROTATE in left-to-right order
         * the previous transformations concatenated.
         ************************************************/
        //Your scale-translate-rotation code that integrates with 'colorUniforms', used by 'myColorVertexShader' goes here
        Matrix combined = rotMatrix * transMatrix * scaleMatrix;

        colorUniforms.matrix = combined;
		
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
        
        //First group of attributes
        imageAttributesA[0].insertDbl(coordinates[0][0]);
        imageAttributesA[0].insertDbl(coordinates[0][1]);

        imageAttributesA[1].insertDbl(coordinates[1][0]);
        imageAttributesA[1].insertDbl(coordinates[1][1]);

        imageAttributesA[2].insertDbl(coordinates[2][0]);
        imageAttributesA[2].insertDbl(coordinates[2][1]);

        // Second group of attributes
        imageAttributesB[0].insertDbl(coordinates[2][0]);
        imageAttributesB[0].insertDbl(coordinates[2][1]);

        imageAttributesB[1].insertDbl(coordinates[3][0]);
        imageAttributesB[1].insertDbl(coordinates[3][1]);

        imageAttributesB[2].insertDbl(coordinates[0][0]);
        imageAttributesB[2].insertDbl(coordinates[0][1]);


        static BufferImage myImage("checker.bmp");
        //Ensure the checkboard image is in this directory, you can use another image though

        Attributes imageUniforms;

        //Your code for the uniform goes here

        /*
        Uniforms
        [0] -> Image reference
        [1] -> Model transform
        [2] -> View transform
        */

        Matrix model;
        model.addTrans(0, 0, 0);
        Matrix view = camera4x4(myCam.x, myCam.y, myCam.z,
                               myCam.yaw, myCam.pitch, myCam.roll);
        Matrix proj = perspective4x4(60, 1.0, 1, 200); // FOV, Aspect, Near, Far

        imageUniforms.insertPtr((void*)&myImage);
        imageUniforms.insertPtr((void*)&model);
        imageUniforms.insertPtr((void*)&view);
        imageUniforms.insertPtr((void*)&proj);

        

        FragmentShader fragImg;
        fragImg.setShader(imageFragShader);
        // Your code for the image fragment shader goes here

        VertexShader vertImg;
        // Your code for the image vertex shader goes here
        // NOTE: This must include the at least the 
        // projection matrix if not more transformations 
        vertImg.setShader(SimpleVertexShader2);
                
        // Draw image triangle 
        DrawPrimitive(TRIANGLE, target, verticesImgA, imageAttributesA, &imageUniforms, &fragImg, &vertImg, &zBuf);
        DrawPrimitive(TRIANGLE, target, verticesImgB, imageAttributesB, &imageUniforms, &fragImg, &vertImg, &zBuf);

        // NOTE: To test the Z-Buffer additinonal draw calls/geometry need to be called into this scene
}

void TestVSD(Buffer2D<PIXEL> & target)
{
        double coords[4][2] = { {0,0}, {1,0}, {1,1}, {0,1} };
        Vertex wall1[]= 
        {
                {10, 0, 80, 1},
                {60, 0, 50, 1},
                {60, 40, 50, 1},
                {10, 40, 80, 1}
        };

        Vertex wall2[]=
        {
                {85, 0, 35, 1},
                {135, 0, 5, 1},
                {135, 40, 5, 1},
                {85, 40, 35, 1}
        };

        Vertex wall3[]=
        {
                {35, 0, 65, 1},
                {110, 0,20, 1},
                {110, 40, 20, 1},
                {35, 40, 65, 1}
        };


        Vertex wall10[]=
        {
                {6, 0, 8, 1},
                {10, 0, 4, 1},
                {10, 40, 4, 1},
                {6, 40, 8, 1}
        };

        Vertex wall11[] =
        {
                {8, 0, 6, 1},
                {9, 0, 10, 1},
                {9, 40, 10, 1},
                {8, 40, 6, 1}
        };

        Quad itsLitYo(wall10);
        Quad isSplitterYo(wall11);
        Node firstNode(wall10);
        Node secondNode(wall11);
        Node* newNode1 = NULL;
        Node* newNode2 = NULL;

        if (firstNode.isQuadIntersected(&secondNode))
                firstNode.split(newNode1, newNode2);


        // First wall vertices and attributes
        Vertex vertsImg1A[3];
        Attributes attrsImg1A[3];
        vertsImg1A[0] = wall1[0];
        vertsImg1A[1] = wall1[1];
        vertsImg1A[2] = wall1[2];

        Vertex vertsImg1B[3];
        Attributes attrsImg1B[3];
        vertsImg1B[0] = wall1[2];
        vertsImg1B[1] = wall1[3];
        vertsImg1B[2] = wall1[0];

        attrsImg1A[0].insertDbl(coords[0][0]);
        attrsImg1A[0].insertDbl(coords[0][1]);

        attrsImg1A[1].insertDbl(coords[1][0]);
        attrsImg1A[1].insertDbl(coords[1][1]);

        attrsImg1A[2].insertDbl(coords[2][0]);
        attrsImg1A[2].insertDbl(coords[2][1]);

        attrsImg1B[0].insertDbl(coords[2][0]);
        attrsImg1B[0].insertDbl(coords[2][1]);

        attrsImg1B[1].insertDbl(coords[3][0]);
        attrsImg1B[1].insertDbl(coords[3][1]);

        attrsImg1B[2].insertDbl(coords[0][0]);
        attrsImg1B[2].insertDbl(coords[0][1]);

        // Second wall vertices and attributes
        Vertex vertsImg2A[3];
        Attributes attrsImg2A[3];
        vertsImg2A[0] = wall2[0];
        vertsImg2A[1] = wall2[1];
        vertsImg2A[2] = wall2[2];

        Vertex vertsImg2B[3];
        Attributes attrsImg2B[3];
        vertsImg2B[0] = wall2[2];
        vertsImg2B[1] = wall2[3];
        vertsImg2B[2] = wall2[0];

        attrsImg2A[0].insertDbl(coords[0][0]);
        attrsImg2A[0].insertDbl(coords[0][1]);

        attrsImg2A[1].insertDbl(coords[1][0]);
        attrsImg2A[1].insertDbl(coords[1][1]);

        attrsImg2A[2].insertDbl(coords[2][0]);
        attrsImg2A[2].insertDbl(coords[2][1]);

        attrsImg2B[0].insertDbl(coords[2][0]);
        attrsImg2B[0].insertDbl(coords[2][1]);

        attrsImg2B[1].insertDbl(coords[3][0]);
        attrsImg2B[1].insertDbl(coords[3][1]);

        attrsImg2B[2].insertDbl(coords[0][0]);
        attrsImg2B[2].insertDbl(coords[0][1]);

        // Third wall vertices and attributes

        Vertex vertsImg3A[3];
        Attributes attrsImg3A[3];
        vertsImg3A[0] = wall3[0];
        vertsImg3A[1] = wall3[1];
        vertsImg3A[2] = wall3[2];

        Vertex vertsImg3B[3];
        Attributes attrsImg3B[3];
        vertsImg3B[0] = wall3[2];
        vertsImg3B[1] = wall3[3];
        vertsImg3B[2] = wall3[0];

        attrsImg3A[0].insertDbl(coords[0][0]);
        attrsImg3A[0].insertDbl(coords[0][1]);

        attrsImg3A[1].insertDbl(coords[1][0]);
        attrsImg3A[1].insertDbl(coords[1][1]);

        attrsImg3A[2].insertDbl(coords[2][0]);
        attrsImg3A[2].insertDbl(coords[2][1]);

        attrsImg3B[0].insertDbl(coords[2][0]);
        attrsImg3B[0].insertDbl(coords[2][1]);

        attrsImg3B[1].insertDbl(coords[3][0]);
        attrsImg3B[1].insertDbl(coords[3][1]);

        attrsImg3B[2].insertDbl(coords[0][0]);
        attrsImg3B[2].insertDbl(coords[0][1]);

        static BufferImage totoro("images/totoro.bmp");
        static BufferImage haku("images/hugs.bmp");
        static BufferImage laputa("images/laputa.bmp");
        Attributes imageUniforms;

        Matrix model;
        model.addTrans(0,0,0);
        Matrix view = camera4x4(myCam.x, myCam.y, myCam.z,
                                myCam.yaw, myCam.pitch, myCam.roll);
        Matrix proj = perspective4x4(60, 1.0, 1, 200);

        imageUniforms.insertPtr((void*)&totoro);
        imageUniforms.insertPtr((void*)&model);
        imageUniforms.insertPtr((void*)&view);
        imageUniforms.insertPtr((void*)&proj);

        FragmentShader fragShader(imageFragShader);
        VertexShader vertShader(SimpleVertexShader2);

        Matrix newModel;
        newModel.addTrans(10, 0, 20);

        imageUniforms[0].ptr = (void*)&laputa;
        imageUniforms[1].ptr = (void*)&newModel;

        DrawPrimitive(TRIANGLE, target, vertsImg3A, attrsImg3A, &imageUniforms, &fragShader, &vertShader);
        DrawPrimitive(TRIANGLE, target, vertsImg3B, attrsImg3B, &imageUniforms, &fragShader, &vertShader);

        imageUniforms[0].ptr = (void*)&totoro;
        imageUniforms[1].ptr = (void*)&model;

        DrawPrimitive(TRIANGLE, target, vertsImg1A, attrsImg1A, &imageUniforms, &fragShader, &vertShader);
        DrawPrimitive(TRIANGLE, target, vertsImg1B, attrsImg1B, &imageUniforms, &fragShader, &vertShader);

        imageUniforms[0].ptr = (void*)&haku;

        DrawPrimitive(TRIANGLE, target, vertsImg2A, attrsImg2A, &imageUniforms, &fragShader, &vertShader);
        DrawPrimitive(TRIANGLE, target, vertsImg2B, attrsImg2B, &imageUniforms, &fragShader, &vertShader);
 
}



#endif