#include "particles.h"
#include "CSCIx229.h"
#include "time.h"

#define INITIAL_SCALE 0.65
#define END_SCALE 0.45
#define INITIAL_Y -10.8
#define FLAME_INITIAL_COLOR (color) {225, 0, 0}
#define PI 3.1415

color flame_initial = FLAME_INITIAL_COLOR;
color flame_end = (color) {240, 240, 0};

float getRandom()
{
  return (float)rand()/(float)RAND_MAX;
}

float lerp(float a, float b, float t)
{
  return a + t * (b - a);
}

void changeColors(color init, color end)
{
  flame_initial = init;
  flame_end = end;
}

void changeVelocity(particle* parray, int len, int type, int velType)
{
  
  int i = 0;
  for(i = 0; i < len; i++)
  {
    particle* p = &parray[i];
    
    switch(velType)
    {
      case PARTICLE_NORMAL:
         if(type == FIRE)
          {
            p->vxs = 0;
            p->vys = getRandom() + 1;
            p->vzs = getRandom() - 0.5;
            
            p->vy = &linear;
            p->vx = &randerm;
            p->vz = &randerm;
          }
          else if(type == SNOW)
          {
            p->vxs = 0;
            p->vys = -9;
            p->vzs = 0;
            
            p->vy = &linear;
            p->vx = &randerm;
            p->vz = &randerm;
          }
        break;
        
      case PARTICLE_RANDOM:
      
          if(type == FIRE)
          {
            p->vxs = 4*getRandom() - 2;
            p->vys = 4*getRandom() - 2;
            p->vzs = 4*getRandom() - 2;
            
            p->vy = &randerm;
            p->vx = &randerm;
            p->vz = &randerm;
          }
          else if(type == SNOW)
          {
            p->vxs = 2*getRandom() - 1;
            p->vys = -9;
            p->vzs = 2*getRandom() - 1;
            
            p->vy = &linear;
            p->vx = &randerm;
            p->vz = &randerm;
          }
          
        break;
        
      case PARTICLE_WAVY:
    
        if(type == FIRE)
        {
          p->vxs = 4 * getRandom();
          p->vys = 3;
          p->vzs = 4 * getRandom();
          
          p->vy = &linear;
          p->vx = &wave;
          p->vz = &wave;
        }
        else if(type == SNOW)
        {
          p->vxs = 3;
          p->vys = -9;
          p->vzs = 3;
          
          p->vy = &linear;
          p->vx = &wave;
          p->vz = &wave;
        }
        
      break;
      
      case PARTICLE_LIGHTSPEED:
    
        if(type == FIRE)
        {
          p->vxs = 8 * getRandom() - 4;
          p->vys = 10;
          p->vzs = 8 * getRandom() - 4;
          
          p->vy = &linear;
          p->vx = &linear;
          p->vz = &linear;
        }
        else if(type == SNOW)
        {
          p->vxs = 0;
          p->vys = -30;
          p->vzs = 0;
          
          p->vy = &linear;
          p->vx = &linear;
          p->vz = &linear;
        }
        
      break;
      
    case PARTICLE_WIND:
    
        if(type == FIRE)
        {
          p->vxs = 0.5;
          p->vys = 3;
          p->vzs = 0.5;
          
          p->vy = &linear;
          p->vx = &randerm;
          p->vz = &linear;
        }
        else if(type == SNOW)
        {
          p->vxs = 2;
          p->vys = -9;
          p->vzs = 2;
          
          p->vy = &linear;
          p->vx = &linear;
          p->vz = &linear;
        }
        
      break;
      
   case PARTICLE_CHIRP:
    
        if(type == FIRE)
        {
          p->vxs = 4 * getRandom() - 2;
          p->vys = 3;
          p->vzs = 4 * getRandom() - 2;
          
          p->vy = &linear;
          p->vx = &chirp;
          p->vz = &linear;
        }
        else if(type == SNOW)
        {
          p->vxs = 2;
          p->vys = -9;
          p->vzs = 2;
          
          p->vy = &linear;
          p->vx = &chirp;
          p->vz = &linear;
        }
        
      break;
        
        
    }
  }
}

void initParticles(particle* parray, int len, int type)
{
  srand(time(NULL));
  
  int i = 0;
  for(i = 0; i < len; i++)
  {
    particle* p = &parray[i];
    p->spawn_time = (double) (15*i) / len;
    
    switch(type)
    {
      case FIRE:
        onParticleDeathFlame(p, p->spawn_time);
        p->life_time = 0;
        p->vy = &linear;
        p->vx = &linear;
        p->vz = &linear;
        
        p->vys = getRandom() + 1;
        p->vzs = getRandom() - 0.5;
        
        break;
        
      case SNOW:
      
        onParticleDeathSnow(p, p->spawn_time);
        p->life_time = 0;
        p->vy = &linear;
        p->vx = &linear;
        p->vz = &linear;
        
        p->vys = -9;
        
        break;
    }
    
  }
}

void updateParticles(particle* parray, double time, double timestep, int len)
{
  int i = 0;
  float percent;
  for(i = 0; i < len; i++)
  {
    particle* p = &parray[i];
    
    percent = (time - p->spawn_time)/p->life_time;
    if(percent > 1) percent = 1;
    
    p->x += p->vxs * p->vx(4, percent) * timestep;
    p->y += p->vys * p->vy(1, percent) * timestep;
    p->z += p->vzs * p->vz(1, percent) * timestep;
    
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
  
  p->texture_id = 0;
  p->life_time = 1.5;
  p->spawn_time = time;
  
  p->initial_alpha = 255;
  p->end_alpha = 0;
 
  p->initial_scale = INITIAL_SCALE;
  p->end_scale = END_SCALE;
  p->current_scale = p->initial_scale;
  
  p->initial_color = flame_initial;
  p->end_color = flame_end;
  p->current_color = p->initial_color;
  
  p->deathptr = &onParticleDeathFlame;
}

void onParticleDeathFlame(particle *p, double time)
{
  p->texture_id = 1;
  p->life_time = 2;
  p->spawn_time = time;
  
  p->initial_scale = 0.4;
  p->end_scale = 0;
  p->current_scale = p->initial_scale;
  
  p->initial_color = (color) {75, 75, 75};
  p->end_color = (color) {255, 255, 255};
  p->current_color = p->initial_color;
  
  p->initial_alpha = 75;
  p->end_alpha = 0;
  
  p->deathptr = &onParticleDeathSmoke;
}

void onParticleDeathSnow(particle *p, double time)
{
  p->x = 2*30*getRandom() - 30;
  p->z = 2*30*getRandom() - 30;
  p->y = 10;
  
  p->texture_id = 2;
  p->life_time = 4;
  p->spawn_time = time;
  
  p->initial_scale = 0.75;
  p->end_scale = 0.25;
  p->current_scale = p->initial_scale;
  
  p->initial_color = (color) {255, 255, 255};
  p->end_color = (color) {255, 255, 255};
  p->current_color = p->initial_color;
  
  p->initial_alpha = 255;
  p->end_alpha = 255;
  
  p->deathptr = &onParticleDeathSnow;
}

float linear(float factor, float percent)
{
  return factor;
}

float randerm(float factor, float percent)
{
  return factor * getRandom() + 1;
}

float wave(float factor, float percent)
{
  return cos(factor * PI * percent);
}

float chirp(float factor, float percent)
{
  return cos(factor * PI * percent * percent);
}
