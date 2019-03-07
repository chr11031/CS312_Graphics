#include "definitions.h"
#include <iostream>

#ifndef COURSE_FUNCTIONS_H
#define COURSE_FUNCTIONS_H

using namespace std;

int x = 0;
int rng = 0;

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
               for (int wi = 0; wi < 64; wi++)
               {
                  for (int hi = 0; hi < 64; hi++)
                  {
                     int neighbors = 0;
                     
                     if (grid[wi] [hi])
                     {
                     
                     }
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
        //pointAttributes.color = color;       

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
   /***************************************************
    * I got rid of this function because it disagreed
    * with the new attributes class.
    ************************************************/  
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
   /****************************************************
    * 2. Interpolated image triangle
    ****************************************************/
   /***************************************************
    * I got rid of this function because it disagreed
    * with the new attributes class.
    ************************************************/   
}


/***********************************************
 * Demonstrate Fragment Shader, linear VBO 
 * interpolation for Project 03. 
 **********************************************/
void TestDrawStatic(Buffer2D<PIXEL> & target)
{
   /**************************************************
    * 1. Interpolated color triangle
    *************************************************/
   /***************************************************
    * I got rid of this function because it disagreed
    * with the new attributes class.
    ************************************************/     
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
   /***************************************************
    * I got rid of this function because it disagreed
    * with the new attributes class.
    ************************************************/   

   // This is for the random pizza image
   if (x == 0)
   {
      rng = rand() % 2;
   }
   
   //rng = 0;
   
   //rng = rand() % 2;

   if (rng == 0)
   {
      /***************************************************
       * I got rid of this function because it disagreed
       * with the new attributes class.
       ************************************************/      
   }
   else if (rng == 1)
   {
       /***************************************************
       * I got rid of this function because it disagreed
       * with the new attributes class.
       ************************************************/ 
   }
   else
   {
      /* code */
   }
   
   x = 100;
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
   /***************************************************
    * I got rid of this function because it disagreed
    * with the new attributes class.
    ************************************************/     
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
        // Ensure the checkboard image is in this directory, you can use another image though

        Attributes imageUniforms;
        //imageUniforms.insertPtr(&myImage); see below

        FragmentShader fragImg;
        fragImg.FragShader = &ImageFragShader;

        VertexShader vertImg;
        vertImg.VertShader = &SimpleVertexShader2;

        // Uniforms
        // [0] Image reference
        // [1] Model transformation
        // [2] View transformation
        // [3] Perspective transform

        Matrix model = translateMatrix(0, 0, 0);
        Matrix view  = viewTransform(myCam.x, myCam.y, myCam.z,
                                     myCam.yaw, myCam.pitch, myCam.roll);
        Matrix proj  = perspectiveTransform(60.0, 1.0, 1, 200); // FOV, Aspect ratio, Near, Far

        imageUniforms.insertPtr(&myImage);
        imageUniforms.insertPtr(&model);
        imageUniforms.insertPtr(&view);
        imageUniforms.insertPtr(&proj);
                
        // Draw image triangle 
        DrawPrimitive(TRIANGLE, target, verticesImgA, imageAttributesA, &imageUniforms, &fragImg, &vertImg, &zBuf);
        DrawPrimitive(TRIANGLE, target, verticesImgB, imageAttributesB, &imageUniforms, &fragImg, &vertImg, &zBuf);

        // NOTE: To test the Z-Buffer additinonal draw calls/geometry need to be called into this scene
}

#endif