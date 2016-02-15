#include "outputethernetconnection.h"


/*
void OutputEthernetConnection::parseProps(const map<string, string>& od)
{
  auto iter = od.find("ip");
  assert(iter != od.end());
  
  ip_ = (*iter).second;
  
  
  iter = od.find("port");
  assert(iter != od.end());
  
  port_ = lexical_cast<short>((*iter).second);
}


void OutputEthernetConnection::read (vector<char>  & buf, std::size_t s)
{
  buf.resize(s);
  socket_.read_some(boost::asio::buffer(buf));
}

void OutputEthernetConnection::write (const vector<char> & buf)
{
  socket_.write_some(boost::asio::buffer(buf));
}

void OutputEthernetConnection::readAsync (vector<char> & buf, std::size_t s, readHandler h)
{
  buf.resize(s);
  socket_.async_read_some(boost::asio::buffer(buf), bind(&OutputEthernetConnection::readSomeHandler, this, _1, h));
}

void OutputEthernetConnection::writeAsync(const vector<char> & buf, writeHandler h)
{
  socket_.async_write_some(boost::asio::buffer(buf), bind(&OutputEthernetConnection::writeSomeHandler, this, _1, h));
}

void OutputEthernetConnection::prepare()
{
  tcp::endpoint p(address::from_string(ip_), port_);
  socket_.connect(p);
}

void OutputEthernetConnection::asyncPrepare(prepareHandler h)
{
  tcp::endpoint p(address::from_string(ip_), port_);
  socket_.async_connect (p, bind(&OutputEthernetConnection::connectHandler, this, _1, h));
}
*/
