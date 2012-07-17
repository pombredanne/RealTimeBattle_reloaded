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

#ifndef __FUNCTION_GADGET__
#define __FUNCTION_GADGET__

struct FunctionDefinition
{
  char* name;
  bool enabled;
};

class Function : public Gadget
{
public:
  Function() {}
  Function(const char* name, Gadget* const p) 
    : Gadget(name, p, GAD_FUNCTION), activated(false) {}
  Function(const char* name, Gadget* const p, const int fcn ) 
    : Gadget(name, p, GAD_FUNCTION), fcn_nr(fcn), activated(false) {}

  ~Function() {}

  static Gadget* new_function( const char* name, Gadget* const p )
  {
    return new Function( name, p );
  }

  //void operator() () { parent->eval_function(fcn_nr); }

  //Without this function, we lose all the informations in the GadgetInfo info
  const Function& operator= (const Function& f) 
    {
      parent = f.parent;
      info.name = f.info.name;
      return *this; 
    }


  int get_fcn_nr() { return fcn_nr; }
  void set(const int fcn) { fcn_nr = fcn; }

  void set_enable( const bool a = true ) 
    { activated = a; }
  bool is_enable() 
    { return activated; }

private:

  int fcn_nr;
  bool activated;

};


#endif // __FUNCTION_GADGET__
