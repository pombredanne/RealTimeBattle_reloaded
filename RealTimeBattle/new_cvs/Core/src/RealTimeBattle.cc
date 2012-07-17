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

#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#include <sys/types.h>
#ifdef TIME_WITH_SYS_TIME 
# include <sys/time.h>
# include <time.h>
#else
# if HAVE_SYS_TIME_H
#  include <sys/time.h>
# else
#  include <time.h>
# endif
#endif


# include <sys/wait.h>


#include <errno.h>
#ifdef HAVE_GETOPT_H
# include <getopt.h>
#else
# include "getopt.h"
#endif

#ifdef HAVE_LOCALE_H
# include <locale.h>
#endif

#include <iostream>
#include <stdio.h>


#ifdef FLOATINGPOINT_H
#include <floatingpoint.h>
#endif

  using namespace std;

#include "IntlDefs.h"
#include "Options.h"
#include "Various.h"
#include "ArenaController.h"

#ifndef WAIT_ANY
#define WAIT_ANY (pid_t)-1
#endif

class Options the_opts;

class ArenaController the_arena_controller;

void 
print_help_message()
{
  cout << endl;
  cout << _(" Usage: RealTimeBattle [options] ") << endl << endl;
  cout << _(" Options:") << endl;
  cout << _("    --debug_mode,                -d   debug mode") << endl;
  cout << _("    --debug_level [0-5],         -D   sets the initial debug level. Implies -d") << endl;
  cout << _("    --normal_mode,               -n   normal mode (default)") << endl;
  cout << _("    --competition_mode,          -c   competition mode") << endl ;
  cout << endl;
  cout << _("    --option_file [file],        -o   selects option-file (default: $HOME/.rtbrc)")  << endl;
  cout << endl;
  cout << _("    --log_file [file],           -l   make log file, if 'file' is '-'\n"
            "                                      the log is sent to STDOUT") << endl;
  cout << endl;
  cout << _("    --tournament_file [file],    -t   specify a tournament file to\n"
            "                                      autostart a tournament") << endl;
  cout << endl;
  cout << _("    --statistics_file [file],    -s   file to print the statistics to\n"
            "                                       when autostarting") << endl;
  cout << endl;
  cout << _("    --message_file [file],       -m   redirect messages to 'file'.\n"
            "                                      With '-' as 'file', STDOUT is used.\n"
            "                                      If both log and messages are sent\n"
            "                                      to STDOUT, '-m' will be ignored") << endl;
  cout << endl;
  cout << _("    --help,                      -h   prints this message") << endl;
  cout << _("    --version,                   -v   prints the version number") << endl;
  cout << endl;
}


void
sig_handler (int signum)
{
  int pid;
  int status;
  while (1)
    {
      pid = waitpid (WAIT_ANY, &status, WNOHANG);
      if (pid < 0)
        {
          if( errno != ECHILD ) 
            Error(true, "waitpid failed", "RealTimeBattle.cc:sig_handler");
          break;
        }
      if (pid == 0)
        break;
    }
  //  if( signum == SIGCHLD ) cerr << "Sigchld caught!" << endl;
  //  if( signum == SIGPIPE ) cerr << "Sigpipe caught!" << endl;
  signal(signum, sig_handler);
}

void
sigfpe_handler(int signum)
{
  exit( 0 );
  signal(signum, sigfpe_handler);
}

int
main ( int argc, char* argv[] )
{
#ifdef FLOATINGPOINT_H
  fpsetmask ( ~ (FP_X_INV | FP_X_DZ | FP_X_IMP | FP_X_OFL | FP_X_UFL | FP_X_DNML) );
  //fpsetmask ( ~ (FP_X_INV | FP_X_DZ | FP_X_IMP) );
#endif


#ifdef HAVE_LOCALE_H
  setlocale( LC_MESSAGES, "" );
  setlocale( LC_NUMERIC, "POSIX" );
#endif
#ifdef RTB_LOCALEDIR
  bindtextdomain( "RealTimeBattle", RTB_LOCALEDIR );
#else
  bindtextdomain( "RealTimeBattle", "/usr/local/share/locale" );
#endif
  textdomain( "RealTimeBattle" );

  the_arena_controller.parse_command_line(argc, argv);

  signal(SIGPIPE, sig_handler);
  signal(SIGUSR1, sig_handler);
  signal(SIGFPE, sigfpe_handler);

  the_arena_controller.update_function( (long int)(the_opts.get_d( OPTION_UPDATE_INTERVAL ) * 1000000.0) );

  return EXIT_SUCCESS;
}
