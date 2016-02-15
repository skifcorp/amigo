#ifndef __SERVER_H_
#define __SERVER_H_

//#include <java/util/concurrent/Executors.h>
//#include <java/util/concurrent/ScheduledExecutorService.h>
#include <java/lang/Float.h>
#include <java/lang/Throwable.h>

#include "org/openscada/opc/lib/da/Server.h"

#include "tools.h"
#include "connectioninformation.h"
#include "group.h"
#include "jexception.h"

//#pragma GCC reset_options

class Server 
{
public:
  //Server
  Server(ConnectionInformation c)
  {
    server = new org::openscada::opc::lib::da::Server(c.javaObject()/*, ::java::util::concurrent::Executors::newSingleThreadScheduledExecutor () */);
  }
  
  //virtual ::java::util::concurrent::ScheduledExecutorService * getScheduler();
  void connect()
  {
/*    try {
      server->connect();
    }
    catch (java::lang::Throwable * t) {
      throw JException (t);
    } */
    //JEX_CAST(server->connect())
    jex_cast( [this]{server->connect();} );
  }
  void disconnect()
  {
    //server->disconnect();
    jex_cast( [this]{server->disconnect();} );
  }
  void dispose()
  {
    jex_cast( [this]{server->dispose();} );
  }
  
  Group addGroup(const string& s)
  {
    //return Group ( server->addGroup( string_to_jstring(s) ) );
    return jex_cast<Group>( [this, &s](){ return Group ( server->addGroup( string_to_jstring(s) ) );}  );
  }
  Group addGroup()
  {
    //return Group (server->addGroup());
    return jex_cast<Group>( [this]{return Group(server->addGroup());} );
  }
  Group findGroup(const string& s)
  {
    return jex_cast<Group>( [this, &s]{ return Group(server->findGroup(string_to_jstring(s)));} );
  }
  
  int getDefaultLocaleID()
  {
    return jex_cast<int>( [this]{return server->getDefaultLocaleID();} );
  }
  
  void setDefaultLocaleID(int id)
  {
     jex_cast( [this, &id]{ server->setDefaultLocaleID(id);} );
  }
  float getDefaultPercentDeadband()
  {
    return jex_cast<float>( [this]{ return server->getDefaultPercentDeadband()->floatValue(); } );
  }
  void setDefaultPercentDeadband(float d)
  {
     jex_cast( [this, &d]{  server->setDefaultPercentDeadband(new java::lang::Float( static_cast<jfloat> (d))); } );
  }
  int getDefaultTimeBias()
  {
    return jex_cast<int>( [this]{   return server->getDefaultTimeBias()->intValue(); } );
  }
  
  void setDefaultTimeBias(int b)
  {
     jex_cast( [this, &b]{  server->setDefaultTimeBias(new java::lang::Integer(b)); } );
  }
  
  int getDefaultUpdateRate()
  {
    return jex_cast<int>( [this]{   return server->getDefaultUpdateRate(); } );
  }
  
  void setDefaultUpdateRate(int ur)
  {
     jex_cast( [this, &ur]{ server->setDefaultUpdateRate(ur); } );
  }
  
  bool isDefaultActive()
  {
    return jex_cast<bool>( [this]{  return server->isDefaultActive();  } ); 
  }
  void setDefaultActive(bool a)
  {
     jex_cast( [this, &a]{  server->setDefaultActive(a); } );
  }
//  ::org::openscada::opc::lib::da::browser::FlatBrowser * getFlatBrowser();
//  ::org::openscada::opc::lib::da::browser::TreeBrowser * getTreeBrowser();
  string getErrorMessage(int i)
  {
     return jex_cast<string>( [this, &i]{  return jstring_to_string(server->getErrorMessage(i));  } );
  }
   
//  void addStateListener(::org::openscada::opc::lib::da::ServerConnectionStateListener *);
//  void removeStateListener(::org::openscada::opc::lib::da::ServerConnectionStateListener *);
/*  OPCSERVERSTATUS getServerState(int i)
  {
    return OPCSERVERSTATUS(server->getServerState(i));
  }
  OPCSERVERSTATUS getServerState()
  {
    return OPCSERVERSTATUS(server->getServerState());
  }*/
  void removeGroup(Group g, bool b)
  {
     jex_cast( [this, &g, &b]{ server->removeGroup(g.javaObject(), b); });
  }

  
  org::openscada::opc::lib::da::Server * javaObject() {return server;}
private:
  org::openscada::opc::lib::da::Server * server;
  
};

#endif

