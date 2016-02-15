#ifndef __MOD_IFACE_H_
#define __MOD_IFACE_H_


struct ModIface
{
  virtual void init() = 0;
  virtual void start() = 0;
  virtual void stop() = 0;
  virtual ~ModIface() {}
};


#endif
