/*
 * Simulation.cpp
 * Copyright (C) 2006  Johann Rudloff
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "Simulation.h"
#include "Profiling.h"

#include <math.h>
#include <iostream>

#ifdef PROFILING
#include <sys/time.h>
#include <time.h>

timeval start_times[1024];
int funclevel = 0;

#endif // PROFILING

namespace Elfelli
{

Vec2::Vec2()
{
}

Vec2::Vec2(float x, float y)
{
  this->x = x;
  this->y = y;
}

Vec2 Vec2::operator=(const Vec2& v)
{
  x = v.x;
  y = v.y;
}

Vec2 Vec2::operator+(const Vec2& v)
{
  Vec2 r;
  r.x = x+v.x;
  r.y = y+v.y;
  return r;
}

void Vec2::operator+=(const Vec2& v)
{
  x += v.x;
  y += v.y;
}

void Vec2::operator-=(const Vec2& v)
{
  x -= v.x;
  y -= v.y;
}

Vec2 Vec2::operator-(const Vec2& v)
{
  Vec2 r;
  r.x = x-v.x;
  r.y = y-v.y;
  return r;
}

void Vec2::operator*=(float c)
{
  x *= c;
  y *= c;
}

void Vec2::operator/=(float c)
{
  x /= c;
  y /= c;
}

Vec2 Vec2::operator*(float c)
{
  Vec2 r;
  r.x = x*c;
  r.y = y*c;
  return r;
}

Vec2 Vec2::operator/(float c)
{
  Vec2 r;
  r.x = x/c;
  r.y = y/c;
  return r;
}

Vec2 Vec2::operator-()
{
  return Vec2(-x, -y);
}

float Vec2::distance(const Vec2& v) const
{
  return sqrt((v.x-x)*(v.x-x)+(v.y-y)*(v.y-y));
}

Vec2 Vec2::normalize() const
{
  float l = length();

  return Vec2(x/l, y/l);
}


inline float norm(float x)
{
  if(x>0)
    return 1.0;
  if(x<0)
    return -1.0;
  return 0.0;
}

Vec2 Simulation::force_at(const Vec2& pos, float charge)
{
  Vec2 f(0,0);
  for(int i=0; i<bodies.size(); i++)
    {
      Body& body = bodies[i];
      Vec2 v = body.pos - pos;
      float dist = v.length();
      Vec2 t = (v.normalize())/(dist*dist);

      f -= t * (charge * body.charge);
    }
  return f;
}

bool Simulation::step(Particle& p, float dtime)
{
  const float m = 5;

  Vec2 f = force_at(p.pos, p.charge);
  p.pos += f.normalize() * m;

  if(p.n > (2000/m))
    {
      if(p.pos.length() > 1000)
        return false;
      if(p.n > 10000)
        return false;
    }

  for(int i=0; i<bodies.size(); i++)
    {
      Vec2& pos = bodies[i].pos;
      if(p.pos.distance(pos) <= 5)
        return false;
    }

  p.n++;
  return true;
}

void Simulation::add_body(const Vec2& v, float charge)
{
  /* Numbers greter than 1024 are reserved for PlateBodies. */
  if(bodies.size() >= 1024) return;

  Body b;
  b.charge = charge;
  b.pos = v;
  bodies.push_back(b);
};

void Simulation::run()
{
  profile_func_start(__PRETTY_FUNCTION__);

  result.clear();

  Particle p;
  const float BODY_SIZE = 5;
  const float START_VEL = 12.0;
  const float STEPSIZE = 1;

  FluxLine l;

  for(int i=0; i<bodies.size(); i++)
    {
      Body& body = bodies[i];
      if(body.charge == 0)
        continue;
      float n = 4*fabs(body.charge);
      for(float angle=0; angle<(2*PI); angle+=(2*PI/n))
        {
          p.n=0;
          l.clear();

          p.pos = body.pos;
          l.add(p.pos);
          
          p.vel = (Vec2(cos(angle),sin(angle)) * START_VEL);
          p.charge = norm(body.charge);
          p.move();
          l.add(p.pos);
          while(step(p, STEPSIZE))
            {
              l.add(p.pos);
            }
          l.add(p.pos);

          result.push_back(l);
        }
    }

 profile_func_end(__PRETTY_FUNCTION__);
}
}
