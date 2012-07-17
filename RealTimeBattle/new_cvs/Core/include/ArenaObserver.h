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

#ifndef __ARENA_OBSERVERS__
#define __ARENA_OBSERVERS__


#include <string>

#include "Vector2D.h"

class Observable_Event;

typedef void * thread_ft_t(void*);

//! ArenaObserver is added in the ArenaController
/*!
  There are two types of Observer :
  -LocalObserver  : will have to check the Arena by himself
  -RemoteObserver : will receive observable events when necessary
  All observers are stored in ArenaController::all_observers
  Remote Observers are also stored in ArenaController::remote_obsevers
*/

class ArenaObserver {
 public:
  ArenaObserver() : ready( false ) {}

  virtual void init( ) {}

  //! Return false if the initialization didn't worked
  bool is_ready() { return ready; }

  //! Called by ArenaController just before running the thread
  virtual void startup() {};

  //! Will be called when a shape wants its change to be noticed
  /*!
    \param 1 An Observable_Event : no need to delete it
  */
  virtual void notify_event( Observable_Event * ) {}

  //! Called every time the clock is updated
  virtual void tic( double ) {} 

  /*!
    \return NULL if doesn't need to be threaded
  */
  virtual thread_ft_t* thread_init_function( ) {return NULL;}

  //! Called by ArenaController when it is about to leave
  /*! 
    When this function is called, the Observer must quit as fast as possible
  */
  virtual void quit()    {};
 protected:
  //! Indicate that the observer as been correctly initialized
  bool ready;
};

//! All visible events
/*!
  \see class TextLog : public RemoteObserver to see how it can be handled
*/

class Observable_Event 
{
 public:
  Observable_Event( char type ) : type( type ) {};
  char get_type() { return type; }
 protected:
  char type;
};

class Robot_Position_Info_Event : public Observable_Event {
public:
  Robot_Position_Info_Event ( int id, 
			      double x, double y, 
			      double ro_ang, double ca_ang, double ra_ang,
			      double energy ) :
    Observable_Event('R'), id(id), x(x), y(y), 
    ro_ang(ro_ang), ca_ang(ca_ang), ra_ang(ra_ang),
    energy(energy) {}

  int id;
  double x, y, ro_ang, ca_ang, ra_ang, energy;
};

class Robot_Print_Message_Event : public Observable_Event {
 public:
  Robot_Print_Message_Event( int id, std::string message ) :
    Observable_Event('P'), id( id ), message( message ) {}

  int id;
  std::string message;
};

class New_Static_Object_Event : public Observable_Event {
 public:
  New_Static_Object_Event( char type, int id, double x, double y ) :
    Observable_Event( type ), id( id ), x( x ), y( y ) {}
  New_Static_Object_Event( char type, int id, const Vector2D& pos ) :
    Observable_Event( type ), id( id ), x( pos[0] ), y( pos[1] ) {}
  int id ;
  double x, y;
};

class Shot_Event : public Observable_Event {
 public:
  Shot_Event( int id, double x, double y, double dx, double dy ) :
    Observable_Event('S'), id( id ), x( x ), y( y ), dx( dx ), dy( dy ) {}

  int id;
  double x, y, dx, dy ;
};

class Object_Die_Event : public Observable_Event {
 public:
  Object_Die_Event( char obj_type, int id ) :
    Observable_Event('D'), obj_type( obj_type ), id( id ) {}

  char obj_type;
  int id;
};

class Robot_Die_Event : public Object_Die_Event {
 public:
  Robot_Die_Event( int id, double pts, int pos ) :
    Object_Die_Event( 'R', id ), pts( pts ), pos( pos ) {}
  double pts;
  int pos;
};

//! Useless
class Game_Event : public Observable_Event {
 public:
  Game_Event( int a, int b ) : Observable_Event('G'), game(a), seq(b) {}
  int game, seq;
};

//! Useless
class Header_Event : public Observable_Event {
 public:
  Header_Event( int a, int b, int c, int d ) :
    Observable_Event('H'), a(a), b(b), c(c), d(d) {};
  int a, b, c, d;
};

class Property_List_Event : public Observable_Event {
 public:
  Property_List_Event( int id, long color, std::string name_ ) :
    Observable_Event('L'), id( id ), color( color )
    {
      if( name_ == "" ) name = "Anonymous";
      else              name = name_;
    }
  int id;
  long color;
  std::string name;
};

class Arena_Line_Event : public Observable_Event {
 public:
  Arena_Line_Event( std::string line ) :
    Observable_Event( 'A' ), line( line ) {};
  std::string line;
};

class Option_Event : public Observable_Event {
 public:
  Option_Event( std::string option_ ) :
    Observable_Event('O'), option( option_ ) {}
  std::string option;
};




#endif
