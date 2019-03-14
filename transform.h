#ifndef TRANSFORM_H
#define TRANSFORM_H

class Transform
{

};

// moves a camera
Transform camera4x4(const double &offX, const double &offY, const double &offZ,
                     const double &yaw, const double &pitch, const double &roll)
{
   Transform trans = translate4x4(-offX, -offY, -offZ);

   Transform rotX = rotate(x, -pitch);
   Transform rotY = rotate(y, -yaw);

   Transform rt = rotX * rotY * trans;

   return rt;
}

// Rotate a 4x4 matrix
Transform rotate4x4(const DIMENSION & dim, const double & degs)
{
   Transform tr(4, 4);
   double rads = degs * M_PI / 180.0;
   double cosT = cos(rads);
   double sinT = sin(rads);

   tr[0][0] = 1;
   tr[0][1] = 0;
   tr[0][2] = 0;
   tr[0][3] = 0;
   tr[1][0] = 0;
   tr[1][1] = 1;
   tr[1][2] = 0;
   tr[1][3] = 0;
   tr[2][0] = 0;
   tr[2][1] = 0;
   tr[2][2] = 1;
   tr[2][3] = 0;
   tr[3][0] = 0;
   tr[3][1] = 0;
   tr[3][2] = 0;
   tr[3][3] = 1;

   switch(dim)
   {
      case X:
         tr[1][1] = cosT;
         tr[1][2] = -sinT;
         tr[2][1] = sinT;
         tr[2][2] = cosT;
         break;
      case Y:
         tr[0][0] = cosT;
         tr[0][2] = sinT;
         tr[2][0] = -sinT;
         tr[2][2] = cosT;
         break;
      case Z:
         tr[0][0] = cosT;
         tr[0][1] = -sinT;
         tr[1][0] = sinT;
         tr[1][1] = cosT;
         break;
   }

   return tr;
}

// scales a 4x4 matrix
Transfrorm scale4x4(const double & scale)
{
   Transform tr(4, 4);
   tr[0][0] = scale;
   tr[0][1] = 0;
   tr[0][2] = 0;
   tr[0][3] = 0;
   tr[1][0] = 0;
   tr[1][1] = scale;
   tr[1][2] = 0;
   tr[1][3] = 0;
   tr[2][0] = 0;
   tr[2][1] = 0;
   tr[2][2] = scale;
   tr[2][3] = 0;
   tr[3][0] = 0;
   tr[3][1] = 0;
   tr[3][2] = 0;
   tr[3][3] = 1;

   return tr;
}

// the perspective projection matrix multiplier
Transform perspective4x4(const double &fovY, const double &aspectRatio,
                         const double &near, const double &far)
{
   Transform rt;

   double top = near * tan((fovYDeg * M_PI) / 180.0) / 2.0;
   double right = aspectRatio * top;

   rt[0][0] = near / right;
   rt[0][1] = 0;
   rt[0][2] = 0;
   rt[0][3] = 0;

   rt[1][0] = 0; 
   rt[1][1] = near /top;
   rt[1][2] = 0;
   rt[1][3] = 0;
   rt[1][4] = 0;

   rt[2][0] = 
   rt[2][1] = 
   rt[2][2] = (far + near) / (far - near);
   rt[2][3] = (-2 * far * near) / (far - near);

   rt[3][0] = 0;
   rt[3][1] = 0;
   rt[3][2] = 1;
   rt[3][3] = 0;

   return rt;
}

// translates a 4x4 matrix
Transform translate4x4(const double a, const double b, const double c)
{
   Transform tr(4, 4);
   tr[0][0] = 1;
   tr[0][1] = 0;
   tr[0][2] = 0;
   tr[0][3] = offX;

   tr[1][0] = 0;
   tr[1][1] = 1;
   tr[1][2] = 0;
   tr[1][3] = offY;

   tr[2][0] = 0;
   tr[2][1] = 0;
   tr[2][2] = 1;
   tr[2][3] = offZ;

   tr[3][0] = 0;
   tr[3][1] = 0;
   tr[3][2] = 0;
   tr[3][3] = 1;

   return tr;
}

#endif