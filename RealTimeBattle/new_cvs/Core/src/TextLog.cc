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

#include <unistd.h>
#include <stdlib.h>
#include <iostream>
#include <iomanip>
#include <stdarg.h>
#include <sys/stat.h>
#include <stdio.h>
#include <sstream>
#include <fcntl.h>

  using namespace std;

#include "TextLog.h"
#include "Various.h"

TextLog::TextLog( string log_fname )
{
  bool log_stdout;


     if( log_fname == "-" || log_fname == "STDOUT" )  // use stdout as (*LOG_FILE)
     {
     log_stdout = true;
     LOG_FILE = new boost::fdostream(STDOUT_FILENO);
     }
     else

    {
      LOG_FILE = new boost::fdostream( open(log_fname.c_str(), ios::out ) );
      log_stdout = false;
      if( !(*LOG_FILE) )
        {
          Error( false, "Couldn't open log file. Contuing without log file",
                 "ArenaRealTime::set_filenames" );

	  ready = false; return;
        }
    }
  ready = true;
  prec  = 2;
}

TextLog::~TextLog()
{
  delete LOG_FILE;
}


void TextLog:: notify_event( Observable_Event * ob_evt )
{
  /*
    // log 'R' only each OPTION_LOG_EVERY_NTH_UPDATE_INTERVAL
    if( update_count_for_logging != 0 && first_letter == 'R' )
    {
    va_end(args);
    return;
    }
  */
  int prec = 2;

  (*LOG_FILE) << setiosflags(ios::fixed) << setprecision(prec);
  (*LOG_FILE) << ob_evt->get_type() ;

  switch( ob_evt->get_type() )
    {
    case 'T':
      break;

    case 'R': // Robot position info
      {
	Robot_Position_Info_Event* r_evt = (Robot_Position_Info_Event*) ob_evt;
	(*LOG_FILE) << r_evt->id << " " << r_evt->x << " " << r_evt->y << " "
		 << r_evt->ro_ang << " " << r_evt->ca_ang << " " << r_evt->ra_ang << " " 
		 << r_evt->energy ;
      }
      break;
	  
    case 'P': // Print a robot message
      {
	Robot_Print_Message_Event* p_evt = (Robot_Print_Message_Event*) ob_evt;
	(*LOG_FILE) << p_evt->id << " " << p_evt->message ;
      }
      break;

    case 'C': // Cookie
    case 'M': // Mine
      {  
	New_Static_Object_Event* s_evt = (New_Static_Object_Event*) ob_evt;
	(*LOG_FILE) << s_evt->id << " " 
		 << s_evt->x << " " << s_evt->y ;
      }
      break;
	  
    case 'S': // Shot
      {
	Shot_Event* s_evt = (Shot_Event*) ob_evt;
	(*LOG_FILE) << s_evt->id << " " << s_evt->x << " " << s_evt->y << " " 
		 << setprecision(prec+1) << s_evt->dx << " " 
		 << setprecision(prec+1) << s_evt->dy; 
      }
      break;
	  
    case 'D': // Die
      {
	Object_Die_Event* d_evt = (Object_Die_Event*) ob_evt;
	if( d_evt->obj_type != 'R' ) {
	  (*LOG_FILE) << d_evt->obj_type << d_evt->id ;
	} else {
	  Robot_Die_Event* r_evt = (Robot_Die_Event*) d_evt;
	  (*LOG_FILE) << "R" << r_evt->id << " "
		   << setprecision(1) << r_evt->pts << " "
		   << r_evt->pos ;
	}
      }
      break;
	  
    case 'G': // Game begins
      {
	Game_Event* g_evt = (Game_Event*) ob_evt;
	(*LOG_FILE) << g_evt->game << " " << g_evt->seq ;
      }
      break;
	  
    case 'H': // Header
      {
	Header_Event* h_evt = (Header_Event*) ob_evt;
	(*LOG_FILE) << h_evt->a << " " << h_evt->b << " " << h_evt->c << " " << h_evt->d;
      }
      break;
	  
    case 'L': // List of robot properties
      {
	Property_List_Event* l_evt = (Property_List_Event*) ob_evt;
	(*LOG_FILE) << l_evt->id << " "
		 << hex << l_evt->color << dec << " "  // robot colour in hexa
		 << l_evt->name;
      }
      break;
	  
    case 'A': // Arena file line
      {
	Arena_Line_Event* a_evt = (Arena_Line_Event*) ob_evt;
	(*LOG_FILE) << a_evt->line ;
      }
      break;
	  
    case 'O':
      {
	Option_Event* o_evt = (Option_Event*) ob_evt;
	(*LOG_FILE) << o_evt->option;
	//cout<<"option "<< o_evt->option << endl;;
      }
      break;
    }
  (*LOG_FILE) << endl;
} 




void TextLog:: tic( double time )
{
  (*LOG_FILE) << setiosflags(ios::fixed) << setprecision(prec);
  (*LOG_FILE) << "T" << setprecision(prec+1) << time << endl ;
}

