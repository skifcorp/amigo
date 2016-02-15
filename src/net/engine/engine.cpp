

#include "engine.h"
#include "inputethernetconnection.h"
#include "inputdriver.h"
#include "tag.h"
#include "init.h"
#include "netmodule.h"
#include "inputdevice.h"
#include "tools.h"
#include "outputethernetconnection.h"
#include "outputdriverbase.h"
#include "modiface.h"

#include <iostream>
#include <thread>
#include <memory>
#include <functional>
#include <list>
#include <vector>
#include <cassert>

#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/system/error_code.hpp>
#include <boost/property_tree/ptree.hpp>

using std::cout;
using std::list;
using std::vector;
using std::thread;
using std::function;

using boost::bind;
using boost::asio::io_service;
using boost::asio::ip::tcp;
using boost::system::get_system_category;
using boost::posix_time::millisec;
using boost::property_tree::ptree;

#if 0
void startInputDevices( TcpInputDevices_t & devs, NetModules_t & modules)
{
  for (auto d : devs) {
    d.second->start(modules[d.first]);
    //thr_list.push_back( ThreadPtr(new thread( bind(&TcpInputDevice::run, d.second.get()) ) ) );
  }
}
#endif

#if 0
void startOutputDrivers(OutputDrivers_t & drvs, NetModules_t & modules, entries_t & entries, const tags_t& tags , IoServicePool& ios, vector<OutputDriverBase::pointer> & out_drv_base)
{

  for (OutputDriver & drv : drvs) {    
    auto iter = modules.find(drv.protocol());
    if (iter == modules.end() ) {
      cerr<<"module: "<<drv.protocol()<<" not found\n"; continue;
    }

    NetModule::pointer m = (*iter).second;
    function<OutputDevice(device_id_t)> get_device_func = [&drv](device_id_t id)
     {
       for ( auto dev : drv.devices() ) {
         if ( dev.id() == id ) { 
           return dev;
         }
       }

       return OutputDevice(-1);
     };
     
     OutputConnection::pointer conn;
     if (drv.transport() == "tcp") {
       conn = OutputConnection::create<OutputEthernetConnection<io_service&>>(drv, &ios.getIoService(), wrap<io_service&>());
     }
     else if (drv.transport() == "udp") {
     }
     else if (drv.transport() == "tty") {
     }

     OutputDriverBase::pointer ptr = m->execute<OutputDriverBase::pointer, entries_t&, const tags_t&, function<OutputDevice(device_id_t)>,OutputConnection::pointer, io_service&>
                 ("createDriver", std::ref(entries), std::ref(tags), get_device_func, conn, ios.getIoService() );
     
     if (ptr) {
       out_drv_base.push_back(ptr);
     }

  }
}

#endif

/*void stopModule (IoServicePool & services)
{
  services.stop();
}*/



class EngIface : public ModIface
{
public:
  EngIface(tags_t & t) :tags(t), services(1)
  {
  }
  ~EngIface()
  {
  }
  virtual void init()
  {
    {
      EntriesParser parser(entries);
      parser.parse ( tags, services.getIoService() );
    }

    {
      InputModulesParser inpp(tags);
      inpp.parse (input_modules);
    }

    {
      TcpInputDevicesParser tcp_inp_parser(tags);
      tcp_inp_parser.parse(tcp_input_devices, services);
    }

    {
      OutputDriversParser od_parser(tags);
      od_parser.parse(out_drivers);
    }

    
    {
      OutputModulesParser outp(tags);
      outp.parse(out_modules);
    }
  }
  
  virtual void start ()
  {
    startInputDevices();

    vector<OutputDriverBase::pointer> out_drv_base;
    startOutputDrivers(out_drv_base);
    
    services.run();
  }
  
  virtual void stop ()
  {
    services.stop();
  }

private:
  tags_t & tags;
  IoServicePool services;
  entries_t entries;
  NetModules_t input_modules;
  TcpInputDevices_t tcp_input_devices;
  OutputDrivers_t out_drivers;
  NetModules_t out_modules;
  
  
  void startInputDevices()
  {
    for (auto d : tcp_input_devices) {
      d.second->start( input_modules[d.first] );
    }
  }


  void startOutputDrivers( vector<OutputDriverBase::pointer> & out_drv_base )
  {

    for (OutputDriver & drv : out_drivers) {    
      auto iter = out_modules.find(drv.protocol());
      if (iter == out_modules.end() ) {
        cerr<<"module: "<<drv.protocol()<<" not found\n"; continue;
      } 

      NetModule::pointer m = (*iter).second;
      function<OutputDevice(device_id_t)> get_device_func = [&drv](device_id_t id)
        {
          for ( auto dev : drv.devices() ) {
            if ( dev.id() == id ) { 
              return dev;
            }
          }

          return OutputDevice(-1);
        };
     
        OutputConnection::pointer conn;
        if (drv.transport() == "tcp") {
          conn = OutputConnection::create<OutputEthernetConnection<io_service&>>(drv, &services.getIoService(), wrap<io_service&>());
        }
        else if (drv.transport() == "udp") {
        }
        else if (drv.transport() == "tty") {
        }

        OutputDriverBase::pointer ptr = m->execute<OutputDriverBase::pointer, entries_t&, const tags_t&, function<OutputDevice(device_id_t)>,OutputConnection::pointer, io_service&>
                 ("createDriver", std::ref(entries), std::ref(tags), get_device_func, conn, services.getIoService() );
     
        if (ptr) {
          out_drv_base.push_back(ptr);
        }

    }
  }
};



extern "C" shared_ptr<ModIface> executeModule ( tags_t & tags )
{
  return shared_ptr<ModIface>(new EngIface(tags));
#if 0
  IoServicePool services(1);
  *stop_module_func = bind (stopModule, std::ref(services));
  
  entries_t * entries = new entries_t();
  {
    EntriesParser parser(*entries);
    parser.parse ( tags, services.getIoService() );
  }
  
  NetModules_t input_modules;
  {
    InputModulesParser inpp(tags);
    inpp.parse (input_modules);
  }
  
  
  
  TcpInputDevices_t tcp_input_devices;
  {
    TcpInputDevicesParser tcp_inp_parser(tags);
    tcp_inp_parser.parse(tcp_input_devices, services);
  }

//  ThreadList thr_list;

  startInputDevices(tcp_input_devices, input_modules);

  OutputDrivers_t out_drivers;
  {
    OutputDriversParser od_parser(tags);
    od_parser.parse(out_drivers);
  }

  NetModules_t out_modules;
  {
    OutputModulesParser outp(tags);
    outp.parse(out_modules);
  }
  
  vector<OutputDriverBase::pointer> out_drv_base;
  startOutputDrivers(out_drivers, out_modules, *entries, tags, services, out_drv_base);
  
//  join_threads(thr_list);


  services.run();
  //services.clear();
  
  delete entries;
#endif

}





/*
class DriverEnvironment
{
public:
  DriverEnvironment(io_service& io, driver & d) : timer(io), service_(io), drv (d)
  {
    start();
  }
  
  ~DriverEnvironment() 
  {
  }
  
  void start();
  void read (OneGroup& );
  void handleReading(const boost::system::error_code& err, OneGroup & gr);
//  void write (OneGroup &);
  void handleWriting(OneGroup & gr);
private:
  monotone_timer timer;
  io_service & service_;
  driver & drv;
};

void DriverEnvironment::start ()
{
  for (OneEntry * dev_ent = drv.firstChild(); dev_ent; dev_ent = dev_ent->nextSibling() ) {
    OneDevice * dev = static_cast<OneDevice *>(dev_ent);
    for (OneEntry * gr_ent = dev->firstChild(); gr_ent; gr_ent = gr_ent->nextSibling() ) {
      OneGroup * gr = static_cast<OneGroup *>(gr_ent);
      read (*gr);
    }
  }
}

void DriverEnvironment::read (OneGroup& gr)
{
  boost::system::error_code err(boost::system::errc::success, get_system_category());
  
  service_.post(bind(&DriverEnvironment::handleReading, this, err, std::ref(gr)));
}

void DriverEnvironment::handleReading( const boost::system::error_code & err,  OneGroup& gr )
{
  if ( err == boost::asio::error::operation_aborted  ) {
    return;
  }
  
  if (gr.clients() > 0 ) {
    drv.dr_work (&gr, MUST_READ);
    gr.initialized = true;
  }

  if ( gr.period > 1 ) {
    timer.expires_from_now (millisec(gr.period*100));
    timer.async_wait( bind( &DriverEnvironment::handleReading, this, _1, std::ref(gr) ) );
  }
}

void DriverEnvironment::handleWriting (OneGroup& gr)
{
  drv.dr_work (&gr, MUST_WRITE);
}

*/




/*
void creator (workspace& ws)
{
  typedef boost::shared_ptr<DriverEnvironment> DriverEnvironmentPtr;
  typedef list<DriverEnvironmentPtr> DriverEnvironmentPtrList;
    

  ThreadList thr_list;
  DriverEnvironmentPtrList drivers_list;
  io_service drivers_service;
  
  for ( auto drv : ws.drivers() ) {
    drivers_list.push_back ( DriverEnvironmentPtr(new DriverEnvironment(drivers_service, *drv.get())) );

    drv->writeGroup = [&drivers_service, &drivers_list](group& gr) {
        drivers_service.post(bind(&DriverEnvironment::handleWriting, drivers_list.back().get(), std::ref(gr)));
      };

    thr_list.push_back( ThreadPtr( new thread(bind(&io_service::run, &drivers_service) ) ) );
  }
  
  TcpConnectionServer server (4242, 5, ws);
  
  thr_list.push_back( ThreadPtr(new thread( bind(&TcpConnectionServer::run, &server) ) ) );
  
  join_threads(thr_list);
}
*/
