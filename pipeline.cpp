#include "definitions.h"
#include "coursefunctions.h"
//#include "shaders.h"

/***********************************************
 * CLEAR_SCREEN
 * Sets the screen to the indicated color value.
 **********************************************/
void clearScreen(Buffer2D<PIXEL> & frame, PIXEL color = 0xff000000)
{
    int h = frame.height();
    int w = frame.width();
    for(int y = 0; y < h; y++)
    {
        for(int x = 0; x < w; x++)
        {
            frame[y][x] = color;
        }
    }
}

/************************************************************
 * UPDATE_SCREEN
 * Blits pixels from RAM to VRAM for rendering.
 ***********************************************************/
void SendFrame(SDL_Texture* GPU_OUTPUT, SDL_Renderer * ren, SDL_Surface* frameBuf) 
{
    SDL_UpdateTexture(GPU_OUTPUT, NULL, frameBuf->pixels, frameBuf->pitch);
    SDL_RenderClear(ren);
    SDL_RenderCopy(ren, GPU_OUTPUT, NULL, NULL);
    SDL_RenderPresent(ren);
}

/*************************************************************
 * POLL_CONTROLS
 * Updates the state of the application based on:
 * keyboard, mouse, touch screen, gamepad inputs. 
 ************************************************************/
void processUserInputs(bool & running)
{
    SDL_Event e;
    int mouseX;
    int mouseY;
    while(SDL_PollEvent(&e)) 
    {
        if(e.type == SDL_QUIT) 
        {
            running = false;
        }
        if(e.key.keysym.sym == 'q' && e.type == SDL_KEYDOWN) 
        {
            running = false;
        }

        //Transformations with mouse
        // if (e.type == SDL_MOUSEMOTION)
        // {
        //     int cur = SDL_ShowCUrsos(SDL_QUERY);
        //     if(cur == SDL_DISABLE)
        //     {
        //         double mouseX = e.motion.xrel;
        //         double mouseY = e.motion.yrel;

        //         myCam.yaw -= mouseX * 0.02;
        //         myCam.pitch -= mouseY * 0.02;
        //     }
        // }

        // if(e.type == SDLMOUSEBUTTONDOWN)
        // {
        //     int cur = SDL_ShowCursos(SDL_QUERY);
        //                 if(cur == SDL_DISABLE)
        //     {
        //         SDL_ShowCursos(SDL_ENABLE);
        //         SDL_SetRelativeMouseMode(SDL_FALSE);
        //     }
        //     else
        //     {
        //         SDL_ShowCursos(SDL_DISABLE);
        //         SDL_SetRelativeMouseMode(SDL_TRUE);
        //     }
        // }

        // //Translation
        // if((e.key.keysym.sym == 'w' && e.type == SDL_KEYDOWN))
        // {
        //     myCam.z += (cos((myCam.yaw / 180.0) * M_PI)) * 0.05;
        //     myCam.x -= (sin((myCam.yaw / 180.0) * M_PI)) * 0.05;
        // }
        // if((e.key.keysym.sym == 's' && e.type == SDL_KEYDOWN))
        // {
        //     myCam.z -= (cos((myCam.yaw / 180.0) * M_PI)) * 0.05;
        //     myCam.x += (sin((myCam.yaw / 180.0) * M_PI)) * 0.05;
        // }
        // if((e.key.keysym.sym == 'a' && e.type == SDL_KEYDOWN))
        // {
        //     myCam.x -= (cos((myCam.yaw / 180.0) * M_PI)) * 0.05;
        //     myCam.z -= (sin((myCam.yaw / 180.0) * M_PI)) * 0.05;
        // }
        // if((e.key.keysym.sym == 'd' && e.type == SDL_KEYDOWN))
        // {
        //     myCam.x += (cos((myCam.yaw / 180.0) * M_PI)) * 0.05;
        //     myCam.z += (sin((myCam.yaw / 180.0) * M_PI)) * 0.05;
        // }


    }
}

/****************************************
 * DRAW_POINT
 * Renders a point to the screen with the
 * appropriate coloring.
 ***************************************/
void DrawPoint(Buffer2D<PIXEL> & target, Vertex* v, Attributes* attrs, Attributes * const uniforms, FragmentShader* const frag)
{
    // Your code goes here
    // Set our pixel according to the attribute value!
    target[(int)v[0].y][(int)v[0].x] = attrs[0].color;
    Attributes wColor;
    wColor.color = 0xffffffff;
    //-----------------------------------------------//
    
    Vertex verts[3];
    Attributes attr[3];
    verts[0] = {150, 250, 1, 1};
    verts[1] = {50, 150, 1, 1};
    verts[2] = {50, 50, 1, 1};
    attr[0].color = 0xffff0000;//red
    attr[1].color = 0xff00ff00;//green
    attr[2].color = 0xff0000ff;//blue

    //Prints the vertex pixel
    for (int i = 0; i<3; i++){
        target[(int)verts[i].y][(int)verts[i].x] = attr[i].color;
    }

    //Drawline in "wColor"
    //v2-v0
    double distX = verts[0].x-verts[2].x;
    double distY = verts[0].y-verts[2].y;
    if (distX>distY){
        Vertex midVrs[(int)distX];
        for (int i = 0; i<distX; i++){
            midVrs[i].x = (int) (verts[2].x + i);
            midVrs[i].y = (int) (verts[2].y + (distY*(i/distX)));
        }
        for (Vertex _v : midVrs){
            target[(int)_v.y][(int)_v.x] = 0xffffffff;
        }
    } else if (distY>distX){
        Vertex midVrs[(int)distY];
        for (int i = 0; i<distY; i++){
            midVrs[i].y = (int) (verts[2].y + i);
            midVrs[i].x = (int) (verts[2].x + (distX*(i/distY)));
        }
        for (Vertex _v : midVrs){
            target[(int)_v.y][(int)_v.x] = 0xffffffff;
        }
    } else if (distX == distY){
        Vertex midVrs[(int)distX];
        for (int i = 0; i<distX; i++){
            midVrs[i].y = (int) (verts[2].y + i);
            midVrs[i].x = (int) (verts[2].x + i);
        }
        for (Vertex _v : midVrs){
            target[(int)_v.y][(int)_v.x] = 0xffffffff;
        }
    }

    
    // //middle points.
    // Vertex midVrs[3];
    // Attributes midAts[3];
    // //for (int i = 0; i<3; i++){
    //     midVrs[0] = {200, 200, 1, 1};
    //     int midX = (verts[0].x + verts[0+1].x)/2;
    //     int midY = (verts[0].y + verts[0+1].y)/2;
    //     midVrs[0].x = midX;
    //     midVrs[0].y = midX;
    //     midAts[0].color = 0xffffffff;
    //     target[midY][midX] = midAts[0].color;
    // //}
    


    //-----------------------------------------------//
    //target[100][100] = 0xffffffff;
    // for (int _x = 0; _x < 256 ; _x++){
    //     for (int _y = 0; _y < 256 ; _y++){
    //         target[_y][_x] = 0xffffffff;
    //         if (_y == verts[0].y && _x == verts[0].x){
    //             target[_y][_x] = 0xff000000;
    //         }
    //     }
    // }
}

/****************************************
 * DRAW_TRIANGLE
 * Renders a line to the screen.
 ***************************************/
void DrawLine(Buffer2D<PIXEL> & target, Vertex* const triangle, Attributes* const attrs, Attributes* const uniforms, FragmentShader* const frag)
{
    // Your code goes here
    //target[(int)triangle[0].y][(int)triangle[0].x] = 0xffff0000;

}

/*************************************************************
 * Area Of Triangle
 * Returns the area of a triangle.
 * Uses an array of Vertex.
 ************************************************************/
double AreaOfTriangle (Vertex* const t){
    // double _aX = (t[0].x-t[1].x);
    // double _aY = (t[0].y-t[1].y);
    // double _aX2 = _aX*_aX;
    // double _aY2 = _aY*_aY;
    // double _a = sqrt(abs(_aX2 - _aY2));


    double a = sqrt(abs(abs(t[0].x-t[1].x)*abs(t[0].x-t[1].x)+abs(t[0].y-t[1].y)*abs(t[0].y-t[1].y)));
    double b = sqrt(abs(abs(t[1].x-t[2].x)*abs(t[1].x-t[2].x)+abs(t[1].y-t[2].y)*abs(t[1].y-t[2].y)));
    double c = sqrt(abs(abs(t[0].x-t[2].x)*abs(t[0].x-t[2].x)+abs(t[0].y-t[2].y)*abs(t[0].y-t[2].y)));

    double s = (a+b+c)/2;

    double area = sqrt((s*(s-a)*(s-b)*(s-c)));
    return area;
}
/*************************************************************
 * Negative Area Of Triangle
 * Returns the area of a triangle.
 * Uses an array of Vertex.
 ************************************************************/
double NegativeAreaOfTriangle (Vertex* const t){

    int rightX = 0;
    int leftX = 512;
    int upY = 0;
    int downY = 512;
    //calculate space//
    for (int i = 0; i<3; i++){
        if (t[i].x > rightX) {rightX = t[i].x;}
        if (t[i].x < leftX) {leftX = t[i].x;}
        if (t[i].y > upY) {upY = t[i].y;}
        if (t[i].y < downY) {downY = t[i].y;}
    }
    int cubeH = upY - downY;
    int cubeW = rightX - leftX;
    int cubeArea = (cubeH * cubeW);

    double area = 0;

    double area0 = (abs(t[0].x-t[1].x)*abs(t[0].y-t[1].y));
    double area1 = (abs(t[1].x-t[2].x)*abs(t[1].y-t[2].y));
    double area2 = (abs(t[2].x-t[0].x)*abs(t[2].y-t[0].y));
    area = (area0 + area1 + area2)/2;

    return cubeArea - area;
}
    //\\*       //\\*       //\\*       //\\*       //\\*       //\\*       //\\*       //\\*       //\\*   
   //  \\*     //  \\*     //  \\*     //  \\*     //  \\*     //  \\*     //  \\*     //  \\*     //  \\*  
  //    \\*   //    \\*   //    \\*   //    \\*   //    \\*   //    \\*   //    \\*   //    \\*   //    \\* 
 //      \\* //      \\* //      \\* //      \\* //      \\* //      \\* //      \\* //      \\* //      \\*
/*************************************************************
 * DRAW_TRIANGLE
 * Renders a triangle to the target buffer. Essential 
 * building block for most of drawing.
 ************************************************************/
void DrawTriangle(Buffer2D<PIXEL> & target, Vertex* const triangle, Attributes* const attrs, Attributes* const uniforms, FragmentShader* const frag)
{
    // Your code goes here
    //target[(int)triangle[0].y][(int)triangle[0].x] = 0xffff0000;
    // ^not done yet, just trying dots

    //area//
    //double fullArea = AreaOfTriangle(triangle);
    double fullArea = AreaOfTriangle(triangle);
    fullArea += 1;
    
    //Colors//
    Attributes acolor[5];
    acolor[0].color = 0xff111111;//dark gray
    acolor[1].color = 0xffff0000;//red
    acolor[2].color = 0xff00ff00;//green
    acolor[3].color = 0xff0000ff;//blue
    acolor[4].color = 0xff000000;//to replace


    //Draw dots//
    // for (int i = 0; i<3; i++){
    //     target[(int)triangle[i].y][(int)triangle[i].x] = attrs[i].color;
    // }
        int rightX = 0;
        int leftX = 512;
        int upY = 0;
        int downY = 512;
        int midX = 0;
        int midY = 0;

    //calculate space//
    for (int i = 0; i<3; i++){
        if (triangle[i].x > rightX) {rightX = triangle[i].x;}
        if (triangle[i].x < leftX) {leftX = triangle[i].x;}
        if (triangle[i].y > upY) {upY = triangle[i].y;}
        if (triangle[i].y < downY) {downY = triangle[i].y;}
    }

    //draw triangle//
    for (int _x = leftX; _x < rightX ; _x++){
        for (int _y = downY; _y < upY ; _y++){
            //draw cube//
            //target[_y][_x] = 0xffffffff;
            //target[_y][_x] = 0xff222222;
            
            //draw triangle//
            //--Area--//
            Vertex _v0[3];
                _v0[0] = {_x, _y, 1, 1};
                _v0[1] = triangle[1];
                _v0[2] = triangle[2];
                double area0 = AreaOfTriangle(_v0);
            Vertex _v1[3];
                _v1[0] = triangle[0];
                _v1[1] = {_x, _y, 1, 1};
                _v1[2] = triangle[2];
                double area1 = AreaOfTriangle(_v1);
            Vertex _v2[3];
                _v2[0] = triangle[0];
                _v2[1] = triangle[1];
                _v2[2] = {_x, _y, 1, 1};
                double area2 = AreaOfTriangle(_v2);

            double areas = area0 + area1 + area2;

            //acolor[3].color = 0xffffffff;
            // make the shader here//

            if (fullArea >= areas){
                // PIXEL colorTest[1] = {0xffffffff};
                // target[_y][_x] = colorTest[0];

                target[_y][_x] = attrs[0].color;

                if (frag != nullptr){

                acolor[0].r = ((area0*attrs[0].r)+(area1*attrs[1].r)+(area2*attrs[2].r))/fullArea;
                acolor[0].g = ((area0*attrs[0].g)+(area1*attrs[1].g)+(area2*attrs[2].g))/fullArea;
                acolor[0].b = ((area0*attrs[0].b)+(area1*attrs[1].b)+(area2*attrs[2].b))/fullArea;

                acolor[0].u = ((area0*attrs[0].u)+(area1*attrs[1].u)+(area2*attrs[2].u))/fullArea;
                acolor[0].v = ((area0*attrs[0].v)+(area1*attrs[1].v)+(area2*attrs[2].v))/fullArea;

                frag->FragShader(target[_y][_x], acolor[0], *uniforms);

                }
            } else {
                //target[_y][_x] = attrs[0].color;
            }
        }
    }
}
// \\       //* \\       //* \\       //* \\       //* \\       //* \\       //* \\       //* \\       //*
//  \\     //*   \\     //*   \\     //*   \\     //*   \\     //*   \\     //*   \\     //*   \\     //*  
//   \\   //*     \\   //*     \\   //*     \\   //*     \\   //*     \\   //*     \\   //*     \\   //*  
//    \\ //*       \\ //*       \\ //*       \\ //*       \\ //*       \\ //*       \\ //*       \\ //*    


// /*************************************************************
//  * DRAW_TRIANGLE
//  * Renders a triangle to the target buffer. Essential 
//  * building block for most of drawing.
//  ************************************************************/
// void DrawSquare(Buffer2D<PIXEL> & target, Vertex* const triangle, Attributes* const attrs, Attributes* const uniforms, FragmentShader* const frag)
// {

// }

/**************************************************************
 * VERTEX_SHADER_EXECUTE_VERTICES
 * Executes the vertex shader on inputs, yielding transformed
 * outputs. 
 *************************************************************/
void VertexShaderExecuteVertices(const VertexShader* vert, Vertex const inputVerts[], Attributes const inputAttrs[], const int& numIn, 
                                 Attributes* const uniforms, Vertex transformedVerts[], Attributes transformedAttrs[])
{
    // Defaults to pass-through behavior
    if(vert == NULL)
    {
        for(int i = 0; i < numIn; i++)
        {
            transformedVerts[i] = inputVerts[i];
            transformedAttrs[i] = inputAttrs[i];
        }
    } else {
        for(int i = 0; i < numIn; i++)
        {
            transformedVerts[i] = inputVerts[i];
            transformedAttrs[i] = inputAttrs[i];

            //Transform//
            if (numIn == 3){
                vert->VertShader(transformedVerts[i], transformedAttrs[i], inputVerts[i], inputAttrs[i], *uniforms);
                //vert.VertexShader(transformedVerts[i], transformedAttrs[i], inputVerts[i], inputAttrs[i], *uniforms);
            }
        }
    }
}

/***************************************************************************
 * DRAW_PRIMITIVE
 * Processes the indicated PRIMITIVES type through pipeline stages of:
 *  1) Vertex Transformation
 *  2) Clipping
 *  3) Normalization
 *  4) ViewPort transform
 *  5) Rasterization & Fragment Shading
 **************************************************************************/
void DrawPrimitive(PRIMITIVES prim, 
                   Buffer2D<PIXEL>& target,
                   const Vertex inputVerts[], 
                   const Attributes inputAttrs[],
                   Attributes* const uniforms,
                   FragmentShader* const frag,                   
                   VertexShader* const vert,
                   Buffer2D<double>* zBuf)
{
    // Setup count for vertices & attributes
    int numIn = 0;
    switch(prim)
    {
        case POINT:
            numIn = 1;
            break;
        case LINE:
            numIn = 2;
            break;
        case TRIANGLE:
            numIn = 3;
            break;
    }

    // Vertex shader 
    Vertex transformedVerts[MAX_VERTICES];
    Attributes transformedAttrs[MAX_VERTICES];
    VertexShaderExecuteVertices(vert, inputVerts, inputAttrs, numIn, uniforms, transformedVerts, transformedAttrs);

    // Vertex Interpolation & Fragment Drawing
    switch(prim)
    {
        case POINT:
            DrawPoint(target, transformedVerts, transformedAttrs, uniforms, frag);
            break;
        case LINE:
            DrawLine(target, transformedVerts, transformedAttrs, uniforms, frag);
            break;
        case TRIANGLE:
            DrawTriangle(target, transformedVerts, transformedAttrs, uniforms, frag);
    }
}

/*************************************************************
 * MAIN:
 * Main game loop, initialization, memory management
 ************************************************************/
int main()
{
    // -----------------------DATA TYPES----------------------
    SDL_Window* WIN;               // Our Window
    SDL_Renderer* REN;             // Interfaces CPU with GPU
    SDL_Texture* GPU_OUTPUT;       // GPU buffer image (GPU Memory)
    SDL_Surface* FRAME_BUF;        // CPU buffer image (Main Memory) 

    // ------------------------INITIALIZATION-------------------
    SDL_Init(SDL_INIT_EVERYTHING);
    WIN = SDL_CreateWindow(WINDOW_NAME, 200, 200, S_WIDTH, S_HEIGHT, 0);
    REN = SDL_CreateRenderer(WIN, -1, SDL_RENDERER_SOFTWARE);
    FRAME_BUF = SDL_CreateRGBSurface(0, S_WIDTH, S_HEIGHT, 32, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
    FRAME_BUF = SDL_ConvertSurface(SDL_GetWindowSurface(WIN), SDL_GetWindowSurface(WIN)->format, 0);
    GPU_OUTPUT = SDL_CreateTextureFromSurface(REN, FRAME_BUF);
    BufferImage frame(FRAME_BUF);

    // Draw loop 
    bool running = true;
    while(running) 
    {           
        // Handle user inputs
        processUserInputs(running);

        // Refresh Screen
        clearScreen(frame);

        // Your code goes here
        //TestDrawPixel(frame); //week1 >done
        //TestDrawTriangle(frame); //week2 >done
        //TestDrawFragments(frame); //week3 >done
        //TestDrawPerspectiveCorrect(frame); //week4 >doesnt work well
        TestVertexShader(frame); //week5 >done
        //TestPipeline(frame); //week6 >ongoing

        // Push to the GPU
        SendFrame(GPU_OUTPUT, REN, FRAME_BUF);
    }

    // Cleanup
    SDL_FreeSurface(FRAME_BUF);
    SDL_DestroyTexture(GPU_OUTPUT);
    SDL_DestroyRenderer(REN);
    SDL_DestroyWindow(WIN);
    SDL_Quit();
    return 0;
}
