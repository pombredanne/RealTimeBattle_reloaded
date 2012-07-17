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


#ifndef __TEAM_H__
#define __TEAM_H__

#include <list>
#include <string>

class Robot;

using namespace std;

//! A robot team
class Team {
 public:
  ~Team();
  void add_robot( Robot* );
  void remove_robot( Robot* );

  list<Robot*>* get_team() {
    return &robots_in_arena;
  }

  int get_size() ;

  //!Create a team to play a game.
  /*! arg is the number of robots playing
          is -1 if all robots plays
      return the list of selected robots
   */
  const list<Robot*>& make_team( int = -1 )  ;

  void broadcast( Robot*, string );

  struct RobotInfo {
    RobotInfo( Robot* r ) : robot_p( r ) , nb_participation ( 0 ) {};
    Robot* robot_p;
    int nb_participation;
  };

 protected:
  list< RobotInfo* > the_team;
  list< Robot* > robots_in_arena; //A subset of the_team

  double av_participation;
};


#endif /* __TEAM_H__ */
