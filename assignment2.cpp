/*
COSC363 assignment 2
Ray-Tracing
Minimum features:
Lighting: at least one light source and also diffuse specular reflections(done)
Shadows(done)
Reflections: the scene must include at least one reflective sphere. (done)
The scene must include at least one box, the box need not be defined as a single object. (done)
There must be at least one planar surface in the scene or a "texture" on it.(done)

Extensions:
Primitives others than a plane, sphere or box. (2 mark) (done)
Anti-aliasing.(1 mark)(done)
Transparent object.(1 mark)(done)
Refractions.(2 marks)(done)			
non-planar object textured using an image.(1 mark) (done)

*/
// g++ -Wall -o "%e" TextureBMP.cpp assignment2.cpp Plane.cpp Ray.cpp SceneObject.cpp Cylinder.cpp Sphere.cpp -lm -lGL -lGLU -lglut -lGLray

#include <iostream>
#include <cmath>
#include <vector>
#include <glm/glm.hpp>
#include "Sphere.h"
#include "SceneObject.h"
#include "Ray.h"
#include <GL/glut.h>
#include "Plane.h"
#include "TextureBMP.h"
#include <GL/glut.h>
#include "Cylinder.h"

using namespace std;

TextureBMP texture1;
TextureBMP texture2;

const float WIDTH = 20.0;  
const float HEIGHT = 20.0;
const float EDIST = 40.0;
const int NUMDIV = 500;
const int MAX_STEPS = 5;
const float XMIN = -WIDTH * 0.5;
const float XMAX =  WIDTH * 0.5;
const float YMIN = -HEIGHT * 0.5;
const float YMAX =  HEIGHT * 0.5;

vector<SceneObject*> sceneObjects;  //A global list containing pointers to objects in the scene

glm::vec3 trace(Ray ray, int step)
{
	glm::vec3 backgroundCol(0);
	glm::vec3 light(10, 40, -3);
	float ambientTerm = 0.2;
    ray.closestPt(sceneObjects);		//Compute the closest point of intersetion of objects with the ray
    //float transpCoeff = 0.2;
    glm::vec3 colorSum;

    if(ray.xindex == -1) return backgroundCol;      //If there is no intersection return background colour

    glm::vec3 col = sceneObjects[ray.xindex]->getColor(); //else return bject's colour
    glm::vec3 normalVector = sceneObjects[ray.xindex]->normal(ray.xpt);
    glm::vec3 lightVector = light -ray.xpt;
    glm::vec3 lightNormal = glm::normalize(lightVector);
    glm::vec3 reflVector = glm::reflect(-lightVector,normalVector);
    reflVector= glm::normalize(reflVector);
    //float specCol = 0.1;
    glm::vec3 specCol;
    float f = 30;
    glm::vec3 specularCol = glm::vec3(1);
    glm::vec3 viewVector (-ray.dir); 
    Ray shadow(ray.xpt, normalize(lightVector));
	shadow.closestPt(sceneObjects);
    int lightDist = glm::length(lightVector); //how to calculate lightdist 
    float lDotn = glm ::dot(lightNormal, normalVector);
    float rDotv = glm ::dot(reflVector,viewVector);
    


//==========================texture ball================================
    if (ray.xindex == 1){
		float a = asin(normalVector.x)/M_PI + 0.5;
		float b = asin(normalVector.y)/M_PI + 0.5;
		col = texture2.getColorAt(a, b);
		}
//======================================================================

//=============================wall color===============================
	if (ray.xindex == 8){
		float texcoords = -(ray.xpt.x - 50)/(100);
		float texcoordt = -(ray.xpt.y - 30)/(50);
		col = texture1.getColorAt(texcoordt, texcoords);
		}
//======================================================================
	

    if (rDotv < 0){
		glm::vec3 specCol(0);
		}
	else{
		specCol = (pow(rDotv, f)) * specularCol;
		}
    
    /*if (lDotn < 0){
		return ambientTerm*col;}
	else{
		return ambientTerm*col + lDotn*col + specCol;}
		*/
	
	if ((lDotn <= 0) || ((shadow.xindex > -1) && (shadow.xdist < lightDist))){
		return ambientTerm * col;} 
	else{
		colorSum = ambientTerm * col + lDotn * col + specCol;
		}
		
	if(ray.xindex == 0 && step < MAX_STEPS){
		glm::vec3 reflectedDir = glm::reflect(ray.dir, normalVector);
		Ray reflectedRay(ray.xpt, reflectedDir);
		glm::vec3 reflectedCol = trace(reflectedRay, step+1);//Recursion!
		colorSum = colorSum + (0.8f*reflectedCol);  
		}

//===============================tranparent=============================		

	if (ray.xindex == 11 && step < MAX_STEPS)
	{
		float trans = 1/1.02;
		glm::vec3 n = sceneObjects[ray.xindex]->normal(ray.xpt);
		glm::vec3 i = glm::refract(ray.dir, n, trans);
		Ray in(ray.xpt, i);
		in.closestPt(sceneObjects);
		if(in.xindex == -1) 
		return backgroundCol; 
		glm::vec3 k = sceneObjects[in.xindex]->normal(in.xpt);
		glm::vec3 j = glm::refract(i,-k, 1.0f/trans);
		Ray out(in.xpt, j);
		out.closestPt(sceneObjects);
		if(out.xindex == -1)
		return backgroundCol; 
		glm::vec3 tranCol = trace(out, step+1);
		colorSum = colorSum * glm::vec3(0.2) + tranCol;
		return colorSum;
}
//======================================================================

//=================================Refraction===========================
if (ray.xindex == 12 && step < MAX_STEPS)
	{
		float ref = 1/1.5;
		glm::vec3 n = sceneObjects[ray.xindex]->normal(ray.xpt);
		glm::vec3 a = glm::refract(ray.dir, n, ref);
		Ray inref(ray.xpt, a);
		inref.closestPt(sceneObjects);
		if(inref.xindex == -1) 
		return backgroundCol; 
		glm::vec3 b = sceneObjects[inref.xindex]->normal(inref.xpt);
		glm::vec3 c = glm::refract(a,-b, 1.0f/ref);
		Ray outref(inref.xpt, c);
		outref.closestPt(sceneObjects);
		if(outref.xindex == -1)
		return backgroundCol; 
		glm::vec3 refCol = trace(outref, step+1);
		colorSum = colorSum * glm::vec3(0.2) + refCol;
		return colorSum;
}
//======================================================================
	return colorSum;
	
	
}

//---The main display module -----------------------------------------------------------
// In a ray tracing application, it just displays the ray traced image by drawing
// each cell as a quad.
//---------------------------------------------------------------------------------------
void display()
{
	float xp, yp;  //grid point
	float cellX = (XMAX-XMIN)/NUMDIV;  //cell width
	float cellY = (YMAX-YMIN)/NUMDIV;  //cell height

	glm::vec3 eye(0., 0., 0.);  //The eye position (source of primary rays) is the origin

	glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

	glBegin(GL_QUADS);  //Each cell is a quad.

	for(int i = 0; i < NUMDIV; i++)  	//For each grid point xp, yp
	{
		xp = XMIN + i*cellX;
		for(int j = 0; j < NUMDIV; j++)
		{
			yp = YMIN + j*cellY;

/*			
			glm::vec3 dir1(xp+0.25*cellX, yp+0.25*cellY, -EDIST);	//direction of the primary ray

			 
		    Ray ray1 = Ray(eye, dir1);		//Create a ray originating from the camera in the direction 'dir'

			
			ray1.normalize();				//Normalize the direction of the ray to a unit vector

			
		    glm::vec3 col1 = trace (ray1, 1); //Trace the primary ray and get the colour value

		    
		    glColor3f(col1.r, col1.g, col1.b);


*/
		    glm::vec3 dir1(xp+0.25*cellX, yp+0.25*cellY, -EDIST);	//direction of the primary ray
			glm::vec3 dir2(xp+0.75*cellX, yp+0.75*cellY, -EDIST);
			glm::vec3 dir3(xp+0.25*cellX, yp+0.75*cellY, -EDIST);
			glm::vec3 dir4(xp+0.75*cellX, yp+0.25*cellY, -EDIST);
			 
		    Ray ray1 = Ray(eye, dir1);		//Create a ray originating from the camera in the direction 'dir'
			Ray ray2 = Ray(eye, dir2);
			Ray ray3 = Ray(eye, dir3);
			Ray ray4 = Ray(eye, dir4);
			
			ray1.normalize();				//Normalize the direction of the ray to a unit vector
			ray2.normalize();
			ray3.normalize();
			ray4.normalize();
			
		    glm::vec3 col1 = trace (ray1, 1); //Trace the primary ray and get the colour value
		    glm::vec3 col2 = trace (ray2, 1);
		    glm::vec3 col3 = trace (ray3, 1);
		    glm::vec3 col4 = trace (ray4, 1);
		    
		    float colred = (col1.r + col2.r + col3.r + col4.r)/4;
		    float colgreen = (col1.g + col2.g + col3.g + col4.g)/4;
		    float colblue = (col1.b + col2.b + col3.b + col4.b)/4;
		    
		    glColor3f(colred, colgreen, colblue);

		    

			
			glVertex2f(xp, yp);				//Draw each cell with its color value
			glVertex2f(xp+cellX, yp);
			glVertex2f(xp+cellX, yp+cellY);
			glVertex2f(xp, yp+cellY);
        }
    }

    glEnd();
    glFlush();
}

void initialize()
{
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(XMIN, XMAX, YMIN, YMAX);
    glClearColor(0, 0, 0, 1);
	texture1 = TextureBMP((char*)"1.bmp");
	texture2 = TextureBMP((char*)"2.bmp");
	

	
//===================================draw a sphere======================	
	//-- Create a pointer to a sphere object
	Sphere *sphere1 = new Sphere(glm::vec3(5.0, 0.0, -110.0), 10.0, glm::vec3(0, 0, 1));
	Sphere *sphere2 = new Sphere(glm::vec3(-5.0, -5.0, -70.0), 3.0, glm::vec3(1, 0, 1));
	
	//--Add the above to the list of scene objects.
	sceneObjects.push_back(sphere1); 
	sceneObjects.push_back(sphere2); 
//======================================================================

//===================================draw a box=========================
	glm::vec3 boxA = glm::vec3(-15.,-10,-80);
	glm::vec3 boxB = glm::vec3(-10.,-10,-80);
 	glm::vec3 boxC = glm::vec3(-10., -10, -100);
	glm::vec3 boxD = glm::vec3(-15., -10, -100);
	glm::vec3 boxE = glm::vec3(-10., -5, -80);
	glm::vec3 boxF = glm::vec3(-10., -5, -100);
	glm::vec3 boxG = glm::vec3(-15., -5, -80);
	glm::vec3 boxH = glm::vec3(-15., -5, -100);
	glm::vec3 boxColor = glm::vec3(0.5,0.5,1);
	
	Plane *plane = new Plane(boxA, boxB, boxC, boxD, boxColor);
	Plane *plane1 = new Plane(boxD, boxH, boxF, boxC, boxColor);
	Plane *plane2 = new Plane(boxA, boxG, boxE, boxB, boxColor);
	Plane *plane3 = new Plane(boxG, boxE, boxF, boxH, boxColor);
	Plane *plane4 = new Plane(boxB, boxE, boxF, boxC, boxColor);
	Plane *plane5 = new Plane(boxA, boxG, boxH, boxD, boxColor);

	sceneObjects.push_back(plane);
	sceneObjects.push_back(plane1);
	sceneObjects.push_back(plane2);
	sceneObjects.push_back(plane3);
	sceneObjects.push_back(plane4);
	sceneObjects.push_back(plane5);
//======================================================================

//=============================wall=====================================
	Plane *plane6 = new Plane(glm::vec3(-50.,-20,-200), //point A
							glm::vec3(50.,-20,-200), //point B
							glm::vec3(50., 30, -200), //point C
							glm::vec3(-50.,30,-200), //poing D
							glm::vec3(0.5,0.5,0)); //Color
	sceneObjects.push_back(plane6);
//======================================================================

//=================================bottom===============================
	Plane *plane7 = new Plane(glm::vec3(-20.,-20,-40), //point A
							glm::vec3(20.,-20,-40), //point B
							glm::vec3(20., -20, -200), //point C
							glm::vec3(-20.,-20,-200), //poing D
							glm::vec3(0.5,0.5,0)); //Color
	sceneObjects.push_back(plane7);
//======================================================================
//==============================draw a cylinder=========================
	Cylinder *cylinder = new Cylinder(glm::vec3(10,-20,-90), 3, 10, glm::vec3(0,1,0));
	sceneObjects.push_back(cylinder);
//======================================================================

	Sphere *sphere3 = new Sphere(glm::vec3(-5.0, 0.0, -100.0), 1.5, glm::vec3(1, 0, 1));
	sceneObjects.push_back(sphere3);
	Sphere *sphere4 = new Sphere(glm::vec3(0.0, 5.0, -60.0), 2.5, glm::vec3(1, 0, 1));
	sceneObjects.push_back(sphere4);
	
//==============================draw a orthogonal=========================
	glm::vec3 orthA = glm::vec3(16,-10,-84);
	glm::vec3 orthB = glm::vec3(4,-10,-84);
	glm::vec3 orthC = glm::vec3(4,-10,-96);
	glm::vec3 orthD = glm::vec3(16,-10,-96);
	glm::vec3 orthG = glm::vec3(12,-7,-86);
	glm::vec3 orthH = glm::vec3(12,-7,-94);
	glm::vec3 orthE = glm::vec3(6,-7,-86);
	glm::vec3 orthF = glm::vec3(6,-7,-94);	
	glm::vec3 orthColor = glm::vec3(0.7,0.2,0.8);
	
	Plane *plane8 = new Plane(orthA, orthB, orthC, orthD, orthColor);
	Plane *plane9 = new Plane(orthD, orthH, orthF, orthC, orthColor);
	Plane *plane10 = new Plane(orthA, orthG, orthE, orthB, orthColor);
	Plane *plane11 = new Plane(orthG, orthE, orthF, orthH, orthColor);
	Plane *plane12 = new Plane(orthB, orthE, orthF, orthC, orthColor);
	Plane *plane13 = new Plane(orthA, orthG, orthH, orthD, orthColor);
	sceneObjects.push_back(plane8);
	sceneObjects.push_back(plane9);
	sceneObjects.push_back(plane10);
	sceneObjects.push_back(plane11);
	sceneObjects.push_back(plane12);
	sceneObjects.push_back(plane13);
//======================================================================
}



int main(int argc, char *argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB );
    glutInitWindowSize(800, 800);
    glutInitWindowPosition(20, 20);
    glutCreateWindow("Raytracer");

    glutDisplayFunc(display);
    initialize();

    glutMainLoop();
    return 0;
}

