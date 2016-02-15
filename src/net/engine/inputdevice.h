#ifndef __INPUT_DEVICE_H_
#define __INPUT_DEVICE_H_

#include "inputdriver.h"
#include "tag.h"
#include "tools.h"
#include "inputethernetconnection.h"
#include "netmodule.h"
#include "engine.h"

#include <iostream>
//#include <memory>
#include <map>
#include <list>
#include <vector>
#include <cassert>
#include <thread>

#include <boost/bind.hpp>
#include <boost/asio.hpp>

using std::cout;
using std::list;
using std::vector;
using std::map;
using std::thread;

using boost::bind;
using boost::asio::io_service;
using boost::asio::ip::tcp;
/*
class IoServicePool
{
public:
  IoServicePool(const IoServicePool& ) = delete;
  
  IoServicePool (std::size_t pool_size):next_io_service (0)
  {
    assert (pool_size > 0);
    
    services.reserve (pool_size);
    works.reserve(pool_size);
    
    for (std::size_t i = 0; i<pool_size; ++i) {
      services.push_back (IoServicePtr(new io_service));
      works.push_back(WorkPtr(new io_service::work(*services.back())));
    }
  }
  void run()
  {
    typedef shared_ptr<thread> ThreadPtr;
    typedef list<ThreadPtr> ThreadList;
    
    ThreadList l;
    
    for (std::size_t i = 0; i<services.size(); ++i ) {
      l.push_back( ThreadPtr(new thread ( bind(&io_service::run, services[i].get() ) ) ) );
    }
    
    join_threads(l);
  }
  
  void stop()
  {
    for (std::size_t i = 0; i < services.size(); ++i ) {
      services[i]->stop();
    }
  }
  
  io_service& getIoService()
  {
    io_service & service = *services[next_io_service];
    ++next_io_service;
    
    if (next_io_service == services.size() )
      next_io_service = 0;
      
    return service;
  }
private:
  typedef shared_ptr<io_service> IoServicePtr;
  typedef shared_ptr<io_service::work> WorkPtr;
  std::size_t next_io_service;
  
  vector<IoServicePtr> services;
  vector<WorkPtr> works;
};*/


class TcpInputDevice
{
public:
  TcpInputDevice ( unsigned short port_num, tags_t & t, IoServicePool& s) : 
      acceptor_(s.getIoService(), tcp::endpoint(tcp::v4(), port_num)), tags(t), services(s)
  {
    // startAccept();
  }
  
  TcpInputDevice (const TcpInputDevice& ) = delete;
  
/*  void run ()
  {
    services.run();
  }*/
  
  void start (NetModule::pointer m)
  {
    module = m;
    startAccept();
  }
  typedef shared_ptr<TcpInputDevice> pointer;
private:
  void startAccept ()
  {
    InputConnection::pointer conn = InputConnection::create<InputEthernetConnection>( std::ref( services.getIoService() ) );

    acceptor_.async_accept ( static_cast<InputEthernetConnection*>(conn.get())->socket(), bind(&TcpInputDevice::handleAccept, this, conn, boost::asio::placeholders::error) );
  }

  void handleAccept (InputConnection::pointer conn, const boost::system::error_code& err)
  {
    tcp::endpoint re = static_cast<InputEthernetConnection*>(conn.get())->socket().remote_endpoint();
    
    cout << "new connection->remote_address: " << re.address() << " port: "<<re.port()<<"\n";
    
    InputDriver::pointer p = module->execute<InputDriver::pointer, InputDrivers&, tags_t&>("createDriver", drivers, tags);
    p->start(conn);
    //module->execute<InputDriver::pointer, const string&, InputDrivers&, tags_t&>("createDriver", drivers, tags);
    startAccept();
  }
private:

  tcp::acceptor acceptor_;
  tags_t & tags;
  InputDrivers drivers;
  NetModule::pointer module;
  IoServicePool & services;  
};

typedef map<string, TcpInputDevice::pointer> TcpInputDevices_t;

#endif
