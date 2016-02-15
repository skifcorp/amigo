#ifndef __OUTPUT_CONNECTION_H_
#define __OUTPUT_CONNECTION_H_

#include <vector>
#include <cstddef>
#include <memory>
#include <iostream>

using std::vector;
using std::cout;

class OutputConnection 
{
public:  
  typedef std::function<void (bool)> readHandler;
  typedef std::function<void (bool)> writeHandler;
  typedef std::function<void (bool)> prepareHandler;
  
  typedef std::shared_ptr<OutputConnection> pointer;
  
  virtual ~OutputConnection()
  {
  }
  
  virtual void read (vector<char> &, std::size_t ) = 0;
  virtual void write(const vector<char> & ) = 0;
  virtual void readAsync (vector<char> &, std::size_t , readHandler) = 0;
  virtual void writeAsync(const vector<char> &, std::size_t, writeHandler ) = 0;
  virtual bool ready () const  = 0;
  
  template <class Conn, class... Par >
  static pointer create(Par ... par )
  {
    return pointer(new Conn(par ...));
  }
  
  virtual void run() = 0;
  
  virtual void prepare() = 0;
  virtual void asyncPrepare(prepareHandler) = 0;
};


#endif
