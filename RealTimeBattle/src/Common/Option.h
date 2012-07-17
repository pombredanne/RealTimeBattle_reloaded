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

#ifndef __OPTION__
#define __OPTION__

//#include "Various.h"
#include <string>

enum option_value_t
{
  OPTION_VALUE_LONG,
  OPTION_VALUE_DOUBLE,
  OPTION_VALUE_STRING,
  OPTION_VALUE_BOOLEAN
};

// ---------------------------------------------------------------------------
// class Option
// ---------------------------------------------------------------------------
// Base class for different options
// ---------------------------------------------------------------------------

class Option
{
public:
  Option                               () {}
  Option                               ( option_value_t v, const int g,
                                         const bool br, const bool lo, const char* t )
    : value_type(v), group(g), broadcast_option(br), log_option(lo), description(t) {}
  virtual ~Option                      () {}

  virtual Option* copy                 () = 0;

  virtual const string get_string_val  () const = 0;
  virtual const string get_string_min  () const = 0;
  virtual const string get_string_max  () const = 0;
  virtual const string get_string_def  () const = 0;
  const option_value_t get_value_type  () const { return value_type; }
  const int get_group                  () const { return group; }
  const bool broadcast                 () const { return broadcast_option; }
  const bool log                       () const { return log_option; }
  const string& get_description        () const { return description; }

  virtual const bool change_value      ( const string&, const bool def = false ) = 0;
  virtual string& make_correct_string_val( string& ) const = 0;
  
protected:
  option_value_t value_type;
  const int group;

  bool broadcast_option;
  bool log_option;

  string description;
};

// ---------------------------------------------------------------------------
// class LongOption
// ---------------------------------------------------------------------------
// Options that consists of an integer.
// User might see the value as hexadecimal.
// ---------------------------------------------------------------------------

class LongOption : public Option
{
public:
  LongOption                           () {}
  LongOption                           ( const int g, const long int val,
                                         const long int mn, const long int mx,
                                         const bool br, const bool lo,
                                         const char* t,
                                         const bool hex = false )
    : Option(OPTION_VALUE_LONG,g,br,lo,t), value(val), default_value(val),
    min_value(mn), max_value(mx), hexadecimal(hex) {}
  ~LongOption                          () {}

  Option* copy                         ();
  const long int get_value             () const { return value; }
  const string get_string_val          () const { return make_string_val( value ); }
  const string get_string_min          () const
  { return make_string_val( min_value ); }
  const string get_string_max          () const
  { return make_string_val( max_value ); }
  const string get_string_def          () const
  { return make_string_val( default_value ); }
  void reset_value                     ();
  const bool is_value_accepted         ( const long int ) const;
  const bool change_value              ( const long int, const bool def = false );
  const bool change_value              ( const string&, const bool def = false );
  const bool is_value_hexadecimal      () const { return hexadecimal; }
  const long int get_min_value         () const { return min_value; }
  const long int get_max_value         () const { return max_value; }
  string& make_correct_string_val      ( string& ) const;

private:
  const string make_string_val         ( const long int ) const;
  long int value;
  long int default_value;
  long int min_value;
  long int max_value;
  bool hexadecimal;
};

// ---------------------------------------------------------------------------
// class DoubleOption
// ---------------------------------------------------------------------------
// Options that consists of a double.
// ---------------------------------------------------------------------------

class DoubleOption : public Option
{
public:
  DoubleOption                         () {}
  DoubleOption                         ( const int g, const double val,
                                         const double mn, const double mx,
                                         const bool br, const bool lo,
                                         const char* t )
    : Option(OPTION_VALUE_DOUBLE,g,br,lo,t), value(val), default_value(val),
    min_value(mn), max_value(mx) {}
  ~DoubleOption                        () {}

  Option* copy                         ();
  const double& get_value              () const { return value; }
  const string get_string_val          () const;
  const string get_string_min          () const;
  const string get_string_max          () const;
  const string get_string_def          () const;
  void reset_value                     ();
  const bool is_value_accepted         ( const double ) const;
  const bool change_value              ( const double, const bool def = false );
  const bool change_value              ( const string&, const bool def = false );
  const double get_min_value           () const { return min_value; }
  const double get_max_value           () const { return max_value; }
  string& make_correct_string_val      ( string& ) const;

private:
  double value;
  double default_value;
  double min_value;
  double max_value;
};

// ---------------------------------------------------------------------------
// class StringOption
// ---------------------------------------------------------------------------
// Options that consists of a string.
// ---------------------------------------------------------------------------

class StringOption : public Option
{
public:
  StringOption                         () {}
  StringOption                         ( const int g, const string& val,
                                         const unsigned int mc,
                                         const bool br, const bool lo,
                                         const char* t )
    : Option(OPTION_VALUE_STRING,g,br,lo,t), value(val),
      default_value(val), max_chars(mc) {}
  ~StringOption                        () {}

  Option* copy                         ();
  const string& get_value              () const { return value; }
  const string get_string_val          () const { return value; }
  const string get_string_min          () const { return ""; }
  const string get_string_max          () const { return ""; }
  const string get_string_def          () const { return default_value; }

  void reset_value                     ();
  const bool change_value              ( const string&, const bool def = false );
  string& make_correct_string_val      ( string& str ) const { return str; }

private:
  string value;
  string default_value;

  unsigned int max_chars;
};

// ---------------------------------------------------------------------------
// class BooleanOption
// ---------------------------------------------------------------------------
// Options that can be just two values, true or false,
// represented as 0 or 1.
// ---------------------------------------------------------------------------

class BooleanOption : public Option
{
public:
  BooleanOption                        () {}
  BooleanOption                        ( const int             g,
                                         const bool            val,
                                         const bool            br,
                                         const bool            lo,
                                         const char*           t )
    : Option( OPTION_VALUE_BOOLEAN, g, br, lo, t ),
      value( val ), default_value( val ) {}
  ~BooleanOption                       () {}

  Option* copy                         ();
  const bool get_value                 () const { return value; }
  const string get_string_val          () const
  { return make_string_of_bool(value); }
  const string get_string_min          () const { return "false"; }
  const string get_string_max          () const { return "true"; }
  const string get_string_def          () const
  { return make_string_of_bool(default_value); }

  const bool change_value              ( const bool            v,
                                         const bool            def = false );
  const bool change_value              ( const string&         v,
                                         const bool            def = false );

  string& make_correct_string_val      ( string&               v ) const;

private:
  string make_string_of_bool           ( const bool            v ) const;

  bool value;
  bool default_value;
};

#endif // __OPTION__
