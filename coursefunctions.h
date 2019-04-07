#include "definitions.h"
#include "shaders.h"

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
                int neighbors = 0;
                for(int y = 0; y < gridH; y++)
                {
                        for(int x = 0; x < gridW; x++)
                        {
                               // How many neighbors are there? 
                               if(gridTmp[y + 1][x] == 1) {
                                       neighbors++;
                               }
                               if(gridTmp[y - 1][x] == 1) {
                                       neighbors++;
                               }
                               if(gridTmp[y][x + 1] == 1) {
                                       neighbors++;
                               }
                               if(gridTmp[y][x - 1] == 1) {
                                       neighbors++;
                               }
                               if(gridTmp[y + 1][x + 1] == 1) {
                                       neighbors++;
                               }
                               if(gridTmp[y - 1][x + 1] == 1) {
                                       neighbors++;
                               }
                               if(gridTmp[y + 1][x - 1] == 1) {
                                       neighbors++;
                               }
                               if(gridTmp[y - 1][x - 1] == 1) {
                                       neighbors++;
                               }

                                // Apply rules
                                if(gridTmp[y][x] == 0) {
                                        if(neighbors == 3) {
                                                grid[y][x] = 1;
                                        } else {
                                                grid[y][x] = 0;
                                        }
                                } else {
                                        if(neighbors == 0 || neighbors == 1 || neighbors > 3) {
                                                grid[y][x] = 0;
                                        } else {
                                                grid[y][x] = 1;
                                        }
                                }

                                // Reset Neighbors for the next pixel
                                neighbors = 0;
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
        pointAttributes[1].d = 1; 

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
        PIXEL colors1[3] = {0xffff0000, 0xff00ff00, 0xff0000ff};
        //attr[0].color = colors1[0];
        //attr[1].color = colors1[1];
        //attr[2].color = colors1[2];

        DrawPrimitive(TRIANGLE, target, verts, attr);

        verts[0] = {300, 402, 1, 1};
        verts[1] = {250, 452, 1, 1};
        verts[2] = {250, 362, 1, 1};
        PIXEL colors2[3] = {0xff00ff00, 0xff00ff00, 0xff00ff00};
        //attr[0].color = colors2[0];
        //attr[1].color = colors2[1];
        //attr[2].color = colors2[2];

        DrawPrimitive(TRIANGLE, target, verts, attr);

        verts[0] = {450, 362, 1, 1};
        verts[1] = {450, 452, 1, 1};
        verts[2] = {350, 402, 1, 1};
        PIXEL colors3[3] = {0xff0000ff, 0xff0000ff, 0xff0000ff};
        //attr[0].color = colors3[0];
        //attr[1].color = colors3[1];
        //attr[2].color = colors3[2];

        DrawPrimitive(TRIANGLE, target, verts, attr);
        
        verts[0] = {110, 262, 1, 1};
        verts[1] = {60, 162, 1, 1};
        verts[2] = {150, 162, 1, 1};
        PIXEL colors4[3] = {0xffff0000, 0xffff0000, 0xffff0000};
        //attr[0].color = colors4[0];
        //attr[1].color = colors4[1];
        //attr[2].color = colors4[2];

        DrawPrimitive(TRIANGLE, target, verts, attr);

        verts[0] = {210, 252, 1, 1};
        verts[1] = {260, 172, 1, 1};
        verts[2] = {310, 202, 1, 1};
        PIXEL colors5[3] = {0xff00ff00, 0xff00ff00, 0xff00ff00};
        //attr[0].color = colors5[0];
        //attr[1].color = colors5[1];
        //attr[2].color = colors5[2];

        DrawPrimitive(TRIANGLE, target, verts, attr);
        
        verts[0] = {370, 202, 1, 1};
        verts[1] = {430, 162, 1, 1};
        verts[2] = {470, 252, 1, 1};
        PIXEL colors6[3] = {0xff0000ff, 0xff0000ff, 0xff0000ff};
        //attr[0].color = colors6[0];
        //attr[1].color = colors6[1];
        //attr[2].color = colors6[2];

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

        colorAttributes[0][1].d = 1.0;
        colorAttributes[0][2].d = 0.0;
        colorAttributes[0][3].d = 0.0;
        
        colorAttributes[1][1].d = 0.0;
        colorAttributes[1][2].d = 1.0;
        colorAttributes[1][3].d = 0.0;
        
        colorAttributes[2][1].d = 0.0;
        colorAttributes[2][2].d = 0.0;
        colorAttributes[2][3].d = 1.0;

        FragmentShader myColorFragShader;
        myColorFragShader.FragShader = ColorFragmentShader;

        Attributes colorUniforms;

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
        
        imageAttributes[0][0].d = 1;
        imageAttributes[0][1].d = 0;

        imageAttributes[1][0].d = 1;
        imageAttributes[1][1].d = 1;

        imageAttributes[2][0].d = 0;
        imageAttributes[2][1].d = 1;

        BufferImage myImage("checker.bmp");

        Attributes imageUniforms;
        imageUniforms[0].ptr = &myImage;

        FragmentShader myImageFragShader(ImageFragmentShader);
        myImageFragShader.FragShader = ImageFragmentShader;

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
        
        imageAttributesA[0].addDouble(coordinates[0][0]); 
        imageAttributesA[0].addDouble(coordinates[0][1]);
        imageAttributesA[1].addDouble(coordinates[1][0]);
        imageAttributesA[1].addDouble(coordinates[1][1]);
        imageAttributesA[2].addDouble(coordinates[2][0]);
        imageAttributesA[2].addDouble(coordinates[2][1]);

        imageAttributesB[0].addDouble(coordinates[2][0]);
        imageAttributesB[0].addDouble(coordinates[2][1]);
        imageAttributesB[1].addDouble(coordinates[3][0]);
        imageAttributesB[1].addDouble(coordinates[3][1]);
        imageAttributesB[2].addDouble(coordinates[0][0]);
        imageAttributesB[2].addDouble(coordinates[0][1]);

        BufferImage myImage("checker.bmp");

        Attributes imageUniforms;
        imageUniforms.addPtr(&myImage);

        FragmentShader fragImg;
        fragImg.FragShader = ImageFragmentShader;
                
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

        /*
        colorAttributes[0].doubles[1] = 1.0;
        colorAttributes[0].doubles[2] = 0.0;
        colorAttributes[0].doubles[3] = 0.0;
        
        colorAttributes[1].doubles[1] = 0.0;
        colorAttributes[1].doubles[2] = 1.0;
        colorAttributes[1].doubles[3] = 0.0;
        
        colorAttributes[2].doubles[1] = 0.0;
        colorAttributes[2].doubles[2] = 0.0;
        colorAttributes[2].doubles[3] = 1.0;        
*/

        FragmentShader myColorFragShader;
        myColorFragShader.FragShader = ColorFragmentShader;

        Attributes colorUniforms;

        VertexShader myColorVertexShader;
        myColorVertexShader.VertShader = TransformationVertexShader;

        /******************************************************************
	* TRANSLATE (move +100 in the X direction, +50 in the Y direction)
         *****************************************************************/
        TransformationMatrix* translate = new TransformationMatrix(TRANSLATE, 100, 50, 0);
        //colorUniforms.ptrs[0] = (void*)translate;

	DrawPrimitive(TRIANGLE, target, colorTriangle, colorAttributes, &colorUniforms, &myColorFragShader, &myColorVertexShader);

        /***********************************
         * SCALE (scale by a factor of 0.5)
         ***********************************/
        TransformationMatrix* scale = new TransformationMatrix(SCALE, 0.5, 0.5, 0.5);
        //colorUniforms.ptrs[0] = (void*)scale;

        DrawPrimitive(TRIANGLE, target, colorTriangle, colorAttributes, &colorUniforms, &myColorFragShader, &myColorVertexShader);

        /**********************************************
         * ROTATE 30 degrees in the X-Y plane around Z
         *********************************************/
        TransformationMatrix* rotate = new TransformationMatrix(ROTATE, 0, 0, 30);
        //colorUniforms.ptrs[0] = (void*)rotate;

        DrawPrimitive(TRIANGLE, target, colorTriangle, colorAttributes, &colorUniforms, &myColorFragShader, &myColorVertexShader);

        /*************************************************
         * SCALE-TRANSLATE-ROTATE in left-to-right order
         * the previous transformations concatenated. 
         ************************************************/
	TransformationMatrix* all = new TransformationMatrix();
        // Multiply in wanted order for final result
        (*all) = (*rotate) * (*translate) * (*scale);
        //colorUniforms.ptrs[0] = (void*)all;
		
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
        
        imageAttributesA[0].addDouble(coordinates[0][0]);
        imageAttributesA[0].addDouble(coordinates[0][1]);
        imageAttributesA[1].addDouble(coordinates[1][0]);
        imageAttributesA[1].addDouble(coordinates[1][1]);
        imageAttributesA[2].addDouble(coordinates[2][0]);
        imageAttributesA[2].addDouble(coordinates[2][1]);

        imageAttributesB[0].addDouble(coordinates[2][0]);
        imageAttributesB[0].addDouble(coordinates[2][1]);
        imageAttributesB[1].addDouble(coordinates[3][0]);
        imageAttributesB[1].addDouble(coordinates[3][1]);
        imageAttributesB[2].addDouble(coordinates[0][0]);
        imageAttributesB[2].addDouble(coordinates[0][1]);

        static BufferImage myImage("checker.bmp");
        Attributes imageUniforms;

        TransformationMatrix model;
        TransformationMatrix view;  
        view.toCameraMatrix(myCam.x, myCam.y, myCam.z, myCam.yaw, myCam.pitch, myCam.roll);
        TransformationMatrix proj;  
        proj.toPerspectiveMatrix(60.0, 1.0, 1, 200); //FOV, AspectRatio, Near, Far

        imageUniforms.addPtr((void*)&myImage);
        imageUniforms.addPtr((void*)&model);
        imageUniforms.addPtr((void*)&view);
        imageUniforms.addPtr((void*)&proj);

        FragmentShader fragImg;
        fragImg.FragShader = ImageFragmentShader;

        VertexShader vertImg;
        vertImg.VertShader = SimpleVertShader;
                
        // Draw image triangle 
        DrawPrimitive(TRIANGLE, target, verticesImgA, imageAttributesA, &imageUniforms, &fragImg, &vertImg, &zBuf);
        DrawPrimitive(TRIANGLE, target, verticesImgB, imageAttributesB, &imageUniforms, &fragImg, &vertImg, &zBuf);

        // NOTE: To test the Z-Buffer additinonal draw calls/geometry need to be called into this scene
}



#endif