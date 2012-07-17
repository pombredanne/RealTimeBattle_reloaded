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

#include <stdarg.h>
#include <sstream>

#include "RobotController.h"
#include "Various.h"
#include "ArenaObserver.h"
#include "ArenaController.h"
#include "Colour.h"
#include "IntlDefs.h"
#include "RobotProcess.h"
#include "Options.h"

RobotController the_robot_controller;

RobotController::RobotController( ) {
  killed_since_last_time = 0;
}

RobotController::~RobotController() {
  for( list<Robot*>::iterator li = all_robots.begin();
  	li != all_robots.end(); li ++ ) {
	delete (*li);
  }
}

void
RobotController::set_nb_team( const int nb_team ) {
  teams.resize( nb_team );
}

void
RobotController::add_robot( Robot* robotp ) {
  int team = robotp->my_team;
  teams[team-1].add_robot( robotp );
  all_robots.push_back( robotp );
}

void
RobotController::remove( Robot* robotp ) {
  int team = robotp->my_team;
  teams[team-1].remove_robot( robotp );  //Remove the robot from Team::robots_in_arena
  all_robots_in_game.remove( robotp );
  ++ killed_since_last_time;
}

void
RobotController::send_signal() {
  for( list<Robot*>::iterator li = all_robots_in_game.begin();
       li != all_robots_in_game.end(); li ++ ) {
    (*li)->send_signal() ;
  }
}

//! Broadcast a message to all the robots
void
RobotController::broadcast( const string s )
{
  list<Robot*>::iterator li;
  for( li = all_robots.begin(); li != all_robots.end(); li++ )
    *((*li)->get_outstreamp()) << s << endl;
}

void
RobotController::broadcast(const message_to_robot_type msg_type ...)
{
  
  va_list args;
  va_start(args, msg_type);

  ostringstream str;
  str << message_to_robot[msg_type].msg << " ";

  for(int i=0; i < message_to_robot[msg_type].number_of_args; i++)
    {
      switch(message_to_robot[msg_type].arg_type[i])
        {
        case NONE: 
          Error(true, "Couldn't send message, no arg_type", "RobotController::broadcast");
          break;
        case INT:
	  str << va_arg(args, int) << " ";
          break;
        case DOUBLE:
	  str << va_arg( args, double ) << " ";   //TODO : limit to 6 digits after the .
          break;
        case STRING:
	  str << va_arg( args, char* ) << " ";
          break;   
        case HEX:
	  str << hex << va_arg( args, int ) << dec << " ";
          break;
        default:
          Error(true, "Couldn't send message, unknown arg_type", "RobotController::broadcast");
        }
    }
  broadcast( str.str() );
}

void
RobotController::broadcast_team( Robot* sender, string mess )
{
  teams[(sender->my_team)-1].broadcast( sender, mess );
}

void
RobotController::start_robots() {
  // execute robot processes
  Robot* robotp;
  list<Robot*>::iterator li;

  for( li = all_robots.begin() ; li != all_robots.end(); li ++ )
    {
      robotp = (*li);
      robotp->start_process();
      robotp->send_init_message();
    }
}

void
RobotController::send_energy() {
  double lvls = (double)the_opts.get_l(OPTION_ROBOT_ENERGY_LEVELS);
  for( list<Robot*>::iterator li = all_robots_in_game.begin() ;
       li != all_robots_in_game.end() ; li ++ ) {

    (*li)->send_message( ENERGY, rint( (*li)->get_energy() / lvls ) * lvls );
  }
}

// NOTE : this should be somewhere else... arena shouldn't take care of robots messages...
void
RobotController::read_robot_messages( bool all )
{
  list<Robot*>::iterator li;
  for( li = all_robots.begin(); 
       li != all_robots.end(); li++ )
    {
      if( (*li)->is_alive() || all ) {
        (*li)->get_messages();
      }
    }
}

void
RobotController::output() {
}


const list<Robot*>&
RobotController::make_teams( )
{
  for( int i = 0; i < teams.size() ; i ++ ) {
    make_team( i );
  }
  return all_robots_in_game;
}

const list<Robot*>&
RobotController::make_team( int team, int size )
{
  add_robots_in_game( teams[team].make_team( size ) );
  return all_robots_in_game;
}

void
RobotController::add_robots_in_game( const list<Robot*>& robot_list ) {
  list<Robot*>::const_iterator li;
  for( li = robot_list.begin(); li != robot_list.end(); li ++ )
    all_robots_in_game.push_back( *li );
}


/*! 0 if game is not finished
    -1 if there is no winner (no robot left)
    team_nb if there is a winner
*/
int
RobotController::we_have_a_winner() {
  unsigned int teams_alive = 0;
  int winner = -1;
  for( size_t i = 0; i < teams.size() ; i ++ )
    {
      if( teams[i].get_size() > 0 ) {
	winner = i + 1;  //If only one team survived, this one will be the winner
	teams_alive ++ ;
      } else {
      }
    }
  if( teams_alive > 1 ) {
    return 0;
  } else if( teams_alive == 0 ) {
    return -1;
  } else {
    return winner;
  }
}

void
RobotController::kill_all() {
  int robots_died_by_timeout = all_robots.size();
		
  for( list<Robot*>::iterator li = all_robots.begin(); li != all_robots.end(); li ++ )
    {
      Robot* robotp = (*li);
      if( robotp->is_alive() || robotp->is_dead_but_stats_not_set() )
	{
	  robotp->die();
	}
    }
}

void
RobotController::start_sequence( ) {

  list<Robot*>::iterator li;
  for( li = all_robots.begin(); 
       li != all_robots.end(); li ++ )
    {
      Robot* robotp = (*li);
      if( !(robotp->is_process_running()) ) 
        {
	  delete robotp;
	  list<Robot*>::iterator li2 = li --;
          all_robots.erase(li2);
          //robots_left--;
        }
      else
        {      
          if( !robotp->set_and_get_has_competed() ) {
	    Property_List_Event evt( robotp->get_id(), robotp->get_rgb_colour(),
				     robotp->get_robot_name() );
	    the_arena_controller.notify_event( &evt );
	  }          
          if( !robotp->is_name_given() )
            {
              robotp->send_message(WARNING, NAME_NOT_GIVEN, "");
              char msg[128];
              snprintf( msg, 127, _("Robot with filename %s has not given any name"),
                        robotp->get_robot_filename().c_str() );
            }

          if( !robotp->is_colour_given() )
            {
              robotp->send_message(WARNING, COLOUR_NOT_GIVEN, "");
              robotp->set_colour( (ColourServer::getHandle()).find_free_colour( 0, 0, true ) );
              robotp->set_colour_given( true );
            }
        }
    }
}

void
RobotController::end_process() {
  list<Robot*>::iterator li;
  for( li = all_robots.begin(); li != all_robots.end(); li ++ )
    {
      (*li)->end_process();
    }
}

void
RobotController::end_sequence() {
  Robot* robotp;

  list<Robot*>::iterator li;
  for( li = all_robots.begin(); li != all_robots.end(); li ++ )
    {
      robotp = (*li);
      if( robotp->is_process_running() ) robotp->kill_process_forcefully();
      robotp->delete_pipes();

      delete robotp;
      //end_robots.erase(li);
    }
  all_robots.clear();
}

void
RobotController::delete_pipes( Robot* robotp ) {
  list<Robot*>::iterator li;
  for( li = all_robots.begin(); li != all_robots.end(); li++ )
    {
          if( (*li) != robotp ) (*li)->delete_pipes();
    }
}

string
RobotController::check_name_uniqueness( Robot* robotp ) {
  int first_avail = 0;
  string robot_name;

  robot_name = robotp->plain_robot_name;
  list<Robot*>::iterator li;
  for( li = all_robots.begin(); li != all_robots.end(); li ++ )
    {
      if( robotp != (*li) && robot_name == (*li)->plain_robot_name )
        {
          if( (*li)->robot_name_uniqueness_number == 0 ) {
	    //That's the second robot with this name
	    (*li)->robot_name_uniqueness_number = 1;
	    (*li)->robot_name += "(1)";
	  } 
	    
	  first_avail = max( (*li)->robot_name_uniqueness_number + 1, first_avail );
        }
    }

  robotp->robot_name_uniqueness_number = first_avail;

  if( first_avail > 0 )
    {
      ostringstream os;
      os << "(" << first_avail << ")" ;
      robot_name += os.str();
    }

  return robot_name;
}

int
RobotController::killed_this_round(){
  int to_return = killed_since_last_time;
  killed_since_last_time = 0;
  return to_return;
}


