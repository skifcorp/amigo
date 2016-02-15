#ifndef __ENGINE_H_
#define __ENGINE_H_

#include "entries.h"
#include "tools.h"

#include "monotone_timer.hpp"
#include <boost/asio.hpp>

#include <iostream>

using boost::asio::monotone_timer;
using boost::asio::io_service;

class IoServicePool
{
public:
  IoServicePool(const IoServicePool& ) = delete;
  
  IoServicePool (std::size_t pool_size):next_io_service (0)
  {
//    std::cout << "IoServicePoool created\n";
    
    assert (pool_size > 0);
    
    services.reserve (pool_size);
    works.reserve(pool_size);
    
    for (std::size_t i = 0; i<pool_size; ++i) {
      services.push_back (IoServicePtr(new io_service));
      works.push_back(WorkPtr(new io_service::work(*services.back())));
    }
  }
  
  ~IoServicePool()
  {
//std::cout << "IoServicePoool deleted\n";
  }
  
  void run()
  {
    typedef shared_ptr<thread> ThreadPtr;
    typedef list<ThreadPtr> ThreadList;
    
    ThreadList l;
    
    for (std::size_t i = 0; i<services.size(); ++i ) {
      l.push_back( ThreadPtr(new thread ( bind(&io_service::run, services[i].get() ) ) ) );
    }

//cout << "run called... before join\n";

    join_threads(l);
  }
  
  void stop()
  {
    std::size_t ss = services.size();  //<----this is for prevent crush when this method called from another thread and services descructed before cycle exiting
                                       //and size() of services begin return dirty value
    for (std::size_t i = 0; i < ss; ++i ) {
      //services[i]->post(stop();
//      cout << "i: "<<i<<"\n";
      services[i]->post( bind(&io_service::stop, services[i].get()) );
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
  void clear ()
  {
    services.clear();
    works.clear();
  }
private:
  typedef shared_ptr<io_service> IoServicePtr;
  typedef shared_ptr<io_service::work> WorkPtr;
  std::size_t next_io_service;
  
  vector<IoServicePtr> services;
  vector<WorkPtr> works;
  
};


#endif
