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


#ifndef __LANDING_ZONE_H__
#define __LANDING_ZONE_H__

#include "Vector2D.h"
#include "MovingObject.h"

class InteractiveZone {
public:
  virtual bool interact( MovingObject* ) = 0 ;
};

class Teleport : public InteractiveZone {
public:
  Teleport( const Vector2D& from, const Vector2D& to ) :
    pos( from ), jump( to - from ) {}
  bool interact( MovingObject* );
  Vector2D pos;
  Vector2D jump;
};

class BlackHole : public InteractiveZone {
public:
  BlackHole( const Vector2D& pos ) :
    pos( pos ) {}

  bool interact( MovingObject* );
  Vector2D pos;
};

//! Place where to put the robot at the begining of a game
class LandingZone : public InteractiveZone {
 public:
  virtual Vector2D get_random_position() = 0;
  virtual double get_direction() ;
};

class StartPosition : public LandingZone {
 public:
  StartPosition( Vector2D& position, double dir ) :
    position( position ), direction( dir ), any_dir( false ) {}
  StartPosition( Vector2D& position ) :
    position( position ), any_dir( true ) {}

  Vector2D get_random_position( ) { return position; }

  bool interact( MovingObject* );

  double get_direction();

  Vector2D position;
  double direction;
  bool any_dir;
};

class StartSquare : public LandingZone {
 public:
  StartSquare( Vector2D& upleft_corner, Vector2D& downright_corner ) :
    upleft_corner(upleft_corner), downright_corner(downright_corner) {}


  bool interact( MovingObject* );
  Vector2D get_random_position( );

  Vector2D upleft_corner;
  Vector2D downright_corner;
};

class Square {
public:
  Square( const Vector2D& upleft_corner, const Vector2D& downright_corner ) :
    upleft_corner(upleft_corner), downright_corner(downright_corner) {}
  bool is_on( MovingObject* );

protected:
  Vector2D upleft_corner;
  Vector2D downright_corner;
};

class Flow : public InteractiveZone, public Square {
public:
  Flow( const Vector2D&, const Vector2D&, const Vector2D& );
  bool interact( MovingObject*);
protected:
  Vector2D acc;
};


#endif
