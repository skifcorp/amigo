#ifndef __ETHERNET_H_
#define __ETHERNET_H_

#include "inputconnection.h"

//#include "monotone_timer.hpp"

#include <boost/asio.hpp>
#include <boost/bind.hpp>
//#include <functional>

#include <iostream>

using std::cerr;
using std::cout;

//using boost::asio::monotone_timer;
using boost::asio::ip::tcp;
using boost::asio::io_service;
using boost::bind;
//namespace {
//  using std::placeholders::_1;
//  using std::placeholders::_2;
//}

class InputEthernetConnection : public InputConnection
{
public:
  InputEthernetConnection ( io_service & io ) : socket_(io)
  {
  }
  
  ~InputEthernetConnection ()
  {
    //cout << " deleting InputEthernetConnection ()\n";
    // cout << "deleting connection: addr: "<<socket_.remote_endpoint().address() << " port: "<<socket_.remote_endpoint().port()<<"\n";
  }
  
  virtual void readAsync (vector<char> & d, std::size_t s, readHandler h)
  {
//    using std::placeholders::_1;
//    using std::placeholders::_2;

    socket_.async_read_some ( boost::asio::buffer(d, s), bind (&InputEthernetConnection::asyncReadHandler, this, _1, _2, h) );
  }
  
  virtual void writeAsync(const vector<char> & d, writeHandler h)
  {
//    using std::placeholders::_1;
//    using std::placeholders::_2;

    socket_.async_write_some (boost::asio::buffer(d), bind(&InputEthernetConnection::asyncWriteHandler, this, _1, _2, h ));
  }
  tcp::socket & socket () {return socket_;}
private:
  tcp::socket socket_;
  
  void asyncReadHandler (const boost::system::error_code & e, std::size_t len, readHandler handler)
  {
    if ( e.value () != 0) {
      cerr<<"asyncReadHandler error: "<<e<<" msg: "<<e.message() <<"\n";
    }
    
    
//    cerr<<"asyncReadHandler: "<<e.value()<<"\n";
    
    handler ( e.value() == 0 );
  }

  void asyncWriteHandler (const boost::system::error_code & e, std::size_t len, writeHandler handler)
  {
    if ( e.value () != 0) {
      cerr<<"asyncWriteHandler error: "<<e<<" msg: "<<e.message()<<"\n";
    }

    handler ( e.value() == 0 );
  }
};



#endif
