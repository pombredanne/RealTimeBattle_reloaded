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


#ifndef __ROBOT_PROCESS_H__
#define __ROBOT_PROCESS_H__

#include <fstream>
#include <sys/types.h>
#include <unistd.h>
#include <list>
#include <string>
#include <istream>
#include <ostream>
#include <string>
#include "fdstream.h"

#include "Robot.h"

using namespace std;

class RobotProcess : public RobotIO {
 public:
  RobotProcess( string robot_filename );

  void notify_tic();
  bool start_process( Robot* );
  void send_signal();
  bool is_process_running();
  void end_process() ;
  void kill_process_forcefully();
  bool get_default_non_blocking_state();
  void set_non_blocking_state(const bool non_bl);
  void delete_pipes();
  void set_signal( bool, int ) ;

 protected:
  bool is_executable;
  bool check_if_filename_is_robot( const string & );


  string filename;
  string robot_plain_filename;

  bool use_non_blocking;

  bool send_usr_signal;
  int signal_to_send;

  int pipes[2];
  pid_t pid;      
};


#endif /* __ROBOT_IO_H__ */
