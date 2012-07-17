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

#ifndef __VARIOUS__
#define __VARIOUS__


#include <list>
#include <string>

  using namespace std;

struct start_tournament_info_t;

namespace various {
  template<class T>
  inline T min( const T& a, const T& b ) {
    if( a < b ) return a;
    else        return b;
  }

  template<class T>
  inline T max( const T& a, const T& b ) {
    if( a > b ) return a;
    else        return b;
  }

  template<class T>
  inline T abs( const T& a ) {
    if( a > 0 ) return a;
    else        return -a;
  }

}


#define sgn(a) ((a) > 0 ? 1 : -1)

enum entry_datatype_t
{
  ENTRY_INT,
  ENTRY_DOUBLE,
  ENTRY_HEX,
  ENTRY_CHAR,
  ENTRY_BOOL
};

struct entry_t
{
  entry_t(entry_datatype_t d, bool s) :
    datatype(d), allow_sign(s) {}
  entry_datatype_t datatype;
  bool allow_sign;
};

void Error(const bool fatal, const string& error_msg, const string& function_name);
void Quit(const bool success = true);

int factorial(const int n);

int binomial(const int n, const int k);

void reorder_pointer_array(void** array, int size);

void read_dirs_from_system(list<string>& robotdirs, list<string>& arenadirs);
void split_colonseparated_dirs(string& dirs, list<string>& arenadirs);

bool check_if_filename_is_robot( string& fname );
bool check_if_filename_is_arena( string& fname );
void check_for_robots_and_arenas( string& word, 
                                  list<start_tournament_info_t>& tour_list,
                                  list<string>& dir_list, 
                                  const bool check_robots, int );
void search_directories( string directory, list<start_tournament_info_t>& tour_list,
                         const bool check_robots, int );
bool parse_tournament_file( const string& fname,
			    bool fatal_error_on_file_failure = true );


void create_tmp_rtb_dir();

#endif /* __VARIOUS__ */



