#include "particles.h"
#include "CSCIx229.h"
#include "time.h"

float getRandom()
{
  return (float)rand()/(float)RAND_MAX;
}

void initParticles(particle* parray)
{
  srand(time(NULL));
  
  pgroup pg = (pgroup)
  {
    2, 1,
    (color) {200, 0, 0}, (color) {255, 255, 255}
  };
  
  int i = 0;
  for(i = 0; i < MAX_PARTICLES; i++)
  {
    particle* p = &parray[i];
    p->group = pg;
    
    p->x = 0;
    p->y = 0;
    p->z = 0;
    
    p->vx = getRandom() - 0.5;
    p->vy = getRandom() + 1;
    p->vz = getRandom() - 0.5;
    
    p->initial_alpha = 255;
    p->end_alpha = p->initial_alpha/2;
    
    p->life_time = 0.001;
    p->spawn_time = 0;
    p->scale = pg.initial_scale;
    p->current_color = pg.initial_color;
    p->deathptr = &onParticleDeathFlame;
    p->texture_id = 0;
  }
}

void updateParticles(particle* parray, double time, double timestep)
{
  int i = 0;
  for(i = 0; i < MAX_PARTICLES; i++)
  {
    particle* p = &parray[i];
    pgroup pg = p->group;
    
    float percent = (time - p->spawn_time)/p->life_time;
    if(percent > 1) percent = 1;
    
    p->x += p->vx * timestep;
    p->y += p->vy * timestep;
    p->z += p->vz * timestep;
    
    p->current_alpha = p->initial_alpha - p->end_alpha*percent;
    p->scale = pg.initial_scale - pg.end_scale*percent;
    
    //printf("Going from %d to %d.\n", p->initial_alpha, p->end_alpha);
    
    p->current_color.r = pg.initial_color.r + percent * (pg.end_color.r - pg.initial_color.r);
    p->current_color.g = pg.initial_color.g + percent * (pg.end_color.g - pg.initial_color.g);
    p->current_color.b = pg.initial_color.b + percent * (pg.end_color.b - pg.initial_color.b);
    
    if((time - p->spawn_time) > p->life_time)
    {
      p->deathptr(p, time);
    }
  }
}

void onParticleDeathSmoke(particle *p, double time)
{
  p->x = getRandom() * 0.5;
  p->y = getRandom() - 0.5;
  p->z = getRandom() * 0.5;
  
  p->texture_id = 0;
  p->life_time = 2*getRandom() + 2;
  p->spawn_time = time;
  
  p->initial_alpha = 255;
  p->current_alpha = p->initial_alpha;
  p->end_alpha = p->initial_alpha/2;
 
  p->group.initial_scale = 2;
  p->group.end_scale = 1;
  p->scale = p->group.initial_scale;
  
  p->group.initial_color = (color) {200, 0, 0};
  p->group.end_color = (color) {255, 255, 255};

  p->current_color = p->group.initial_color;
  
  p->deathptr = &onParticleDeathFlame;
}

void onParticleDeathFlame(particle *p, double time)
{
  p->texture_id = 1;
  p->life_time = getRandom();
  p->spawn_time = time;
  
  p->group.initial_scale = 1;
  p->group.end_scale = 0;
  p->scale = p->group.initial_scale;
  
  p->group.initial_color = (color) {100, 100, 100};
  p->group.end_color = (color) {255, 255, 255};
  p->current_color = p->group.initial_color;
  
  p->initial_alpha = 125;
  p->end_alpha = 0;
  
  p->deathptr = &onParticleDeathSmoke;
}
