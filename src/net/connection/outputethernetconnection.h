#ifndef __OUTPUTETHERNETCONNECTION_H_
#define __OUTPUTETHERNETCONNECTION_H_

#include "outputconnection.h"

#include <string>
#include <functional>
#include <map>
#include <type_traits>
#include <iostream>

using std::string;
using std::function;
using std::map;
using std::is_reference;
using std::remove_reference;
using std::is_same;
using std::enable_if;
using std::cerr;

#include <boost/asio.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/bind.hpp>

using boost::asio::io_service;
using boost::asio::ip::tcp;
using boost::system::error_code;
using boost::lexical_cast;
using boost::bind;
using boost::asio::ip::address;

template <bool b, class ignored>
struct MakeDependent                   //<--- struct for solving SFINAE problem for class temlate using evil trick from Dave Abrahams
{
  enum {value = b};
};

template<class T>
struct wrap
{
  typedef T type;
};

template <class __io_service>
class OutputEthernetConnection : public OutputConnection
{
public:
  typedef function<void (bool)> prepareHandler;
  
  OutputEthernetConnection(const map<string, string>& od) : socket_(service_)
  {
    parseProps(od);
    static_assert( is_same<typename remove_reference<__io_service>::type, io_service>::value, "You can specify only io_service or io_service& type!!!" );
  }

  OutputEthernetConnection() : socket_(service_)
  {

  }
#if 0  
  template <class __io_service_local_>
  OutputEthernetConnection(__io_service_local_ ios, typename enable_if< MakeDependent<is_reference<__io_service>::value, T>::value >::type * dummy = 0):service_(ios), socket_(service_)
  {
  }
#endif
  
  
  template <class __io_service_local_>
  OutputEthernetConnection(__io_service_local_ * ios, wrap<__io_service> w) : service_(*ios), socket_(service_)
  {
    //static_assert (!is_reference<__io_service_local_>::value, "is reference");
  }

  template <class __io_service_local_>
  OutputEthernetConnection(const map<string, string>& m, __io_service_local_ * ios, wrap<__io_service> w) : service_(*ios), socket_(service_)
  {
    //static_assert (!is_reference<__io_service_local_>::value, "is reference");
    parseProps(m);
  }


  void setIp(const string& i) {ip_ = i;}
  string ip() const {return ip_;}
  
  void setPort (short p) {port_ = p;}
  short port() const {return port_;}
  
  ~OutputEthernetConnection()
  {
//    cout << "before close\n";
    close();
//    cout << "before close\n";
  }
  
  virtual void run ()
  {
    service_.run();
  }
  
  virtual void read (vector<char> & buf, std::size_t s)
  {
    buf.resize(s);
    socket_.read_some(boost::asio::buffer(buf));
  }
  virtual void write(const vector<char> & buf) 
  {
    socket_.write_some(boost::asio::buffer(buf));  
  }
  
  virtual void readAsync (vector<char> & buf, std::size_t s, readHandler h)
  {
    buf.resize(s);
    socket_.async_read_some(boost::asio::buffer(buf), bind(&OutputEthernetConnection::readSomeHandler, this, _1, h));
  }
  
  virtual void writeAsync(const vector<char> & buf, std::size_t s, writeHandler h)
  {
    socket_.async_write_some(boost::asio::buffer(buf, s), bind(&OutputEthernetConnection::writeSomeHandler, this, _1, h));
  }
  
  virtual void prepare()
  {
    tcp::endpoint p(address::from_string(ip_), port_);
    socket_.connect(p);
  }
  
  virtual void asyncPrepare(prepareHandler h)
  {
    cout << "preparing: ip-"<<ip_ <<" port: "<<port_<<"\n";
    
    tcp::endpoint p(address::from_string(ip_), port_);
    socket_.async_connect(p, bind(&OutputEthernetConnection::connectHandler, this, _1, h));
  }
  
  void close ()
  {
    if ( socket_.is_open() ) {
      socket_.shutdown(tcp::socket::shutdown_both);
      socket_.close();
    }
  }
  virtual bool ready() const {return socket_.is_open();}
  
  io_service & get_io_service ()
  {
    return service_;
  }
private:
  __io_service service_;
  tcp::socket socket_;
  
  void connectHandler(const error_code& rc, prepareHandler h)
  {
    cout << "connectHandler CALLED!!!!!!!!!!\n";
    
    if (rc) {
      cerr<<"connect error: "<<rc<<" msg: "<<rc.message()<<" ip: "<<ip_<<" port: "<<port_<<"\n";
      socket_.close();
    }
    h(rc == 0);
  }
  void writeSomeHandler(const error_code& rc, writeHandler h)
  {
    if (rc) {
      cerr<<"writeSome error: "<<rc<<" msg: "<<rc.message()<<" ip: "<<ip_<<" port: "<<port_<<"\n";
      socket_.close();      
    }

    h(rc == 0);
  }
  void readSomeHandler(const error_code& rc, readHandler h)
  {
    if (rc) {
      cerr<<"readSome error: "<<rc<<" msg: "<<rc.message()<<" ip: "<<ip_<<" port: "<<port_<<"\n";
      socket_.close();
    }

    h(rc == 0);
  }
  
  void parseProps(const map<string, string>& od)
  {
    auto iter = od.find("ip");
    assert(iter != od.end());
  
    ip_ = (*iter).second;
  
  
    iter = od.find("port");
    assert(iter != od.end());
  
    port_ = lexical_cast<short>((*iter).second);
  }
  string ip_;
  short port_;
};

#endif
