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
        static int neighbors[8][2] =
        {
                {0, 1},
                {0, -1},
                {1, 0},
                {-1, 0},
                {1, -1},
                {1, 1},
                {-1, -1},
                {-1, 1}
        };

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
                for (int y = 0; y < gridH; y++)
                {
                        for (int x = 0; x < gridW; x++)
                        {
                                int num_alive = 0;
                                int num_dead = 0;

                                for (int i = 0; i < 8; i++)
                                {
                                        int yi = y + neighbors[i][0];
                                        int xi = x + neighbors[i][1];
                                        if (yi < gridH && yi >= 0 && xi < gridW && xi >= 0)
                                                gridTmp[yi][xi] == 1 ? num_alive++ : num_dead++;
                                }

                                grid[y][x] = gridTmp[y][x] == 1 && num_alive == 2 || num_alive == 3 ? 1 : 0;
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

/****************************************************
 * Solves for the determinate of two given matrices.
****************************************************/
double determinate(Vertex a, Vertex b)
{
        return a.x * b.y - a.y * b.x;
}

/****************************************************
 * Subtracts the x and y values of two vertexs
****************************************************/
Vertex subtractVert(Vertex a, Vertex b)
{
        return {a.x - b.x, a.y - b.y};
}

/****************************************************
 * Interpolates data according to determinate
 * values
****************************************************/
void interp(Vertex * triangle, Attributes* const attrs, double * interpBin, double const weights[3])
{
        // Weights to multiple attribute data determined by what percent of the
        // original triangle the smaller triangles (determinents) take up
        // however dividing by the triangle area is unnecessary because the area
        // is canceled out by multiplying by z

        // taking the reciprocal here lets us use only one divide per fragment
        // in return we must use a multiply for every attribute
        double z = 1 / (weights[0] * triangle[2].w + 
                        weights[1] * triangle[0].w + 
                        weights[2] * triangle[1].w);

        // convert attribute data to appropriate data type
        double * d1 = (double*)(attrs[0]).data;
        double * d2 = (double*)(attrs[1]).data;
        double * d3 = (double*)(attrs[2]).data;

        // interpolate according to how length of data in attributes
        for (int i = 0; i < attrs[0].dLen; i++)
            interpBin[i] = (weights[0] * d3[i] + 
                            weights[1] * d1[i] + 
                            weights[2] * d2[i]) 
                            * z;
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
        pointAttributes.data = &color;

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
        attr[0].data = &colors1[0];

        DrawPrimitive(TRIANGLE, target, verts, attr);

        verts[0] = {300, 402, 1, 1};
        verts[1] = {250, 452, 1, 1};
        verts[2] = {250, 362, 1, 1};
        PIXEL colors2[3] = {0xff0000ff, 0xffff0000, 0xffff0000};
        attr[0].data = &colors2[0];

        DrawPrimitive(TRIANGLE, target, verts, attr);

        verts[0] = {450, 362, 1, 1};
        verts[1] = {450, 452, 1, 1};
        verts[2] = {350, 402, 1, 1};
        PIXEL colors3[3] = {0xff00ff00, 0xff00ff00, 0xff00ff00};
        attr[0].data = &colors3[0];

        DrawPrimitive(TRIANGLE, target, verts, attr);
        
        verts[0] = {110, 262, 1, 1};
        verts[1] = {60, 162, 1, 1};
        verts[2] = {150, 162, 1, 1};
        PIXEL colors4[3] = {0xfff00fff, 0xff00ff00, 0xff00ff00};
        attr[0].data = &colors4[0];

        DrawPrimitive(TRIANGLE, target, verts, attr);

        verts[0] = {210, 252, 1, 1};
        verts[1] = {260, 172, 1, 1};
        verts[2] = {310, 202, 1, 1};
        PIXEL colors5[3] = {0xff00ffff, 0xff00ff00, 0xff00ff00};
        attr[0].data = &colors5[0];

        DrawPrimitive(TRIANGLE, target, verts, attr);
        
        verts[0] = {370, 202, 1, 1};
        verts[1] = {430, 162, 1, 1};
        verts[2] = {470, 252, 1, 1};
        PIXEL colors6[3] = {0xffffff00, 0xff00ff00, 0xff00ff00};
        attr[0].data = &colors6[0];

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
        // *************************************************/
        Vertex colorTriangle[3] =
        {
            {250, 112, 1, 1},
            {450, 452, 1, 1},
            {50, 452, 1, 1}
        };

        Attributes colorAttributes[3] = 
        {
            RgbAttr(1, 0, 0),
            RgbAttr(0, 1, 0),
            RgbAttr(0, 0, 1)
        };

        FragmentShader myColorFragShader;
        myColorFragShader.FragShader = ColorFragShader;

        Attributes colorUniforms;
        // Your code for the uniform goes here, if any (don't pass NULL here)

        DrawPrimitive(TRIANGLE, target, colorTriangle, colorAttributes, &colorUniforms, &myColorFragShader);

        /****************************************************
         * 2. Interpolated image triangle
        ****************************************************/
        Vertex imageTriangle[3];
        imageTriangle[0] = {425, 112, 1, 1};
        imageTriangle[1] = {500, 252, 1, 1};
        imageTriangle[2] = {350, 252, 1, 1};

        Attributes imageAttributes[3] = 
        {
            ImageAttr(0, 0),
            ImageAttr(0, 1),
            ImageAttr(1, 1)
        };

        // static BufferImage myImage = BufferImage("checker.bmp");

        Attributes imageUniforms;

        FragmentShader myImageFragShader;
        myImageFragShader.FragShader = FragShaderUVwithoutImage;

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

        double coordinates[4][2] = { {0/divA,0/divA}, {1/divA,0/divA}, {1/divB,1/divB}, {0/divB,1/divB} };

        Vertex verticesImgA[3];
        verticesImgA[0] = quad[0];
        verticesImgA[1] = quad[1];
        verticesImgA[2] = quad[2];

        Attributes imageAttributesA[3] = 
        {
            ImageAttr(coordinates[0][0], coordinates[0][1]),
            ImageAttr(coordinates[1][0], coordinates[1][1]),
            ImageAttr(coordinates[2][0], coordinates[2][1])
        };

        Vertex verticesImgB[3];        
        verticesImgB[0] = quad[2];
        verticesImgB[1] = quad[3];
        verticesImgB[2] = quad[0];

        Attributes imageAttributesB[3] = 
        {
            ImageAttr(coordinates[2][0], coordinates[2][1]),
            ImageAttr(coordinates[3][0], coordinates[3][1]),
            ImageAttr(coordinates[0][0], coordinates[0][1])
        };

        // Your texture coordinate code goes here for 'imageAttributesA, imageAttributesB'

        // BufferImage myImage("checker.bmp");
        // Ensure the checkboard image is in this directory
        // static BufferImage myImage = BufferImage("checker.bmp");

        Attributes imageUniforms;

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
        colorTriangle[0] = { 350, 112, 1, 1};
        colorTriangle[1] = { 400, 200, 1, 1};
        colorTriangle[2] = { 300, 200, 1, 1};

        FragmentShader myColorFragShader;
        myColorFragShader.FragShader = ColorFragShader;


        Attributes colorAttributes[3] = 
        {
            RgbAttr(1, 0, 0),
            RgbAttr(0, 1, 0),
            RgbAttr(0, 0, 1)
        };

        
        VertexShader myColorVertexShader;
        myColorVertexShader.VertShader = TransformVertShader;
        // Your code for the vertex shader goes here 

        /******************************************************************
                 * TRANSLATE (move +100 in the X direction, +50 in the Y direction)
         *****************************************************************/
        // Your translating code that integrates with 'colorUniforms', used by 'myColorVertexShader' goes here
        TransUni colorUniforms1 = TransUni();
        colorUniforms1.get();

        DrawPrimitive(TRIANGLE, target, colorTriangle, colorAttributes, &colorUniforms1, &myColorFragShader, &myColorVertexShader);

        // /***********************************
        //  * SCALE (scale by a factor of 0.5)
        //  ***********************************/
        // // Your scaling code that integrates with 'colorUniforms', used by 'myColorVertexShader' goes here
        TransUni colorUniforms2 = TransUni();
        colorUniforms2.get().scale(0.5, 0.5);

        DrawPrimitive(TRIANGLE, target, colorTriangle, colorAttributes, &colorUniforms2, &myColorFragShader, &myColorVertexShader);

        // /**********************************************
        //  * ROTATE 45 degrees in the X-Y plane around Z
        //  *********************************************/
        // // Your rotation code that integrates with 'colorUniforms', used by 'myColorVertexShader' goes here
        TransUni colorUniforms3 = TransUni();
        
        // As per slack 30 degrees mirrors more closely with the provided visual
        // In my case 29.5 gives me a perfect rotation where as 30 does not leave the right side of the triangle flat
        colorUniforms3.get().rotate(29.5);

        DrawPrimitive(TRIANGLE, target, colorTriangle, colorAttributes, &colorUniforms3, &myColorFragShader, &myColorVertexShader);

        // /*************************************************
        //  * SCALE-TRANSLATE-ROTATE in left-to-right order
        //  * the previous transformations concatenated.
        //  ************************************************/
        //         // Your scale-translate-rotation code that integrates with 'colorUniforms', used by 'myColorVertexShader' goes here
                
        TransUni colorUniforms4 = TransUni();
        colorUniforms4.get().scale(0.5, 0.5).translate(100, 50).rotate(29.5);

        DrawPrimitive(TRIANGLE, target, colorTriangle, colorAttributes, &colorUniforms4, &myColorFragShader, &myColorVertexShader);
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