#include "SDL2/SDL.h"

#ifndef CONTROLS_H
#define CONTROLS_H

#define DEG_TO_RAD M_PI / 180.0

#define WALKING_SPEED 0.2

// Everything needed for the view/camera transform
struct camControls
{
    double x = 0;
    double y = 0;
    double z = 0;
    double yaw = 0;
    double roll = 0;
    double pitch = 0;
};
camControls myCam;

void firstPersonCamControl(SDL_Event e)
{
    if(e.type == SDL_MOUSEMOTION)
        {
            int cur = SDL_ShowCursor(SDL_QUERY);
            if(cur == SDL_DISABLE)
            {
                double mouseX = e.motion.xrel;
                double mouseY = e.motion.yrel;
                myCam.yaw += mouseX * 0.02;
                myCam.pitch += mouseY * 0.02;
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

        if((e.key.keysym.sym == 'w' && e.type == SDL_KEYDOWN))
        {
            myCam.z += cos(myCam.yaw * DEG_TO_RAD) * WALKING_SPEED;
            myCam.x += sin(myCam.yaw * DEG_TO_RAD) * WALKING_SPEED;
        }
        if((e.key.keysym.sym == 's' && e.type == SDL_KEYDOWN))
        {
            myCam.z -= cos(myCam.yaw * DEG_TO_RAD) * WALKING_SPEED;
            myCam.x -= sin(myCam.yaw * DEG_TO_RAD) * WALKING_SPEED;
        }
        if((e.key.keysym.sym == 'a' && e.type == SDL_KEYDOWN))
        {
            myCam.x -= cos(myCam.yaw * DEG_TO_RAD) * WALKING_SPEED;
            myCam.z += sin(myCam.yaw * DEG_TO_RAD) * WALKING_SPEED;
        }
        if((e.key.keysym.sym == 'd' && e.type == SDL_KEYDOWN))
        {
            myCam.x += cos(myCam.yaw * DEG_TO_RAD) * WALKING_SPEED;
            myCam.z -= sin(myCam.yaw * DEG_TO_RAD) * WALKING_SPEED;
        }
}

#endif