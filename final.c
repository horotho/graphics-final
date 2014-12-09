/* 
 * All CSCIx229 library functions are credited to Prof. Schreuder.
 */

#include "CSCIx229.h"
#include "time.h"
#include "particles.h"

#define PI 3.14159265
#define NUM_TEXTURES 5
#define NUM_TREES 20
#define AXES_LENGTH 3

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
enum {SCENE_CAMPFIRE = 0, SCENE_WITCH = 1, SCENE_ROCKET = 2};

char* text[] = {"Orthogonal", "Perspective"};

int MODE = MODE_PERSPECTIVE;
int AXES_MODE = AXES_MODE_ON;
int LIGHTING_MODE = LIGHTING_ON;
int SCENE_MODE = SCENE_CAMPFIRE;

int th = 0;        // Azimuth of view angle
int ph = 0;        // Elevation of view angle
int fov = 70;       // Field of view angle in degrees (y direction)
int prism_index = 0;

double elapsed_time = 0.0;
double dim = 6.0;   // Dimension of orthogonal box
double asp = 1;     // Aspect ratio of the window

double ambient = 0.3;
double specular = 0.5;
double diffuse = 0.8;

double x = 0, y = -9.5, z = 6;
double lx = 0, ly = 0, lz = -1, px = 1, pz = 0;
double pangle = 0;

double prevTime;
int mouseX = 0, mouseY = 0;

unsigned int particle_textures[5];
unsigned int misc_tex[3];
unsigned int sky_textures[1];
unsigned int prev_texture;
unsigned int logs, cauldron;

particle* particles;
vec3* tree_positions;

void initTextures()
{
  int i;
  char buffer[50];
  char* pnames[] = {"flame.png", "smoke.png", "sparkle.png", "sparkle2.png", "rain.png"};
  char* gnames[] = {"ground.png", "house_front.png", "ftree.png"};
  
  for(i = 0; i < NUM_TEXTURES; i++)
  {
    sprintf(buffer, "%s%s", "textures/", pnames[i]);  
    particle_textures[i] = loadPng(buffer);
  }
  
  for(i = 0; i < 3; i++)
  {
    sprintf(buffer, "%s%s", "textures/", gnames[i]);  
    misc_tex[i] = loadPng(buffer);
  }
  
  sky_textures[0] = loadPng("textures/sky.png");
  
  logs = LoadOBJ("wood_logs.obj");
  cauldron = LoadOBJ("pot.obj");
}

void initTrees()
{
    int i;
    for(i = 0; i < NUM_TREES; i++)
    {
        tree_positions[i] = (vec3){getRandom() * 4 * dim, -11, getRandom() * 4 * dim};
    }
}

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
      gluPerspective(fov, asp, dim/20, 20 * dim);
   }
   else
   {
      glOrtho(-asp*dim, +asp*dim, -dim,+dim, -dim, 2*dim);
   }
      
   glMatrixMode(GL_MODELVIEW);
   
   glLoadIdentity();
}

void drawSky(double d)
{
  glDisable(GL_BLEND);
  glEnable(GL_DEPTH_TEST);
    
  glBindTexture(GL_TEXTURE_2D, sky_textures[0]);

  glColor4ub(255, 255, 255, 255);
  glBegin(GL_QUADS);
    // Top
    glNormal3f(0, 1, 0);
    glTexCoord2f(0.50, 0.30); glVertex3f(+d, +d, -d);
    glTexCoord2f(0.50, 0.00); glVertex3f(+d, +d, +d);
    glTexCoord2f(0.25, 0.00); glVertex3f(-d, +d, +d);
    glTexCoord2f(0.25, 0.30); glVertex3f(-d, +d, -d);
    
    // Front
    glNormal3f(0, 0, -1);
    glTexCoord2f(0.25, 0.30); glVertex3f(-d, +d, -d);
    glTexCoord2f(0.25, 0.50); glVertex3f(-d, -d, -d);
    glTexCoord2f(0.50, 0.50); glVertex3f(+d, -d, -d);
    glTexCoord2f(0.50, 0.30); glVertex3f(+d, +d, -d);
    
    // Back
    glNormal3f(0, 0, 1);
    glTexCoord2f(0.75, 0.30); glVertex3f(+d, +d, +d);
    glTexCoord2f(1.00, 0.30); glVertex3f(-d, +d, +d);
    glTexCoord2f(1.00, 0.50); glVertex3f(-d, -d, +d);
    glTexCoord2f(0.75, 0.50); glVertex3f(+d, -d, +d);
    
    //Left
    glNormal3f(-1, 0, 0); 
    glTexCoord2f(0.25, 0.50); glVertex3f(-d, -d, -d);
    glTexCoord2f(0.00, 0.50); glVertex3f(-d, -d, +d);
    glTexCoord2f(0.00, 0.30); glVertex3f(-d, +d, +d);
    glTexCoord2f(0.25, 0.30); glVertex3f(-d, +d, -d);
    
    // Right
    glNormal3f(1, 0, 0);
    glTexCoord2f(0.50, 0.50); glVertex3f(+d, -d, -d);
    glTexCoord2f(0.75, 0.50); glVertex3f(+d, -d, +d);
    glTexCoord2f(0.75, 0.30); glVertex3f(+d, +d, +d);
    glTexCoord2f(0.50, 0.30); glVertex3f(+d, +d, -d);
    
    //Bottom
    glTexCoord2f(0.25, 0.50); glVertex3f(-d, -d, -d);
    glTexCoord2f(0.50, 0.50); glVertex3f(-d, -d, +d);
    glTexCoord2f(0.50, 0.70); glVertex3f(+d, -d, +d);
    glTexCoord2f(0.25, 0.70); glVertex3f(+d, -d, -d);

  glEnd();
  
  prev_texture = sky_textures[0];

  glEnable(GL_BLEND);
  glDisable(GL_DEPTH_TEST);
}

void drawHouse()
{
  glDisable(GL_BLEND);
  glPushMatrix();
  
  if(prev_texture != misc_tex[1])
    glBindTexture(GL_TEXTURE_2D, misc_tex[1]);
  
  glTranslatef(-5, -11, 0);
  glScalef(5, 5, 5);
  glRotatef(90, 0, 1, 0);
  glColor4ub(255, 255, 255, 255);
  
  glBegin(GL_QUADS);
   
    glNormal3f(1, 0, 0);
    glTexCoord2f(0, 1); glVertex3f(0, 0, 0);
    glTexCoord2f(1, 1); glVertex3f(1, 0, 0);
    glTexCoord2f(1, 0); glVertex3f(1, 1, 0);
    glTexCoord2f(0, 0); glVertex3f(0, 1, 0);
  glEnd();
  
  glPopMatrix();
  
  glDisable(GL_CULL_FACE);
  prev_texture = misc_tex[1];
}

void drawLogs()
{
  glEnable(GL_DEPTH_TEST);
  glPushMatrix();
  
  glColor4ub(255, 255, 255, 255);
  glTranslatef(-0.5, -11, -0.5);
  glScalef(0.02, 0.02, 0.02);
  glRotatef(-30, 0, 1, 0);
  glCallList(logs);
  
  glPopMatrix();
  
  glPushMatrix();
  
  glColor4ub(255, 255, 255, 255);
  glTranslatef(0.25, -11, -0.25);
  glScalef(0.02, 0.02, 0.02);
  glRotatef(30, 0, 1, 0);
  glCallList(logs);
  
  glPopMatrix();
  
  glPushMatrix();
  
  glColor4ub(255, 255, 255, 255);
  glTranslatef(0, -11, 0.5);
  glScalef(0.02, 0.02, 0.02);
  glRotatef(90, 0, 1, 0);
  glCallList(logs);
  
  glPopMatrix();
  
}

void drawCauldron()
{
  glPushMatrix();
  
  glColor4ub(255, 255, 255, 255);
  glTranslatef(0, -11, 0.5);
  glScalef(10, 10, 10);
  glRotatef(90, 0, 1, 0);
  glCallList(cauldron);
  
  glPopMatrix();
}

void drawTrees()
{
  glEnable(GL_BLEND);
  glEnable(GL_TEXTURE_2D);
  glDepthMask(0);
  
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  
  float shiny[] = {16};
  float ambd[] = {1,1,1,1};
  glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, shiny);
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, ambd);
  
    int i;
    for(i = 0; i < NUM_TREES; i++)
    {
        
         if(prev_texture != misc_tex[2])
            glBindTexture(GL_TEXTURE_2D, misc_tex[2]);
            
        glPushMatrix();

        vec3 v = tree_positions[i];
        glTranslatef(v.x, v.y, v.z);
        glScalef(3, 3, 3);
        //glRotatef(pangle, 0, 1, 0);

        glBegin(GL_QUADS);

            glNormal3f(1, 0, 0);
            glTexCoord2f(0, 0.8); glVertex3f(0, 0, 0);
            glTexCoord2f(0.8, 0.8); glVertex3f(1, 0, 0);
            glTexCoord2f(0.8, 0); glVertex3f(1, 1, 0);
            glTexCoord2f(0, 0); glVertex3f(0, 1, 0);

        glEnd();

        glPopMatrix();

        prev_texture = misc_tex[2];
    }
    
  glBindTexture(GL_TEXTURE_2D, 0);
  glDepthMask(1);
}

void drawParticles()
{
  glEnable(GL_BLEND);
  glEnable(GL_TEXTURE_2D);
  glEnable(GL_POINT_SPRITE);
  
  glDepthMask(0);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  
  float shiny[] = {16};
  float ambd[] = {1,1,1,1};
  glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, shiny);
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, ambd);
    
   
  int i;
  for(i = 0; i < MAX_PARTICLES; i++)
  {
    particle p = particles[i];
    
    if(prev_texture != particle_textures[p.texture_id])
      glBindTexture(GL_TEXTURE_2D, particle_textures[p.texture_id]);
    
    glColor4ub(p.current_color.r, p.current_color.g, p.current_color.b, p.current_alpha);
    
    double rx = p.x * Cos(pangle) + p.z * Sin(pangle);
    double rx2 = (p.x + p.current_scale) * Cos(pangle) + p.z * Sin(pangle);
    double rz = p.z * Cos(pangle) - p.x * Sin(pangle);
    double rz2 = p.z * Cos(pangle) - (p.x + p.current_scale) * Sin(pangle);
    
    glBegin(GL_QUADS);
      glNormal3f(Cos(pangle), 0, -Sin(pangle));
      glTexCoord2f(0.10, 0.10);  glVertex3f(rx, p.y, rz);
      glTexCoord2f(0.90, 0.10);  glVertex3f(rx, p.y + p.current_scale, rz);
      glTexCoord2f(0.90, 0.90);  glVertex3f(rx2, p.y + p.current_scale, rz2);
      glTexCoord2f(0.10, 0.90);  glVertex3f(rx2, p.y, rz2);
    glEnd();
    
    prev_texture = particle_textures[p.texture_id];
  }

  glBindTexture(GL_TEXTURE_2D, 0);
  glDepthMask(1);
  //printf("Time to render particles: %0.0f ms\n", glutGet(GLUT_ELAPSED_TIME) - start);
  
}

void drawGround()
{
  double size = 5 * dim;
  int i, j;
  for(i = -size; i <= size; i++)
  {
    for(j = -size; j <= size; j++)
    {
      glPushMatrix();
  
      if(prev_texture != misc_tex[0])
        glBindTexture(GL_TEXTURE_2D, misc_tex[0]);
        
      glTranslated(i, -11, j);
      
      //glColor4ub(255, 255, 255, 255);
      glBegin(GL_QUADS);
        glNormal3f(0, 1, 0);
        glTexCoord2d(0, 0); glVertex3f(0, 0, 0);
        glTexCoord2d(1, 0); glVertex3f(1, 0, 0);
        glTexCoord2d(1, 1); glVertex3f(1, 0, 1);
        glTexCoord2d(0, 1); glVertex3f(0, 0, 1);
      glEnd();
      
      glPopMatrix();
      
      prev_texture = misc_tex[0];
    }
  }
 
}

void drawSourcePrism(double x, double y, double z, double s)
{
  
   /* 
   * Material Setup with modifications
   * Credit: Prof Schreuder
   */
   
  float white[] = {1, 1, 1, 1};
  float emit[] = {1, 1, 1, 1};
  float shiny[] = {16};

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

void display()
{
  //double start = glutGet(GLUT_ELAPSED_TIME);
  
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    /* Eye Position
     * Credit: Prof Schreuder */
     
    // Perspective - set eye position
    if (MODE == MODE_PERSPECTIVE)
    {
      gluLookAt(x, y, z,   x + lx, y + ly, z + lz,    0, 1, 0);
      
      //printf("lx = %0.3f, ly = %0.3f, lz = %0.3f\n", lx, ly, lz);
      //printf("px = %0.0f, pz = %0.0f\n", px, pz);
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
      float diffuse_vector[]   = {diffuse * 1.4, diffuse * 1.2, diffuse, 1.0};
      float specular_vector[]  = {specular, specular, specular, 1.0};
      //  Light position
      
      //float position_vector[] = {distance*cos(t * (PI/180)), elevation, distance*sin(t * (PI/180)), 1.0};
      //drawSourcePrism(distance*cos(t * (PI/180)), elevation, distance*sin(t * (PI/180)), 0.3);
      
      float position_vector[] = {Cos(pangle), -10 + 0.5 * cos(elapsed_time) * getRandom(), -Sin(pangle), 1};
      
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
     
    double size = 5 * dim;
    
    switch(SCENE_MODE)
    {
        case SCENE_CAMPFIRE:
        
            glEnable(GL_FOG);
         
            drawSky(size); // Draw the skybox
            drawGround(); // Draw the ground
            drawLogs(); // Draw the firepit logs
            //drawCauldron();
            drawHouse(); // Draw the house
            //drawTrees();
            drawParticles(); // Draw all of the fire particles last as they have alpha
            
            glDisable(GL_BLEND);
            glDisable(GL_FOG);
            
            break;
    }
        
    /* Axis Drawing and Labeling Code
    * Credit: Prof. Schreuder */

    if(AXES_MODE == AXES_MODE_ON)
    {
      //  Draw axes in white
      glTranslatef(0, -11, 0);
      glLineWidth(2);
      glBegin(GL_LINES);
      glColor3f(1, 0, 0);
      glVertex3d(-AXES_LENGTH,0,0);
      glVertex3d(0, 0, 0);
      
      glColor3f(1, 1, 1);
      glVertex3d(0, 0, 0);
      glVertex3d(AXES_LENGTH,0,0);
      
      glColor3f(1, 0, 0);
      glVertex3d(0,-AXES_LENGTH,0);
      glVertex3d(0, 0, 0);
      
      glColor3f(1, 1, 1);
      glVertex3d(0, 0, 0);
      glVertex3d(0, AXES_LENGTH,0);
      
      glColor3f(1, 0, 0);
      glVertex3d(0,0,-AXES_LENGTH);
      glVertex3d(0, 0, 0);
      
      glColor3f(1, 1, 1);
      glVertex3d(0, 0, 0);
      glVertex3d(0,0,AXES_LENGTH);
      glEnd();
      //  Label axes
      glRasterPos3d(1.5,0,0);
      Print("X");
      glRasterPos3d(0,1.5,0);
      Print("Y");
      glRasterPos3d(0,0,1.5);
      Print("Z");
      
      glColor3f(1, 1, 1);
      glWindowPos2i(5, 5);
      Print("View Angle = %d, %d, FOV = %d, DIM = %2.1f",
                  th, ph, fov, dim);
                  
      /* End Code Borrowing */
      
      glWindowPos2i(5, 20);
      Print("Projection Mode = %s, Axes = %s, Lighting = %s",
                  text[MODE],
                  AXES_MODE == AXES_MODE_ON ? "ON" : "OFF",
                  LIGHTING_MODE == LIGHTING_ON ? "ON" : "OFF"
                 );
                 
      glWindowPos2i(5, 35);
                  
      glWindowPos2i(5, 50);
      Print("Ambient = %2.2f, Diffuse = %2.2f, Specular = %2.2f",
                  ambient, diffuse, specular);
    }

    
    glFlush();
    glutSwapBuffers();
    
    //printf("Time for entire frame is %0.0f ms.\n", glutGet(GLUT_ELAPSED_TIME) - start);
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
  
  if(x - mouseX > 0)
  {
     th++;
  }
  if(x - mouseX < 0)
  {
     th--;
  }
  
  if(y - mouseY > 0)
  {
    ph++;
    ly -= 0.01;
  }
  if(y - mouseY < 0)
  {
    ph--;
    ly += 0.01;
  }

  th = th % 360;
  ph = ph % 360;
  
  lx = Sin(th);
  lz = -Cos(th);
  
  px = Sin(th + 90);
  pz = -Cos(th + 90);

  mouseX = x;
  mouseY = y;

  setProjection();
  glutPostRedisplay();
}

void idle()
{
   //  Elapsed time in seconds
   elapsed_time = glutGet(GLUT_ELAPSED_TIME)/1000.0;
   
   pangle = atan2(x, z) * (180/PI);
   updateParticles(particles, elapsed_time, elapsed_time - prevTime);
   
   prevTime = elapsed_time;
   
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
void key(unsigned char ch, int xx, int yy)
{
  
  switch(ch)
  {
    case 'r':
      SCENE_MODE = (SCENE_MODE + 1) % 3;
      break;
    case 'q':
      MODE = !MODE;
      break;
    case 'z':
      AXES_MODE = !AXES_MODE;
      break;
    case 'a':
      x -= px * 0.2;
      z -= pz * 0.2;
      break;
    case 'w':
      x += lx * 0.2;
      z += lz * 0.2;
      break;
    case 's':
      x -= lx * 0.2;
      z -= lz * 0.2;
      break;
    case 'd':
      x += px * 0.2;
      z += pz * 0.2;
      break;
    case 'x':
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
   if(key == GLUT_KEY_RIGHT)
   {
     th += 5;
   }
   
   //  Left arrow key - decrease azimuth by 5 degrees
   else if(key == GLUT_KEY_LEFT)
   {
     th -= 5;
   }
   
   //  Up arrow key - increase elevation by 5 degrees
   else if(key == GLUT_KEY_UP)
   {
     ph += 5;
     ly += 0.01;
   }
   
   //  Down arrow key - decrease elevation by 5 degrees
   else if(key == GLUT_KEY_DOWN)
   {
     ph -= 5;
     ly -= 0.01;
   }
   
   //  F1 key increases the dimension of the box by 0.1
   else if(key == GLUT_KEY_F1)   dim += 0.1;
   
   //  F2 key decreases the dimension of the box by 0.1
   else if(key == GLUT_KEY_F2 && dim > 1) dim -= 0.1;
   
   //  Keep angles to +/-360 degrees
   th %= 360;
   ph %= 360;
   
   lx = Sin(th);
   lz = -Cos(th);
   
   setProjection();
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}

int main(int argc, char *argv[])
{
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH | GLUT_ALPHA);

  glutInitWindowSize(500, 500);
  glutInitWindowPosition(1000, 0);
  
  glutCreateWindow("Final Project");
  
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutKeyboardFunc(key);
  glutSpecialFunc(special);
  glutMotionFunc(motion);
  glutMouseFunc(mouse);
  glutIdleFunc(idle);
 
  particles = (particle*) malloc(sizeof(particle) * MAX_PARTICLES);
  tree_positions = (vec3*) malloc(sizeof(vec3) * NUM_TREES);
  
  initTextures();
  initParticles(particles);
  initTrees();
  
  glEnable(GL_DEPTH_TEST);
  
  GLfloat fogColor[4] = {0.25f, 0.25f, 0.25f, 1.0f}; 

  glFogi(GL_FOG_MODE, GL_EXP);
  glFogfv(GL_FOG_COLOR, fogColor); 
  glFogf(GL_FOG_DENSITY, 0.08f); 
  glEnable(GL_FOG);
  
  glutMainLoop();
  
  free(particles);
  free(tree_positions);
  
  return 0; 
}
