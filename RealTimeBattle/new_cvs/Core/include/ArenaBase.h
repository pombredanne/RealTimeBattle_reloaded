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

#ifndef __ARENA_BASE__
#define __ARENA_BASE__

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <iostream>
#include <fstream>
#include <list>
#include <vector>
#include <map>

#include "Vector2D.h"
#include "Messagetypes.h"
#include "Structs.h"
#include "Timer.h"
#include "Shape.h"
#include "ArenaObject.h"
#include "ArenaObserver.h"
#include "LandingZone.h"

class ArenaController;

extern ArenaController the_arena_controller;
#define the_arena (*( the_arena_controller.get_the_arena()) )

class Robot;
class Options;
class WallLine;
class WallCircle;
class WallInnerCircle;

class RandomFunction;

class Extras;

class Team;


/* TODO : make a next( list<ArenaObject>::iterator& li ) That
  returns the next ArenaObject of ALL objects in arena as if
  wall, moveable and extras were ONE list
*/

static const double infinity = 1.0e10;  //approximatly ;-)
static const int max_debug_level = 5;

extern class Options the_opts;

//! The arena base is quite useless now
/*! 
  Was usefull when there where two subclasses : ArenaRealTime and ArenaReplay
  But it's not the case anymore (only ArenaRealTime survived
  We must find a way to replace ArenaReplay/ArenaRealTime classes
*/
class ArenaBase  
{
public:
  ArenaBase();
  virtual ~ArenaBase();

  friend class ArenaController;
  friend class ArenaParser; //! To parse arena files and store definitions
  
  enum game_mode_t { DEBUG_MODE, NORMAL_MODE, COMPETITION_MODE };

  /*!
    \param pos where to start
    \param vel direction to look at
    \param size of the object
    \param closest_shape type of the closest object (returned by ref ? )
    \param colliding_object the closest object
    \param the robot we are asking this for (NULL if it a shot ? )
  */

  double get_shortest_distance(const Vector2D& pos,
                               const Vector2D& vel,
                               const double size,
                               ArenaObject*& colliding_object,
                               const class MovingObject* = NULL );

  bool space_available(const Vector2D& pos, const double margin);

  void clear();


  void parse_arena_file(string& filename);

  Vector2D get_random_position();


  //Control functions

  //!Place robots and instanciate other objects
  void prepare_to_start(  );
  //!Step of time_step
  bool step( double );
  void quit_ordered();
  //! clear the lists
  void end_game();

private:
  void check_initialization();

  void update( double );
  void update_robots( double ); //???

public:

  void delete_lists();

  list<ArenaObject*>* get_extra_objects_list() { return &extra_objects_list; }

  list<RandomFunction*> random_func;

  //!Increase the count for type (Robots, Shots, Cookies and Mines)
  //TODO : check that the type derive from Moving or Arena Object...
  int increase_count( const /*type_info&*/ int type ) { return (count[type] ++) ; }


  //void print_message( const string& messager, const string& text );

  double get_shooting_penalty();

  void ground_interaction( MovingObject* );

  int increase_robot_count()  { return increase_count( ROBOT ); /*typeid(Robot) );*/ }
  int increase_shot_count()   { return increase_count( SHOOT ); /*typeid(Shot);*/ }
  int increase_cookie_count() { return increase_count( COOKIE ); /*typeid(Extras);*/ }
  int increase_mine_count()   { return increase_count( MINE ); /*typeid(Extras);*/ }

  //! Insert a object to delete at next end of update()
  void please_delete( ArenaObject* aop ) {
    ready_for_deletion.push_back( aop );
  }

protected:
  //! This objects will be deleted at next end of update()
  list<ArenaObject*> ready_for_deletion;

  list<ArenaObject*> moveables;
  list<ArenaObject*> extra_objects_list;
  list<ArenaObject*> walls;

  void move_shots(const double time_period);  //????

  //! All the objects in the arena that are still alive
  //list<ArenaObject*> object_lists[LAST_OBJECT_TYPE];
  list<Vector2D> exclusion_points;


  typedef list<LandingZone*> ZoneList;
  typedef map<int, ZoneList* > ZoneMap;

  ZoneMap landing_zones; //Landing zones for each team

public:
  LandingZone* boundary; //arena boundaries

protected:

  map<int, int> count;

  //  double timestep;

  list<InteractiveZone*> floors;

//A serie of functions used to manipulate moveables list
public:
  void add_moveable( ArenaObject* r );
  void remove_moveable( ArenaObject* r );

protected:  //A serie of functions used to manipulate object_lists[Robots & Shots]
  /*! There is a probleme in removing a moveable object for this list if we are in a loop
   *  that makes them move : problem comes from the fact that we can either remove the
   *  current object or the next object in the list. Simulating a double queue avoid this problem
   */
  list<ArenaObject*>::iterator list_head; //! to simulate a queue
  list<ArenaObject*>::iterator moveables_reset_head(); //! Place head_list on obj_lists[ROBOTS].begin()
  list<ArenaObject*>::iterator moveables_head(); //! Return the head and 'pop' front


  //A serie of functions that makes moveable, extra and wall list as if it was only one list
  list<ArenaObject*>::iterator all_objects_begin();
  list<ArenaObject*>::iterator all_objects_end();
  list<ArenaObject*>::iterator next( list<ArenaObject*>::iterator& );
};


#endif /* __ARENA_BASE__ */
