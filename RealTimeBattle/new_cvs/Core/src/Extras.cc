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


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <iostream>

using namespace std;

#include "Extras.h"
#include "ArenaController.h"
#include "Options.h"
#include "ArenaObserver.h"

Extras::Extras(const Vector2D& c, const double r, const double e, const long int col)
  : MovingObject(c, r, Vector2D(0, 0))
{
  alive = true;
  get_shape()->set_colour( col );
  energy = e;
}


Extras::Extras( const ExtraDef& ed ) {
  alive = true;
  get_shape()->set_colour (ed.colour);
  energy = ed.min_energy + ( ed.max_energy - ed.min_energy ) * (double)rand() / (double)RAND_MAX;
  get_shape()->set_radius( ed.radius );
  get_shape()->set_center( Vector2D(0, 0) );
  type_int = ed.type_int;
}

void
Extras::die()
{
   alive = false;
   energy = 0;

   Object_Die_Event e(log_file_char, id );
   the_arena_controller.notify_event( &e );
   the_arena.get_extra_objects_list()->remove( this );
   the_arena.please_delete( this );
}


ExtraDef::ExtraDef( const ExtraDef& ed ) {
  type_char = ed.type_char;
  type_int = ed.type_int;
  min_energy = ed.min_energy;
  max_energy = ed.max_energy;
  radius = ed.radius;
  colour = ed.colour;
}

void ExtraCreator::do_exec() {
  Extras* e = new Extras( my_def );
  if( !the_arena.boundary->interact( e ) )
    delete e;
  else {
    the_arena.get_extra_objects_list()->push_back( e );
    e->set_id( the_arena.increase_count( my_def.type_int ) );
    New_Static_Object_Event evt( e->log_file_char, e->get_id(), e->get_shape()->get_center() );
    the_arena_controller.notify_event( & evt );
  }
}

