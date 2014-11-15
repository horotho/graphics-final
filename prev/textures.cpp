/* 
 * I used parts of the CSCI libary, 
 * and compiled and linked whatever I needed.
 * 
 * Credit: Prof. Schreuder
 */

#include <iostream>
#include <stdlib.h>
#include <GL/glut.h>
#include "terrain.h"
#include "imageloader.h"
#include "vec3f.h"
#include "CSCIx229.h"

using namespace std;

#define LEN 8192
#define PI 3.1415

enum
{
	MOUSE_LEFT_BUTTON = 0,
	MOUSE_MIDDLE_BUTTON = 1,
	MOUSE_RIGHT_BUTTON = 2,
	MOUSE_SCROLL_UP = 3,
	MOUSE_SCROLL_DOWN = 4
};

enum {MODE_ORTHOGONAL = 0, MODE_PERSPECTIVE = 1};
enum {AXES_MODE_OFF = 0, AXES_MODE_ON = 1};
enum {LIGHTING_ON = 1, LIGHTING_OFF = 0};

char* text[] = {"Orthogonal", "Perspective"};

int MODE = MODE_PERSPECTIVE;
int AXES_MODE = AXES_MODE_ON;
int LIGHTING_MODE = LIGHTING_ON;

int th = 45;        // Azimuth of view angle
int ph = 55;        // Elevation of view angle
int fov = 70;       // Field of view angle in degrees (y direction)
int speed_up = 4;     // The factor by which the light movement is sped up
int prism_index = 0;

double dim = 6.0;   // Dimension of orthogonal box
double asp = 1;     // Aspect ratio of the window
double t = 0;       // The current time step in light movement
double elevation = 1; // The current light elevation
double distance = 4; // Light distance away from the origin

double ambient = 0.15;
double specular = 0.5;
double diffuse = 0.8;

int mouseX = 0, mouseY = 0;

unsigned int prism_textures[5];
unsigned int cone_textures[1];

Terrain* terrain;

/*
 *  Projection Alteration
 *  Credit: Prof Schreuder
 *  Modifications made
 */
static void setProjection()
{
   glMatrixMode(GL_PROJECTION);
   
   glLoadIdentity();
   
   if (MODE == MODE_PERSPECTIVE)
   {
      gluPerspective(fov, asp, dim/8, 8 * dim);
   }
   else
   {
      glOrtho(-asp*dim, +asp*dim, -dim,+dim, -dim, 2*dim);
   }
      
   glMatrixMode(GL_MODELVIEW);
   
   glLoadIdentity();
}


/* Character Printing Code
 * Credit: Prof Schreuder */
 
void printString(const char* format , ...)
{
   char    buf[LEN];
   char*   ch=buf;
   va_list args;
   //  Turn the parameters into a character string
   va_start(args,format);
   vsnprintf(buf,LEN,format,args);
   va_end(args);
   //  Display the characters one at a time at the current raster position
   while (*ch)
      glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12,*ch++);
}

void drawCone(float radius)
{
  float angle;
  float height = radius * 2;
  float cone_angle = atan(radius / height);
  
   /* 
   * Material Setup with modifications
   * Credit: Prof Schreuder
   */
   
  float white[] = {1, 1, 1, 1};
  float black[] = {0, 0, 0, 1};
  float shiny[] = {8};

  glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, shiny);
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, white);
  glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, black);
  
  glPushMatrix();
  
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, cone_textures[0]);
  
  // Cone without base
  glBegin(GL_TRIANGLE_FAN);
  
    glVertex3f(0, 0, 0);
    glNormal3f(0, -1, 0);
    
    for(angle = 0.0f; angle < 2*PI; angle += PI/32)
    {
      glTexCoord2f(0.25*cos(angle) + 0.5, 0.25*sin(angle) + 0.5);
      glNormal3f(cos(cone_angle) * cos(angle), sin(cone_angle), cos(cone_angle) * sin(angle));
      glVertex3f(radius * cos(angle), height, radius * sin(angle));
    }
    
  glEnd();
  
  // Base of the cone
  glBegin(GL_TRIANGLE_FAN);
    
    glNormal3f(0, 1, 0);
    glVertex3f(0, height, 0);
    
    for(angle = 0.0f; angle < 2*PI; angle += PI/32)
    {
      glNormal3f(0, 1, 0);
      glTexCoord2f(0.25*cos(angle) + 0.5, 0.25*sin(angle) + 0.5);
      glVertex3f(radius * cos(angle), height, radius * sin(angle));
    }
    
    glTexCoord2f(0, 0);
    
  glEnd();
  
  glPopMatrix();
  glDisable(GL_TEXTURE_2D);
  
}

void drawSourcePrism(double x, double y, double z, double s)
{
  
   /* 
   * Material Setup with modifications
   * Credit: Prof Schreuder
   */
   
  float white[] = {1, 1, 1, 1};
  float emit[] = {1, 1, 1, 1};
  float shiny[] = {2};

  glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, shiny);
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, white);
  glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emit);
   
  glPushMatrix();
  
  glTranslatef(x, y, z);
  glScalef(s, s, s);
  
  //Source prism is white
  glColor3f(1.0, 1.0, 1.0);
  
  glBegin(GL_TRIANGLES);
    
    glNormal3f(0.25, -0.125, 0.0);
    glVertex3f(0, 0, 0);
    glVertex3f(0.25, 0.5, 0.25);
    glVertex3f(0, 0, 0.5);
    
    glNormal3f(0.0, 0.125, -0.25);
    glVertex3f(0, 0, 0);
    glVertex3f(0.25, 0.5, 0.25);
    glVertex3f(0.5, 0, 0);
    
    glNormal3f(0.25, 0.125, 0.0);
    glVertex3f(0.5, 0, 0);
    glVertex3f(0.25, 0.5, 0.25);
    glVertex3f(0.5, 0, 0.5);
    
    glNormal3f(0.0, 0.125, 0.25);
    glVertex3f(0.5, 0, 0.5);
    glVertex3f(0.25, 0.5, 0.25);
    glVertex3f(0, 0, 0.5);
  
  glEnd();

  glBegin(GL_QUADS);
    
    glNormal3f(0, -1.0, 0);
    glVertex3f(0, 0, 0);
    glVertex3f(0.5, 0, 0);
    glVertex3f(0.5, 0, 0.5);
    glVertex3f(0, 0, 0.5);
    
  glEnd();
  
  glPopMatrix();
}

void drawPrism(double x,  double y,  double z, double s,
               double th, double ph, int index)
{
  
  /* 
   * Material Setup with modifications
   * Credit: Prof Schreuder
   */
  
  float white[] = {1, 1, 1, 1};
  float black[] = {0, 0, 0, 1};
  float shiny[] = {16};

  glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, shiny);
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, white);
  glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, black);
  
  float coords[] = {0.0f,0.0f,  0.5f,0.5f,  1.0f,0.0f};
   
  glEnable(GL_TEXTURE_2D);
  glPushMatrix();
  
  glTranslatef(x, y, z);
  glRotated(th, 0, 1, 0);
  glRotated(ph, 0, 0, 1);
  glScalef(s, s, s);

  glBindTexture(GL_TEXTURE_2D, prism_textures[index]);
  glBegin(GL_TRIANGLES);
    
    // Left face
    glNormal3f(-0.25, 0.125, 0.0);
    glTexCoord2f(coords[0], coords[1]);
    glVertex3f(0, 0, 0);
    glTexCoord2f(coords[2], coords[3]);
    glVertex3f(0.25, 0.5, 0.25);
    glTexCoord2f(coords[4], coords[5]);
    glVertex3f(0, 0, 0.5);
    
    // Back face
    glNormal3f(0.0, 0.125, -0.25);
    glTexCoord2f(coords[1], coords[0]);
    glVertex3f(0, 0, 0);
    glTexCoord2f(coords[3], coords[2]);
    glVertex3f(0.25, 0.5, 0.25);
    glTexCoord2f(coords[5], coords[4]);
    glVertex3f(0.5, 0, 0);
    
    // Right face
    glNormal3f(0.25, 0.125, 0.0);
    glTexCoord2f(coords[2], coords[1]);
    glVertex3f(0.5, 0, 0);
    glTexCoord2f(coords[3], coords[4]);
    glVertex3f(0.25, 0.5, 0.25);
    glTexCoord2f(coords[5], coords[0]);
    glVertex3f(0.5, 0, 0.5);
    
    // Front face
    glNormal3f(0.0, 0.125, 0.25);
    glTexCoord2f(coords[0], coords[1]);
    glVertex3f(0.5, 0, 0.5);
    glTexCoord2f(coords[2], coords[3]);
    glVertex3f(0.25, 0.5, 0.25);
    glTexCoord2f(coords[4], coords[5]);
    glVertex3f(0, 0, 0.5);
    
  glEnd();
  
  glBindTexture(GL_TEXTURE_2D, cone_textures[0]);
  glBegin(GL_QUADS);
  
    // Bottom face
    glNormal3f(0, -1.0, 0);
    glTexCoord2f(0, 0);
    glVertex3f(0, 0, 0);
    glTexCoord2f(0, 1);
    glVertex3f(0.5, 0, 0);
    glTexCoord2f(1, 1);
    glVertex3f(0.5, 0, 0.5);
    glTexCoord2f(1, 0);
    glVertex3f(0, 0, 0.5);
    
  glEnd();
  
  glPopMatrix();
  glDisable(GL_TEXTURE_2D);
  
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    /* Eye Position
     * Credit: Prof Schreuder */
     
    // Perspective - set eye position
    if (MODE == MODE_PERSPECTIVE)
    {
      // Cross products galore!
      double Ex = -dim * sin(th * (PI/180)) * cos(ph * (PI/180));
      
      double Ey = +dim * sin(ph * (PI/180));
      
      double Ez = +dim * cos(th * (PI/180)) * cos(ph * (PI/180));
      
      gluLookAt(Ex, Ey, Ez, 0, 0, 0, 0, cos(ph * (PI/180)), 0);
    }
    //  Orthogonal - set world orientation
    else
    {
      glRotatef(ph,1,0,0);
      glRotatef(th,0,1,0);
    }
    
    /*
     * Lighting code, with modifications
     * Credit: Prof. Schreuder
     */
     
    glShadeModel(GL_SMOOTH);
    
    if (LIGHTING_MODE == LIGHTING_ON)
    {
      //  Translate intensity to color vectors
      float ambient_vector[]   = {ambient, ambient, ambient, 1.0};
      float diffuse_vector[]   = {diffuse, diffuse, diffuse, 1.0};
      float specular_vector[]  = {specular, specular, specular, 1.0};
      //  Light position
      float position_vector[] = {distance * cos(t * (PI/180)), elevation, distance*sin(t * (PI/180)), 1.0};
      drawSourcePrism(distance*cos(t * (PI/180)), elevation, distance*sin(t * (PI/180)), 0.3);
      //  OpenGL should normalize normal vectors
      glEnable(GL_NORMALIZE);
      //  Enable lighting
      glEnable(GL_LIGHTING);
      //  glColor sets ambient and diffuse color materials
      glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
      glEnable(GL_COLOR_MATERIAL);
      
      //  Enable light 0
      glEnable(GL_LIGHT0);
      
      //  Set ambient, diffuse, specular components and position of light 0
      glLightfv(GL_LIGHT0, GL_AMBIENT , ambient_vector);
      glLightfv(GL_LIGHT0, GL_DIFFUSE , diffuse_vector);
      glLightfv(GL_LIGHT0, GL_SPECULAR, specular_vector);
      glLightfv(GL_LIGHT0, GL_POSITION, position_vector);
   }
   else
     glDisable(GL_LIGHTING);
    
    // End Code Borrowing
    
    //int i, j;
    //for(i = 1; i < 5; i++)
    //{
      //for(j = 0; j < 5; j++)
      //{
        //drawPrism(j * 0.5 - 0.25, 0.25 * j, -0.75 * i, 0.8, 0, 0, j);
        //drawPrism(j * 0.5 - 0.25, 0.25 * j,  0.75 * i - 0.5, 0.8, 0, 0, i);
        //drawPrism(-j * 0.5 - 0.25, 0.25 * j, -0.75 * i, 0.8, 0, 0, j);
        //drawPrism(-j * 0.5 - 0.25, 0.25 * j,  0.75 * i - 0.5, 0.8, 0, 0, i);
      //}
    //}
    
    //drawCone(1);
    
    /* Axis Drawing and Labeling Code
    * Credit: Prof. Schreuder */

    if(AXES_MODE == AXES_MODE_ON)
    {
      //  Draw axes in white
      glColor3f(1,1,1);
      glBegin(GL_LINES);
      glVertex3d(-1.5,0,0);
      glVertex3d(1.5,0,0);
      glVertex3d(0,-1.5,0);
      glVertex3d(0,1.5,0);
      glVertex3d(0,0,-1.5);
      glVertex3d(0,0,1.5);
      glEnd();
      //  Label axes
      glRasterPos3d(1.5,0,0);
      printString("X");
      glRasterPos3d(0,1.5,0);
      printString("Y");
      glRasterPos3d(0,0,1.5);
      printString("Z");
    }

    glColor3f(1, 1, 1);
    glWindowPos2i(5, 5);
    printString("View Angle = %d, %d, FOV = %d, DIM = %2.1f",
                th, ph, fov, dim);
                
    /* End Code Borrowing */
    
    glWindowPos2i(5, 20);
    printString("Projection Mode = %s, Axes = %s, Lighting = %s",
                text[MODE],
                AXES_MODE == AXES_MODE_ON ? "ON" : "OFF",
                LIGHTING_MODE == LIGHTING_ON ? "ON" : "OFF"
               );
               
    glWindowPos2i(5, 35);
    printString("Light Speed = %3.3f, Light Elevation = %2.2f, Light Distance = %2.2f", 
                pow(2.0, speed_up), elevation, distance);
                
    glWindowPos2i(5, 50);
    printString("Ambient = %2.2f, Diffuse = %2.2f, Specular = %2.2f",
                ambient, diffuse, specular);
                
    glFlush();
    glutSwapBuffers();
}

/*
 * GLUT calls this routine when the window is resized
 * Credit: Prof. Schreuder
 */
void reshape(int width, int height)
{
   //  Ratio of the width to the height of the window
   asp = (height > 0) ? (double) width/height : 1;
   
   //  Set the viewport to the entire window
   glViewport(0, 0, width, height);
   
   // Re-set the projection
   setProjection();
}

/* 
 * GLUT uses this when the mouse is clicked and dragged.
 * Alters rotation based on mouse direction.
 */
void motion(int x, int y)
{
  
  if(x - mouseX > 0) th++;
  if(x - mouseX < 0) th--;
  if(y - mouseY > 0) ph++;
  if(y - mouseY < 0) ph--;

  th = th % 360;
  ph = ph % 360;

  mouseX = x;
  mouseY = y;

  setProjection();
  glutPostRedisplay();
}

/*
 * Idle Code
 * Credit: Prof Schreuder
 */

void idle()
{
   //  Elapsed time in seconds
   double time = glutGet(GLUT_ELAPSED_TIME)/1000.0;
   t = fmod(pow(2.0, speed_up) * time, 360.0);
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}

/*
 * GLUT uses this to detect mouse button presses and scrolls.
 * Used to increase/decrease fov. Forward scroll decreases fov
 * backwards scroll increases fov.
 */
void mouse(int button, int state, int x, int y)
{
    if(button == MOUSE_SCROLL_UP && fov > 1)
    {
      if(MODE == MODE_PERSPECTIVE)
        fov = (fov - 1) % 180;
    }
    else if(button == MOUSE_SCROLL_DOWN )
    {
      if(MODE == MODE_PERSPECTIVE)
        fov = (fov + 1) % 180;
    }
    
    setProjection();
    glutPostRedisplay();
}

/*
 * Used to switch between perspective modes and alter fov:
 * 1 : Orthogonal
 * 2 : Perspective
 * + : increase fov
 * - : decrease fov
 */
void key(unsigned char ch, int x, int y)
{
  switch(ch)
  {
    case 'q':
      MODE = !MODE;
      break;
    case 'a':
      AXES_MODE = !AXES_MODE;
      break;
    case 'w':
      LIGHTING_MODE = !LIGHTING_MODE;
      break;
    case '+':
      if(MODE == MODE_PERSPECTIVE)
        fov = (fov + 1) % 180;
      break;
    case '-':
      if(MODE == MODE_PERSPECTIVE)
        fov = fov > 1 ? fov - 1 : 1;
      break;
    case 't':
      speed_up++;
      break;
    case 'g':
      speed_up--;
      break;
    case 'r':
      elevation += 0.05;
      if(elevation > 1.8) elevation = 1.8;
      break;
    case 'f':
      elevation -= 0.05;
      if(elevation < -2) elevation = -2;
      break;
    case 'y':
      ambient += 0.01;
      if(ambient > 1.0) ambient = 1.0;
      break;
    case 'h':
      ambient -= 0.01;
      if(ambient < 0.0) ambient = 0.0;
      break;
    case 'u':
      diffuse += 0.01;
      if(diffuse > 1.0) diffuse = 1.0;
      break;
    case 'j':
      diffuse -= 0.01;
      if(diffuse < 0.0) diffuse = 0.0;
      break;
    case 'i':
      specular += 0.01;
      if(specular > 1.0) specular = 1.0;
      break;
    case 'k':
      specular -= 0.01;
      if(specular < 0.0) specular = 0.0;
      break;
    case 'o':
      distance += 0.05;
      if(distance > 8) distance = 8;
      break;
    case 'l':
      distance -= 0.05;
      if(distance < 0 ) distance = 0;
      break;
    default:
      break;
  }
  
  setProjection();
  glutPostRedisplay();
}

/*
 * This function is called by GLUT when special keys are pressed
 * Credit: Prof. Schreuder
 * Slight modifications made
 */
void special(int key, int x, int y)
{
   //  Right arrow key - increase azimuth by 5 degrees
   if(key == GLUT_KEY_RIGHT)     th += 5;
   
   //  Left arrow key - decrease azimuth by 5 degrees
   else if(key == GLUT_KEY_LEFT) th -= 5;
   
   //  Up arrow key - increase elevation by 5 degrees
   else if(key == GLUT_KEY_UP)   ph += 5;
   
   //  Down arrow key - decrease elevation by 5 degrees
   else if(key == GLUT_KEY_DOWN) ph -= 5;
   
   //  F1 key increases the dimension of the box by 0.1
   else if(key == GLUT_KEY_F1)   dim += 0.1;
   
   //  F2 key decreases the dimension of the box by 0.1
   else if(key == GLUT_KEY_F2 && dim > 1) dim -= 0.1;
   
   //  Keep angles to +/-360 degrees
   th %= 360;
   ph %= 360;
   
   setProjection();
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}

int main(int argc, char *argv[])
{
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

  glutInitWindowSize(800, 800);
  glutInitWindowPosition(0, 0);
  
  glutCreateWindow("Lighting");

  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutKeyboardFunc(key);
  glutSpecialFunc(special);
  glutMotionFunc(motion);
  glutMouseFunc(mouse);
  glutIdleFunc(idle);
  
  //cone_textures[0] = LoadTexBMP("cone_tex.bmp");
  //prism_textures[0] = LoadTexBMP("tr_tex1.bmp");
  //prism_textures[1] = LoadTexBMP("tr_tex2.bmp");
  //prism_textures[2] = LoadTexBMP("tr_tex3.bmp");
  //prism_textures[3] = LoadTexBMP("tr_tex4.bmp");
  //prism_textures[4] = LoadTexBMP("tr_tex5.bmp");
  
  terrain = loadTerrain("heightmap.bmp", 20);

  glEnable(GL_DEPTH_TEST);
  
  glutMainLoop();

  return 0; 
}
