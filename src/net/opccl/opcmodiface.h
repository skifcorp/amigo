#ifndef __OPCMODIFACE_H_
#define __OPCMODIFACE_H_

#include <atomic>

using std::atomic_bool;

#include "modiface.h"
#include "tag.h"
#include "opcclconf.h"


class OpcModIface : public ModIface
{
public:
  OpcModIface(tags_t & t):want_stop(false), tags(t) {}
  ~OpcModIface(){}
  
  virtual void init();
  virtual void start();
  virtual void stop ();
  
private:
  atomic_bool want_stop;
  tags_t & tags;
  OpcHosts_t opc_hosts;
  
  void run(const OpcHost& );
};

#endif
