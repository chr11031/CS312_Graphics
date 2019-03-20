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

        static Buffer2D<double> zBuf(target.width(), target.height());
         topLeft.zeroOut();
         topRight.zeroOut();
         botLeft.zeroOut();
         botRight.zeroOut();
        // Your code goes here 
        // Feel free to copy from other test functions to get started!

        Vertex quad[] = { {-20,-20, 50, 1},
                          {20, -20, 80, 1},
                          {20, 20, 100, 1},
                          {-20,20, 70, 1}};

        //Vertex cube[] = 
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
		Attributes imageUniforms1;
		Attributes imageUniforms2;
		Attributes imageUniforms3;
		Matrix model = model.transMatrix(0, 0, 0);
		Matrix view = camera4x4(myCam.x , myCam.y, myCam.z, 
					   myCam.yaw , myCam.pitch, myCam.roll);
		Matrix proj = perspective4x4(60, 1.0, 1, 200);

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
		vertImg.VertShader = vShader2;
		
        DrawPrimitive(TRIANGLE, topLeft, verticesImgA, imageAttributesA, &imageUniforms, &fragImg, &vertImg, &zBuf);
        DrawPrimitive(TRIANGLE, topLeft, verticesImgB, imageAttributesB, &imageUniforms, &fragImg, &vertImg, &zBuf);

		Matrix model1 = model1.transMatrix(0, 0, 0);
		Matrix view1 = camera4x4(myCam1.x + 50 , myCam1.y , myCam1.z + 20, 
					   myCam1.yaw-90 , myCam1.pitch, myCam1.roll);
		Matrix proj1 = ortho(160, 160, 1, 200);

		// Uniforms
		// [0] -> Image reference
		// [1] -> Model transform 
		// [2] -> View transform 
		
		imageUniforms1.insertPtr((void*)&myImage);
		imageUniforms1.insertPtr((void*)&model1);
		imageUniforms1.insertPtr((void*)&view1);
		imageUniforms1.insertPtr((void*)&proj1);

        FragmentShader fragImg1;
		fragImg1.FragShader = ImageFragShader;

        VertexShader vertImg1;
		vertImg1.VertShader = vShader2;
        DrawPrimitive(TRIANGLE, topRight, verticesImgA, imageAttributesA, &imageUniforms1, &fragImg1, &vertImg1);
        DrawPrimitive(TRIANGLE, topRight, verticesImgB, imageAttributesB, &imageUniforms1, &fragImg1, &vertImg1);
		
		Matrix model2 = model2.transMatrix(0, 0, 0);
		Matrix view2 = camera4x4(myCam2.x , myCam2.y+50, myCam2.z, 
					   myCam2.yaw , myCam2.pitch + 60, myCam2.roll);
		Matrix proj2 = ortho(160, 160, 1, 200);

		// Uniforms
		// [0] -> Image reference
		// [1] -> Model transform 
		// [2] -> View transform 
		
		imageUniforms2.insertPtr((void*)&myImage);
		imageUniforms2.insertPtr((void*)&model2);
		imageUniforms2.insertPtr((void*)&view2);
		imageUniforms2.insertPtr((void*)&proj2);

        FragmentShader fragImg2;
		fragImg2.FragShader = FragShaderUVwithoutImage;

        VertexShader vertImg2;
		vertImg2.VertShader = vShader2;
                
        DrawPrimitive(TRIANGLE, botLeft, verticesImgA, imageAttributesA, &imageUniforms2, &fragImg2, &vertImg2, &zBuf);
        DrawPrimitive(TRIANGLE, botLeft, verticesImgB, imageAttributesB, &imageUniforms2, &fragImg2, &vertImg2, &zBuf);
		Matrix model3 = model3.transMatrix(0, 0, 0);
		Matrix view3 = camera4x4(myCam3.x, myCam3.y, myCam3.z, 
					   myCam3.yaw, myCam3.pitch, myCam3.roll);
		Matrix proj3 = ortho(100, 100, 1, 200);

		// Uniforms
		// [0] -> Image reference
		// [1] -> Model transform 
		// [2] -> View transform 
		
		imageUniforms3.insertPtr((void*)&myImage);
		imageUniforms3.insertPtr((void*)&model3);
		imageUniforms3.insertPtr((void*)&view3);
		imageUniforms3.insertPtr((void*)&proj3);

        FragmentShader fragImg3;
		fragImg3.FragShader = ImageFragShader;

        VertexShader vertImg3;
		vertImg3.VertShader = vShader2;
        DrawPrimitive(TRIANGLE, botRight, verticesImgA, imageAttributesA, &imageUniforms3, &fragImg3, &vertImg3, &zBuf);
        DrawPrimitive(TRIANGLE, botRight, verticesImgB, imageAttributesB, &imageUniforms3, &fragImg3, &vertImg3, &zBuf);
        // // Draw image triangle 
        // DrawPrimitive(TRIANGLE, topLeft, verticesImgA, imageAttributesA, &imageUniforms, &fragImg, &vertImg, &zBuf);
        // DrawPrimitive(TRIANGLE, topLeft, verticesImgB, imageAttributesB, &imageUniforms, &fragImg, &vertImg, &zBuf);

        // DrawPrimitive(TRIANGLE, topRight, verticesImgA, imageAttributesA, &imageUniforms, &fragImg1, &vertImg1, &zBuf);
        // DrawPrimitive(TRIANGLE, topRight, verticesImgB, imageAttributesB, &imageUniforms, &fragImg1, &vertImg1, &zBuf);

        // DrawPrimitive(TRIANGLE, botLeft, verticesImgA, imageAttributesA, &imageUniforms, &fragImg2, &vertImg2, &zBuf);
        // DrawPrimitive(TRIANGLE, botLeft, verticesImgB, imageAttributesB, &imageUniforms, &fragImg2, &vertImg2, &zBuf);

        // DrawPrimitive(TRIANGLE, botRight, verticesImgA, imageAttributesA, &imageUniforms, &fragImg3, &vertImg3, &zBuf);
        // DrawPrimitive(TRIANGLE, botRight, verticesImgB, imageAttributesB, &imageUniforms, &fragImg3, &vertImg3, &zBuf);
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
        PIXEL colors1[3] = {0xffff0000, 0xffff0000, 0xffff0000};
        // Your color code goes here for 'attr'
        verts[0] = {300, 402, 1, 1};
        verts[1] = {250, 452, 1, 1};
        verts[2] = {250, 362, 1, 1};
        PIXEL colors2[3] = {0xff00ff00, 0xff00ff00, 0xff00ff00};
        // Your color code goes here for 'attr'

        DrawPrimitive(TRIANGLE, target, verts, attr);
        verts[0] = {450, 362, 1, 1};
        verts[1] = {450, 452, 1, 1};
        verts[2] = {350, 402, 1, 1};
        PIXEL colors3[3] = {0xff0000ff, 0xff0000ff, 0xff0000ff};
        // Your color code goes here for 'attr'

        DrawPrimitive(TRIANGLE, target, verts, attr);
        verts[0] = {110, 262, 1, 1};
        verts[1] = {60, 162, 1, 1};
        verts[2] = {150, 162, 1, 1};
        PIXEL colors4[3] = {0xffff0000, 0xffff0000, 0xffff0000};

        DrawPrimitive(TRIANGLE, target, verts, attr);
        verts[0] = {210, 252, 1, 1};
        verts[1] = {260, 172, 1, 1};
        verts[2] = {310, 202, 1, 1};
        PIXEL colors5[3] = {0xff00ff00, 0xff00ff00, 0xff00ff00};
        DrawPrimitive(TRIANGLE, target, verts, attr);
        verts[0] = {370, 202, 1, 1};
        verts[1] = {430, 162, 1, 1};
        verts[2] = {470, 252, 1, 1};
        PIXEL colors6[3] = {0xff0000ff, 0xff0000ff, 0xff0000ff};
        // Your color code goes here for 'attr'
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
        colorTriangle[0] = {250, 112, 1, 1}; // bottom
        colorTriangle[1] = {450, 452, 1, 1}; //right
        colorTriangle[2] = {50, 452, 1, 1}; //left
        PIXEL colors[3] = {0xffff0000, 0xff00ff00, 0xff0000ff}; // Or {{1.0,0.0,0.0}, {0.0,1.0,0.0}, {0.0,0.0,1.0}}
        // Your color code goes here for 'colorAttributes'
        //add red green blue for att
        // colorAttributes[0].collector[0] = 1.0;
        // colorAttributes[1].collector[0] = 0.0;
        // colorAttributes[2].collector[0] = 0.0;
        // colorAttributes[0].collector[1] = 0.0;
        // colorAttributes[1].collector[1] = 1.0;
        // colorAttributes[2].collector[1] = 0.0;
        // colorAttributes[0].collector[2] = 0.0;
        // colorAttributes[1].collector[2] = 0.0;
        // colorAttributes[2].collector[2] = 1.0;

        FragmentShader myColorFragShader;
        // Your code for the color fragment shader goes here
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
        // imageAttributes[0].collector[0] = coordinates[0][0];
        // imageAttributes[0].collector[1] = coordinates[0][1];
        // imageAttributes[1].collector[0] = coordinates[1][0];
        // imageAttributes[1].collector[1] = coordinates[1][1];
        // imageAttributes[2].collector[0] = coordinates[2][0];
        // imageAttributes[2].collector[1] = coordinates[2][1];
        // Your texture coordinate code goes here for 'imageAttributes'

        //static BufferImage myImage("kerrigan.bmp");

        Attributes imageUniforms;

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
        static BufferImage myImage("checker.bmp");

        Attributes imageUniforms;

        FragmentShader fragImg;
        fragImg.FragShader = ImageFragShader;

        FragmentShader frag;
 
        DrawPrimitive(TRIANGLE, target, verticesImgA, imageAttributesA, &imageUniforms, &fragImg);
        DrawPrimitive(TRIANGLE, target, verticesImgB, imageAttributesB, &imageUniforms, &fragImg);

        DrawPrimitive(TRIANGLE, target, verticesImgA, imageAttributesA, &imageUniforms, &frag);
        DrawPrimitive(TRIANGLE, target, verticesImgB, imageAttributesB, &imageUniforms, &frag);
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
        // colorAttributes[0].collector[0] = 1.0;
        // colorAttributes[1].collector[0] = 0.0;
        // colorAttributes[2].collector[0] = 0.0;
        // colorAttributes[0].collector[1] = 0.0;
        // colorAttributes[1].collector[1] = 1.0;
        // colorAttributes[2].collector[1] = 0.0;
        // colorAttributes[0].collector[2] = 0.0;
        // colorAttributes[1].collector[2] = 0.0;
        // colorAttributes[2].collector[2] = 1.0;

        FragmentShader myColorFragShader;
        myColorFragShader.FragShader = ColorFragShader;

        Attributes colorUniforms;
        VertexShader myColorVertexShader;
        myColorVertexShader.VertShader = vShader;

        /******************************************************************
		 * TRANSLATE (move +100 in the X direction, +50 in the Y direction)
         *****************************************************************/
        // Your translating code that integrates with 'colorUniforms', used by 'myColorVertexShader' goes here
        Matrix trans = trans.transMatrix(100, 50, 1);
        colorUniforms.matrix = trans;
	DrawPrimitive(TRIANGLE, target, colorTriangle, colorAttributes, &colorUniforms, &myColorFragShader, &myColorVertexShader);

        /***********************************
         * SCALE (scale by a factor of 0.5)
         ***********************************/
        // Your scaling code that integrates with 'colorUniforms', used by 'myColorVertexShader' goes here
         Matrix scale = scale.scaleMatrix(0.5,0.5,1);
         colorUniforms.matrix = scale;
        DrawPrimitive(TRIANGLE, target, colorTriangle, colorAttributes, &colorUniforms, &myColorFragShader, &myColorVertexShader);

        /**********************************************
         * ROTATE 45 degrees in the X-Y plane around Z
         *********************************************/
        // Your rotation code that integrates with 'colorUniforms', used by 'myColorVertexShader' goes here
        Matrix rotate = rotate.rotatZMatrix(30);
        colorUniforms.matrix = rotate;
        DrawPrimitive(TRIANGLE, target, colorTriangle, colorAttributes, &colorUniforms, &myColorFragShader, &myColorVertexShader);

        /*************************************************
         * SCALE-TRANSLATE-ROTATE in left-to-right order
         * the previous transformations concatenated.
         ************************************************/
		// Your scale-translate-rotation code that integrates with 'colorUniforms', used by 'myColorVertexShader' goes here
        colorUniforms.matrix =  rotate.rotatZMatrix(30.0) * trans.transMatrix(100,50,1) * scale.scaleMatrix(0.5,0.5,1);
        DrawPrimitive(TRIANGLE, target, colorTriangle, colorAttributes, &colorUniforms, &myColorFragShader, &myColorVertexShader);	
}

/********************************************
 * This is the code for my personal project 
 *******************************************/
void PersonalProject(Buffer2D<PIXEL> & target)
{
       static Buffer2D<double> zBuf(target.width(), target.height());

        /**************************************************
        * 1. Image quad (2 TRIs) Code (texture interpolated)
        **************************************************/
        Vertex quad[] = { {-20,-20, 50, 1},
                          {20, -20, 50, 1},
                          {20, 20, 50, 1},
                          {-20,20, 50, 1}};
    
        Vertex wall[] = { {-10,-10, 50, 1},  
                          {10, -10, 50, 1},
                          {-10, -10, 50, 1}, 
                          {-10, 10, 50, 1}};    
              
        // checkered square
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
			
	static BufferImage forest("bricks.bmp");
	Attributes imageUniforms;
        Attributes wallUniforms;
	Matrix model = model.transMatrix(0, 0, 0);
	Matrix view = camera4x4(myCam.x, myCam.y, myCam.z, 
				   myCam.yaw, myCam.pitch, myCam.roll);
	Matrix proj = perspective4x4(60, 1.0, 1, 200);
	imageUniforms.insertPtr((void*)&forest);
	imageUniforms.insertPtr((void*)&model);
	imageUniforms.insertPtr((void*)&view);
	imageUniforms.insertPtr((void*)&proj);
        FragmentShader fragImg;
	fragImg.FragShader = FragShaderPattern;

        VertexShader vertImg;
	vertImg.VertShader = vShader2;
        DrawPrimitive(TRIANGLE, target, verticesImgA, imageAttributesA, &imageUniforms, &fragImg, &vertImg, &zBuf);
        DrawPrimitive(TRIANGLE, target, verticesImgB, imageAttributesB, &imageUniforms, &fragImg, &vertImg, &zBuf);

	Attributes imageUniforms1;
	Matrix model2 = model2.transMatrix(40, 0, 0);
        Matrix rotate2 = rotate4x4(Y, 90) * model2;
	Matrix view2 = camera4x4(myCam.x, myCam.y, myCam.z, 
				   myCam.yaw, myCam.pitch, myCam.roll);
	Matrix proj2 = perspective4x4(60, 1.0, 1, 200);
	imageUniforms1.insertPtr((void*)&forest);
	imageUniforms1.insertPtr((void*)&rotate2);
	imageUniforms1.insertPtr((void*)&view2);
	imageUniforms1.insertPtr((void*)&proj2);

        FragmentShader fragImg1;
	fragImg1.FragShader = FragShaderStatic;

        VertexShader vertImg1;
	vertImg1.VertShader = vShader2;
        DrawPrimitive(TRIANGLE, target, verticesImgA, imageAttributesA, &imageUniforms1, &fragImg1, &vertImg1, &zBuf);
        DrawPrimitive(TRIANGLE, target, verticesImgB, imageAttributesB, &imageUniforms1, &fragImg1, &vertImg1, &zBuf);

        Attributes imageUniforms12;
	Matrix model22 = model22.transMatrix(10, 0, 0);
        Matrix rotate22 = rotate4x4(Y, 90) * model22;
	Matrix view22 = camera4x4(myCam.x, myCam.y, myCam.z, 
				   myCam.yaw, myCam.pitch, myCam.roll);
	Matrix proj22 = perspective4x4(60, 1.0, 1, 200);
	imageUniforms12.insertPtr((void*)&forest);
	imageUniforms12.insertPtr((void*)&rotate22);
	imageUniforms12.insertPtr((void*)&view22);
	imageUniforms12.insertPtr((void*)&proj22);

        FragmentShader fragImg12;
	fragImg12.FragShader = FragShaderStatic;

        VertexShader vertImg12;
	vertImg12.VertShader = vShader2;
        DrawPrimitive(TRIANGLE, target, verticesImgA, imageAttributesA, &imageUniforms12, &fragImg12, &vertImg12, &zBuf);
        DrawPrimitive(TRIANGLE, target, verticesImgB, imageAttributesB, &imageUniforms12, &fragImg12, &vertImg12, &zBuf);



	Attributes imageUniforms2;
	Matrix model3 = model3.transMatrix(20, 0, 0);
        Matrix rotate3 = rotate4x4(Y, -90) * model3;
	Matrix view3 = camera4x4(myCam.x, myCam.y, myCam.z, 
				 myCam.yaw, myCam.pitch, myCam.roll);
	Matrix proj3 = perspective4x4(60, 1.0, 1, 200);
	imageUniforms2.insertPtr((void*)&forest);
	imageUniforms2.insertPtr((void*)&rotate3);
	imageUniforms2.insertPtr((void*)&view3);
	imageUniforms2.insertPtr((void*)&proj3);
        FragmentShader fragImg2;
	fragImg2.FragShader = FragShaderOrangeNegative;

        VertexShader vertImg2;
	vertImg2.VertShader = vShader2;
        DrawPrimitive(TRIANGLE, target, verticesImgA, imageAttributesA, &imageUniforms2, &fragImg2, &vertImg2, &zBuf);
        DrawPrimitive(TRIANGLE, target, verticesImgB, imageAttributesB, &imageUniforms2, &fragImg2, &vertImg2, &zBuf);

	Attributes imageUniforms3;
	Matrix model4 = model4.transMatrix(0, 0, 15);
        Matrix rotate4 = rotate4x4(Y, 180) *model4;
	Matrix view4 = camera4x4(myCam.x, myCam.y, myCam.z, 
				   myCam.yaw, myCam.pitch, myCam.roll);
	Matrix proj4 = perspective4x4(60, 1.0, 1, 200);
		
	imageUniforms3.insertPtr((void*)&forest);
	imageUniforms3.insertPtr((void*)&rotate4);
	imageUniforms3.insertPtr((void*)&view4);
	imageUniforms3.insertPtr((void*)&proj4);

        FragmentShader fragImg3;
	fragImg3.FragShader = ImageFragShader;

        VertexShader vertImg3;
	vertImg3.VertShader = vShader2;
		
        DrawPrimitive(TRIANGLE, target, verticesImgA, imageAttributesA, &imageUniforms3, &fragImg3, &vertImg3, &zBuf);
        DrawPrimitive(TRIANGLE, target, verticesImgB, imageAttributesB, &imageUniforms3, &fragImg3, &vertImg3, &zBuf);
}

#endif