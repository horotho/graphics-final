#include "particles.h"
#include "CSCIx229.h"
#include "time.h"

#define INITIAL_SCALE 1.25
#define END_SCALE 0.5
#define INITIAL_Y -10.8
#define FLAME_INITIAL_COLOR (color) {200, 0, 0}
#define PI 3.1415

float getRandom()
{
  return (float)rand()/(float)RAND_MAX;
}

float lerp(float a, float b, float t)
{
  return a + t * (b - a);
}

void initParticles(particle* parray)
{
  srand(time(NULL));
  
  int i = 0;
  for(i = 0; i < MAX_PARTICLES; i++)
  {
    particle* p = &parray[i];
    
    p->x = -0.5;
    p->y = INITIAL_Y;
    p->z = -0.5;
    
    ///p->vx = getRandom() - 0.5;
    p->vy = getRandom() + 1;
    p->vz = getRandom() - 0.5;
    
    p->initial_alpha = 255;
    p->end_alpha = p->initial_alpha/2;
    
    p->life_time = 0;
    p->spawn_time = (double) 20*i/MAX_PARTICLES;
    
    p->initial_scale = INITIAL_SCALE;
    p->current_scale = p->initial_scale;
    p->end_scale = END_SCALE;
    
    p->initial_color = FLAME_INITIAL_COLOR;
    p->current_color = p->initial_color;
    p->end_color = (color) {255, 255, 255};
    
    p->deathptr = &onParticleDeathSmoke;
    p->texture_id = 0;
  }
}

void updateParticles(particle* parray, double time, double timestep)
{
  int i = 0;
  float percent;
  for(i = 0; i < MAX_PARTICLES; i++)
  {
    particle* p = &parray[i];
    
    percent = (time - p->spawn_time)/p->life_time;
    if(percent > 1) percent = 1;
    
    p->x += p->vx * timestep;
    //p->x = sin(PI*percent);
    p->y += p->vy * timestep;
    p->z += p->vz * timestep;
    //p->z = cos(PI*percent);
    
    p->current_alpha = (int) lerp(p->initial_alpha, p->end_alpha, percent);
    p->current_scale = lerp(p->initial_scale, p->end_scale, percent);
    
    p->current_color.r = (int) lerp(p->initial_color.r, p->end_color.r, percent);
    p->current_color.g = (int) lerp(p->initial_color.g, p->end_color.g, percent);
    p->current_color.b = (int) lerp(p->initial_color.b, p->end_color.b, percent);
    
    if(percent == 1)
    {
      p->deathptr(p, time);
    }
  }
}

void onParticleDeathSmoke(particle *p, double time)
{
  p->x = 0.5*getRandom() - 0.45;
  p->y = INITIAL_Y + getRandom() - 0.5;
  p->z = 0.5*getRandom() - 0.45;
  
  p->vy = getRandom() + 1;
  
  p->texture_id = 0;
  p->life_time = 1.5;
  p->spawn_time = time;
  
  p->initial_alpha = 255;
  p->end_alpha = 0;
 
  p->initial_scale = INITIAL_SCALE;
  p->end_scale = END_SCALE;
  p->current_scale = p->initial_scale;
  
  p->initial_color = FLAME_INITIAL_COLOR;
  p->end_color = (color) {255, 255, 255};
  p->current_color = p->initial_color;
  
  p->deathptr = &onParticleDeathFlame;
}

void onParticleDeathFlame(particle *p, double time)
{
  p->vx = 0;
  p->vy *= 2;
  
  p->texture_id = 1;
  p->life_time = 2;
  p->spawn_time = time;
  
  p->initial_scale = 0.75;
  p->end_scale = 0;
  p->current_scale = p->initial_scale;
  
  p->initial_color = (color) {75, 75, 75};
  p->end_color = (color) {255, 255, 255};
  p->current_color = p->initial_color;
  
  p->initial_alpha = 75;
  p->end_alpha = 0;
  
  p->deathptr = &onParticleDeathSmoke;
}
