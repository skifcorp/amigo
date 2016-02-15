#include "server.h"
#include "group.h"
#include "item.h"
#include "jivariant.h"
#include "jisystem.h"
#include "jexception.h"
#include "connectioninformation.h"
#include "jinitializer.h"


#include "opcmodiface.h"
#include "opcclconfparser.h"


#include <thread>
#include <list>

using std::thread;


#include <boost/bind.hpp>

using boost::bind;

void OpcModIface::init ()
{
  OpcclConfParser parser;
  
  opc_hosts = parser.parse(tags);
  
}


/*void OpcModIface::prestart()
{
  
}*/


void OpcModIface::run(const OpcHost& host)
{
  JInitializer init;

  ConnectionInformation ci;
  ci.setHost (host.host );
  ci.setDomain (host.domain );
  ci.setUser (host.user );
  ci.setPassword ( host.password );
  ci.setClsid ( host.clsid );

  bool connected = false;
  Server server(ci);

  vector<Item> items;
  
  while (!want_stop) {
    try {
      if ( !connected ) {
        server.connect();                //throw if not connected
        connected = true;

        for (OpcHost::const_iterator gr_iter = host.begin(); gr_iter != host.end(); ++gr_iter) {
          const OpcGroup ogr = *gr_iter;
          Group gr = server.addGroup (ogr.name);
          for (const OpcTag & tg : ogr) {
            items.push_back( gr.addItem( tg.name() ) );
          }
        }
      }


      for (Item item : items) {
        ItemState st = item.read(false);
        JIVariant v  = st.getValue();
        cout << "value: "<<v.getObjectAsShort() << " quality: "<<st.getQuality()<<"\n";
      }

    }
    catch (java::lang::Throwable * ex) {
      connected = false;
      server.disconnect();
      items.clear();
      cerr<<"exception on opccl... host: "<<ci.getHost()<<" user: "<<ci.getUser()<<" clsid: "<<ci.getClsid()<<"\n";
      ex->printStackTrace();
    }
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
  
  want_stop.store(false);
}

void OpcModIface::start()
{
  JInitializer::init();
  JInitializer init;
  
  JISystem::setInBuiltLogHandler(false);
  
  vector<thread> threads;
  
  for (OpcHost & host:opc_hosts) {
    cout << "before thread\n";
    threads.push_back(thread(bind(&OpcModIface::run, this, std::cref(host))));
//    threads.push_back(thread(&OpcModIface::run, this, static_cast<OpcHost&&>(host)));
  }
  
  for (thread & t : threads) t.join();
  want_stop.store(false);
}


void OpcModIface::stop()
{
  want_stop.store(true);
}

