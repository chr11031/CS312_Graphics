#include "definitions.h"
#include <list>
#include <stdlib.h>
#ifndef SNAKE_H
#define SNAKE_H
#define START -25
#define STARTZ 50
#define CPF 150
#define INC 5
#define PIXEL       Uint32

using namespace std;

class Snake
{
public:
    bool playing;
    bool running = true;
    bool grid[8][8][8];
    Vertex head;
    Vertex tail;
    Vertex fruit;
    int facing;
    int camFacing;
    int length;
    list <Vertex> snake;
    Matrix model = translateMatrix(0, 0, 0);
    Matrix proj = perspectiveMatrix(60, 1.0, 1, 200);
    int counter;

    Snake()
    {
        for (int i = 0; i < 512; i++)
        {
            grid[(int)(i/64)][(int)((i/8) % 8)][(int)(i % 8)] = false;
        }
        facing = 4;
        camFacing = 1;
        grid[0][0][0] = true;
        grid[1][0][0] = true;
        grid[2][0][0] = true;
        playing = false;
        length = 4;
        Vertex temp = {0,0,0,0};
        tail = temp;
        snake.push_front(temp);
        temp.x += 1;
        snake.push_back(temp);
        temp.x += 1;
        head = temp;
        snake.push_back(temp);
        counter = 0;

        grid[5][5][0] = true;
        temp.x = temp.y = 5;
        fruit = temp;
    }
    
    void drawCube(Buffer2D<PIXEL> & target, Vertex vertices[8], PIXEL color)
    {
        static Buffer2D<double> zBuf(target.width(), target.height());
        Vertex verts[3];
        Attributes attr[3];
        for (int i = 0; i < 3; i++)
        {
            attr[i].color = color;
            attr[i].numValues = 0;
        }

        Attributes imageUniforms;
        imageUniforms.numValues = 0;
    	Matrix view = cameraMatrix(myCam.x, myCam.y, myCam.z, 
					   myCam.yaw, myCam.pitch, myCam.roll);
    	imageUniforms.matrix = model;
    	imageUniforms.matrix2 = view;
    	imageUniforms.matrix3 = proj;

        FragmentShader fragImg;
        fragImg.FragShader = flatColorFragShader;

        VertexShader vertImg;
        vertImg.VertShader = SimpleVertexShader2;
        verts[0] = vertices[0];
        if (camFacing < 4)
        {
            verts[1] = vertices[1];
            verts[2] = vertices[3];
            DrawPrimitive(TRIANGLE, target, verts, attr, &imageUniforms, &fragImg, &vertImg, &zBuf);
            verts[1] = verts[2];
            verts[2] = vertices[2];
            DrawPrimitive(TRIANGLE, target, verts, attr, &imageUniforms, &fragImg, &vertImg, &zBuf);
        }
        else
        {
            verts[1] = vertices[3];
            verts[2] = vertices[1];
            DrawPrimitive(TRIANGLE, target, verts, attr, &imageUniforms, &fragImg, &vertImg, &zBuf);
            verts[1] = vertices[2];
            verts[2] = vertices[3];
            DrawPrimitive(TRIANGLE, target, verts, attr, &imageUniforms, &fragImg, &vertImg, &zBuf);
        }
        if (camFacing != 5)
        {
            verts[1] = vertices[6];
            verts[2] = vertices[2];
            DrawPrimitive(TRIANGLE, target, verts, attr, &imageUniforms, &fragImg, &vertImg, &zBuf);
            verts[1] = vertices[4];
            verts[2] = vertices[6];
            DrawPrimitive(TRIANGLE, target, verts, attr, &imageUniforms, &fragImg, &vertImg, &zBuf);
        }
        else
        {
            verts[1] = vertices[2];
            verts[2] = vertices[6];
            DrawPrimitive(TRIANGLE, target, verts, attr, &imageUniforms, &fragImg, &vertImg, &zBuf);
            verts[1] = vertices[6];
            verts[2] = vertices[4];
            DrawPrimitive(TRIANGLE, target, verts, attr, &imageUniforms, &fragImg, &vertImg, &zBuf);
        }
        if (camFacing != 6)
        {
            verts[1] = vertices[5];
            verts[2] = vertices[4];
            DrawPrimitive(TRIANGLE, target, verts, attr, &imageUniforms, &fragImg, &vertImg, &zBuf);
            verts[1] = vertices[1];
            verts[2] = vertices[5];
            DrawPrimitive(TRIANGLE, target, verts, attr, &imageUniforms, &fragImg, &vertImg, &zBuf);
        }
        else
        {
            verts[1] = vertices[4];
            verts[2] = vertices[5];
            DrawPrimitive(TRIANGLE, target, verts, attr, &imageUniforms, &fragImg, &vertImg, &zBuf);
            verts[1] = vertices[5];
            verts[2] = vertices[1];
            DrawPrimitive(TRIANGLE, target, verts, attr, &imageUniforms, &fragImg, &vertImg, &zBuf);
        }
        verts[2] = vertices[7];
        if (camFacing < 4)
        {
            verts[1] = vertices[5];
            verts[0] = vertices[4];
            DrawPrimitive(TRIANGLE, target, verts, attr, &imageUniforms, &fragImg, &vertImg, &zBuf);
            verts[0] = vertices[6];
            verts[1] = vertices[4];
            DrawPrimitive(TRIANGLE, target, verts, attr, &imageUniforms, &fragImg, &vertImg, &zBuf);
        }
        else
        {
            verts[1] = vertices[4];
            verts[0] = vertices[5];
            DrawPrimitive(TRIANGLE, target, verts, attr, &imageUniforms, &fragImg, &vertImg, &zBuf);
            verts[0] = vertices[4];
            verts[1] = vertices[6];
            DrawPrimitive(TRIANGLE, target, verts, attr, &imageUniforms, &fragImg, &vertImg, &zBuf);
        }
        if (camFacing != 6)
        {
            verts[0] = vertices[6];
            verts[1] = vertices[2];
            DrawPrimitive(TRIANGLE, target, verts, attr, &imageUniforms, &fragImg, &vertImg, &zBuf);
            verts[0] = vertices[2];
            verts[1] = vertices[3];
            DrawPrimitive(TRIANGLE, target, verts, attr, &imageUniforms, &fragImg, &vertImg, &zBuf);
        }
        else
        {
            verts[0] = vertices[2];
            verts[1] = vertices[6];
            DrawPrimitive(TRIANGLE, target, verts, attr, &imageUniforms, &fragImg, &vertImg, &zBuf);
            verts[0] = vertices[3];
            verts[1] = vertices[2];
            DrawPrimitive(TRIANGLE, target, verts, attr, &imageUniforms, &fragImg, &vertImg, &zBuf);
        }
        if (camFacing != 5)
        {
            verts[0] = vertices[1];
            verts[1] = vertices[5];
            DrawPrimitive(TRIANGLE, target, verts, attr, &imageUniforms, &fragImg, &vertImg, &zBuf);
            verts[0] = vertices[3];
            verts[1] = vertices[1];
            DrawPrimitive(TRIANGLE, target, verts, attr, &imageUniforms, &fragImg, &vertImg, &zBuf);
        }
        else
        {
            verts[0] = vertices[5];
            verts[1] = vertices[1];
            DrawPrimitive(TRIANGLE, target, verts, attr, &imageUniforms, &fragImg, &vertImg, &zBuf);
            verts[0] = vertices[1];
            verts[1] = vertices[3];
            DrawPrimitive(TRIANGLE, target, verts, attr, &imageUniforms, &fragImg, &vertImg, &zBuf);
        }
        
    }

    void dealWithInputs()
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

            if(e.type == SDL_MOUSEMOTION)
		    {
			    int cur = SDL_ShowCursor(SDL_QUERY);
    			if(cur == SDL_DISABLE)
	    		{
		    		double mouseX = e.motion.xrel;
			    	double mouseY = e.motion.yrel;

    				myCam.yaw -= mouseX * 0.1;
	    			myCam.pitch += mouseY * 0.1;
    			}
	    	}

		    if(e.type == SDL_MOUSEBUTTONDOWN)
    		{
	    		int cur = SDL_ShowCursor(SDL_QUERY);
		    	if(cur == SDL_DISABLE)
    			{
	    			SDL_ShowCursor(SDL_ENABLE);
		    		SDL_SetRelativeMouseMode(SDL_FALSE);
			    }
    			else
	    		{
		    		SDL_ShowCursor(SDL_DISABLE);
			    	SDL_SetRelativeMouseMode(SDL_TRUE);
    			}
	    	}

    		// Movement
            if(e.type == SDL_KEYDOWN)
		    {
                int newAttemptFacing = 0;
                switch(e.key.keysym.sym)
                {
                case SDLK_SPACE:
                    playing = true;
                    break;
                case SDLK_w:
                    switch (camFacing)
                    {
                        case 6:
                            newAttemptFacing = 5;
                            break;
                        case 3:
                            newAttemptFacing = 6;
                            break;
                        default:
                            newAttemptFacing = 1;
                            break;
                    }
                    break;
                case SDLK_s:
                    switch (camFacing)
                    {
                        case 6:
                            newAttemptFacing = 6;
                            break;
                        case 3:
                            newAttemptFacing = 5;
                            break;
                        default:
                            newAttemptFacing = 2;
                            break;
                    }
                    break;
                case SDLK_a:
                    switch (camFacing)
                    {
                        case 2:
                            newAttemptFacing = 5;
                            break;
                        case 4:
                            newAttemptFacing = 4;
                            break;
                        case 5:
                            newAttemptFacing = 6;
                            break;
                        default:
                            newAttemptFacing = 3;
                            break;
                    }
                    break;
                case SDLK_d:
                    switch (camFacing)
                    {
                        case 2:
                            newAttemptFacing = 6;
                            break;
                        case 4:
                            newAttemptFacing = 3;
                            break;
                        case 5:
                            newAttemptFacing = 5;
                            break;
                        default:
                            newAttemptFacing = 4;
                            break;
                    }
                    break;
                case SDLK_f:
                    switch (camFacing)
                    {
                        case 2:
                            newAttemptFacing = 4;
                            break;
                        case 3:
                            newAttemptFacing = 1;
                            break;
                        case 4:
                            newAttemptFacing = 6;
                            break;
                        case 5:
                            newAttemptFacing = 3;
                            break;
                        case 6:
                            newAttemptFacing = 2;
                            break;
                        default:
                            newAttemptFacing = 5;
                            break;
                    }
                    break;
                case SDLK_t:
                    switch (camFacing)
                    {
                        case 2:
                            newAttemptFacing = 3;
                            break;
                        case 3:
                            newAttemptFacing = 2;
                            break;
                        case 4:
                            newAttemptFacing = 5;
                            break;
                        case 5:
                            newAttemptFacing = 4;
                            break;
                        case 6:
                            newAttemptFacing = 1;
                            break;
                        default:
                            newAttemptFacing = 6;
                            break;
                    }
                    break;
                case SDLK_1:
                    camFacing = 1;
                    myCam.x = 0;
                    myCam.y = 0;
                    myCam.z = 0;
                    myCam.yaw = 0;
                    myCam.pitch = 0;
                    break;
                case SDLK_2:
                    camFacing = 2;
                    myCam.x = 75;
                    myCam.y = 0;
                    myCam.z = 75;
                    myCam.yaw = 270;
                    myCam.pitch = 0;
                    break;
                case SDLK_3:
                    camFacing = 3;
                    myCam.x = 0;
                    myCam.y = 75;
                    myCam.z = 75;
                    myCam.pitch = 90;
                    myCam.yaw = 0;
                    break;
                case SDLK_4:
                    camFacing = 4;
                    myCam.x = 0;
                    myCam.y = 0;
                    myCam.z = 150;
                    myCam.yaw = 180;
                    myCam.pitch = 0;
                    break;
                case SDLK_5:
                    camFacing = 5;
                    myCam.x = -75;
                    myCam.y = 0;
                    myCam.z = 75;
                    myCam.yaw = 90;
                    myCam.pitch = 0;
                    break;
                case SDLK_6:
                    camFacing = 6;
                    myCam.x = 0;
                    myCam.y = -75;
                    myCam.z = 75;
                    myCam.pitch = 270;
                    myCam.yaw = 0;
                    break;
                default:
                    break;
                }
                switch (newAttemptFacing)
                {
                case 1:
                    if (facing != 2)
                        facing =1;
                    break;
                case 2:
                    if (facing != 1)
                        facing = 2;
                    break;
                case 3:
                    if (facing != 4)
                        facing = 3;
                    break;
                case 4:
                    if (facing != 3)
                        facing = 4;
                    break;
                case 5:
                    if (facing != 6)
                        facing = 5;
                    break;
                case 6:
                    if (facing != 5)
                        facing = 6;
                    break;
                default:
                    break;
                }
            
            }
            
        }
    }

    void makeNewFruit()
    {
        int v1 = rand() % 8;
        int v2 = rand() % 8;
        int v3 = rand() % 8;
        while(grid[v1][v2][v3])
        {
            v1 = rand() % 8;
            v2 = rand() % 8;
            v3 = rand() % 8;
        }
        Vertex temp = {v1, v2, v3, 0};
        grid[v1][v2][v3] = true;
        fruit = temp;
        length += 1;
    }

    void update()
    {
        Vertex temp = snake.back();
        switch (facing)
        {
            case 6:
                temp.z += 1;
                break;
            case 5:
                temp.z -= 1;
                break;
            case 4:
                temp.x += 1;
                break;
            case 3:
                temp.x -= 1;
                break;
            case 2:
                temp.y -= 1;
                break;
            case 1:
            default:
                temp.y += 1;
                break;
        }
        head = temp;
        //death check
        if (temp.x > 7 || temp.x < 0 || 
            temp.y > 7 || temp.y < 0 || 
            temp.z < 0 || temp.z > 7)
        {
            playing = false;
            return;
        }

        snake.push_back(head);
        if (snake.size() == length)
        {
            Vertex temp = snake.front();
            grid[(int)(temp.x)][(int)(temp.y)][(int)(temp.z)] = false;
            snake.pop_front();
        }
        //death check
        if (!grid[(int)(temp.x)][(int)(temp.y)][(int)(temp.z)])
        {
            grid[(int)(temp.x)][(int)(temp.y)][(int)(temp.z)] = true;
        }
        else if (head == fruit)
        {
            makeNewFruit();
        }
        else
        {
            playing = false;
            return;
        }
    }

    void drawSnake(Buffer2D<PIXEL> & target)
    {
        //fruit
        Vertex v1 = {START + INC * fruit.x,  START + INC * fruit.y,  STARTZ + INC * fruit.z, 1};
        Vertex v2 = {START + INC * fruit.x + 5,  START + INC * fruit.y,  STARTZ + INC * fruit.z, 1};
        Vertex v3 = {START + INC * fruit.x,  START + INC * fruit.y + 5,  STARTZ + INC * fruit.z, 1};
        Vertex v4 = {START + INC * fruit.x + 5,  START + INC * fruit.y + 5,  STARTZ + INC * fruit.z, 1};
        Vertex v5 = {START + INC * fruit.x,  START + INC * fruit.y,  STARTZ + INC * fruit.z + 5, 1};
        Vertex v6 = {START + INC * fruit.x + 5,  START + INC * fruit.y,  STARTZ + INC * fruit.z + 5, 1};
        Vertex v7 = {START + INC * fruit.x,  START + INC * fruit.y + 5,  STARTZ + INC * fruit.z + 5, 1};
        Vertex v8 = {START + INC * fruit.x + 5,  START + INC * fruit.y + 5,  STARTZ + INC * fruit.z + 5, 1};

        Vertex vertices[8] = {v1, v2, v3, v4, v5, v6, v7, v8};
        drawCube(target, vertices, 0xFFFF0000);
        
        for (list<Vertex> :: iterator it = snake.begin(); it != snake.end(); ++it)
        {
            Vertex v1 = {START + INC * (*it).x,  START + INC * (*it).y,  STARTZ + INC * (*it).z, 1};
            Vertex v2 = {START + INC * (*it).x + 5,  START + INC * (*it).y,  STARTZ + INC * (*it).z, 1};
            Vertex v3 = {START + INC * (*it).x,  START + INC * (*it).y + 5,  STARTZ + INC * (*it).z, 1};
            Vertex v4 = {START + INC * (*it).x + 5,  START + INC * (*it).y + 5,  STARTZ + INC * (*it).z, 1};
            Vertex v5 = {START + INC * (*it).x,  START + INC * (*it).y,  STARTZ + INC * (*it).z + 5, 1};
            Vertex v6 = {START + INC * (*it).x + 5,  START + INC * (*it).y,  STARTZ + INC * (*it).z + 5, 1};
            Vertex v7 = {START + INC * (*it).x,  START + INC * (*it).y + 5,  STARTZ + INC * (*it).z + 5, 1};
            Vertex v8 = {START + INC * (*it).x + 5,  START + INC * (*it).y + 5,  STARTZ + INC * (*it).z + 5, 1};

            Vertex vertices[8] = {v1, v2, v3, v4, v5, v6, v7, v8};
            drawCube(target, vertices, 0xFF359907);
        }
    }
    void drawZSnake(Buffer2D<PIXEL> & target)
    {
        int x = 0;
        int y = 0;
        int z = 0;
        for (int i1 = 0; i1 < 8; i1++)
        for (int i2 = 0; i2 < 8; i2++)
        for (int i3 = 0; i3 < 8; i3++)
        {
            switch (camFacing)
            {
                case 6:
                    z = i2;
                    x = i3;
                    y = 7-i1;
                    break;
                case 5:
                    x = 7-i1;
                    y = i2;
                    z = i3;
                    break;
                case 4:
                    z = i1;
                    y = i2;
                    x = i3;
                    break;
                case 3:
                    y = i1;
                    x = i2;
                    z = i3;
                    break;
                case 2:
                    x = i1;
                    y = i2;
                    z = i3;
                    break;
                case 1:
                default:
                    z = 7 - i1;
                    x = i2;
                    y = i3;
                    break;
            }
            if (grid[x][y][z])
            {
            Vertex v1 = {START + INC * x,  START + INC * y,  STARTZ + INC * z, 1};
            Vertex v2 = {START + INC * x + 5,  START + INC * y,  STARTZ + INC * z, 1};
            Vertex v3 = {START + INC * x,  START + INC * y + 5,  STARTZ + INC * z, 1};
            Vertex v4 = {START + INC * x + 5,  START + INC * y + 5,  STARTZ + INC * z, 1};
            Vertex v5 = {START + INC * x,  START + INC * y,  STARTZ + INC * z + 5, 1};
            Vertex v6 = {START + INC * x + 5,  START + INC * y,  STARTZ + INC * z + 5, 1};
            Vertex v7 = {START + INC * x,  START + INC * y + 5,  STARTZ + INC * z + 5, 1};
            Vertex v8 = {START + INC * x + 5,  START + INC * y + 5,  STARTZ + INC * z + 5, 1};

            Vertex vertices[8] = {v1, v2, v3, v4, v5, v6, v7, v8};

            drawCube(target, vertices, ( (fruit.x == x && fruit.y == y && fruit.z == z) ? 0xFFFF0000 : 0xFF359907));
            }
        }
    }
};

void DrawGrid(Buffer2D<PIXEL> & target)
{

    static Buffer2D<double> zBuf(target.width(), target.height());
    Vertex verts[3];
    Attributes attr[3];
    for (int i = 0; i < 3; i++)
    {
        attr[i].color = 0xFFFFFFFF;
        attr[i].numValues = 0;
    }

    Attributes imageUniforms;
        imageUniforms.numValues = 0;
	Matrix model = translateMatrix(0, 0, 0);
	Matrix view = cameraMatrix(myCam.x, myCam.y, myCam.z, 
					   myCam.yaw, myCam.pitch, myCam.roll);
	Matrix proj = perspectiveMatrix(60, 1.0, 1, 200);
	imageUniforms.matrix = model;
	imageUniforms.matrix2 = view;
	imageUniforms.matrix3 = proj;


    FragmentShader fragImg;
    fragImg.FragShader = flatColorFragShader;

    VertexShader vertImg;
    vertImg.VertShader = SimpleVertexShader2;

    verts[0].w = 1;
    verts[1].w = 1;
    verts[2].w = 1;

    verts[0].y = -25;
    verts[1].y = 15;
    verts[2].y = 16;
    for (int x = 0; x <= 8; x++)
    {
        verts[0].x = verts[1].x = verts[2].x = -25 + (5 * x);
        for(int z = 0; z <= 1; z++)
        {
            verts[2].z = verts[0].z = verts[1].z = 50 + (40 * z);
            DrawPrimitive(TRIANGLE, target, verts, attr, &imageUniforms, &fragImg, &vertImg, &zBuf);
        }
    }
    
    verts[0].z = 50;
    verts[1].z = 90;
    verts[2].z = 91;
    for (int x = 0; x <= 1; x++)
    {
        verts[0].x = verts[1].x = verts[2].x = -25 +  (40 * x);
        for(int y = 0; y <= 8; y++)
        {
            verts[2].y = verts[0].y = verts[1].y = -25 +  (5 * y);
            DrawPrimitive(TRIANGLE, target, verts, attr, &imageUniforms, &fragImg, &vertImg, &zBuf);
        }
    }
    verts[0].x = -25;
    verts[1].x = 15;
    verts[2].x = 16;
    for (int y = 0; y <= 1; y++)
    {
        verts[2].y = verts[0].y = verts[1].y = -25 +  (40 * y);
        for(int z = 0; z <= 8; z++)
        {
            verts[2].z = verts[0].z = verts[1].z = 50 + (5 * z);
            DrawPrimitive(TRIANGLE, target, verts, attr, &imageUniforms, &fragImg, &vertImg, &zBuf);
        }
    }
    
}

/************************************************
 * Personal Project #1: 3D Snake
 ************************************************/
Snake PlaySnake(Buffer2D<PIXEL> & target, Snake game)
{
    DrawGrid(target);
    game.dealWithInputs();
    if (game.playing)
        game.update();
    game.drawZSnake(target);
    
    return game;
}


#endif