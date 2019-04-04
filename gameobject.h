#include "definitions.h"
#include "matrix.h"
#include "shaders.h"
#include "model.h"

#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

enum DRAW_MODE 
{
    COLOR,
    IMAGE
};

class GameObject
{
    private:
        Model* model;
        Matrix transform;

        double x = 0;
        double y = 0;
        double z = 0;

        double rotationX = 0;
        double rotationY = 0;
        double rotationZ = 0;

        double scaleX = 1;
        double scaleY = 1;
        double scaleZ = 1;

        void updateTransform()
        {
            transform = TranslationMatrix(x, y, z) *
                        ScaleMatrix(scaleX, scaleY, scaleZ) * 
                        XRotationMatrix(rotationX) * 
                        ZRotationMatrix(rotationY) * 
                        ZRotationMatrix(rotationX);
        }

    public:
        DRAW_MODE mode;

        BufferImage* texture;
        double r = 0.0;
        double g = 0.0;
        double b = 0.0;

        bool lightEnabled;

        GameObject(Model* model, BufferImage* texture, bool lightEnabled = false)
        {
            this->model = model;
            this->texture = texture;
            this->lightEnabled = lightEnabled;
            this->mode = IMAGE;
        }

        GameObject(Model* model, double r, double g, double b, bool lightEnabled = false)
        {
            this->model = model;
            this->r = r;
            this->g = g;
            this->b = b;
            this->lightEnabled = lightEnabled;
            this->mode = COLOR;
        }

        double getX() { return x; }
        double getY() { return y; }
        double getZ() { return z; }

        void setPosition(double x, double y, double z)
        {
            this->x = x;
            this->y = y;
            this->z = z;

            updateTransform();
        }

        double getRotationX() { return rotationX; }
        double getRotationY() { return rotationY; }
        double getRotationZ() { return rotationZ; }

        void setRotation(double x, double y, double z)
        {
            rotationX = x;
            rotationY = y;
            rotationZ = z;

            updateTransform();
        }

        double getScaleX() { return rotationX; }
        double getScaleY() { return scaleY; }
        double getScaleZ() { return scaleZ; }

        void setScale(double x, double y, double z)
        {
            scaleX = x;
            scaleY = y;
            scaleZ = z;

            updateTransform();
        }

        void draw(Buffer2D<PIXEL> &target,
                  Matrix &view,
                  Matrix &projection,
                  Buffer2D<double>* zBuf)
        {
            Vertex tri[3];

			Attributes vAttr[3];

            Attributes uniforms;
            uniforms.insertPtr(texture);
            uniforms.insertPtr(&transform);
            uniforms.insertPtr(&view);
            uniforms.insertPtr(&projection);

            if(mode == COLOR)
            {
                uniforms.insertDbl(r);
                uniforms.insertDbl(g);
                uniforms.insertDbl(b);
            }

            FragmentShader frag;
            if(mode == COLOR)
                frag.FragShader = UniformColorFragShader;
            else if(mode == IMAGE)
                frag.FragShader = ImageFragShader;
            else
                frag.FragShader = DefaultFragShader;

            VertexShader vert;
            vert.VertShader = SimpleVertexShader;


            int indexCount = model->vertexIndices.size();
            for(int i = 0; i < indexCount; )
            {
                if(mode == IMAGE)
                {
                    vAttr[2].clear();
                    tri[2] = model->vertices[model->vertexIndices[i]];
                    vAttr[2].insertDbl(model->uvs[model->uvIndices[i]].u);
                    vAttr[2].insertDbl(model->uvs[model->uvIndices[i++]].v);

                    vAttr[1].clear();
                    tri[1] = model->vertices[model->vertexIndices[i]];
                    vAttr[1].insertDbl(model->uvs[model->uvIndices[i]].u);
                    vAttr[1].insertDbl(model->uvs[model->uvIndices[i++]].v);

                    vAttr[0].clear();
                    tri[0] = model->vertices[model->vertexIndices[i]];
                    vAttr[0].insertDbl(model->uvs[model->uvIndices[i]].u);
                    vAttr[0].insertDbl(model->uvs[model->uvIndices[i++]].v);
                }
                else
                {
                    tri[2] = model->vertices[model->vertexIndices[i++]];
                    tri[1] = model->vertices[model->vertexIndices[i++]];
                    tri[0] = model->vertices[model->vertexIndices[i++]];
                }

                DrawPrimitive(TRIANGLE, target, tri, vAttr, &uniforms, &frag, &vert, zBuf);
            }
        }
};

#endif