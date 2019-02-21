#include "definitions.h"

#ifndef COURSE_FUNCTIONS_H
#define COURSE_FUNCTIONS_H



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
        colorTriangle[0] = (Vertex){250, 112, 1, 1};
        colorTriangle[1] = (Vertex){450, 452, 1, 1};
        colorTriangle[2] = (Vertex){50, 452, 1, 1};
        PIXEL colors[3] = {0xffff0000, 0xff00ff00, 0xff0000ff}; // Or {{1.0,0.0,0.0}, {0.0,1.0,0.0}, {0.0,0.0,1.0}}
        
        // assign the color variables.
        colorAttributes[0].vars[0] = (colors[0] >> 16) & 0x000000ff;
        colorAttributes[0].vars[1] = (colors[0] >> 8) & 0x000000ff;
        colorAttributes[0].vars[2] = (colors[0] >> 0) & 0x000000ff;
        colorAttributes[0].maxVar = 2;

        colorAttributes[1].vars[0] = (colors[1] >> 16) & 0x000000ff;
        colorAttributes[1].vars[1] = (colors[1] >> 8) & 0x000000ff;
        colorAttributes[1].vars[2] = (colors[1] >> 0) & 0x000000ff;
        colorAttributes[1].maxVar = 2;


        colorAttributes[2].vars[0] = (colors[2] >> 16) & 0x000000ff;
        colorAttributes[2].vars[1] = (colors[2] >> 8) & 0x000000ff;
        colorAttributes[2].vars[2] = (colors[2] >> 0) & 0x000000ff;
        colorAttributes[2].maxVar = 2;


        // assign a shader to draw this triangle
        FragmentShader myColorFragShader;
        myColorFragShader.FragShader = colorFragShader;

        // We have no color uniforms, so we don't do anything with this.
        Attributes colorUniforms;

        // Do it.
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
        // Your texture coordinate code goes here for 'imageAttributes'
        imageAttributes[0].vars[0] = 1;
        imageAttributes[0].vars[1] = 0;
        imageAttributes[0].maxVar = 1;

        imageAttributes[1].vars[0] = 1;
        imageAttributes[1].vars[1] = 1;
        imageAttributes[1].maxVar = 1;
        
        
        imageAttributes[2].vars[0] = 0;
        imageAttributes[2].vars[1] = 1;
        imageAttributes[2].maxVar = 1;


        // Temporary bug fix.
        static BufferImage*  myImage = NULL;
        if(myImage == NULL)
                myImage = new BufferImage("./checker.bmp");
        (*myImage)[0] = (*myImage)[1];

        // Assing the immage to our void pointer
        Attributes imageUniforms;
        imageUniforms.ptr = myImage;

        // 
        FragmentShader myImageFragShader;
        myImageFragShader.FragShader = imgFragShader;

        // Do it.
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
        Vertex quad[] = {(Vertex){(-1200 / divA) + (S_WIDTH/2), (-1500 / divA) + (S_HEIGHT/2), divA, 1.0/divA },
                         (Vertex){(1200  / divA) + (S_WIDTH/2), (-1500 / divA) + (S_HEIGHT/2), divA, 1.0/divA },
                         (Vertex){(1200  / divB) + (S_WIDTH/2), (1500  / divB) + (S_HEIGHT/2), divB, 1.0/divB },
                         (Vertex){(-1200 / divB) + (S_WIDTH/2), (1500  / divB) + (S_HEIGHT/2), divB, 1.0/divB }};

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

        // Add all the coordinates, and also define the var size.
        imageAttributesA[0].vars[0] = coordinates[0][0];
        imageAttributesA[0].vars[1] = coordinates[0][1];
        imageAttributesA[0].maxVar = 1;
        imageAttributesA[1].vars[0] = coordinates[1][0];
        imageAttributesA[1].vars[1] = coordinates[1][1];
        imageAttributesA[1].maxVar = 1;
        imageAttributesA[2].vars[0] = coordinates[2][0];
        imageAttributesA[2].vars[1] = coordinates[2][1];
        imageAttributesA[2].maxVar = 1;
        imageAttributesB[0].vars[0] = coordinates[2][0];
        imageAttributesB[0].vars[1] = coordinates[2][1];
        imageAttributesB[0].maxVar = 1;
        imageAttributesB[1].vars[0] = coordinates[3][0];
        imageAttributesB[1].vars[1] = coordinates[3][1];
        imageAttributesA[1].maxVar = 1;
        imageAttributesB[2].vars[0] = coordinates[0][0];
        imageAttributesB[2].vars[1] = coordinates[0][1];
        imageAttributesB[2].maxVar = 1;

        static BufferImage*  myImage = NULL;
        if(myImage == NULL)
                myImage = new BufferImage("./doit.bmp");

        Attributes imageUniforms;
        imageUniforms.ptr = myImage;

        FragmentShader fragImg;
        fragImg = imgFragShader;        

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
        colorAttributes[0].vars[0] = (colors[0] >> 16) & 0x000000ff;
        colorAttributes[0].vars[1] = (colors[0] >> 8) & 0x000000ff;
        colorAttributes[0].vars[2] = (colors[0] >> 0) & 0x000000ff;
        colorAttributes[0].maxVar = 2;

        colorAttributes[1].vars[0] = (colors[1] >> 16) & 0x000000ff;
        colorAttributes[1].vars[1] = (colors[1] >> 8) & 0x000000ff;
        colorAttributes[1].vars[2] = (colors[1] >> 0) & 0x000000ff;
        colorAttributes[1].maxVar = 2;

        colorAttributes[2].vars[0] = (colors[2] >> 16) & 0x000000ff;
        colorAttributes[2].vars[1] = (colors[2] >> 8) & 0x000000ff;
        colorAttributes[2].vars[2] = (colors[2] >> 0) & 0x000000ff;
        colorAttributes[2].maxVar = 2;


       // assign a shader to draw this triangle
        FragmentShader myColorFragShader;
        myColorFragShader.FragShader = colorFragShader;

        Attributes colorUniforms;
        Matrix theMatrix;
        colorUniforms.ptr2 = &theMatrix;
        
        // Your code for the vertex shader goes here 
        VertexShader myColorVertexShader;
        myColorVertexShader.VertShader = transformationVertShader;

        // Test draw triangle
	// DrawPrimitive(TRIANGLE, target, colorTriangle, colorAttributes, &colorUniforms, &myColorFragShader, &myColorVertexShader);

        

        /******************************************************************
	 * TRANSLATE (move +100 in the X direction, +50 in the Y direction)
         *****************************************************************/
        // Your translating code that integrates with 'colorUniforms', used by 'myColorVertexShader' goes here
        theMatrix.translate(100, 50);

        // Do it.
	DrawPrimitive(TRIANGLE, target, colorTriangle, colorAttributes, &colorUniforms, &myColorFragShader, &myColorVertexShader);
        // Reset
        theMatrix.reset();

        /***********************************
         * SCALE (scale by a factor of 0.5)
         ***********************************/
        // Your scaling code that integrates with 'colorUniforms', used by 'myColorVertexShader' goes here
        theMatrix.scale(0.5);

        // Do it.
        DrawPrimitive(TRIANGLE, target, colorTriangle, colorAttributes, &colorUniforms, &myColorFragShader, &myColorVertexShader);
        // Reset
        theMatrix.reset();

        /**********************************************
         * ROTATE 30 degrees in the X-Y plane around Z
         *********************************************/
        // Your rotation code that integrates with 'colorUniforms', used by 'myColorVertexShader' goes here
        theMatrix.rotate(30);

        // Do it.
        DrawPrimitive(TRIANGLE, target, colorTriangle, colorAttributes, &colorUniforms, &myColorFragShader, &myColorVertexShader);
        // Reset
        theMatrix.reset();

        /*************************************************
         * SCALE-TRANSLATE-ROTATE in left-to-right order
         * the previous transformations concatenated.
         ************************************************/
	// Your scale-translate-rotation code that integrates with 'colorUniforms', used by 'myColorVertexShader' goes here

        Matrix scaleMatr; scaleMatr.scale(0.5);
        Matrix transMatr; transMatr.translate(100, 50);
        Matrix rotatMatr; rotatMatr.rotate(30);

        theMatrix =  transMatr * scaleMatr;

        theMatrix = rotatMatr * theMatrix;

        // Do it.
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
        Vertex quad[] = { (Vertex){-20,-20, 50, 1},
                          (Vertex){20, -20, 50, 1},
                          (Vertex){20, 20, 50, 1},
                          (Vertex){-20,20, 50, 1}};

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