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

#include <math.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <iostream>
#include <iomanip>
#include <sstream>

using namespace std;

#if HAVE_DIRENT_H
# include <dirent.h>
# define NAMLEN(dirent) strlen((dirent)->d_name)
#else
# define dirent direct
# define NAMLEN(dirent) (dirent)->d_namlen
# if HAVE_SYS_NDIR_H
#  include <sys/ndir.h>
# endif
# if HAVE_SYS_DIR_H
#  include <sys/dir.h>
# endif
# if HAVE_NDIR_H
#  include <ndir.h>
# endif
#endif

#include "ArenaBase.h"
#include "ArenaController.h"

#include "IntlDefs.h"
#include "Extras.h"
#include "Various.h"
#include "Shot.h"
#include "Options.h"
#include "Wall.h"
#include "Robot.h"
#include "Team.h"

#include "Colour.h"
#include "TextLog.h"
#include "RobotController.h"
#include "ArenaParser.h"
#include "Functors.h"

ArenaBase::ArenaBase( )
{
  landing_zones[0] = new ZoneList;

  random_func.push_back(
  new ExtraCreator( the_opts.get_d(OPTION_COOKIE_FREQUENCY),
                    ExtraDef( 'C', COOKIE,
                              the_opts.get_d(OPTION_COOKIE_MIN_ENERGY),
                              the_opts.get_d(OPTION_COOKIE_MAX_ENERGY),
	                      the_opts.get_d(OPTION_COOKIE_RADIUS),
	                      the_opts.get_l(OPTION_COOKIE_COLOUR) )));


  random_func.push_back(
  new ExtraCreator( the_opts.get_d(OPTION_COOKIE_FREQUENCY),
                     ExtraDef( 'M', MINE,
                               -the_opts.get_d(OPTION_MINE_MAX_ENERGY),
	                       -the_opts.get_d(OPTION_MINE_MIN_ENERGY),
	                       the_opts.get_d(OPTION_MINE_RADIUS),
	                       the_opts.get_l(OPTION_MINE_COLOUR) )));

}

ArenaBase::~ArenaBase()
{
  sleep(1);
  clear();

  for( ZoneMap::iterator zm = landing_zones.begin();
       zm != landing_zones.end(); zm ++ ) {
    if( zm->second != NULL ) {
      for( ZoneList::iterator zl = zm->second->begin();
	   zl != zm->second->end(); zl ++ )
	delete (*zl);  //Delete the StartPosition

      delete zm->second;  //Delete the list
    }
  }

  for( list<InteractiveZone*>::iterator li = floors.begin();
      li != floors.end(); li ++ )
    delete (*li);

  for( list<RandomFunction*>::iterator it = random_func.begin();
       it != random_func.end(); ++it )
    delete (*it);
}


void
ArenaBase::parse_arena_file(string& filename)
{
  //  Vector2D vec1, vec2, vec0;
  ArenaParser parser;
  parser.parse( filename );
}


double
ArenaBase::get_shortest_distance(const Vector2D& pos, const Vector2D& dir,
                                  const double size,
                                  ArenaObject*& colliding_object, const MovingObject* the_object)
{
  double dist = infinity;
  double d;

  list<ArenaObject*>::iterator li;

  for( li = moveables.begin(); li != moveables.end(); ++li )
  {
    if( (*li) != the_object ) {
      d = (*li)->get_distance( pos, dir, size );
      if( d < dist ) {
	colliding_object = (*li);
	dist = d;
      }
    }
  }

  for( li = walls.begin();
       li != walls.end() ; ++li ) {
    d = (*li)->get_distance( pos, dir, size );
    if( d < dist ) {
      colliding_object = (*li);
      dist = d;
    }
  }

  for( li = extra_objects_list.begin();
        li != extra_objects_list.end(); ++ li ) {
    d = (*li)->get_distance( pos, dir, size );
    if( d < dist ) {
      colliding_object = (*li);
      dist = d;
    }
  }

  return dist;
}



bool
ArenaBase::space_available(const Vector2D& pos, const double margin)
{
  list<ArenaObject*>::iterator li;

  for( li = all_objects_begin();  li != all_objects_end(); next( li ) )
      if( (*li)->within_distance(pos, margin) ) return false;

  // Check if it is possible to see any exclusion_points

  Vector2D vec;
  double dist;
  ArenaObject* aop;

  list<Vector2D>::iterator li_ex;

  for( li_ex = exclusion_points.begin(); li_ex != exclusion_points.end(); li_ex++)
    {
      vec = *(li_ex);
      dist = length(vec - pos);
      if( dist <= margin ||
          dist <= get_shortest_distance(pos, unit(vec - pos), 0.0,
                                        aop, NULL) )
        return false;
    }

  return true;
}


double 
ArenaBase::get_shooting_penalty()
{
  int robots_left = the_robot_controller.robots_left();
  return min( the_opts.get_d(OPTION_SHOOTING_PENALTY), 0.5 / (double)robots_left );
}

  struct shape_eraser {
    void operator()(ArenaObject* ao) { if( typeid( *ao ) != typeid( Robot ) ) delete ao; }
  } shape_eraser;

void
ArenaBase::delete_lists()
{
  // clear the lists;
  for_each( walls.begin(), walls.end(), shape_eraser );
  for_each( extra_objects_list.begin(), extra_objects_list.end(), shape_eraser );
  for_each( moveables.begin(), moveables.end(), shape_eraser );

  exclusion_points.clear();
}


void
ArenaBase::clear()    //NOTE : Is it useful ?
{
  delete_lists();
}


Vector2D
ArenaBase::get_random_position()
{
  return boundary->get_random_position() ;
}



void
ArenaBase::quit_ordered()
{
  //set_state( EXITING );
}

bool
ArenaBase::step( double timestep )
{
  update( timestep );
  int winner = the_robot_controller.we_have_a_winner( );

  if( winner != 0 /* || total_time > the_opts.get_d(OPTION_TIMEOUT) */ )
    {
      if( winner != -1 )
	cerr << "The team " << winner << " won\n";
      else
        cerr << "No winner" << endl;
      end_game();
      return false;
    }
  else
    {
      for( list<RandomFunction*>::iterator it = random_func.begin();
            it != random_func.end(); it ++ )
        if( (*it)->can_exec( timestep ) )
	  (*it)->do_exec();
    }
  return true;
}

//! Make everything move
void
ArenaBase::update( double timestep )
{
  //Move all moveable objects
  list<ArenaObject*>::iterator li;
  for( li = moveables_reset_head();
       li != moveables.end();
       li = moveables_head() )
    {
      ((MovingObject*)(*li))->move(timestep);
    }

  update_robots( timestep );

  /* Remove dead mines and cookies */
  for( li = ready_for_deletion.begin();
       li != ready_for_deletion.end(); li++ )
       delete (*li);
  ready_for_deletion.clear();

}


void
ArenaBase::update_robots( double timestep )
{
  if( the_robot_controller.killed_this_round() > 0 )
    {
      the_robot_controller.broadcast( ROBOTS_LEFT, the_robot_controller.robots_left() );
    }

  the_robot_controller.send_signal();
}


void
ArenaBase::end_game()
{
  delete_lists();
}


void
ArenaBase::prepare_to_start( )
{
  //TODO : reset count[]

  // Place robots on the arena
  bool found_space;
  double angle;
  Vector2D pos;

  the_arena_controller.set_state(STARTING_ROBOTS);

  list<Robot*> robot_l = the_robot_controller.make_teams();

  for( list<Robot*>::const_iterator li = robot_l.begin(); li != robot_l.end(); li ++ )
    {
      Robot* robotp = (*li);
      robotp->get_messages();

      found_space = false;

      ZoneList* zl = NULL;

      if( (zl = landing_zones[ robotp->my_team ]) == NULL ) {
	zl = landing_zones[ 0 ];
      }

      ZoneList::iterator zl_it;
      for( zl_it = zl->begin(); zl_it != zl->end(); zl_it++ ) {
	found_space = (*zl_it)->interact(robotp);
	if( found_space ) break;
      }

      //Find a random place on base floor if not done
      if( !( found_space || boundary->interact(robotp) ) )
        Error(true, "Couldn't find space for all robots", "ArenaBase::prepare_to_start");

      robotp->reset_values();

      moveables.push_back( robotp );

      robotp->live();
    }

  //  print_to_logfile('G', sequence_nr, games_per_sequence - games_remaining_in_sequence + 1);

  the_robot_controller.broadcast(GAME_STARTS);
  the_robot_controller.broadcast(ROBOTS_LEFT, the_robot_controller.robots_left() );
  the_opts.broadcast_opts();

  for( list<ArenaObject*>::iterator li = moveables.begin();
       li != moveables.end(); li++ )
    {
      Robot* robotp = (Robot*)(*li);
      robotp->send_signal();
      robotp->move(0.0);  // To log current position
    }
}

void
ArenaBase::ground_interaction( MovingObject* mo ) {
  for( list<InteractiveZone*>::iterator li = floors.begin();
        li != floors.end(); li ++ )
    if( (*li)->interact( mo ) ) return;
}


////////////////////////////////////////////////
// Functions that simulates moveables as a    //
// double queue                               //
////////////////////////////////////////////////

void ArenaBase::add_moveable( ArenaObject* m ) {
  //Push front so the object will take effect only next time step
  moveables.push_front( m );
}

void ArenaBase::remove_moveable( ArenaObject* m ) {
    list<ArenaObject*>::iterator to_remove;
    if( (*list_head) == m ) {
      to_remove = list_head++;
      moveables.erase( to_remove );
    } else {
      moveables.remove( m );
    }

    if( typeid( *m ) == typeid( Robot ) ) {
      //object_lists[ROBOT].remove( m );
      the_robot_controller.remove( (Robot*)m );
    } else if( typeid( *m ) == typeid( Shot ) ) {
      //object_lists[SHOT].remove( m );
      ready_for_deletion.push_back( m );
    } else {
      cerr << "ArenaBase::remove_object : try to remove uncompatible object" << endl;
    }
}

list<ArenaObject*>::iterator
ArenaBase::moveables_reset_head() { //! Place head_list on obj_lists[ROBOTS].begin()
    list_head = moveables.begin();
    return list_head++;
}

list<ArenaObject*>::iterator
ArenaBase::moveables_head() { //! Return the head and 'pop' front
    return (list_head++);
}


////////////////////////////////////////////////
// Functions that simulate moveables, extras  //
// and walls lists as one list                //
// TODO : Maybe make a for_all_object algo    //
// TODO : if it is interesting                //
////////////////////////////////////////////////

list<ArenaObject*>::iterator
ArenaBase::next( list<ArenaObject*>::iterator& it ) {
  ++it;

  if( it != moveables.end() ) return it;
  else it = extra_objects_list.begin();  //Jump to the extra list

  //Check if extra is empty
  if( it != extra_objects_list.end()) return it;
  else return (it = walls.begin());  //Jump to wall
}

list<ArenaObject*>::iterator
ArenaBase::all_objects_begin() {
  list<ArenaObject*>::iterator it;
  if( (it = moveables.begin()) == moveables.end() )
    return next( it );
  else
    return it;
}

list<ArenaObject*>::iterator
ArenaBase::all_objects_end() {
  return walls.end();
}


