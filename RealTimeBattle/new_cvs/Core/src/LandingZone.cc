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


#include "LandingZone.h"
#include "ArenaBase.h"
#include "ArenaController.h"



double 
LandingZone::get_direction() {
  return ((double)rand())*2.0*M_PI/RAND_MAX;
}

double
StartPosition::get_direction() {
  if( any_dir ) return LandingZone::get_direction();
  else return direction;
}

bool
StartPosition::interact( MovingObject* mo ) {
  Vector2D pos = get_random_position( );
  double angle = get_direction();
  if( the_arena.space_available(pos, mo->get_radius()*1.2) ) {
    mo->set_pos(pos, angle);
    return true;
  }
  return false;
}

Vector2D
StartSquare::get_random_position() {
  return Vector2D(  upleft_corner[0] +
		    (downright_corner[0]-upleft_corner[0])* (double)rand()/(double)RAND_MAX, 
		    upleft_corner[1] +
		    (downright_corner[1]-upleft_corner[1])* (double)rand()/(double)RAND_MAX );
}

bool
StartSquare::interact( MovingObject* mo ) {
  Vector2D pos;
  bool found_space = false;
  for( int i=0; i<100 && !found_space; i++)
    {
      pos = get_random_position();
      found_space = the_arena.space_available(pos, mo->get_radius()*1.2);
    }

  if( found_space ) {
    mo->set_pos( pos,((double)rand())*2.0*M_PI/RAND_MAX );
    return true;
  }
  return false;
}

bool
Teleport::interact( MovingObject* mo ) {
  if( length( mo->get_center() - pos ) > mo->get_radius() )
    return false;

  Vector2D pos = mo->get_center() + jump;

  if( !the_arena.space_available( pos, mo->get_radius()*1.2 ) ) {
    cerr << "Not able to teleport" << endl;
    return false;
  }
  mo->set_pos( pos );
  return true;
}

bool
BlackHole::interact( MovingObject* mo ) {
  if( length( mo->get_center() - pos ) > mo->get_radius() )
    return false;

  mo->die();
  return true;
};

bool Square::is_on( MovingObject* mo) {
  Vector2D pos = mo->get_center();
  if( upleft_corner[0] < pos[0] && pos[0] < downright_corner[0] &&
      upleft_corner[1] < pos[1] && pos[1] < downright_corner[1] )
    return true;
  else
    return false;
}



Flow::Flow( const Vector2D& c1, const Vector2D& c2, const Vector2D& acc ) :
  Square( c1, c2 ), acc( acc )
{};


bool
Flow::interact( MovingObject* mo ) {
  if( ! is_on( mo ) )
    return false;
  mo->add_acc( acc );
  return true;
}

