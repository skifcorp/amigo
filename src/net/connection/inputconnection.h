#ifndef __INPUT_CONNECTION_H_
#define __INPUT_CONNECTION_H_

#include <functional>
#include <vector>
#include <cstddef>
#include <memory>
#include <iostream>

using std::vector;
using std::cout;

class InputConnection : public std::enable_shared_from_this<InputConnection>
{
public:  
  typedef std::function<void (bool)> readHandler;
  typedef std::function<void (bool)> writeHandler;
  typedef std::shared_ptr<InputConnection> pointer;
  
  virtual ~InputConnection()
  {
  }
  
//  virtual void read  (data_t &, std::size_t ) = 0;
//  virtual void write (const data_t & ) = 0;
  
  virtual void readAsync (vector<char> &, std::size_t , readHandler) = 0;
  virtual void writeAsync(const vector<char> &, writeHandler ) = 0;
  
  template <class Conn, class... Par >
  static pointer create(Par ... par )
  {
    return pointer(new Conn(par ...));
  }
};


#endif
