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


#ifndef __ROBOT_CONTROLLER__
#define __ROBOT_CONTROLLER__

#include <vector>
#include <string>
#include <stdarg.h>
#include <list>

#include "Robot.h"
#include "Team.h"

//! RobotController: Handle robot messages, teams, ...
class RobotController {
 public:
  RobotController( );
  ~RobotController();

  void set_nb_team( const int nb_team );
  //!Add a robot to a team
  void add_robot( Robot* );
  //!Set the size of a team : no more than _size_ robots will play a game
  void set_team_size( const int team, const int size );
  //!Make the teams : check if time size is ok...
  const std::list<Robot*>& make_teams( );
  const std::list<Robot*>& make_team( int team, int size = -1);

  //!Add robot_list to all_robots_in_game (used when creating teams)
  void add_robots_in_game( const std::list<Robot*>& robot_list );
  //!Remove a robot from the all_robots_in_game and from Team::robots_in_arena lists
  void remove( Robot* );

  int robots_left() const { return all_robots_in_game.size(); }

  void send_signal( );

  void start_robots();
  void send_energy();

  int we_have_a_winner();

  //! broadcast a message to all the robots
  void broadcast( const string s );
  void broadcast(enum message_to_robot_type ...);
  //! broadcast a message to the team
  /*!
    Send the message to all the robots in the team that are compatible with RTB_1.1.X
    except to the sender
  */
  void broadcast_team( Robot* sender, std::string mess );

  void read_robot_messages( bool = false );

  void kill_all();
  void start_sequence();
  void end_process();
  void end_sequence();
  void delete_pipes( Robot* );

  void output();

  std::string check_name_uniqueness( Robot* robotp );

  int killed_this_round() ;
  int killed_since_last_time;



 protected:
  std::vector<Team> teams;
  std::list<Robot*> all_robots_in_game;
  std::list<Robot*> all_robots;
};

extern RobotController the_robot_controller;

#endif
