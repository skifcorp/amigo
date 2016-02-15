#ifndef __INPUT_H_
#define __INPUT_H_

#include "inputconnection.h"

#include <memory>
#include <set>

using std::set;
using std::shared_ptr;

class InputDriver;

typedef shared_ptr<InputDriver> InputDriverPtr;

class InputDrivers
{
public:
  InputDrivers (){}
  void append (InputDriverPtr p)
  {
    drivers.insert (p);
  }
  void remove (InputDriverPtr p)
  {
    drivers.erase (p);
  }
private:
  set<InputDriverPtr> drivers;
};


class InputDriver : public std::enable_shared_from_this<InputDriver>
{
public:
  InputDriver(InputDrivers& dr) : drivers (dr) {}
  
  typedef InputDriverPtr pointer;
  
  virtual ~InputDriver() {}
  
  void start (InputConnection::pointer p)
  {
    drivers.append (shared_from_this());
    start_(p);
  }
  void stop ()
  {
    //assert (0);

    stop_();
    drivers.remove (shared_from_this());
  }
protected:
  //workspace & ws
private:
  InputDrivers & drivers;
  virtual void start_(InputConnection::pointer p) = 0;
  virtual void stop_() = 0;
};

#endif
