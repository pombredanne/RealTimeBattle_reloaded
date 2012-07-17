/*
RealTimeBattle, a robot programming game for Unix
Copyright (C) 1998-2002  Erik Ouchterlony and Ragnar Ouchterlony
Part Copyright (C) 2003       Benoit Rousseau

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

#include <iostream>
#include <stdio.h>


#ifdef FLOATINGPOINT_H
#include <floatingpoint.h>
#endif

#include "Team.h"
#include "IntlDefs.h"
#include "Options.h"
#include "Various.h"
#include "ArenaController.h"
#include "RobotProcess.h"


#include <string>
#include <pthread.h>
#include <sstream>
#include <stdarg.h>


#include "RobotController.h"
#include "ArenaController.h"
#include "ArenaObserver.h"
#include "TextLog.h"
#include "DebugLog.h"
#include "Robot.h"
#include "Colour.h"
#include "RobotMessageIO.h"

using namespace std;

void 
print_help_message();

ArenaController::ArenaController()
{
  started = false;
  state = NOT_STARTED;

  option_filename = "";
  statistics_filename = "";
  log_filename = "";
  tournament_filename = "";
  message_filename = "";

  game_mode = ArenaBase::NORMAL_MODE;
  debug_level = 1;
  auto_start_and_end = false;

  timer.reset();

  ArenaObserver* log = new DebugLog(  );
  if( log->is_ready() ) {
    arena_observers.push_back( log );
  } else { //Something went wrong while creating the observer
    cerr << "Debug log not possible\n";
    delete log;  
  } 
}

void
ArenaController::update_function(const long int interval_usec)
{
  struct timeval timeout;

  bool res = true;
  double time_scale =  the_opts.get_d( OPTION_TIMESCALE );

  /* TODO : run all the threads */
  //gtk_obs->create();
  //thread_ft_t* thread_ft_p =
  //  gtk_obs->thread_init_function();

  //pthread_create( &gtk_thread, NULL, thread_ft_p, gtk_obs );

  do
    {
      fd_set rfds;
      int retval;

      timeout.tv_sec = 0;
      timeout.tv_usec = (int) (interval_usec * time_scale);

      FD_ZERO(&rfds);
      FD_SET(0, &rfds);
      retval = select(1, &rfds, NULL, NULL, &timeout);
      /* Check the input */
      if( retval != -1 && FD_ISSET(0, &rfds) ) {
	key_controller();
      }

      res = timeout_function( );

    } 
  while( res );

  //gtk_obs-> quit() ;

  //pthread_join( gtk_thread, NULL );
}

void 
ArenaController::key_controller() {
  char c;
  cin >> c;
  switch( c ) 
    {
    case 'q':
      Quit( );
      break;
    case 'v' :
      cout<<"*RealTimeBattle-Core " << VERSION << endl;
      break;
    }
}

double
ArenaController::timeout_function() {
  double timestep = 0.0;
  if( state != PAUSED && state != GAME_IN_PROGRESS )
    {
      timestep = timer.update( );
    }
  switch( state )
    {
    case NOT_STARTED:
      if( parse_tournament_file( tournament_filename ) ) {

	//Start robots
	set_state( STARTING_ROBOTS );
	the_robot_controller.start_robots();

	timer.next_check( the_opts.get_d(OPTION_ROBOT_STARTUP_TIME) );

	//Create arena
	my_arena = new ArenaBase();
      }
      break;

    case FINISHED:
      if( auto_start_and_end )
        {
          //if( statistics_file_name != "" )
          //  save_statistics_to_file( statistics_file_name );

          Quit();
        }
      break;

    case STARTING_ROBOTS:
      {
        the_robot_controller.read_robot_messages( true );

        if( timer.check() )
	  {
	    start_sequence_follow_up();
	    cout<< "P0 RealTimeBattle Core " << VERSION << endl;
	  }
      }
      break;

    case SHUTTING_DOWN_ROBOTS:
      {
	the_robot_controller.read_robot_messages( true );

        if( timer.check() ) {
	  end_sequence_follow_up();
	  set_state( EXITING );
	}
      }
      break;

    case GAME_IN_PROGRESS:
      {
	try
	  {
	    notify_tic( timer.get_total_time() );
	    the_robot_controller.read_robot_messages();

	    // Get timestep just before simulating the step
	    timestep = timer.update();

	    if( ! my_arena->step( timestep ) ) {
	      end_sequence();
	      the_robot_controller.kill_all();
	    }
	  }
	catch( ... )
	  {
	    cerr<<"Unknown error occured"<<endl;
	  }
      }
      break;

    case PAUSED:
      break;

    case PAUSING_BETWEEN_GAMES:
      //if( !my_arena->pause_after_next_game ) my_arena->start_game();
      break;

    case EXITING:
      return false;

    case BEFORE_GAME_START:
      break;//case NO_STATE:
      //Error(true, "Arena shouldn't be in this state here!!", "ArenaController::timeout_function");
    }
  return true;
}

void
ArenaController::start_sequence_follow_up()
{
  // check if the process have started correctly

  the_robot_controller.start_sequence();

  Game_Event evt( 1, 1 ); //( sequence_nr, game_nr + 1 );
  notify_event( &evt );

  my_arena->parse_arena_file( arena_filename );

  set_state( BEFORE_GAME_START );

  notify_tic( 0.0 );
  my_arena->prepare_to_start( );
  set_state( GAME_IN_PROGRESS );
  //game_nr++;

  timer.reset();  // Game starts !
  timer.next_check( the_opts.get_d(OPTION_CHECK_INTERVAL) );
}



void
ArenaController::end_sequence()
{
  // kill all robot processes
  the_robot_controller.end_process();

  // wait a second before checking
  set_state( SHUTTING_DOWN_ROBOTS);
  timer.next_check( 1.0 );
}

void
ArenaController::end_sequence_follow_up()
{
  // check if the process have stopped, otherwise kill
  the_robot_controller.end_sequence();
}

void ArenaController::set_state( const state_t st )
{
  state = st;
}



ArenaController::~ArenaController()
{
  if( started ) close_arena();
  list<ArenaObserver*>::iterator li;
  for( li = arena_observers.begin() ; li != arena_observers.end(); li ++ )
    delete (*li);

  arena_observers.clear();
}

void
ArenaController::close_arena()
{
  if( started )
    {
      delete my_arena;
    }

  started = false;
}



void
ArenaController::set_tournament_values(list<start_tournament_info_t>& robotfilename_list,
				       start_tournament_info_t& arenafilename,
				       int nb_teams)
{
  clear();

  // Create robot classes and put them into the list robots

  Robot* robotp;

  int team_count = 0;

  if( nb_teams != 0 )
    the_robot_controller.set_nb_team( nb_teams );
  else
    the_robot_controller.set_nb_team( robotfilename_list.size() ); //One team by robot ?

  arena_filename = arenafilename.filename;

  list<start_tournament_info_t>::iterator li;

  int robot_count = 0;

  for( li = robotfilename_list.begin();
       li != robotfilename_list.end();
       li ++ )
    {
      //Set values before creating the robot
      string filename;
      char robot_io_type;
      int robot_team;
      if(    li->filename[0] == 'l'          //Local robot (RobotProcess)
	  || li->filename[0] == 'm'          //Messages (RobotMessageIO)
	  || li->filename[0] == 'n'   ) {    //Network (RobotNetworkIO)
	robot_io_type = li->filename[0];
	filename = li->filename.substr( 2, li->filename.size() );
      } else {
	robot_io_type = 'l';
	filename = li->filename;
      }

      //Create is IO
      RobotIO* robot_io = NULL;
      switch( robot_io_type ) {
      case 'l': //Local robot
	robot_io = new RobotProcess(filename);
	break;
      case 'm': //Message IO
	robot_io = new RobotMessageIO(filename);
	break;
      case 'n': //Network IO
	robot_io = NULL;  //new RobotNetworkIO(filename)
	cerr << "Network robots not supported yet : robot " << filename << " dropped" << endl;
	break;
      default:
	cerr << "Unknow IO type : robot " << filename <<" dropped" << endl;
	break;
      }

      if( robot_io ) {// && robot_io->succeed()
	if( nb_teams == 0 ) robot_team = ++team_count;
	else robot_team = li->team;
	robotp = new Robot( ++robot_count, filename, robot_team, robot_io );
	the_robot_controller.add_robot( robotp );
      }
    }

  // set random seed
  tournament_random_seed = timer.t_randomize();


  // start first sequence
  Header_Event evt( 1, robot_count, 1, robot_count );
  notify_event( &evt );

  the_opts.log_all_options();
}

void
ArenaController::parse_command_line(int argc, char **argv)
{
  int version_flag=false, help_flag=false;
  int c;

  extern char* optarg;
  extern int optind;

  string log_filename = "" ;

  //gtk_obs = new BasicGtkGui( argc, argv );

  /* NOTE : END NOT GOOD !!! */
  static struct option long_options[] =
  {
    //option, argument?, flag, value
    {"version", 0, &version_flag, true},
    {"help", 0, &help_flag, true},

    {"debug_mode", 0, &game_mode, ArenaBase::DEBUG_MODE},
    {"debug_level", 1, 0, 0},
    {"normal_mode", 0, &game_mode, ArenaBase::NORMAL_MODE},
    {"competition_mode", 0, &game_mode, ArenaBase::COMPETITION_MODE},
    {"option_file", 1, 0, 0},
    {"log_file", 1, 0, 0},
    {"statistics_file", 1, 0, 0},
    {"tournament_file", 1, 0, 0},
    {"message_file", 1, 0, 0},

    {0, 0, 0, 0}
  };

  for(;;)
    {
      int option_index = 0;
     
      c = getopt_long( argc, argv, "dncD:vho:l:s:t:m:r:g",
                       long_options, &option_index );

      // Detect the end of the options.
      if (c == -1)
        break;
     
      switch (c)
        {

        case 0:
          // If this option sets a flag, do nothing else now.
          if( long_options[option_index].flag != 0 )
            break;
          
          switch( option_index )
            {
            case 3:
	      sscanf( optarg, "%d", & (debug_level) );
              game_mode = ArenaBase::DEBUG_MODE;
              break;
            case 6: 
              option_filename = (string)optarg;
              break;
            case 7:
	      log_filename = (string)optarg;
	      break;
            case 8:
              statistics_filename = (string)optarg;
              break;
            case 9:
              tournament_filename = (string)optarg;
              auto_start_and_end = true;
              break;
            case 10:
              message_filename = (string)optarg;
              break;
            default:
              Error( true, "Bad error, this shouldn't happen",
                     "RealTimeBattle.cc:parse_command_line" );
              exit( EXIT_FAILURE );
            }
          break;


        case 'd':
          game_mode = ArenaBase::DEBUG_MODE;
          break;

        case 'D':
	  sscanf( optarg, "%d", & (the_arena_controller.debug_level) );
          game_mode = ArenaBase::DEBUG_MODE;
          break;

        case 'n':
          game_mode = ArenaBase::NORMAL_MODE;
          break;

        case 'c':
          game_mode = ArenaBase::COMPETITION_MODE;
          break;

        case 'v':
          version_flag = true;
          break;

        case 'h':
          help_flag = true;
          break;

        case 'o':
          option_filename = (string)optarg;
          break;

        case 'l':
          log_filename = (string)optarg;
          break;

        case 's':
          statistics_filename = (string)optarg;
          break;

        case 't':
          tournament_filename = (string)optarg;
          auto_start_and_end = true;
          break;

        case 'm':
          message_filename = (string)optarg;
          break;

        default:
          print_help_message();
          exit( EXIT_FAILURE );
        }
    }

  if( debug_level > max_debug_level )
    debug_level = 5;
  if( debug_level < 0 )
    debug_level = 0;

  if(optind != argc) 
    {
      print_help_message();
      exit( EXIT_FAILURE );
    }

  if( version_flag ) {
      cout << "RealTimeBattle Core " << VERSION << endl;
  }

  if( help_flag )
    print_help_message();

  if( help_flag || version_flag ) exit( EXIT_SUCCESS );

  if( log_filename != "" )
    {
      ArenaObserver* log = new TextLog( log_filename );
      if( log->is_ready() ) {
	arena_observers.push_back( log );
      } else { //Something went wrong while creating the observer
	delete log;  
      } 
    }

  if( option_filename == "" )
    {
      the_opts.get_options_from_rtbrc();
      the_arena_controller.option_filename = ".rtbrc";
    }
  else
    the_opts.read_options_file( option_filename,true );

  //Start an interactive mode ?
  auto_start_and_end
    = ( the_arena_controller.tournament_filename != "" );

  if( auto_start_and_end == false )
    {
      cerr << "No tournament file gives me nothing to do. Leaving ..." << endl;
      exit( EXIT_SUCCESS );
    }
}

void
ArenaController::clear()
{
  set_state( NOT_STARTED );
}

void
ArenaController::notify_tic( double time )
{
  list<ArenaObserver*>::iterator li;
  for( li = arena_observers.begin(); li != arena_observers.end(); li ++ )
    (*li)->tic( time );
}

void
ArenaController::notify_event( Observable_Event* ob_evt )
{
  list<ArenaObserver*>::iterator li;
  for( li  = the_arena_controller.arena_observers.begin(); 
       li != the_arena_controller.arena_observers.end(); li ++ )
    (*li)->notify_event( ob_evt );
}

bool
ArenaController::check_state_for_message(const message_from_robot_type msg_t) {
  return true;
}




