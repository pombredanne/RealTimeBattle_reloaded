/*
RealTimeBattle, a robot programming game for Unix
Copyright (C) 1998-2002  Erik Ouchterlony and Ragnar Ouchterlony

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

#include <stack>
#include <fstream>
#include <strstream>

#include <typeinfo>
#include <set>
#include <algorithm>
#include <vector>

#include <unistd.h>
#include <math.h>
#include <assert.h>

#include "Arena.h"
#include "AllGadgets.h"
#include "Vector2D.h"
#include "String.h"
#include "Messagetypes.h"
#include "Various.h"
#include "Shot.h"
#include "Robot.h"
#include "SubSquareIterators.h"
#include "IntlDefs.h"

Arena::Arena(const string& s, set<Robot*> &S)
{
  robots = S;
  load_arena_file(s, gadget_hierarchy);
}

void
Arena::add_shot(Shot* s, bool from_robot)
{
  if(  shots.insert(s).second == false )
    Error(true, "Couldn't add element", "Arena::add_shot");
}


void
Arena::update_robots()
{
  set<Robot*>::iterator li;

  for( li = robots.begin(); li != robots.end(); li++ )
    if( !(*li)->is_killed() )  
      {
        (*li)->update();
      }
}

void
Arena::find_subsquares_for_shape( Shape* s )
{
  for(int i=0; i< number_of_subsquares_x; i++)
    for(int j=0; j< number_of_subsquares_y; j++)
      {        
        if( s->is_inside_subsquare( subsquares[i][j] ) )
          {
            subsquares[i][j].add( s );
          }
            
      }
}

SubSquare*
Arena::get_subsquare_at( const Vector2D& pos ) const
{
  int x = (int)((pos[0] - top_left[0] ) / subsquare_size[0]);
  int y = (int)((pos[1] - top_left[1] ) / subsquare_size[1]);

  assert( x >= 0 && x < number_of_subsquares_x );
  assert( y >= 0 && y < number_of_subsquares_y );
  
  return( &subsquares[x][y] );
}

void
Arena::get_subsquare_at( const Vector2D& pos, int& x, int& y ) const
{
  x = (int)((pos[0] - top_left[0] ) / subsquare_size[0]);
  y = (int)((pos[1] - top_left[1] ) / subsquare_size[1]);
}


double
Arena::get_shortest_distance(const Vector2D& pos, const Vector2D& vel, 
                             const double size, enum object_type& closest_shape, 
                             Shape*& colliding_object, 
                             const class Shape* from_shape = NULL )
{
  
  SubSquareLineIterator ss_it(pos, vel);
  double dist;

  for( ; ss_it.ok(); ss_it++ )
   {
     
     dist = subsquares[ss_it.x()][ss_it.y()].
       get_shortest_distance(pos, vel, size, closest_shape, colliding_object, from_shape );
     
     if( dist < DBL_MAX/2.0 ) return dist;
   }     

 return DBL_MAX;
}


void
Arena::load_arena_file( const string& filename, Gadget& hierarchy )
{
  enum load_file_mode_t { LAF_DEFINING_MODE, LAF_VARIABLE_MODE,
                          LAF_SCRIPT_MODE, LAF_GEOMETRY_MODE,
			  LAF_ENVIRONMENT_MODE };

  load_file_mode_t mode = LAF_DEFINING_MODE;
  stack<int> mode_stack;

  string top_filename = filename;
  if( !find_full_arena_filename( top_filename ))
    Error( true, "Couldn't find this arena: " + filename, "Arena::load_arena_file" );

  string top_file_path = top_filename.substr( 0, top_filename.rfind( '/' ) );

  stack<ifstream*> file_stack;   ifstream* current_file;
  file_stack.push( current_file = new ifstream( top_filename.c_str() ) );

  bool first_line = true;

  Gadget* current_gadget = &hierarchy;

  string str_buffer;
  while( !file_stack.empty() )
    {
      str_buffer = "";
      bool keep_reading_line = true;

      //Read a complete line (continue if the '\\' caracter is at the end)
      //What happend if there is a space at the end : "\\ \n" ?
      while( keep_reading_line )
        {
	  char buffer[400];
	  
	  if( current_file->fail() || current_file->eof() )
	    { //TODO : make two separate cases !
	      delete current_file;
	      file_stack.pop();
	      //if(file_stack.size() > 0)
	      current_file = file_stack.top();
	      continue;
	    }
	  current_file->get( buffer, 400, '\n' );
	  current_file->get();

          str_buffer += string(buffer);

          if( str_buffer[str_buffer.length() - 1] == '\\' )
	    str_buffer.erase( str_buffer.length() - 1 );
	  else
	    keep_reading_line = false;
        }

      vector<string> wordlist;
      wordlist = special_split_string( str_buffer, wordlist );

      if( wordlist.size() > 0 )
        {
	  /*
	  cout<<(int) mode<<" : ";
	  for(unsigned int i = 0; i < mode_stack.size(); i++)
	    cout<<"\t";
	  for(unsigned int i = 0; i < wordlist.size(); i ++)
	    cout<<"{"<<wordlist[i]<<"} ";
	  cout<<endl;
	  */
	  //
	  //  Mode free analyse
	  //

          if( first_line && wordlist[0][0] == '!' )  //This may be the arena version
            {
              if( !sufficient_arena_version( wordlist ) )   ;
              first_line = false;
            }
          if( equal_strings_nocase( wordlist[0], "Include" ) )
            {
	      cout<<"Including a new file "<< wordlist[1] <<"\n";
              if( !find_full_arena_filename( wordlist[1], top_file_path, true ) )
                Error( false, "Couldn't find this include file: " + wordlist[1],
                       "Arena::load_arena_file" );
	      //Put the include file on the top of the stack
              file_stack.push( current_file = new ifstream( wordlist[1].c_str() ) );
            }
          else if( /*mode == LAF_DEFINING_MODE &&*/
                   equal_strings_nocase( wordlist[0], "Define" ) &&
                   wordlist.size() > 2 )  
            {
              Gadget* gadget =
                AllGadgets::create_gadget_by_stringtype( wordlist[1],
                                                         wordlist[2].c_str(),
                                                         current_gadget );

              assert( gadget != NULL );

              current_gadget->get_my_gadgets().add( gadget->get_info() );
              current_gadget = dynamic_cast<Gadget*> (gadget);  //Is it necessary ?

	      mode_stack.push(mode);

              if( gadget->get_info().type == GAD_SCRIPT )  //Use a stack instead !!!
                mode = LAF_SCRIPT_MODE;
	      else if( gadget->get_info().type == GAD_GEOMETRY )
		mode = LAF_GEOMETRY_MODE;
	      else if( gadget->get_info().type == GAD_VARIABLE )
		mode = LAF_VARIABLE_MODE;
	      else if( gadget->get_info().type == GAD_ENVIRONMENT )
		mode = LAF_ENVIRONMENT_MODE;
	      else
		mode = LAF_DEFINING_MODE;
            }
          else if( equal_strings_nocase( wordlist[0], "EndDefine" ) )
            {
              if( wordlist.size() > 2 && wordlist[2] != current_gadget->get_name() )
                Error( true, "Ending wrong definition", "Arena::load_arena_file" );
              current_gadget = current_gadget->get_parent();

	      //TODO : make it end in a different way !
	      if(mode == LAF_ENVIRONMENT_MODE)  //this should be the end of the file ?!?
		{
		  //NOTE : This must not figure in the final code !!!
		  cout<<"Now the arena is ready ;\n";
		  /*
		    cout<<"You can try to browse its definition!!!\n";
		    stack<Gadget*> gadget_stack;
		    current_gadget = &hierarchy;
		    while(1) {
		    string action;
		    cout<<"We are in gadget "<<current_gadget->get_name()<<endl;
		    for(set<GadgetInfo>::iterator i = current_gadget->get_my_gadgets().the_set.begin();
		    i != current_gadget->get_my_gadgets().the_set.end(); i ++)
		    cout<<"\t"<<(*i).name<<endl;
		    cin>>action;
		    if(action == "q") return;
		    if(action == ".." && current_gadget->get_parent() != NULL) current_gadget = current_gadget->get_parent();
		    else current_gadget = current_gadget->get_my_gadgets().find_by_name(action);

		    }
		  */
		  return;
		}

	      if(mode_stack.size() > 0)
		{
		  mode = (load_file_mode_t)mode_stack.top();
		  mode_stack.pop();
		}
	    }
	  else if( equal_strings_nocase( wordlist[0], "InfoString") )
	    {
	    }

	  //
	  // Mode dependant analyse
	  //

          else if( mode == LAF_DEFINING_MODE &&
                   equal_strings_nocase( wordlist[0], "Function" ) &&
                   wordlist.size() > 2 )
            { //Is this the right place for this ???
	      Function* func_p = dynamic_cast<Function*> ( current_gadget->find_by_name( wordlist[1] ) );
	      assert( func_p != NULL );

              if( equal_strings_nocase( wordlist[2], "default" ) )
                {
                  //Set Function allowance to default (do nothing)
                }
              else if( equal_strings_nocase( wordlist[2], "true" ) )
                {
		  //Set Function allowance to true
		  func_p->set_enable();
                }
              else if( equal_strings_nocase( wordlist[2], "false" ) )
                {
                  //Set Function allowance to false
		  func_p->set_enable( false );
                }
            }          
          else if( mode == LAF_VARIABLE_MODE )
            {
	      Variable* var = dynamic_cast<Variable*> (current_gadget);
	      var->set_variable_info(wordlist[0], wordlist[1]);
            }
          else if( mode == LAF_SCRIPT_MODE )
            {
              (dynamic_cast<Script*>(current_gadget))->add_script_line( wordlist[0] );
            }
          else if( mode == LAF_GEOMETRY_MODE )
            {
	      GeometryGadget* the_geometry = dynamic_cast<GeometryGadget*> (current_gadget);

	      if(equal_strings_nocase(wordlist[0], "Scale"))
		{
		  istrstream is(wordlist[1].c_str());
		  double d; is >> d;
		  the_geometry->set_scale( d );
		}
	      else if(equal_strings_nocase(wordlist[0], "Boundary"))
		{
		  wordlist.erase( wordlist.begin() );
		  the_geometry->set_boundary( wordlist );
		}
	      else {
		WallGadget* the_shape_wall 
		  = dynamic_cast<WallGadget*> (the_geometry->find_by_name( wordlist[1] )); 
		
		the_geometry->set_shape(wordlist, the_shape_wall);
	      }		
            }
	  else if( mode == LAF_ENVIRONMENT_MODE && 
		   equal_strings_nocase( wordlist[0], "GeometryList" ))
	    {
	      EnvironmentGadget* environment = dynamic_cast<EnvironmentGadget*> (current_gadget);
	      wordlist.erase( wordlist.begin() );
	      environment->set_geometry_list(wordlist);
	    }
          else if( wordlist.size() > 1 )
            {
	      if(wordlist[1][0] == '$')
		{ //This is a particular variable of the system
		  Gadget* gadp = current_gadget->find_by_name
		    ( wordlist[1].substr(1, wordlist[1].length() - 1) );
		  assert( gadp != NULL );
		  Gadget* gad_res = current_gadget->create_instance(wordlist[0], gadp);
		  assert( gad_res != NULL );
		}
	      else
		{
		  Gadget* gadp;
		  
		  if( (gadp = (current_gadget->find_by_name( wordlist[0] ))))
		    { //This is a normal variable ;
		      Variable* var_p = (Variable*) gadp;
		      
		      if(var_p->type() == DOUBLE_V) {
			istrstream is(wordlist[1].c_str()); double d;
			is >> d;
			(*var_p) = Value( d );
		      } else if(var_p->type() == INT_V) {
			int i;
			istrstream is(wordlist[1].c_str());
			is>>i;
			(*var_p) = Value( i );
		      } else if(var_p->type() == BOOL_V) {
			bool b = (wordlist[1] == string("true"));
			var_p->make_bool( b );
		      }
		    }
		}
            }
        }
    }
}

// Returns true if filename found. filename changes to be the full path of the
// supplied filename. If filename is already a full filename nothing is changed.
// Searches the arena path for the arena (searches top_file_path first if this
// is non-empty). 
const bool
Arena::find_full_arena_filename( string& filename, const string& top_file_path,
                                 const bool included ) const
{
  bool (*check_file_function)( const string& ) = check_if_filename_is_arena;
  if( included )
    check_file_function = check_if_filename_is_regular_file;

  if( (*check_file_function)( filename ) )
    return true;
  if( (!top_file_path.empty()) &&
      (filename.length() > 0) &&
      (filename[0] != '/') &&
      ((*check_file_function)( top_file_path + filename )) )
    {
      filename = top_file_path + filename;
      return true;
    }
  list<string> arenadirs;
  read_arenadirs_from_system( arenadirs );
  for( list<string>::const_iterator lci = arenadirs.begin();
       lci != arenadirs.end(); lci++ )
    if( (*check_file_function)( *lci + filename ) )
      {
        filename = top_file_path + filename;
        return true;
      }
  return false;
}

// Returns true if the version on the arena file is accepted.
const bool
Arena::sufficient_arena_version( vector<string>& wordlist ) const
{
  // Note: Is it necessary to specify arena-version?
  return true;
}

void
Arena::garbage_collector()
{
  // The killed objects must be removed from all subsquares first

  // clean subsquares

  for( int i=0; i < number_of_subsquares_x; i++ )
    for( int j=0; j < number_of_subsquares_y; j++ )
      {
        subsquares[i][j].clean();
      }


  //clean lists

  for( set<Shot*>::iterator li = shots.begin(); li != shots.end(); li++ )
    if( (*li)->is_killed() )  { shots.erase(li); delete (*li); }

  for( set<Robot*>::iterator li = robots.begin(); li != robots.end(); li++ )
    if( (*li)->is_killed() )  robots.erase(li);

  for( set<MovingObject*>::iterator 
         li = moving_objects.begin(); li != moving_objects.end(); li++ )
    if( (*li)->is_killed() ) { moving_objects.erase(li);  delete (*li); }

  for( set<Shape*>::iterator
         li = updatable_objects.begin(); li != updatable_objects.end(); li++ )
    if( (*li)->is_killed() ) { updatable_objects.erase(li);  delete (*li); }

  for( set<Shape*>::iterator
         li = static_objects.begin(); li != static_objects.end(); li++ )
    if( (*li)->is_killed() ) { static_objects.erase(li);  delete (*li); }

}


bool
Arena::all_robots_ready()
{
  for(set<Robot*>::iterator i = robots.begin();
      i != robots.end(); i ++ )
    {
      if( ! (**i).is_connected() )  return false;
    }
  return true;
}

void
Arena::start()
{
  Script* begining = dynamic_cast<Script*>
    (gadget_hierarchy.find_by_name( "TheEnvironment" )->find_by_name( "AtBegining" ));
  if( begining )
    {
      cout<<"I have the good script !!!\n";
      sleep( 1 );
    }
}
