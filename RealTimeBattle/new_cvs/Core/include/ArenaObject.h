/*
RealTimeBattle, a robot programming game for Unix
Copyright (C) 1998-2002  Erik Ouchterlony and Ragnar Ouchterlony
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


#ifndef __ARENA_OBJECT__
#define __ARENA_OBJECT__

#include "Shape.h"
#include "Vector2D.h"

class shape_auto_ptr { //A quick autoptr class
public:
  shape_auto_ptr( Shape* s ): s( s ) {}
  ~shape_auto_ptr() { delete s; }
  Shape* val() const { return s; }
protected:
  Shape* s;
};

//! A generic class for Wall and MovingObjects
class ArenaObject {
public:
  ArenaObject( int id, double bounce_coeff, double hardness, Shape* my_s )
    : id( id ) ,
      bounce_coeff( bounce_coeff ), hardness_coeff( hardness ),
      my_shape( my_s )
    {}
  ArenaObject( Shape* );

  int get_id() const { return id; }
  //void set_id( const int i ) { id = i; }
  double get_bounce_coeff() const { return bounce_coeff; }
  double get_hardness_coeff() const { return hardness_coeff; }
  virtual ~ArenaObject() {}


  //Interface to my_shape
  inline double get_distance(const Vector2D& pos, const Vector2D& vel,
                             const double size) {
    return my_shape.val()->get_distance( pos, vel, size );
  }

  inline bool within_distance(const Vector2D& pos, const double size) {
    return my_shape.val()->within_distance( pos, size );
  }

  inline Vector2D get_normal(const Vector2D& pos) {
    return my_shape.val()->get_normal( pos );
  }

  inline void set_colour( const long c ) { my_shape.val()->set_colour( c ); }
  inline long get_rgb_colour() const { return my_shape.val()->get_rgb_colour(); };

protected:
  int id;
  double bounce_coeff;
  double hardness_coeff;
  shape_auto_ptr my_shape;
};

#endif


