// Includes go here

/**************************************************
 * INTERPOLATE
 * This function will take the area, determinants, 
 * and attributes for a specified vertex and 
 * calculate linear interpolation for them.
 * ************************************************/
double interp(const double areaTriangle, 
              const double firstDet, 
              const double secndDet, 
              const double thirdDet, 
              const double attr1, 
              const double attr2, 
              const double attr3)
{
    // This follows the equation in the reading. 
    // The order of the attr's is important so it matches up to the 
    // CCW order displayed in the slides. 
    return (((firstDet / areaTriangle) * attr3) + 
            ((secndDet / areaTriangle) * attr1) + 
            ((thirdDet / areaTriangle) * attr2));
}