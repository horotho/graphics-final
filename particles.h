#ifndef _PARTICLES_H
#define _PARTICLES_H

#define MAX_PARTICLES 200

typedef struct color color;
typedef struct pgroup pgroup;
typedef struct particle particle;

struct color
{
  int r, g, b;
};

struct pgroup
{
  float initial_scale, end_scale;
  color initial_color, end_color;
};

struct particle
{
  unsigned int initial_alpha, current_alpha, end_alpha;
  unsigned int texture_id;
  double life_time, spawn_time;
  float x, y, z;
  float vx, vy, vz;
  float scale;
  color current_color;
  pgroup group;
  void (*deathptr)(particle *p, double time);
};


void initParticles(particle* parray);
void updateParticles(particle* parray, double time, double timestep);
void onParticleDeathFlame(particle *p, double time);
void onParticleDeathSmoke(particle *p, double time);
float getRandom();

#endif
