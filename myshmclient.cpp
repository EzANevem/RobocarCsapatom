/**
 * @brief Justine - this is a rapid prototype for development of Robocar City Emulator
 *
 * @file myshmclient.cpp
 * @author  Norbert Bátfai <nbatfai@gmail.com>
 * @version 0.0.10
 *
 * @section LICENSE
 *
 * Copyright (C) 2014 Norbert Bátfai, batfai.norbert@inf.unideb.hu
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * @section DESCRIPTION
 * Robocar City Emulator and Robocar World Championship
 *
 * desc
 *
 */

#include <myshmclient.hpp>
#include <limits.h>
#include <float.h>
//#include <trafficlexer.hpp>

char data[524288];

// ezt a függvényt használjuk a légvonalbeli távolság meghatározásásra
double justine::sampleclient::MyShmClient::dst ( osmium::unsigned_object_id_type n1, osmium::unsigned_object_id_type n2 ) const
{

     justine::robocar::shm_map_Type::iterator iter1=shm_map->find ( n1 );
     justine::robocar::shm_map_Type::iterator iter2=shm_map->find ( n2 );

     osmium::geom::Coordinates c1 {iter1->second.lon/10000000.0, iter1->second.lat/10000000.0};
     osmium::geom::Coordinates c2 {iter2->second.lon/10000000.0, iter2->second.lat/10000000.0};

     return osmium::geom::haversine::distance ( c1, c2 );

}

double justine::sampleclient::MyShmClient::dst ( double lon1, double lat1, double lon2, double lat2 ) const
{

     osmium::geom::Coordinates c1 {lon1, lat1};
     osmium::geom::Coordinates c2 {lon2, lat2};

     return osmium::geom::haversine::distance ( c1, c2 );

}

// ezel a node-ok közötti légvonal távolságot határozzuk meg, majd ezeket összedajuk -> relatíve valódi távolság (az utak hossza)
double justine::sampleclient::MyShmClient::distanceMy(std::vector<osmium::unsigned_object_id_type >& wnl)
{
                double sum_length=0;

                for (auto it = wnl.begin(); it != wnl.end()-1; ++it) {
                        sum_length += justine::sampleclient::MyShmClient::dst(*it, *(it+1));
                }
     return sum_length;
}

std::vector<justine::sampleclient::MyShmClient::Gangster> justine::sampleclient::MyShmClient::gangsters ( boost::asio::ip::tcp::socket & socket, int id,
    osmium::unsigned_object_id_type cop )
{

  boost::system::error_code err;

  size_t length = std::sprintf ( data, "<gangsters " );
  length += std::sprintf ( data+length, "%d>", id );

  socket.send ( boost::asio::buffer ( data, length ) );

  length = socket.read_some ( boost::asio::buffer ( data ), err );

  if ( err == boost::asio::error::eof )
    {

      // TODO

    }
  else if ( err )
    {

      throw boost::system::system_error ( err );
    }

  /* reading all gangsters into a vector */
  int idd {0};
  unsigned f, t, s;
  int n {0};
  int nn {0};
  std::vector<Gangster> gangsters;

  while ( std::sscanf ( data+nn, "<OK %d %u %u %u>%n", &idd, &f, &t, &s, &n ) == 4 )
    {
      nn += n;
      gangsters.push_back ( Gangster {idd, f, t, s} );
    }

  std::sort ( gangsters.begin(), gangsters.end(), [this, cop] ( Gangster x, Gangster y )
  {
    return dst ( cop, x.to ) < dst ( cop, y.to );
  } );

  std::cout.write ( data, length );
  std::cout << "Command GANGSTER sent." << std::endl;

  return gangsters;
}

std::vector<justine::sampleclient::MyShmClient::Cop> justine::sampleclient::MyShmClient::initcops ( boost::asio::ip::tcp::socket & socket )
{

  boost::system::error_code err;

  size_t length = std::sprintf ( data, "<init guided %s 10 c>", m_teamname.c_str() );

  socket.send ( boost::asio::buffer ( data, length ) );

  length = socket.read_some ( boost::asio::buffer ( data ), err );


  if ( err == boost::asio::error::eof )
    {

      // TODO

    }
  else if ( err )
    {

      throw boost::system::system_error ( err );
    }

  /* reading all gangsters into a vector */
  int idd {0};
  int f, t;
  char c;
  int n {0};
  int nn {0};
  std::vector<Cop> cops;

  while ( std::sscanf ( data+nn, "<OK %d %d/%d %c>%n", &idd, &f, &t, &c, &n ) == 4 )
    {
      nn += n;
      cops.push_back ( idd );
    }

  std::cout.write ( data, length );
  std::cout << "Command INIT sent." << std::endl;

  return cops;
}


int justine::sampleclient::MyShmClient::init ( boost::asio::ip::tcp::socket & socket )
{

  boost::system::error_code err;

  size_t length = std::sprintf ( data, "<init guided %s 1 c>", m_teamname.c_str() );

  socket.send ( boost::asio::buffer ( data, length ) );

  length = socket.read_some ( boost::asio::buffer ( data ), err );

  if ( err == boost::asio::error::eof )
    {

      // TODO

    }
  else if ( err )
    {

      throw boost::system::system_error ( err );

    }

  int id {0};
  std::sscanf ( data, "<OK %d", &id );

  std::cout.write ( data, length );
  std::cout << "Command INIT sent." << std::endl;

  return id;

}

void justine::sampleclient::MyShmClient::pos ( boost::asio::ip::tcp::socket & socket, int id )
{

  boost::system::error_code err;

  size_t length = std::sprintf ( data, "<pos " );
  length += std::sprintf ( data+length, "%d %u %u>", id, 2969934868u, 651365957u );

  socket.send ( boost::asio::buffer ( data, length ) );

  length = socket.read_some ( boost::asio::buffer ( data ), err );

  if ( err == boost::asio::error::eof )
    {

      // TODO

    }
  else if ( err )
    {

      throw boost::system::system_error ( err );

    }

  std::cout.write ( data, length );
  std::cout << "Command POS sent." << std::endl;
}

void justine::sampleclient::MyShmClient::car ( boost::asio::ip::tcp::socket & socket, int id, unsigned *f, unsigned *t, unsigned* s )
{

  boost::system::error_code err;

  size_t length = std::sprintf ( data, "<car " );
  length += std::sprintf ( data+length, "%d>", id );

  socket.send ( boost::asio::buffer ( data, length ) );

  length = socket.read_some ( boost::asio::buffer ( data ), err );

  if ( err == boost::asio::error::eof )
    {

      // TODO

    }
  else if ( err )
    {

      throw boost::system::system_error ( err );
    }

  int idd {0};
  std::sscanf ( data, "<OK %d %u %u %u", &idd, f, t, s );

  std::cout.write ( data, length );
  std::cout << "Command CAR sent." << std::endl;

}

void justine::sampleclient::MyShmClient::route (
  boost::asio::ip::tcp::socket & socket,
  int id,
  std::vector<osmium::unsigned_object_id_type> & path
)
{

  boost::system::error_code err;

  size_t length = std::sprintf ( data,
                                 "<route %d %d", path.size(), id );

  for ( auto ui: path )
    length += std::sprintf ( data+length, " %u", ui );

  length += std::sprintf ( data+length, ">" );

  socket.send ( boost::asio::buffer ( data, length ) );

  length = socket.read_some ( boost::asio::buffer ( data ), err );

  if ( err == boost::asio::error::eof )
    {

      // TODO

    }
  else if ( err )
    {

      throw boost::system::system_error ( err );

    }

  std::cout.write ( data, length );
  std::cout << "Command ROUTE sent." << std::endl;

}

void justine::sampleclient::MyShmClient::start ( boost::asio::io_service& io_service, const char * port )
{

#ifdef DEBUG
  foo();
#endif

  boost::asio::ip::tcp::resolver resolver ( io_service );
  boost::asio::ip::tcp::resolver::query query ( boost::asio::ip::tcp::v4(), "localhost", port );
  boost::asio::ip::tcp::resolver::iterator iterator = resolver.resolve ( query );

  boost::asio::ip::tcp::socket socket ( io_service );
  boost::asio::connect ( socket, iterator );

  int id = init ( socket );

  pos ( socket, id );

  bool pursuit {false}; // üldözés flag 
  unsigned int g {0u};
  unsigned int f {0u};
  unsigned int t {0u};
  unsigned int s {0u};

  std::vector<Gangster> gngstrs;

  for ( ;; )
    {
      std::this_thread::sleep_for ( std::chrono::milliseconds ( 200 ) );

      car ( socket, id, &f, &t, &s );
      if ( true )//!pursuit )
      {
      	gngstrs = gangsters ( socket, id, t );

      	if ( gngstrs.size() > 0 )
     		g = gngstrs[0].to;
    	else
    		g = 0;
     	if ( g > 0 )
     	{
		  if(dst(f, gngstrs[0].to) == 0) // ha egy gangstert el kaptunk akkor új íterációs ciklusba kezdünk
			continue;
     	  std::vector<osmium::unsigned_object_id_type> path = hasDijkstraPathMy ( t, gngstrs ); // a cop to-ja és a gnagsterek to-ja alapján a legközlebbi gangsterre útvonalat tervez.
     	  if ( path.size() > 1 && pursuit==false )
     	  {
//     		std::copy ( path.begin(), path.end(),
//     		std::ostream_iterator<osmium::unsigned_object_id_type> ( std::cout, " -> " ) );
     		route ( socket, id, path );
     		pursuit = true; // és üldözésbe fog
     	  }
        }
      }
      if(pursuit) // ha üldöz
      {
      	std::vector<osmium::unsigned_object_id_type> pathPur = hasDijkstraPath ( t, gngstrs[0].to ); // útvonal a g_pursuit-hoz
      	if ( distanceMy(pathPur) < 150 || distanceMy(pathPur) > 1000 ) 
      	{
      		route ( socket, id, pathPur ); // akkor ez a pathPur megfelelő útvonal lesz
      	} 
      	else // viszont ha közepesen távol van akkor próbáljon új utat keresni. Az éppen aktuális pozijához a gangsternek
      	{
      		std::vector<osmium::unsigned_object_id_type> pathPur2 = hasDijkstraPath ( t, gngstrs[0].from );
      		if ( pathPur2.size() > 0 && distanceMy(pathPur2) < distanceMy(pathPur) ) // ha ez az út rövidebb mint az általános path
      		{
      			route ( socket, id, pathPur2 ); // akkor erre menjen tovább
      		}
      	}
      }
      if ( t == g )
      	pursuit = false;
    }
}

void justine::sampleclient::MyShmClient::start10 ( boost::asio::io_service& io_service, const char * port )
{

#ifdef DEBUG
  foo();
#endif

  boost::asio::ip::tcp::resolver resolver ( io_service );
  boost::asio::ip::tcp::resolver::query query ( boost::asio::ip::tcp::v4(), "localhost", port );
  boost::asio::ip::tcp::resolver::iterator iterator = resolver.resolve ( query );

  boost::asio::ip::tcp::socket socket ( io_service );
  boost::asio::connect ( socket, iterator );

  std::vector<Cop> cops = initcops ( socket );

  bool pursuit {false}; // üldözés flag 
  unsigned int g {0u};
  unsigned int f {0u};
  unsigned int t {0u};
  unsigned int s {0u};

  std::vector<Gangster> gngstrs;

  for ( ;; )
    {
      std::this_thread::sleep_for ( std::chrono::milliseconds ( 200 ) );

      for ( auto cop:cops )
        {
          car ( socket, cop, &f, &t, &s );

          if ( true )//!true )
          {
          	gngstrs = gangsters ( socket, cop, t );
          	if ( gngstrs.size() > 0 )
            		g = gngstrs[0].to;
          	else
            		g = 0;
          	if ( g > 0 )
            	{
            		if(dst(f, gngstrs[0].to) == 0) // ha egy gangstert el kaptunk akkor új íterációs ciklusba kezdünk
            			continue;
              		std::vector<osmium::unsigned_object_id_type> path = hasDijkstraPathMy ( t, gngstrs ); // a cop to-ja és a gnagsterek to-ja alapján a legközlebbi gangsterre útvonalat tervez.
              		if ( path.size() > 1 && pursuit==false )
                	{
//                  		std::copy ( path.begin(), path.end(),
//                             	std::ostream_iterator<osmium::unsigned_object_id_type> ( std::cout, " -> " ) );
                  		route ( socket, cop, path );
                  		pursuit = true; // és üldözésbe fog
                	}
            	}
          }
          if(pursuit) // ha üldöz
          {
          	std::vector<osmium::unsigned_object_id_type> pathPur = hasDijkstraPath ( t, gngstrs[0].to ); // útvonal a g_pursuit-hoz				
          	// ha az út túl rövid (<150) vagy tú hosszú (>1000) 
          	// ha közel van akkor már ne váltson más gangsterre próbáljn elé vágni, vagy ha még nagyonon meszse van akkor próbáljon elé kerülni egy rövidebb úton
          	if ( distanceMy(pathPur) < 150 || distanceMy(pathPur) > 1000 ) 
          	{
          		route ( socket, cop, pathPur ); // akkor ez a pathPur megflelő útvonal lesz
          	} 
          	else // viszont ha közepesen távol van akkor próbáljon új utat keresni. Az éppen aktális pozijához a gangsternek
          	{
          		std::vector<osmium::unsigned_object_id_type> pathPur2 = hasDijkstraPath ( t, gngstrs[0].from );
          		if ( pathPur2.size() > 0 && distanceMy(pathPur2) < distanceMy(pathPur) ) // ha ez az út rövidebb mint az általános path
          		{
          			route ( socket, cop, pathPur2 ); // akkor erre menjen tovább
          		}
          	}
          }
          if ( t == g )
          	pursuit = false;
        }
    }
}
