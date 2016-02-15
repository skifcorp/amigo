#ifndef __VIS_H_
#define __VIS_H_

#include "tag.h"

struct VisIface
{
  virtual void showProjWindow() = 0;
  virtual void hideProjWindow() = 0;
  virtual void executeModule() = 0;
  virtual void start() = 0;
  virtual void stop() = 0;
  virtual ~VisIface(){}
};

#endif
