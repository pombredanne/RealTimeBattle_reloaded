/*
RealTimeBattle, a robot programming game for Unix
Copyright (C) 1998-2000  Erik Ouchterlony and Ragnar Ouchterlony
Copyright (C) 2003       Benoit Rousseau

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software Foundation,
Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

#ifndef __MOVING_OBJECT__
#define __MOVING_OBJECT__

class Vector2D;

#include "GeometricalObjects.h"
#include "ArenaObject.h"

class MovingObject : public ArenaObject
{
public:
  MovingObject() : ArenaObject( new Circle(Vector2D(0,0), 0) ) {}
  MovingObject(const Vector2D& c, const double r, const Vector2D& vel)
    : ArenaObject( new Circle(c, r) ), velocity(vel),
    acceleration(Vector2D(0,0)), mass( 0 ), alive( true ) {}
  ~MovingObject() {}

  void change_velocity( const double ) ;
  virtual void set_pos( const Vector2D& pos, double = 0 ) {
    static_cast<Circle*>(my_shape.val())->set_center( pos );
  }

  virtual void move(const double timestep) = 0;
  void move( const double timestep, int iterstep, const double eps );
  //virtual void hit_wall( Shape*, const Vector2D normal ) {};
  //virtual void hit_object( MovingObject*, const Vector2D normal ) {};
  virtual Vector2D own_acceleration( const double ) { return Vector2D( 0, 0 ); };

  virtual void die() {};

  //friend void bounce_on_object(MovingObject*, MovingObject*, const Vector2D& normal);

  //!Change energy and return false if dies (true otherwise)
  bool change_energy( const double );
  double get_energy() { return energy; }

  bool is_alive() { return alive; }

  //void bounce_on_wall(const double bounce_c, const double hardness_c, const Vector2D& normal);

  void add_acc( const Vector2D& acc ) {
    acceleration += acc;
  }

  Circle* get_shape() const { return static_cast<Circle*>( my_shape.val() ); }
  double get_radius() { return get_shape()->get_radius(); }
  Vector2D get_center() { return get_shape()->get_center(); }

protected:
  Vector2D acceleration;
  Vector2D velocity;
  double mass;
  double energy;
  bool alive;
};

#endif /* __MOVING_OBJECT__ */
