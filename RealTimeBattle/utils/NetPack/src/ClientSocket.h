/*
NetPack for RealTimeBattle 
Copyright (C) 2002  Benoit Rousseau

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


#ifndef __CLIENT_SOCKET_H__
#define __CLIENT_SOCKET_H__

#include "NetConnection.h"
#include "SocketHandler.h"
#include "Packets.h"
#include <string>

class SocketClient : public SocketHandler {
public:
  SocketClient() : server_connection( NULL ), client_packet_factory( NULL ) {}
  virtual ~SocketClient();
  void set_packet_factory( PacketFactory* );
  NetConnection* connect_to_server( string hostname, int port_nb = 0 );

  void send_to_server( string );

protected:
  //Functions needed by SocketHandler
  virtual void handle_stdin( char * ) {};
  virtual int  check_fd( );
  virtual int  set_fd( );


  NetConnection* server_connection;
  PacketFactory* client_packet_factory;
};

#endif //__CLIENT_SOCKET_H__
