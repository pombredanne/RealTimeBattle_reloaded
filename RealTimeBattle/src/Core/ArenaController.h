/*
RealTimeBattle, a robot programming game for Unix
Copyright (C) 1998-2001  Erik Ouchterlony and Ragnar Ouchterlony

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

#include <list>

#include "Arena.h"
#include "OptionHandler.h"
#include "InformationDistributor.h"

class ArenaController;

extern ArenaController the_arena_controller;

//#define the_arena (*( the_arena_controller.get_my_arena()) )
#define the_opts (*( the_arena_controller.get_main_opts() ))

static const int max_debug_level = 5;


class ArenaController
{
public:
  ArenaController();
  ~ArenaController();

  int init( int, char** );
  void parse_command_line( int, char** );

  OptionHandler* get_main_opts() const { return main_opts; }
  InformationDistributor* get_distributor() { return &distributor; }

  bool is_started() { return started; }
  bool is_realtime() { return realtime; }


private:
  
  void initialize_options();
  void print_help_message();

  bool started;
  bool realtime;

  OptionHandler* main_opts;
  InformationDistributor distributor;

public: // global names etc.
  string option_filename;
  string statistics_filename;
  string log_filename;
  string tournament_filename;
  string message_filename;
  string replay_filename;

  int game_mode;
  int debug_level;
  bool auto_start_and_end;
};

#endif // __ARENA_CONTROLLER__
