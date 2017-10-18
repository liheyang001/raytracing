#include "Cylinder.h"
#include <math.h>

float Cylinder::intersect(glm::vec3 posn, glm::vec3 dir)
{   
    float a = (dir.x * dir.x) + (dir.z * dir.z);
    float b = 2*(dir.x*(posn.x-center.x) 
    + dir.z*(posn.z-center.z));
    float c = (posn.x - center.x) * (posn.x - center.x) 
    + (posn.z - center.z) * (posn.z - center.z) - (radius*radius);
    
    float delta = b*b - 4*(a*c);
	if(fabs(delta) < 0.001) return -1.0; 
    if(delta < 0.0) return -1.0;
    
    float t1 = (-b - sqrt(delta))/(2*a);
    float t2 = (-b + sqrt(delta))/(2*a);
    float t;
    
    if (t1>t2) t = t2;
    else t = t1;
    
    float r = posn.y + t*dir.y;
    
    if ((r >= center.y) and (r <= center.y + height))return t;
    else return -1;
}


glm::vec3 Cylinder::normal(glm::vec3 p)
{
    glm::vec3 n = glm::vec3 (p.x-center.x,0,p.z-center.z);
    n = glm::normalize(n);
    return n;
}
