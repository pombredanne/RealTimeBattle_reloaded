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

#include <list>
#include "Robot.h"
#include "Team.h"
#include <iostream>
#include <sstream>

Team::~Team() {
  for( list<RobotInfo*>::iterator li = the_team.begin();
       li != the_team.end(); li ++ )
    delete (*li);
}

void 
Team::add_robot( Robot* r ) {
  the_team.push_back( new RobotInfo( r ) );
}

void 
Team::remove_robot( Robot* r )
{
  robots_in_arena.remove( r );
}

int
Team::get_size() {
  return robots_in_arena.size();
}


void
Team::broadcast( Robot* sender, string mess ) {
  ostringstream os;
  os << message_to_robot[FROM_TEAM_MESSAGE].msg << " " 
     << sender->get_id() << " "
     << mess ;

  for( list<Robot*>::iterator li = robots_in_arena.begin();
       li != robots_in_arena.end() ; li ++ ) 
    if( (*li)->compatible_with_this_version && (*li) != sender )
      (*((*li)->get_outstreamp())) << os.str() << endl;
}

const list<Robot*>& 
Team::make_team( int nb_robots ) {
  if( nb_robots == -1 ) nb_robots = the_team.size();

  robots_in_arena.clear();

  list<RobotInfo*>::iterator li;
  for( li = the_team.begin() ; li != the_team.end(); li ++ ) {
    ((*li)->nb_participation) ++ ;
    robots_in_arena.push_back( (*li)->robot_p );
  }
  return robots_in_arena;
}
