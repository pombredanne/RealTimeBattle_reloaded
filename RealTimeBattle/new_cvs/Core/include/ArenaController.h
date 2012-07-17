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

#ifndef __ARENA_CONTROLLER__
#define __ARENA_CONTROLLER__

#include "ArenaBase.h"

#include <string>
#include <list>

#include "ArenaObserver.h"
#include "Timer.h"
#include "Team.h"

// <<Singleton>>
class ArenaController
{
public:
  ArenaController();
  ~ArenaController();
  
  void close_arena();
  void parse_command_line(int, char **);

  ArenaBase* get_the_arena() { return my_arena; }
  bool is_started() { return started; }
  bool is_realtime() { return realtime; }

  void update_function(const long int interval_usec);

  double timeout_function();

  void start_robots();
  void start_sequence_follow_up();
  void end_sequence_follow_up();
  void end_sequence();

  void notify_tic( double );
  void notify_event( Observable_Event* );

  void key_controller();

  state_t get_state() { return state; }

  bool check_state_for_message(const message_from_robot_type msg_t);
  double get_total_time() { return 0.0; }
private:
  
  bool started;
  bool realtime;

  ArenaBase* my_arena;

  void clear();

public: // global names etc.
  string option_filename;
  string statistics_filename;
  string log_filename;
  string tournament_filename;
  string message_filename;
  string arena_filename;

  int game_mode;
  int debug_level;
  bool auto_start_and_end;

  list<ArenaObserver*> arena_observers;

  void set_state( const state_t );

  Timer timer;

  void set_tournament_values(list<start_tournament_info_t>& robotfilename_list,
			     start_tournament_info_t& arenafilename,
			     int nb_teams );



 protected:
  state_t state;

  unsigned int tournament_random_seed;

  //pthread_t gtk_thread;
};

extern ArenaController the_arena_controller;

#endif /* __ARENA_CONTROLLER__ */
