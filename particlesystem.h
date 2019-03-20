#include <chrono>
#include "definitions.h"
#include "matrix.h"
#include "shaders.h"

#ifndef PARTICLE_SYSTEM_H
#define PARTICLE_SYSTEM_H

struct Particle
{
    Normal direction;
    double r;
    double g;
    double b;
    int startFrame;
};

class ParticleSystem
{
    private:
        Particle* particles;
        int maxParticles;
        int particleCount = 0;
        int offset = 0;
        Vertex position;
        Vertex direction;
        double angleVariance;
        double speed;
        double speedVariance;
        int currentFrame = 0;


    public:
        ParticleSystem(int maxParticles, Vertex position, Vertex direction, double angleVariance, double speed, double speedVariance)
        {
            particles = new Particle[maxParticles];
            this->maxParticles = maxParticles;
            this->position = position;
            this->direction = direction;
            this->angleVariance = angleVariance;
            this->speed = speed;
            this->speedVariance = speedVariance;
        }

        ~ParticleSystem()
        {
            delete[] particles;
        }

        void addParticle(double r, double g, double b)
        {
            Matrix rotation = XRotationMatrix(DEG_TO_RAD * ((randomDouble() - 0.5) * angleVariance)) *
                              YRotationMatrix(DEG_TO_RAD * ((randomDouble() - 0.5) * angleVariance)) *
                              ZRotationMatrix(DEG_TO_RAD * ((randomDouble() - 0.5) * angleVariance));

            Vertex dir = rotation * direction;
            double speed = (randomDouble() - 0.5) * speedVariance + this->speed;

            Particle* p = particles + offset;
            p->direction.x = dir.x * speed;
            p->direction.y = dir.y * speed;
            p->direction.z = dir.z * speed;
            p->r = r;
            p->g = g;
            p->b = b;
            p->startFrame = currentFrame;

            offset++;
            if(offset >= maxParticles)
                offset = 0;

            particleCount++;
            if(particleCount > maxParticles)
                particleCount = maxParticles;
        }

        void draw(Buffer2D<PIXEL> &target,
                  Matrix &view,
                  Matrix &projection,
                  Buffer2D<double>* zBuf)
        {
            Particle* p; 

            Attributes uniforms;
            uniforms.insertDbl(currentFrame);
            uniforms.insertPtr(&view);
            uniforms.insertPtr(&projection);

            FragmentShader frag;
            frag.FragShader = ParticleFragShader;

            VertexShader vert;
            vert.VertShader = ParticleVertexShader;

            for(int i = 0; i < particleCount; i++)
            {
                p = particles + i;

                Attributes vAttr;
                vAttr.insertDbl(p->r);
                vAttr.insertDbl(p->g);
                vAttr.insertDbl(p->b);
                vAttr.insertDbl(p->direction.x);
                vAttr.insertDbl(p->direction.y);
                vAttr.insertDbl(p->direction.z);
                vAttr.insertDbl(p->startFrame);

                DrawPrimitive(POINT, target, &position, &vAttr, &uniforms, &frag, &vert, zBuf);
            }
            currentFrame++;
        }
};

#endif