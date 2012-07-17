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

#include "DebugLog.h"
#include "Various.h"

DebugLog::DebugLog( ) 
{
  bool log_stdout;
 
  log_stdout = true;
  //LOG_FILE.attach(STDOUT_FILENO);
  ready = true;
  prec  = 2;
}

DebugLog::~DebugLog() 
{
  LOG_FILE.close();
}

#include <map>
map<int, double> robots_energy;

void DebugLog:: notify_event( Observable_Event * ob_evt ) 
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

  LOG_FILE << setiosflags(ios::fixed) << setprecision(prec);

  switch( ob_evt->get_type() )
    {
    case 'R': // Robot position info
      {
	Robot_Position_Info_Event* r_evt = (Robot_Position_Info_Event*) ob_evt;
	//	LOG_FILE<<"<Robot energy : "<< r_evt->energy <<">"<<endl;
	robots_energy[r_evt->id] = r_evt->energy;
      }
      break;
	  
      
    case 'P': // Print a robot message
      {
	Robot_Print_Message_Event* p_evt = (Robot_Print_Message_Event*) ob_evt;
	LOG_FILE << p_evt->id << " " << p_evt->message << endl;
      }
      break;
      
      
    case 'C': // Cookie
    case 'M': // Mine
      {  
	New_Static_Object_Event* s_evt = (New_Static_Object_Event*) ob_evt;
	LOG_FILE << "<" << flush;
	LOG_FILE << ob_evt->get_type() ;
	LOG_FILE << s_evt->id << " " 
		 << s_evt->x << " " << s_evt->y << ">" << endl;
      }
      break;

    case 'D': // Die
      {
	Object_Die_Event* d_evt = (Object_Die_Event*) ob_evt;

	if( d_evt->obj_type == 'S' ) break;
	LOG_FILE << "<" << flush;
	LOG_FILE << ob_evt->get_type() ;
	if( d_evt->obj_type != 'R' ) {
	  LOG_FILE << d_evt->obj_type << d_evt->id ;
	} else {
	  Robot_Die_Event* r_evt = (Robot_Die_Event*) d_evt;
	  LOG_FILE << "R" << r_evt->id << " "
		   << setprecision(1) << r_evt->pts << " "
		   << r_evt->pos ;
	}
	LOG_FILE << ">" << endl;
  
      }
      break;
      /*  
	  case 'G': // Game begins
	  {
	  Game_Event* g_evt = (Game_Event*) ob_evt;
	  LOG_FILE << g_evt->game << " " << g_evt->seq ;
	  }
	  break;
	  
	  case 'H': // Header
	  {
	  Header_Event* h_evt = (Header_Event*) ob_evt;
	  LOG_FILE << h_evt->a << " " << h_evt->b << " " << h_evt->c << " " << h_evt->d;
	  }
	  break;
	  
	  case 'L': // List of robot properties
	  {
	  Property_List_Event* l_evt = (Property_List_Event*) ob_evt;
	  LOG_FILE << l_evt->id << " "
	  << hex << l_evt->color << dec << " "  // robot colour in hexa
	  << l_evt->name;
	  }
	  break;
	  
	  case 'A': // Arena file line
	  {
	  cout<<"Arena log\n";
	  Arena_Line_Event* a_evt = (Arena_Line_Event*) ob_evt;
	  LOG_FILE << a_evt->line ;
	  }
	  break;
      */  
      /*
    case 'O':
      {
	Option_Event* o_evt = (Option_Event*) ob_evt;
	LOG_FILE << "<"<<o_evt->option<<">\n";
      }
      break;
      */
    }
} 


int count_debug_log_var = 0;

void DebugLog:: tic( double time )
{
  if( count_debug_log_var ++ == 50 ) {
  LOG_FILE << setiosflags(ios::fixed) << setprecision(prec);
  LOG_FILE << "<T" << setprecision(prec+1) << time << ">"<< endl ;

  count_debug_log_var = 0;
  LOG_FILE <<"<" <<flush;
  for( map<int, double>::iterator mi = robots_energy.begin();
       mi != robots_energy.end(); mi ++ )
    {
      LOG_FILE << mi->second << " ";
    }
  LOG_FILE << ">" << endl;
  }
}

