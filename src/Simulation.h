// -*- C++ -*-
/*
 * Simulation.h
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

#ifndef _SIMULATION_H_
#define _SIMULATION_H_

#include <vector>
#include <math.h>

const float PI = 3.14159265358979;

namespace Elfelli
{

class Vec2
{
public:
  Vec2();
  Vec2(float x, float y);

  Vec2 operator=(const Vec2& v);
  Vec2 operator+(const Vec2& v);
  Vec2 operator-(const Vec2& v);
  Vec2 operator*(float c);
  Vec2 operator/(float c);

  Vec2 operator-();

  void operator+=(const Vec2& v);
  void operator-=(const Vec2& v);
  void operator*=(float c);
  void operator/=(float c);

  float get_x() const {return x;};
  float get_y() const {return y;};

  float set_x(float x){this->x=x;};
  float set_y(float y){this->y=y;};

  float distance(const Vec2& v) const;
  float length() const {return sqrt(x*x+y*y);};
  Vec2 normalize() const;

private:
  float x, y;

};

struct Body
{
  Vec2 pos;
  float charge;
};

struct PlateBody
{
  Vec2 pos_a;
  Vec2 pos_b;
  float charge;
};

struct Particle
{
  Particle(){n=0;};
  void move(){pos+=vel;};

  Vec2 pos;
  Vec2 vel;

  int n;
  float charge;
};

struct FluxLine
{
  void add(const Vec2& p){points.push_back(p);};
  void clear(){points.clear();};

  std::vector<Vec2> points;
};

class Simulation
{
public:
  Vec2 force_at(const Vec2& pos, float charge);
  void reset(){bodies.clear();result.clear();};

  void add_body(const Vec2& v, float charge);
  void add_plate(const Vec2& a, const Vec2& b, float charge);

  Body& operator[](int n){return bodies[n];};
  int n_bodies(){return bodies.size();};

private:
  bool step(Particle& p, float dtime);

protected:
  virtual void run();

  std::vector<Body> bodies;
  std::vector<PlateBody> plates;
  std::vector<FluxLine> result;

};

}

#endif // _SIMULATION_H_
