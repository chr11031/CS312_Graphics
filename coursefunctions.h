#include "definitions.h"
#include "shaders.h"

#ifndef COURSE_FUNCTIONS_H
#define COURSE_FUNCTIONS_H


/***************************************************
 * Checks whether the number is within the grid
 * bounndaries
 **************************************************/
bool isLegal(int num) {
        return (num >= 0 && num < 64);     
}

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
                        int gridY = gridH - mouseY / scaleFactor;
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
                for (int x = 0; x < gridW; x++) {
                        for (int y = 0; y < gridH; y++) {

                                int aliveNeighbors = 0;
                                for (int tmpx = x-1; tmpx <= x+1; tmpx++) {
                                        for (int tmpy = y-1; tmpy <= y+1; tmpy++) {
                                                if (!(tmpx == x && tmpy == y) // Don't count yourself as a neighbor
                                                && (tmpx >= 0 && tmpx < gridW && tmpy >= 0 && tmpy < gridH) // Don't go off the edge of the array
                                                && grid[tmpx][tmpy]) { // Is the neighbor cell alive?
                                                                aliveNeighbors++;
                                                }
                                        }
                                }

                                if (grid[x][y]) {
                                        // This cell is alive
                                        gridTmp[x][y] = (!(aliveNeighbors < 2 || aliveNeighbors > 3));
                                } else {
                                        // This cell is dead
                                        gridTmp[x][y] = (aliveNeighbors == 3);
                                }
                        }
                }

                // Copy to the grid
                for (int x = 0; x < gridW; x++) {
                        for (int y = 0; y < gridH; y++) {
                                grid[x][y] = gridTmp[x][y];
                        }
                }

                // Wait a 10th of a second between iterations
                SDL_Delay(100);
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
void testBox1Reg(Buffer2D<PIXEL> & target);
void testBox2Right(Buffer2D<PIXEL> & target);
void testBox3Top(Buffer2D<PIXEL> & target);
void testBox4Left(Buffer2D<PIXEL> & target);

void CADView(Buffer2D<PIXEL> & target)
{
        // Each CAD Quadrant
        static int halfWid = target.width()/2;
        static int halfHgt = target.height()/2;
        static Buffer2D<PIXEL> topLeft(halfWid, halfHgt);
        static Buffer2D<PIXEL> topRight(halfWid, halfHgt);
        static Buffer2D<PIXEL> botLeft(halfWid, halfHgt);
        static Buffer2D<PIXEL> botRight(halfWid, halfHgt);

        topLeft.zeroOut();
        topRight.zeroOut();
        botLeft.zeroOut();
        botRight.zeroOut();

        // Your code goes here 
        // Feel free to copy from other test functions to get started!

        testBox1Reg(topLeft);
        testBox2Right(topRight);
        testBox4Left(botLeft);
        testBox3Top(botRight);

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
        
      //testBox3Top(target);

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
        verts[0] = {100, 362, 1, 1};
        verts[1] = {150, 452, 1, 1};
        verts[2] = {50, 452, 1, 1};
        PIXEL colors1[3] = {0xffff0000, 0xffff0000, 0xffff0000};
	attr[0].numMembers = 0;
	attr[1].numMembers = 0;
	attr[2].numMembers = 0;
	attr[0].insertDbl(colors1[0]);
 	attr[1].insertDbl(colors1[1]);
	attr[2].insertDbl(colors1[2]);
        DrawPrimitive(TRIANGLE, target, verts, attr);

        verts[0] = {300, 402, 1, 1};
        verts[1] = {250, 452, 1, 1};
        verts[2] = {250, 362, 1, 1};
        PIXEL colors2[3] = {0xffff0000, 0xffff0000, 0xffff0000};
	attr[0].numMembers = 0;
	attr[1].numMembers = 0;
	attr[2].numMembers = 0;
	attr[0].insertDbl(colors2[0]);
 	attr[1].insertDbl(colors2[1]);
	attr[2].insertDbl(colors2[2]);

        DrawPrimitive(TRIANGLE, target, verts, attr);

        verts[0] = {450, 362, 1, 1};
        verts[1] = {450, 452, 1, 1};
        verts[2] = {350, 402, 1, 1};
        PIXEL colors3[3] = {0xff00ff00, 0xff00ff00, 0xff00ff00};
	attr[0].numMembers = 0;
	attr[1].numMembers = 0;
	attr[2].numMembers = 0;
	attr[0].insertDbl(colors3[0]);
 	attr[1].insertDbl(colors3[1]);
	attr[2].insertDbl(colors3[2]);

        DrawPrimitive(TRIANGLE, target, verts, attr);
        
        verts[0] = {110, 262, 1, 1};
        verts[1] = {60, 162, 1, 1};
        verts[2] = {150, 162, 1, 1};
        PIXEL colors4[3] = {0xff00ff00, 0xff00ff00, 0xff00ff00};
	attr[0].numMembers = 0;
	attr[1].numMembers = 0;
	attr[2].numMembers = 0;
	attr[0].insertDbl(colors4[0]);
 	attr[1].insertDbl(colors4[1]);
	attr[2].insertDbl(colors4[2]);

        DrawPrimitive(TRIANGLE, target, verts, attr);

        verts[0] = {210, 252, 1, 1};
        verts[1] = {260, 172, 1, 1};
        verts[2] = {310, 202, 1, 1};
        PIXEL colors5[3] = {0xff0000ff, 0xff0000ff, 0xff0000ff};
	attr[0].numMembers = 0;
	attr[1].numMembers = 0;
	attr[2].numMembers = 0;
	attr[0].insertDbl(colors5[0]);
 	attr[1].insertDbl(colors5[1]);
	attr[2].insertDbl(colors5[2]);

        DrawPrimitive(TRIANGLE, target, verts, attr);
        
        verts[0] = {370, 202, 1, 1};
        verts[1] = {430, 162, 1, 1};
        verts[2] = {470, 252, 1, 1};
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
        colorTriangle[0] = {250, 112, 1, 1};
        colorTriangle[1] = {450, 452, 1, 1};
        colorTriangle[2] = {50, 452, 1, 1};
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
        imageTriangle[0] = {425, 112, 1, 1};
        imageTriangle[1] = {500, 252, 1, 1};
        imageTriangle[2] = {350, 252, 1, 1};
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
        fragImg.FragShader = ImageFragShader;
	        
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

/********************************************
 * This is my personal Project it will use the 
 *  whole pipeline and will make a box that will
 *  change color is the end goal
 *******************************************/
void personalProject(Buffer2D<PIXEL> & target, int & counter)
{       
        static Buffer2D<double> zBuf(target.width(), target.height());
        // Will need to be cleared every frame, like the screen

        /**************************************************
        * 1. Image quad (2 TRIs) Code (texture interpolated)
        **************************************************/
        Vertex quad3D[] = { {-20,-20, 50, 1}, //0
                          {20, -20, 50, 1}, //1
                          {20,  20, 50, 1}, //2
                          {-20, 20, 50, 1}, //3
                          {-20,-20, 150, 1}, //4
                          {20, -20, 150, 1}, //5
                          {20,  20, 150, 1}, //6
                          {-20, 20, 150, 1}}; //7

        Vertex verticesImg1A[3];
        verticesImg1A[0] = quad3D[0];
        verticesImg1A[1] = quad3D[1];
        verticesImg1A[2] = quad3D[2];

        Vertex verticesImg2A[3];        
        verticesImg2A[0] = quad3D[2];
        verticesImg2A[1] = quad3D[3];
        verticesImg2A[2] = quad3D[0];

        Vertex verticesImg1B[3];
        verticesImg1B[0] = quad3D[3];
        verticesImg1B[1] = quad3D[2];
        verticesImg1B[2] = quad3D[7];

        Vertex verticesImg2B[3];
        verticesImg2B[0] = quad3D[7];
        verticesImg2B[1] = quad3D[6];
        verticesImg2B[2] = quad3D[3];

        Vertex verticesImg1C[3];
        verticesImg1C[0] = quad3D[6];
        verticesImg1C[1] = quad3D[7];
        verticesImg1C[2] = quad3D[4];

        Vertex verticesImg2C[3];
        verticesImg2C[0] = quad3D[4];
        verticesImg2C[1] = quad3D[5];
        verticesImg2C[2] = quad3D[6];

        Vertex verticesImg1D[3];
        verticesImg1D[0] = quad3D[5];
        verticesImg1D[1] = quad3D[4];
        verticesImg1D[2] = quad3D[1];

        Vertex verticesImg2D[3];
        verticesImg2D[0] = quad3D[1];
        verticesImg2D[1] = quad3D[0];
        verticesImg2D[2] = quad3D[5];

        Vertex verticesImg1E[3];
        verticesImg1E[0] = quad3D[4];
        verticesImg1E[1] = quad3D[7];
        verticesImg1E[2] = quad3D[2];

        Vertex verticesImg2E[3];
        verticesImg2E[0] = quad3D[2];
        verticesImg2E[1] = quad3D[1];
        verticesImg2E[2] = quad3D[4];

        Vertex verticesImg1F[3];
        verticesImg1F[0] = quad3D[6];
        verticesImg1F[1] = quad3D[5];
        verticesImg1F[2] = quad3D[0];

        Vertex verticesImg2F[3];
        verticesImg2F[0] = quad3D[0];
        verticesImg2F[1] = quad3D[3];
        verticesImg2F[2] = quad3D[6];

        Attributes imageAttributesA[3];
        Attributes imageAttributesB[3];
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

                switch (counter)
                {
                        case 0:
                                fragImg.FragShader = ImagePersonalProjectShader; 
                                break;
                        case 1:
                                fragImg.FragShader = ImagePersonalProjectShader1; 
                                break;
                        case 2:
                                fragImg.FragShader = ImagePersonalProjectShader2;
                                break;
                        default:
                                fragImg.FragShader = ImagePersonalProjectShader; 
                                counter = 0;
                                break;
                }
                
                     
        VertexShader vertImg;
		vertImg.VertShader = SimpleVertexShader2;
		       
    
        // Draw image triangle 
        DrawPrimitive(TRIANGLE, target, verticesImg1A, imageAttributesA, &imageUniforms, &fragImg, &vertImg, &zBuf);
        DrawPrimitive(TRIANGLE, target, verticesImg2A, imageAttributesB, &imageUniforms, &fragImg, &vertImg, &zBuf);
        DrawPrimitive(TRIANGLE, target, verticesImg1B, imageAttributesA, &imageUniforms, &fragImg, &vertImg, &zBuf);
        //DrawPrimitive(TRIANGLE, target, verticesImg2B, imageAttributesB, &imageUniforms, &fragImg, &vertImg, &zBuf);
        //DrawPrimitive(TRIANGLE, target, verticesImg1C, imageAttributesA, &imageUniforms, &fragImg, &vertImg, &zBuf);
        //DrawPrimitive(TRIANGLE, target, verticesImg2C, imageAttributesB, &imageUniforms, &fragImg, &vertImg, &zBuf);

        //DrawPrimitive(TRIANGLE, target, verticesImg1D, imageAttributesA, &imageUniforms, &fragImg, &vertImg, &zBuf);
        DrawPrimitive(TRIANGLE, target, verticesImg2D, imageAttributesB, &imageUniforms, &fragImg, &vertImg, &zBuf);
        DrawPrimitive(TRIANGLE, target, verticesImg1E, imageAttributesA, &imageUniforms, &fragImg, &vertImg, &zBuf);
        DrawPrimitive(TRIANGLE, target, verticesImg2E, imageAttributesB, &imageUniforms, &fragImg, &vertImg, &zBuf);
        DrawPrimitive(TRIANGLE, target, verticesImg1F, imageAttributesA, &imageUniforms, &fragImg, &vertImg, &zBuf);
        DrawPrimitive(TRIANGLE, target, verticesImg2F, imageAttributesB, &imageUniforms, &fragImg, &vertImg, &zBuf);
      

}

void personalProject1(Buffer2D<PIXEL> & target, int & counter)
{       
        static Buffer2D<double> zBuf(target.width(), target.height());
        // Will need to be cleared every frame, like the screen

        /**************************************************
        * 1. Image quad (2 TRIs) Code (texture interpolated)
        **************************************************/
        Vertex quad3D[] = { {-20,-20, 50, 1}, //0
                          {20, -20, 50, 1}, //1
                          {20,  20, 50, 1}, //2
                          {-20, 20, 50, 1}, //3
                          {-20,-20, 150, 1}, //4
                          {20, -20, 150, 1}, //5
                          {20,  20, 150, 1}, //6
                          {-20, 20, 150, 1}}; //7

        Vertex verticesImg1A[3];
        verticesImg1A[0] = quad3D[0];
        verticesImg1A[1] = quad3D[1];
        verticesImg1A[2] = quad3D[2];

        Vertex verticesImg2A[3];        
        verticesImg2A[0] = quad3D[2];
        verticesImg2A[1] = quad3D[3];
        verticesImg2A[2] = quad3D[0];

        Vertex verticesImg1B[3];
        verticesImg1B[0] = quad3D[3];
        verticesImg1B[1] = quad3D[2];
        verticesImg1B[2] = quad3D[7];

        Vertex verticesImg2B[3];
        verticesImg2B[0] = quad3D[7];
        verticesImg2B[1] = quad3D[6];
        verticesImg2B[2] = quad3D[3];

        Vertex verticesImg1C[3];
        verticesImg1C[0] = quad3D[6];
        verticesImg1C[1] = quad3D[7];
        verticesImg1C[2] = quad3D[4];

        Vertex verticesImg2C[3];
        verticesImg2C[0] = quad3D[4];
        verticesImg2C[1] = quad3D[5];
        verticesImg2C[2] = quad3D[6];

        Vertex verticesImg1D[3];
        verticesImg1D[0] = quad3D[5];
        verticesImg1D[1] = quad3D[4];
        verticesImg1D[2] = quad3D[1];

        Vertex verticesImg2D[3];
        verticesImg2D[0] = quad3D[1];
        verticesImg2D[1] = quad3D[0];
        verticesImg2D[2] = quad3D[5];

        Vertex verticesImg1E[3];
        verticesImg1E[0] = quad3D[4];
        verticesImg1E[1] = quad3D[7];
        verticesImg1E[2] = quad3D[2];

        Vertex verticesImg2E[3];
        verticesImg2E[0] = quad3D[2];
        verticesImg2E[1] = quad3D[1];
        verticesImg2E[2] = quad3D[4];

        Vertex verticesImg1F[3];
        verticesImg1F[0] = quad3D[6];
        verticesImg1F[1] = quad3D[5];
        verticesImg1F[2] = quad3D[0];

        Vertex verticesImg2F[3];
        verticesImg2F[0] = quad3D[0];
        verticesImg2F[1] = quad3D[3];
        verticesImg2F[2] = quad3D[6];

        Attributes imageAttributesA[3];
        Attributes imageAttributesB[3];
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
                double NEAR = 1; double FAR = 200;
                double LEFT = -60; double RIGHT = 60;
                double BOTTOM = -60; double TOP = 60;

                myOr.x = 0;
                myOr.y = 0;
                myOr.z = 100;
                myOr.yaw = 30;
                myOr.pitch = -60;
                myOr.roll = -30;
                
		Transform view = camera4x4(myOr.x, myOr.y, myOr.z, myOr.yaw, myOr.pitch, myOr.roll);
		Transform proj = orthgraphic(RIGHT, LEFT, TOP, BOTTOM, NEAR, FAR);

		// Uniforms
		// [0] -> Image reference
		// [1] -> Model transform 
		// [2] -> View transform 
		
		imageUniforms.insertPtr((void*)&myImage);
		imageUniforms.insertPtr((void*)&model);
		imageUniforms.insertPtr((void*)&view);
		imageUniforms.insertPtr((void*)&proj);

        FragmentShader fragImg;

                switch (counter)
                {
                        case 0:
                                fragImg.FragShader = ImagePersonalProjectShader; 
                                break;
                        case 1:
                                fragImg.FragShader = ImagePersonalProjectShader1; 
                                break;
                        case 2:
                                fragImg.FragShader = ImagePersonalProjectShader2;
                                break;
                        default:
                                fragImg.FragShader = ImagePersonalProjectShader; 
                                counter = 0;
                                break;
                }
                
                     
        VertexShader vertImg;
		vertImg.VertShader = SimpleVertexShader2;
		       
    
        // Draw image triangle 
        DrawPrimitive(TRIANGLE, target, verticesImg1A, imageAttributesA, &imageUniforms, &fragImg, &vertImg, &zBuf);
        DrawPrimitive(TRIANGLE, target, verticesImg2A, imageAttributesB, &imageUniforms, &fragImg, &vertImg, &zBuf);
        DrawPrimitive(TRIANGLE, target, verticesImg1B, imageAttributesA, &imageUniforms, &fragImg, &vertImg, &zBuf);
        //DrawPrimitive(TRIANGLE, target, verticesImg2B, imageAttributesB, &imageUniforms, &fragImg, &vertImg, &zBuf);
        //DrawPrimitive(TRIANGLE, target, verticesImg1C, imageAttributesA, &imageUniforms, &fragImg, &vertImg, &zBuf);
        //DrawPrimitive(TRIANGLE, target, verticesImg2C, imageAttributesB, &imageUniforms, &fragImg, &vertImg, &zBuf);

        //DrawPrimitive(TRIANGLE, target, verticesImg1D, imageAttributesA, &imageUniforms, &fragImg, &vertImg, &zBuf);
        DrawPrimitive(TRIANGLE, target, verticesImg2D, imageAttributesB, &imageUniforms, &fragImg, &vertImg, &zBuf);
        DrawPrimitive(TRIANGLE, target, verticesImg1E, imageAttributesA, &imageUniforms, &fragImg, &vertImg, &zBuf);
        DrawPrimitive(TRIANGLE, target, verticesImg2E, imageAttributesB, &imageUniforms, &fragImg, &vertImg, &zBuf);
        DrawPrimitive(TRIANGLE, target, verticesImg1F, imageAttributesA, &imageUniforms, &fragImg, &vertImg, &zBuf);
        DrawPrimitive(TRIANGLE, target, verticesImg2F, imageAttributesB, &imageUniforms, &fragImg, &vertImg, &zBuf);
      

        // NOTE: To test the Z-Buffer additinonal draw calls/geometry need to be called into this scene
}

void personalProject2(Buffer2D<PIXEL> & target, int & picture)
{       
        static Buffer2D<double> zBuf(target.width(), target.height());
        // Will need to be cleared every frame, like the screen

        /**************************************************
        * 1. Image quad (2 TRIs) Code (texture interpolated)
        **************************************************/
       Vertex quad[] = { {-20,-20, 50, 1},
                          {20, -20, 50, 1},
                          {20, 20, 50, 1},
                          {-20,20, 50, 1},
                          {-20,-20, 150, 1},
                          {20, -20, 150, 1},
                          {20, 20, 150, 1},
                          {-20,20, 150, 1}};


        Vertex verticesImgA[3];
        verticesImgA[0] = quad[0];
        verticesImgA[1] = quad[1];
        verticesImgA[2] = quad[2];

        Vertex verticesImgA1[3];        
        verticesImgA1[0] = quad[2];
        verticesImgA1[1] = quad[3];
        verticesImgA1[2] = quad[0];

        


        Attributes imageAttributesA[3];
        Attributes imageAttributesB[3];
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
				
		static BufferImage myImage("C:\\Users\\Brooke\\Documents\\School\\cs312\\CS312_Graphics\\ball1.bmp");
                static BufferImage myImage1("C:\\Users\\Brooke\\Documents\\School\\cs312\\CS312_Graphics\\ball2.bmp");
                static BufferImage myImage2("C:\\Users\\Brooke\\Documents\\School\\cs312\\CS312_Graphics\\ball3.bmp");
                static BufferImage myImage3("C:\\Users\\Brooke\\Documents\\School\\cs312\\CS312_Graphics\\pika.bmp");
                static BufferImage myImage4("C:\\Users\\Brooke\\Documents\\School\\cs312\\CS312_Graphics\\theEnd.bmp");
		Attributes imageUniforms;

		Transform model = translate4x4(0, 0, 0);
		Transform view = camera4x4(myCam.x, myCam.y, myCam.z, 
					   myCam.yaw, myCam.pitch, myCam.roll);
		Transform proj = perspective4x4(60, 1.0, 1, 200);

		// Uniforms
		// [0] -> Image reference
		// [1] -> Model transform 
		// [2] -> View transform 
		switch (picture)
                {
                        case 0:
                                imageUniforms.insertPtr((void*)&myImage);
                                break;
                        case 1:
                                imageUniforms.insertPtr((void*)&myImage1);
                                break;
                        case 2:
                                imageUniforms.insertPtr((void*)&myImage2); 
                                break;
                        case 3:
                                imageUniforms.insertPtr((void*)&myImage3);
                                break;
                        case 4:
                                imageUniforms.insertPtr((void*)&myImage4);
                                break;
                        default:
                                imageUniforms.insertPtr((void*)&myImage);
                                picture = 0;
                                break;
                }    

		imageUniforms.insertPtr((void*)&model);
		imageUniforms.insertPtr((void*)&view);
		imageUniforms.insertPtr((void*)&proj);

        FragmentShader fragImg;
               fragImg.FragShader = ImageFragShader;
                     
        VertexShader vertImg;
		vertImg.VertShader = SimpleVertexShader2;
		       
        // Draw image triangle 
        DrawPrimitive(TRIANGLE, target, verticesImgA, imageAttributesA, &imageUniforms, &fragImg, &vertImg, &zBuf);
        DrawPrimitive(TRIANGLE, target, verticesImgA1, imageAttributesB, &imageUniforms, &fragImg, &vertImg, &zBuf);
        // NOTE: To test the Z-Buffer additinonal draw calls/geometry need to be called into this scene
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
		fragImg.FragShader = ImageFragShader;

        VertexShader vertImg;
		vertImg.VertShader = SimpleVertexShader2;
		       
        // Draw image triangle 
        DrawPrimitive(TRIANGLE, target, verticesImgA, imageAttributesA, &imageUniforms, &fragImg, &vertImg, &zBuf);
        DrawPrimitive(TRIANGLE, target, verticesImgB, imageAttributesB, &imageUniforms, &fragImg, &vertImg, &zBuf);

        // NOTE: To test the Z-Buffer additinonal draw calls/geometry need to be called into this scene
}

/********************************************
 * Verify that the whole pipeline works. By
 * the end of week 07 you should be able to
 * run this code successfully.
 *******************************************/
void testBox1Reg(Buffer2D<PIXEL> & target)
{
        
        static Buffer2D<double> zBuf(target.width(), target.height());
        // Will need to be cleared every frame, like the screen

        /**************************************************
        * 1. 3D
        **************************************************/
        Vertex quad3D[] = { {-20,-20, 50, 1},
                          {20, -20, 50, 1},
                          {20, 20, 50, 1},
                          {-20,20, 50, 1},
                          {-20,-20, 150, 1},
                          {20, -20, 150, 1},
                          {20, 20, 150, 1},
                          {-20,20, 150, 1}};

        Vertex verticesImg1A[3];
        Attributes imageAttributesA[3];
        verticesImg1A[0] = quad3D[0];
        verticesImg1A[1] = quad3D[1];
        verticesImg1A[2] = quad3D[2];

        Vertex verticesImg2A[3];        
        Attributes imageAttributesB[3];
        verticesImg2A[0] = quad3D[2];
        verticesImg2A[1] = quad3D[3];
        verticesImg2A[2] = quad3D[0];

        Vertex verticesImg1B[3];
        verticesImg1B[0] = quad3D[3];
        verticesImg1B[1] = quad3D[2];
        verticesImg1B[2] = quad3D[7];

        Vertex verticesImg2B[3];
        verticesImg2B[0] = quad3D[7];
        verticesImg2B[1] = quad3D[6];
        verticesImg2B[2] = quad3D[3];

        Vertex verticesImg1C[3];
        verticesImg1C[0] = quad3D[6];
        verticesImg1C[1] = quad3D[7];
        verticesImg1C[2] = quad3D[4];

        Vertex verticesImg2C[3];
        verticesImg2C[0] = quad3D[4];
        verticesImg2C[1] = quad3D[5];
        verticesImg2C[2] = quad3D[6];

        Vertex verticesImg1D[3];
        verticesImg1D[0] = quad3D[5];
        verticesImg1D[1] = quad3D[4];
        verticesImg1D[2] = quad3D[1];

        Vertex verticesImg2D[3];
        verticesImg2D[0] = quad3D[1];
        verticesImg2D[1] = quad3D[0];
        verticesImg2D[2] = quad3D[5];

        Vertex verticesImg1E[3];
        verticesImg1E[0] = quad3D[4];
        verticesImg1E[1] = quad3D[7];
        verticesImg1E[2] = quad3D[2];

        Vertex verticesImg2E[3];
        verticesImg2E[0] = quad3D[2];
        verticesImg2E[1] = quad3D[1];
        verticesImg2E[2] = quad3D[4];

        Vertex verticesImg1F[3];
        verticesImg1F[0] = quad3D[6];
        verticesImg1F[1] = quad3D[5];
        verticesImg1F[2] = quad3D[0];

        Vertex verticesImg2F[3];
        verticesImg2F[0] = quad3D[0];
        verticesImg2F[1] = quad3D[3];
        verticesImg2F[2] = quad3D[6];



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
                double NEAR = 1; double FAR = 200;
                double LEFT = -60; double RIGHT = 60;
                double BOTTOM = -60; double TOP = 60;

                myOr.x = 0;
                myOr.y = 0;
                myOr.z = 100;
                myOr.yaw = 30;
                myOr.pitch = -60;
                myOr.roll = -30;
                
		Transform view = camera4x4(myOr.x, myOr.y, myOr.z, myOr.yaw, myOr.pitch, myOr.roll);
		Transform proj = orthgraphic(RIGHT, LEFT, TOP, BOTTOM, NEAR, FAR); //perspective4x4(60, 1.0, 1, 200);

		// Uniforms
		// [0] -> Image reference
		// [1] -> Model transform 
		// [2] -> View transform 
		
		imageUniforms.insertPtr((void*)&myImage);
		imageUniforms.insertPtr((void*)&model);
		imageUniforms.insertPtr((void*)&view);
		imageUniforms.insertPtr((void*)&proj);

        FragmentShader fragImg;
		fragImg.FragShader = ImageFragShader;

        VertexShader vertImg;
		vertImg.VertShader = SimpleVertexShader2;
		       
        // Draw image triangle 
        DrawPrimitive(TRIANGLE, target, verticesImg1A, imageAttributesA, &imageUniforms, &fragImg, &vertImg, &zBuf);
        DrawPrimitive(TRIANGLE, target, verticesImg2A, imageAttributesB, &imageUniforms, &fragImg, &vertImg, &zBuf);
        DrawPrimitive(TRIANGLE, target, verticesImg1B, imageAttributesA, &imageUniforms, &fragImg, &vertImg, &zBuf);
        DrawPrimitive(TRIANGLE, target, verticesImg2B, imageAttributesB, &imageUniforms, &fragImg, &vertImg, &zBuf);
        DrawPrimitive(TRIANGLE, target, verticesImg1C, imageAttributesA, &imageUniforms, &fragImg, &vertImg, &zBuf);
        DrawPrimitive(TRIANGLE, target, verticesImg2C, imageAttributesB, &imageUniforms, &fragImg, &vertImg, &zBuf);

        DrawPrimitive(TRIANGLE, target, verticesImg1D, imageAttributesA, &imageUniforms, &fragImg, &vertImg, &zBuf);
        DrawPrimitive(TRIANGLE, target, verticesImg2D, imageAttributesB, &imageUniforms, &fragImg, &vertImg, &zBuf);
        DrawPrimitive(TRIANGLE, target, verticesImg1E, imageAttributesA, &imageUniforms, &fragImg, &vertImg, &zBuf);
        DrawPrimitive(TRIANGLE, target, verticesImg2E, imageAttributesB, &imageUniforms, &fragImg, &vertImg, &zBuf);
        DrawPrimitive(TRIANGLE, target, verticesImg1F, imageAttributesA, &imageUniforms, &fragImg, &vertImg, &zBuf);
        DrawPrimitive(TRIANGLE, target, verticesImg2F, imageAttributesB, &imageUniforms, &fragImg, &vertImg, &zBuf);

        // NOTE: To test the Z-Buffer additinonal draw calls/geometry need to be called into this scene
}
/********************************************
 * Verify that the whole pipeline works. By
 * the end of week 07 you should be able to
 * run this code successfully.
 *******************************************/
void testBox2Right(Buffer2D<PIXEL> & target)
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
                double NEAR = 1; double FAR = 200;
                double LEFT = -60; double RIGHT = 60;
                double BOTTOM = -60; double TOP = 60;

                camRight.x = 90;
                camRight.y = 30;
                camRight.z = 0;
                camRight.yaw = -60;
                camRight.pitch = 30;
                camRight.roll = 0;
                
                /*	Transform trans = translate4x4(-offX, -offY, -offZ);
	                Transform rotX = rotate4x4(X, -pitch);
	                Transform rotY = rotate4x4(Y, -yaw);*/
		
		Transform view = camera4x4(camRight.x, camRight.y, camRight.z, 
					   camRight.yaw, camRight.pitch, camRight.roll);
		Transform proj = orthgraphic(RIGHT, LEFT, TOP, BOTTOM, NEAR, FAR); //perspective4x4(60, 1.0, 1, 200);
               
		//Transform proj = perspective4x4(-100, 1.0, 1, 200);

		// Uniforms
		// [0] -> Image reference
		// [1] -> Model transform 
		// [2] -> View transform 
		
		imageUniforms.insertPtr((void*)&myImage);
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

        // NOTE: To test the Z-Buffer additinonal draw calls/geometry need to be called into this scene
}

/********************************************
 * Verify that the whole pipeline works. By
 * the end of week 07 you should be able to
 * run this code successfully.
 *******************************************/
void testBox3Top(Buffer2D<PIXEL> & target)
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
                double NEAR = 1; double FAR = 200;
                double LEFT = -60; double RIGHT = 60;
                double BOTTOM = -60; double TOP = 60;
                

                camTop.x = 1;
                camTop.y = 1;
                camTop.z = 0;
                camTop.yaw = 10;
                camTop.pitch = -60;
                camTop.roll = -40;

		Transform view = camera4x4(camTop.x, camTop.y, camTop.z, 
					   camTop.yaw, camTop.pitch, camTop.roll);
		Transform proj = orthgraphic(RIGHT, LEFT, TOP, BOTTOM, NEAR, FAR); //perspective4x4(60, 1.0, 1, 200);


		// Uniforms
		// [0] -> Image reference
		// [1] -> Model transform 
		// [2] -> View transform 
		
		imageUniforms.insertPtr((void*)&myImage);
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

        // NOTE: To test the Z-Buffer additinonal draw calls/geometry need to be called into this scene
}

/********************************************
 * Verify that the whole pipeline works. By
 * the end of week 07 you should be able to
 * run this code successfully.
 *******************************************/
void testBox4Left(Buffer2D<PIXEL> & target)
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
                double NEAR = 1; double FAR = 200;
                double LEFT = -60; double RIGHT = 60;
                double BOTTOM = -60; double TOP = 60;
                
                camLeft.x = 0;
                camLeft.y = 0;
                camLeft.z = 0;
                camLeft.yaw = 0;
                camLeft.pitch = 0;
                camLeft.roll = 0;

	        Transform view = camera4x4(camLeft.x, camLeft.y, camLeft.z, 
					   camLeft.yaw, camLeft.pitch, camLeft.roll);
		Transform proj = orthgraphic(RIGHT, LEFT, TOP, BOTTOM, NEAR, FAR); //perspective4x4(60, 1.0, 1, 200);

		// Uniforms
		// [0] -> Image reference
		// [1] -> Model transform 
		// [2] -> View transform 
		
		imageUniforms.insertPtr((void*)&myImage);
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

        // NOTE: To test the Z-Buffer additinonal draw calls/geometry need to be called into this scene
}




#endif