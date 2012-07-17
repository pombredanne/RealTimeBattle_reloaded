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

#include <sys/resource.h>
#include <sys/stat.h>
#include <stdarg.h>
#include <unistd.h>
#include <signal.h>
#include <iostream>

#include <fcntl.h>

#include "fdstream.h"
#include "Various.h"
#include "RobotController.h"
#include "Options.h"

using namespace std;

#include "RobotProcess.h"

RobotProcess::RobotProcess( string robot_filename ) :
  filename( robot_filename ) 
{
  string::size_type nr = filename.rfind( '/', filename.size() );
  robot_plain_filename = filename.substr( nr + 1, filename.size() );

  send_usr_signal = false;
  signal_to_send = 0;

  is_executable = check_if_filename_is_robot( robot_filename );

  instreamp = NULL;
  outstreamp = NULL;
  pipes[0] = pipes[1] = -1;
  pid = -1;

  use_non_blocking = get_default_non_blocking_state();
}

bool
RobotProcess::check_if_filename_is_robot( const string& fname )
{ 
  struct stat filestat;
  if( stat( fname.c_str(), &filestat ) != 0 ) 
    return false;

  // Check if file is a regular file that can be executed and ends with .robot
  if( S_ISREG( filestat.st_mode) && 
      ( filestat.st_mode & ( S_IXOTH | S_IXGRP | S_IXUSR )) &&
      ( fname.substr( fname.length() - 6, fname.length() ) ) == ".robot" )
    return true;

  return false;
}




void RobotProcess::notify_tic() {}


bool
RobotProcess::start_process( Robot* robotp ) 
{
  if( !is_executable ) return false;
  
  int pipe_in[2], pipe_out[2];
  if (pipe (pipe_in)) 
    Error(true, "Couldn't setup pipe_in for robot " + filename, "Robot::start_process");

  if (pipe (pipe_out)) 
    Error(true, "Couldn't setup pipe_out for robot " + filename, "Robot::start_process");

  if( (pid = fork()) < 0 )
    Error(true, "Couldn't fork childprocess for robot " + filename, "Robot::start_process");


  if(pid == 0)   // Child process, to be the new robot
    {
      // Make pipe_out the standard input for the robot
      close(pipe_out[1]);
      dup2(pipe_out[0], STDIN_FILENO);

      // Make pipe_in the standard output
      dup2(pipe_in[1],STDOUT_FILENO);
      close(pipe_in[0]);

      // Make the pipes non-blocking
      if( use_non_blocking )
        {
          int pd_flags;
          if( (pd_flags = fcntl(pipe_out[0], F_GETFL, 0)) == -1 ) 
            Error(true, "Couldn't get pd_flags for pipe_out in robot " + filename, 
                  "Robot::start_process, child");
          pd_flags |= O_NONBLOCK;
          if( fcntl(pipe_out[0], F_SETFL, pd_flags) == -1 ) 
            Error(true, "Couldn't change pd_flags for pipe_out in robot " + filename, 
                  "Robot::start_process, child");
          
          
          if( (pd_flags = fcntl(pipe_in[1], F_GETFL, 0)) == -1 ) 
            Error(true, "Couldn't get pd_flags for pipe_in in robot " + filename, 
                  "Robot::start_process, child");
          pd_flags |= O_NONBLOCK;
          if( fcntl(pipe_in[1], F_SETFL, pd_flags) == -1 ) 
            Error(true, "Couldn't change pd_flags for pipe_in in robot " + filename, 
                  "Robot::start_process, child");
        }
      
      // Check file attributes

      struct stat filestat;
      if( 0 != stat( filename.c_str(), &filestat ) ) 
        Error(true, "Couldn't get stats for robot " + filename, "Robot::start_process, child");
      if( !S_ISREG( filestat.st_mode) )
        Error(true, "Robot file isn't regular, error for robot " + filename, 
              "Robot::start_process, child");
      if( !(filestat.st_mode & S_IXOTH) )
        Error(true, "Robot file isn't executable for user, error for robot " + filename, 
              "Robot::start_process, child");
      if( (filestat.st_mode & S_ISUID) )
        Error(true, "Set user ID is not allowed, error for robot " + filename, 
              "Robot::start_process, child");

      // Lower priority by one
     
      int old;
      if( (old = getpriority (PRIO_PROCESS, 0)) == -1 )
        Error(true, "Couldn't get priority for robot " + filename, "Robot::start_process, child");
      if( setpriority (PRIO_PROCESS, 0, old + 1) == -1)
        Error(true, "Couldn't set priority for robot " + filename, "Robot::start_process, child");
      
      // Close all pipes not belonging to the robot
      
      the_robot_controller.delete_pipes( robotp );

      if( execl(filename.c_str(), filename.c_str(), NULL) == -1 )
        Error(true, "Couldn't open robot " + filename, "Robot::start_process, child");

      Error(true, "Robot didn't execute, SHOULD NEVER HAPPEN!, error for " + filename, 
            "Robot::start_process, child");
    }
  else
    {
      close(pipe_out[0]);     // Close input side of pipe_out
      close(pipe_in[1]);      // Close output side of pipe_in  
      
      pipes[0] = pipe_out[1];
      pipes[1] = pipe_in[0];

      // Make the pipes non-blocking
      int pd_flags;
      if( (pd_flags = fcntl(pipe_in[0], F_GETFL, 0)) == -1 ) 
        Error(true, "Couldn't get pd_flags for pipe_in in robot " + filename, 
              "Robot::start_process, parent");
      pd_flags |= O_NONBLOCK;
      if( fcntl(pipe_in[0], F_SETFL, pd_flags) == -1 ) 
        Error(true, "Couldn't change pd_flags for pipe_in in robot " + filename, 
              "Robot::start_process, parent");
      if( (pd_flags = fcntl(pipe_out[1], F_GETFL, 0)) == -1 ) 
        Error(true, "Couldn't get pd_flags for pipe_out in robot " + filename, 
              "Robot::start_process, parent");
      pd_flags |= O_NONBLOCK;
      if( fcntl(pipe_out[1], F_SETFL, pd_flags) == -1 ) 
        Error(true, "Couldn't change pd_flags for pipe_out in robot " + filename, 
              "Robot::start_process, parent");

      outstreamp = new boost::fdostream(pipe_out[1]);  
      instreamp = new boost::fdistream(pipe_in[0]);
    }

  // wait some time to let process start up
  struct timeval timeout;
  timeout.tv_sec = 0;
  timeout.tv_usec = 40000;  //  1/25 s 

  select(FD_SETSIZE, NULL, NULL, NULL, &timeout);
  
  use_non_blocking = get_default_non_blocking_state();
  return true;
}



void 
RobotProcess::send_signal()
{
  if( send_usr_signal && pid > 0 )
    {
        kill(pid, signal_to_send);
    }
}

bool 
RobotProcess::is_process_running() 
{
}

void 
RobotProcess::end_process() 
{
  send_signal();
}

void RobotProcess::kill_process_forcefully() 
{
  if( pid > 0 )
    kill(pid, SIGKILL);

  delete_pipes();
}

void 
RobotProcess::delete_pipes() 
{
  if( instreamp != NULL ) delete instreamp;
  instreamp = NULL;
  if( outstreamp != NULL ) delete outstreamp;
  outstreamp = NULL;
  
  if( pipes[0] != -1 ) 
    {
      close(pipes[0]);
      pipes[0] = -1;
    }
  if( pipes[1] != -1 ) 
    {
      close(pipes[1]);
      pipes[1] = -1;
    }
}

bool
RobotProcess::get_default_non_blocking_state( ) {
  string filename = the_opts.get_s( OPTION_TMP_RTB_DIR ) +
    "/" + robot_plain_filename;
  
  int fd;
  if( ( fd = open(filename.c_str(), O_RDONLY) ) != -1 )
    {
      close(fd);
      return false;
    }
  
  return true;
}

// If non_blocking is _not_ used, a file, OPTION_TMP_RTB_DIR/"robotname"
// , is created.
void
RobotProcess::set_non_blocking_state(const bool non_bl)
{
  if( non_bl == use_non_blocking ) return;

  string filename = the_opts.get_s( OPTION_TMP_RTB_DIR ) +
    "/" + robot_plain_filename;

  create_tmp_rtb_dir();

  if( non_bl )
    remove( filename.c_str() );
  else
    {
      int fd = open(filename.c_str(), O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH );
      if( fd != -1 )  close( fd );
    }
  
 
  use_non_blocking = non_bl;

  //  restart_process

  kill_process_forcefully();
}

void
RobotProcess::set_signal( bool val, int sig) {
  send_usr_signal = val;
  signal_to_send = sig;
}
