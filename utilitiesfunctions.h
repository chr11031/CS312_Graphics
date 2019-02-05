#ifndef UTILITIES_FUNCTIONS_H
#define UTILITIES_FUNCTIONS_H

/**************************************************
 * GET_CROSS_PRODCUT
 * Calculates the cross product (determinant)
 * between two points from (ax, ay) and (bx, by)
 **************************************************/
double getCrossProduct(double ax, double ay, double bx, double by) 
{
    return ax * by - ay * bx;
}

/**************************************************
 * GET_INTERPOLATION
 * Calucate the interpolation using the 
 * Barycentric Coordinates method.
 **************************************************/

double getInterpolation(double x, double y, Vertex* const v, double attr1, double attr2, double attr3)
{
    double dom = (v[1].y - v[2].y) * (v[0].x - v[2].x) + (v[2].x - v[1].x) * (v[0].y - v[2].y);
    double w1 = ((v[1].y - v[2].y) * (x - v[2].x) + (v[2].x - v[1].x) * (y - v[2].y)) / dom;
    double w2 = ((v[2].y - v[0].y) * (x - v[2].x) + (v[0].x - v[2].x) * (y - v[2].y)) / dom;
    double w3 = 1.0 - w1 - w2;
    return ((w1 * attr1) + (w2 * attr2) + (w3 * attr3));
}

#endif