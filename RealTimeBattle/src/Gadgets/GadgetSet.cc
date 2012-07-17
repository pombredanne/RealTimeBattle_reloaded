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
# include <config.h>
#endif

#include <set>

#include "GadgetSet.h"

void Error(const bool fatal, const string& error_msg, const string& function_name);

#include "Gadget.h"

void
GadgetSet::add(const GadgetInfo& gi)
{
  if(  the_set.insert(gi).second == false )
    {
      Error(true, "Couldn't add element", "GagdetSet::add");
    }
}


void
GadgetSet::remove(const GadgetInfo& gi)
{
  if(  the_set.erase(gi) == 0 )
    {
      Error(true, "No elements removed", "GagdetSet::remove");
    }
}

Gadget*
GadgetSet::find_by_name(const GadgetInfo& gi) const
{
  set_iterator i = the_set.find( gi );
  if( i != the_set.end() )
    return i->gadgetp;
  else 
    return NULL;
}

Gadget*
GadgetSet::find_by_name(const string& str) const
{
  set_iterator i = the_set.find(GadgetInfo(str));
  if( i != the_set.end() )
    return i->gadgetp;
  else
    return NULL;
}
