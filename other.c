void drawCone(double x, double y, double z, float radius, float alpha)
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
  
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, cone_textures[0]);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  
  glTranslatef(x, y, z);
  
  // Cone without base
  glBegin(GL_TRIANGLE_FAN);
  
    glColor4f(1.0, 1.0, 1.0, alpha);
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
  
    glColor4f(1.0, 1.0, 1.0, alpha);
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
  glDisable(GL_DEPTH_TEST);
  
}
