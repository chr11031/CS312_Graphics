#include "definitions.h"
#include "shaders.h"
#include <string>
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
	
        //DrawPrimitive(POINT, target, &vert, &pointAttributes);
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
        verts[0] = (Vertex){100, 362, 1, 1};
        verts[1] = (Vertex){150, 452, 1, 1};
        verts[2] = (Vertex){50, 452, 1, 1};
        PIXEL colors1[3] = {0xffff0000, 0xffff0000, 0xffff0000};
	attr[0].numMembers = 0;
	attr[1].numMembers = 0;
	attr[2].numMembers = 0;
	attr[0].insertDbl(colors1[0]);
 	attr[1].insertDbl(colors1[1]);
	attr[2].insertDbl(colors1[2]);
        DrawPrimitive(TRIANGLE, target, verts, attr);

        verts[0] = (Vertex){300, 402, 1, 1};
        verts[1] = (Vertex){250, 452, 1, 1};
        verts[2] = (Vertex){250, 362, 1, 1};
        PIXEL colors2[3] = {0xffff0000, 0xffff0000, 0xffff0000};
	attr[0].numMembers = 0;
	attr[1].numMembers = 0;
	attr[2].numMembers = 0;
	attr[0].insertDbl(colors2[0]);
 	attr[1].insertDbl(colors2[1]);
	attr[2].insertDbl(colors2[2]);

        DrawPrimitive(TRIANGLE, target, verts, attr);

        verts[0] = (Vertex){450, 362, 1, 1};
        verts[1] = (Vertex){450, 452, 1, 1};
        verts[2] = (Vertex){350, 402, 1, 1};
        PIXEL colors3[3] = {0xff00ff00, 0xff00ff00, 0xff00ff00};
	attr[0].numMembers = 0;
	attr[1].numMembers = 0;
	attr[2].numMembers = 0;
	attr[0].insertDbl(colors3[0]);
 	attr[1].insertDbl(colors3[1]);
	attr[2].insertDbl(colors3[2]);

        DrawPrimitive(TRIANGLE, target, verts, attr);
        
        verts[0] = (Vertex){110, 262, 1, 1};
        verts[1] = (Vertex){60, 162, 1, 1};
        verts[2] = (Vertex){150, 162, 1, 1};
        PIXEL colors4[3] = {0xff00ff00, 0xff00ff00, 0xff00ff00};
	attr[0].numMembers = 0;
	attr[1].numMembers = 0;
	attr[2].numMembers = 0;
	attr[0].insertDbl(colors4[0]);
 	attr[1].insertDbl(colors4[1]);
	attr[2].insertDbl(colors4[2]);

        DrawPrimitive(TRIANGLE, target, verts, attr);

        verts[0] = (Vertex){210, 252, 1, 1};
        verts[1] = (Vertex){260, 172, 1, 1};
        verts[2] = (Vertex){310, 202, 1, 1};
        PIXEL colors5[3] = {0xff0000ff, 0xff0000ff, 0xff0000ff};
	attr[0].numMembers = 0;
	attr[1].numMembers = 0;
	attr[2].numMembers = 0;
	attr[0].insertDbl(colors5[0]);
 	attr[1].insertDbl(colors5[1]);
	attr[2].insertDbl(colors5[2]);

        DrawPrimitive(TRIANGLE, target, verts, attr);
        
        verts[0] = (Vertex){370, 202, 1, 1};
        verts[1] = (Vertex){430, 162, 1, 1};
        verts[2] = (Vertex){470, 252, 1, 1};
        PIXEL colors6[3] = {0xff0000ff, 0xff0000ff, 0xff0000ff};
	attr[0].numMembers = 0;
	attr[1].numMembers = 0;
	attr[2].numMembers = 0;
	attr[0].insertDbl(colors5[6]);
 	attr[1].insertDbl(colors5[6]);
	attr[2].insertDbl(colors5[6]);

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
        **************************************************/
        Vertex colorTriangle[3];
        Attributes colorAttributes[3];
        colorTriangle[0] = (Vertex){250, 112, 1, 1};
        colorTriangle[1] = (Vertex){450, 452, 1, 1};
        colorTriangle[2] = (Vertex){50, 452, 1, 1};
        PIXEL colors[3] = {0xffff0000, 0xff00ff00, 0xff0000ff}; // Or {{1.0,0.0,0.0}, {0.0,1.0,0.0}, {0.0,0.0,1.0}}
	colorAttributes[0].insertDbl(1.0);
	colorAttributes[0].insertDbl(0.0);
	colorAttributes[0].insertDbl(0.0);
	colorAttributes[1].insertDbl(0.0);
	colorAttributes[1].insertDbl(1.0);
	colorAttributes[1].insertDbl(0.0);
	colorAttributes[2].insertDbl(0.0);
	colorAttributes[2].insertDbl(0.0);
	colorAttributes[2].insertDbl(1.0);

        // Your color code goes here for 'colorAttributes'

        FragmentShader myColorFragShader;
	myColorFragShader.FragShader = ColorFragShader;

        Attributes colorUniforms;
	// Nothing gets setup this time

        DrawPrimitive(TRIANGLE, target, colorTriangle, colorAttributes, &colorUniforms, &myColorFragShader);

        /****************************************************
         * 2. Interpolated image triangle
        ****************************************************/
        Vertex imageTriangle[3];
        Attributes imageAttributes[3];
        imageTriangle[0] = (Vertex){425, 112, 1, 1};
        imageTriangle[1] = (Vertex){500, 252, 1, 1};
        imageTriangle[2] = (Vertex){350, 252, 1, 1};
        double coordinates[3][2] = { {1,0}, {1,1}, {0,1} };
        imageAttributes[0].insertDbl(coordinates[0][0]);
	imageAttributes[0].insertDbl(coordinates[0][1]);
	imageAttributes[1].insertDbl(coordinates[1][0]);
        imageAttributes[1].insertDbl(coordinates[1][1]);
	imageAttributes[2].insertDbl(coordinates[2][0]);
	imageAttributes[2].insertDbl(coordinates[2][1]);

        // Your texture coordinate code goes here for 'imageAttributes'

        static BufferImage myImage("sax.bmp");
        // Provide an image in this directory that you would like to use (powers of 2 dimensions)

        Attributes imageUniforms;
        imageUniforms.insertPtr(&myImage);

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

	
	// Setup Attributes for 'A'
	imageAttributesA[0].insertDbl(coordinates[0][0]);
	imageAttributesA[0].insertDbl(coordinates[0][1]);
	imageAttributesA[1].insertDbl(coordinates[1][0]);
	imageAttributesA[1].insertDbl(coordinates[1][1]);
	imageAttributesA[2].insertDbl(coordinates[2][0]);
	imageAttributesA[2].insertDbl(coordinates[2][1]);

	// Setup Attributes for 'B'
	imageAttributesB[0].insertDbl(coordinates[2][0]);
	imageAttributesB[0].insertDbl(coordinates[2][1]);
	imageAttributesB[1].insertDbl(coordinates[3][0]);
	imageAttributesB[1].insertDbl(coordinates[3][1]);
	imageAttributesB[2].insertDbl(coordinates[0][0]);
	imageAttributesB[2].insertDbl(coordinates[0][1]);



        static BufferImage myImage("checker.bmp");
        // Ensure the checkboard image is in this directory

        Attributes imageUniforms;
	imageUniforms.insertPtr(&myImage);

        FragmentShader fragImg;
        fragImg.FragShader = FragShaderUVwithoutImage;
	        
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
        colorTriangle[0] = (Vertex){ 350, 112, 1, 1};
        colorTriangle[1] = (Vertex){ 400, 200, 1, 1};
        colorTriangle[2] = (Vertex){ 300, 200, 1, 1};

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
        myColorFragShader.setShader(ColorFragShader);

        Attributes colorUniforms;
        // Your code for the uniform goes here, if any (don't pass NULL here)
        
        VertexShader myColorVertexShader(SimpleVertexShader);

        /******************************************************************
		* TRANSLATE (move +100 in the X direction, +50 in the Y direction)
         *****************************************************************/
        // Your translating code that integrates with 'colorUniforms', used by 'myColorVertexShader' goes here

		Transform trans = translate4x4(100, 50, 0);
		colorUniforms.insertPtr(&trans);
	
		DrawPrimitive(TRIANGLE, target, colorTriangle, colorAttributes, &colorUniforms, &myColorFragShader, &myColorVertexShader);

        /***********************************
         * SCALE (scale by a factor of 0.5)
         ***********************************/
        // Your scaling code that integrates with 'colorUniforms', used by 'myColorVertexShader' goes here

        Transform scale = scale4x4(0.5);
        colorUniforms[0].ptr = &scale;

        DrawPrimitive(TRIANGLE, target, colorTriangle, colorAttributes, &colorUniforms, &myColorFragShader, &myColorVertexShader);

        /**********************************************
         * ROTATE 30 degrees in the X-Y plane around Z
         *********************************************/
        // Your rotation code that integrates with 'colorUniforms', used by 'myColorVertexShader' goes here

        Transform rot = rotate4x4(Z, 30);
        colorUniforms[0].ptr = &rot;

        DrawPrimitive(TRIANGLE, target, colorTriangle, colorAttributes, &colorUniforms, &myColorFragShader, &myColorVertexShader);

        /*************************************************
         * SCALE-TRANSLATE-ROTATE in left-to-right order
         * the previous transformations concatenated.
         ************************************************/
		// Your scale-translate-rotation code that integrates with 'colorUniforms', used by 'myColorVertexShader' goes here

        Transform combined = rot * trans * scale;
        colorUniforms[0].ptr = &combined;		

        DrawPrimitive(TRIANGLE, target, colorTriangle, colorAttributes, &colorUniforms, &myColorFragShader, &myColorVertexShader);	
}

// Builds a square base off a vertex that is passed in and an image
void buildSquare(Buffer2D<PIXEL> & target, Vertex quad[], BufferImage *myImage)
{
        static Buffer2D<double> zBuf(target.width(), target.height());

        // Attributes for 1st tri
        Vertex verticesImgA[3];
        Attributes imageAttributesA[3];
        verticesImgA[0] = quad[0];
        verticesImgA[1] = quad[1];
        verticesImgA[2] = quad[2];

        // Attributes for 2nd tri
        Vertex verticesImgB[3];        
        Attributes imageAttributesB[3];
        verticesImgB[0] = quad[2];
        verticesImgB[1] = quad[3];
        verticesImgB[2] = quad[0];

        double coordinates[4][2] = { {0,0}, {1,0}, {1,1}, {0,1} };
        // Your texture coordinate code goes here for 'imageAttributesA, imageAttributesB'

        // First group of attributes 
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

        // MVP Transformations
        Transform model = translate4x4(0, 0, 0);
        Transform view = camera4x4(myCam.x, myCam.y, myCam.z, 
                                        myCam.yaw, myCam.pitch, myCam.roll);
        Transform proj = perspective4x4(60, 1.0, 1, 200);

        // Inserting the matrixies into the attributes
        Attributes imageUniforms;

        imageUniforms.insertPtr((void*)myImage);
        imageUniforms.insertPtr((void*)&model);
        imageUniforms.insertPtr((void*)&view);
        imageUniforms.insertPtr((void*)&proj);

        FragmentShader fragImg;
        fragImg.FragShader = ImageFragShader;

        VertexShader vertImg;
        vertImg.VertShader = SimpleVertexShader2;

        // Draw image triangle 
        DrawPrimitive(TRIANGLE, target, verticesImgA, imageAttributesA, &imageUniforms, &fragImg, &vertImg, &zBuf);
        DrawPrimitive(TRIANGLE, target, verticesImgB, imageAttributesB, &imageUniforms, &fragImg, &vertImg, &zBuf);

}

/************************************************
 * The personal project tests
 ***********************************************/
void dungeonCrawler(Buffer2D<PIXEL> & target)
{
        DMap map;
        bool direction[4]; //Use and array.  E, S, W, N

        static BufferImage walls("wall.bmp");
        static BufferImage floors("floor.bmp");

        int deg = ((int)abs(myCam.yaw) % 360);

        if (myCam.yaw < 0)
                deg = (360 - deg);

        //std::cout << "Yaw = " << deg << std::endl;

        // Renders the world based off of the map
        for (int r = 0; r < map.rows; r++)
        {
                for (int c = 0; c < map.cols; c++)
                {
                        if (map.map[c][r] == 1)
                        {
                                // Floor coord
                                Vertex floor[] = { {-15 - (30 * map.getDistX(r)), -20, -15 + (30 * map.getDistY(c)), 1},   // Q3, Left  Bot
                                                   { 15 - (30 * map.getDistX(r)), -20, -15 + (30 * map.getDistY(c)), 1},   // Q4, Right Bot
                                                   { 15 - (30 * map.getDistX(r)), -20,  15 + (30 * map.getDistY(c)), 1},   // Q1, Right Top
                                                   {-15 - (30 * map.getDistX(r)), -20,  15 + (30 * map.getDistY(c)), 1}};  // Q4, Left  Top
                                // Roof coord
                                Vertex roof[] = {  {-15 - (30 * map.getDistX(r)), 10,  15 + (30 * map.getDistY(c)), 1},   // Q3, Left  Bot
                                                   { 15 - (30 * map.getDistX(r)), 10,  15 + (30 * map.getDistY(c)), 1},   // Q4, Right Bot
                                                   { 15 - (30 * map.getDistX(r)), 10, -15 + (30 * map.getDistY(c)), 1},   // Q1, Right Top
                                                   {-15 - (30 * map.getDistX(r)), 10, -15 + (30 * map.getDistY(c)), 1}};  // Q4, Left  Top


                                // Get wall stuff working
                                map.isEdge(c, r, direction);
                                map.checkWalls(c, r, direction);

                                // Builds the walls based off the direction
                                for (int i = 0; i < 4; i++)
                                        if(direction[i])
                                        {
                                                if (i == 0)
                                                {
                                                        // E-Wall Vertcies
                                                        Vertex wallE[] = { { 15 - (30 * map.getDistX(r)), -20,  15 + (30 * map.getDistY(c)), 1},   // Q3, Left  Bot
                                                                           { 15 - (30 * map.getDistX(r)), -20, -15 + (30 * map.getDistY(c)), 1},   // Q4, Right Bot
                                                                           { 15 - (30 * map.getDistX(r)),  10, -15 + (30 * map.getDistY(c)), 1},   // Q1, Right Top
                                                                           { 15 - (30 * map.getDistX(r)),  10,  15 + (30 * map.getDistY(c)), 1}};  // Q4, Left  Top
                                                        buildSquare(target, wallE, &walls);
                                                }
                                                else if (i == 1)
                                                {
                                                        // S-Wall Vertcies
                                                        Vertex wallS[] = { { 15 - (30 * map.getDistX(r)), -20, -15 + (30 * map.getDistY(c)), 1},   // Q3, Left  Bot
                                                                           {-15 - (30 * map.getDistX(r)), -20, -15 + (30 * map.getDistY(c)), 1},   // Q4, Right Bot
                                                                           {-15 - (30 * map.getDistX(r)),  10, -15 + (30 * map.getDistY(c)), 1},   // Q1, Right Top
                                                                           { 15 - (30 * map.getDistX(r)),  10, -15 + (30 * map.getDistY(c)), 1}};  // Q4, Left  Top
                                                        buildSquare(target, wallS, &walls);
                                                }
                                                else if (i == 2)
                                                {
                                                        // W-Wall Vertcies
                                                        Vertex wallW[] = { {-15 - (30 * map.getDistX(r)), -20, -15 + (30 * map.getDistY(c)), 1},   // Q3, Left  Bot
                                                                           {-15 - (30 * map.getDistX(r)), -20,  15 + (30 * map.getDistY(c)), 1},   // Q4, Right Bot
                                                                           {-15 - (30 * map.getDistX(r)),  10,  15 + (30 * map.getDistY(c)), 1},   // Q1, Right Top
                                                                           {-15 - (30 * map.getDistX(r)),  10, -15 + (30 * map.getDistY(c)), 1}};  // Q4, Left  Top
                                                        buildSquare(target, wallW, &walls);
                                                }
                                                
                                                else if (i == 3)
                                                {
                                                        // N-Wall Vertcies
                                                        Vertex wallN[] = { {-15 - (30 * map.getDistX(r)), -20,  15 + (30 * map.getDistY(c)), 1},   // Q3, Left  Bot
                                                                           { 15 - (30 * map.getDistX(r)), -20,  15 + (30 * map.getDistY(c)), 1},   // Q4, Right Bot
                                                                           { 15 - (30 * map.getDistX(r)),  10,  15 + (30 * map.getDistY(c)), 1},   // Q1, Right Top
                                                                           {-15 - (30 * map.getDistX(r)),  10,  15 + (30 * map.getDistY(c)), 1}};  // Q4, Left  Top
                                                        buildSquare(target, wallN, &walls);
                                                }
                                        }


                                // Builds the floor
                                buildSquare(target, floor, &floors);

                                // Builds the roof
                                buildSquare(target, roof, &floors);
                        }
                }
        }

        


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
        imageAttributesA[0].insertDbl(coordinates[0][0]); // First group of attributes 
        imageAttributesA[0].insertDbl(coordinates[0][1]);
        imageAttributesA[1].insertDbl(coordinates[1][0]);
        imageAttributesA[1].insertDbl(coordinates[1][1]);
        imageAttributesA[2].insertDbl(coordinates[2][0]);
        imageAttributesA[2].insertDbl(coordinates[2][1]);
        imageAttributesB[0].insertDbl(coordinates[2][0]); // Second group of attributes
        imageAttributesB[0].insertDbl(coordinates[2][1]);
        imageAttributesB[1].insertDbl(coordinates[3][0]);
        imageAttributesB[1].insertDbl(coordinates[3][1]);
        imageAttributesB[2].insertDbl(coordinates[0][0]);
        imageAttributesB[2].insertDbl(coordinates[0][1]);
                        
        static BufferImage myImage("checker.bmp");
        Attributes imageUniforms;

        Transform model = translate4x4(0, 0, 0);
        Transform view = camera4x4(myCam.x, myCam.y, myCam.z, 
                                        myCam.yaw, myCam.pitch, myCam.roll);
        Transform proj = perspective4x4(60, 1.0, 1, 200);

        // Uniforms
        // [0] -> Image reference
        // [1] -> Model transform 
        // [2] -> View transform 
        
        imageUniforms.insertPtr((void*)&myImage);
        imageUniforms.insertPtr((void*)&model);
        imageUniforms.insertPtr((void*)&view);
        imageUniforms.insertPtr((void*)&proj);

        FragmentShader fragImg;
        fragImg.FragShader = FragShaderUVwithoutImage;

        VertexShader vertImg;
        vertImg.VertShader = SimpleVertexShader2;
		
		
                
        // Draw image triangle 
        DrawPrimitive(TRIANGLE, target, verticesImgA, imageAttributesA, &imageUniforms, &fragImg, &vertImg, &zBuf);
        DrawPrimitive(TRIANGLE, target, verticesImgB, imageAttributesB, &imageUniforms, &fragImg, &vertImg, &zBuf);

        // NOTE: To test the Z-Buffer additinonal draw calls/geometry need to be called into this scene
}



#endif