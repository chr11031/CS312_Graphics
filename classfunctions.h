#include "definitions.h"

#ifndef COURSE_FUNCTIONS_H
#define COURSE_FUNCTIONS_H

/***************************************************
 * Team Activity for week #1
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
















/******************************************
 * Demonstrate transformations for 
 * project 07.
 *****************************************/
void TestDrawClip(Buffer2D<PIXEL> & target)
{
        // Z-Buffer setup, use
        static Buffer2D<double> zBuf(target.width(), target.height());
        clearZBuffer(zBuf);

        /**************************************************
        * 1. First triangle Code (texture interpolated)
        *************************************************/
        Vertex verticesImgA[3];
        verticesImgA[0].x = -20;
        verticesImgA[0].y = 0;
        verticesImgA[0].z = 50;
        verticesImgA[0].w = 1;
        verticesImgA[1].x = 20;
        verticesImgA[1].y = 0;
        verticesImgA[1].z = 50;
        verticesImgA[1].w = 1;
        verticesImgA[2].x = 20;
        verticesImgA[2].y = 20;
        verticesImgA[2].z = 50;
        verticesImgA[2].w = 1;

        Vertex verticesImgB[3];        
        verticesImgB[0].x = 20;
        verticesImgB[0].y = 20;
        verticesImgB[0].z = 50;
        verticesImgB[0].w = 1;
        verticesImgB[1].x = -20;
        verticesImgB[1].y = 20;
        verticesImgB[1].z = 50;
        verticesImgB[1].w = 1;
        verticesImgB[2].x = -20;
        verticesImgB[2].y = 0;
        verticesImgB[2].z = 50;
        verticesImgB[2].w = 1;


        // Texture coordinates 
        double arrF[] = { 0, 0, 1.0, 0, 1.0, 1.0, 0.0, 1.0};
        unsigned short arrS[] = 
        { 0, 0, 255, 0, 255, 255, 0, 255}; // Assumes 256 x 256 image (change accordingly but must be powers of 2)
        VBO attributesImgA[3];
        attributesImgA[0].insertdouble(arrS[0]);
        attributesImgA[0].insertdouble(arrS[1]);
        attributesImgA[1].insertdouble(arrS[2]);
        attributesImgA[1].insertdouble(arrS[3]);
        attributesImgA[2].insertdouble(arrS[4]);
        attributesImgA[2].insertdouble(arrS[5]);

        VBO attributesImgB[3];
        attributesImgB[0].insertdouble(arrS[4]);
        attributesImgB[0].insertdouble(arrS[5]);
        attributesImgB[1].insertdouble(arrS[6]);
        attributesImgB[1].insertdouble(arrS[7]);
        attributesImgB[2].insertdouble(arrS[0]);
        attributesImgB[2].insertdouble(arrS[1]);

        BufferImage testImg("sax.bmp");
        VBO uniformsImgA;
        VBO uniformsImgB;
        uniformsImgA.insertPointer((void*)&testImg);
        uniformsImgB.insertPointer((void*)&testImg);

        FragmentShader* fragImg = new FragmentShader(FragShader2DImage);
        VertexShader* vertImg = new VertexShader(VertexShaderProjection);

        /******************************************
         * Translation transform
         *****************************************/
        double offX = 0;
        double offY = 0;
        double offZ = 0;
        Transform trans = translate4x4(offX, offY, offZ);
        uniformsImgA.insertPointer((void*)&trans);
        uniformsImgB.insertPointer((void*)&trans);

        /******************************************
         * Camera View
         *****************************************/
        double yaw = evilYaw;
        double pitch = evilPitch;
        double roll = evilRoll;
        double xPos = evilX;
        double yPos = evilY;
        double zPos = evilZ;

        Transform cam = viewTransform4x4(yaw, pitch, roll, xPos, yPos, zPos);
        uniformsImgA.insertPointer((void*)&cam);
        uniformsImgB.insertPointer((void*)&cam);

        /******************************************
         * Perspective Projection transform
         *****************************************/
        double fovY = 60;
        double aspectRatio = 1.0;
        double near = 1;
        double far = 200;
        Transform perspective = perspectiveProjection4x4(fovY, aspectRatio, near, far);
        uniformsImgA.insertPointer((void*)&perspective);
        uniformsImgB.insertPointer((void*)&perspective);

        // Draw image triangle 
        DrawPrimitive(TRIANGLE, target, zBuf, verticesImgA, attributesImgA, &uniformsImgA, vertImg, fragImg);
        DrawPrimitive(TRIANGLE, target, zBuf, verticesImgB, attributesImgB, &uniformsImgB, vertImg, fragImg);
}



#endif