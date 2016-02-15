#ifndef __JISYSTEM_H_
#define __JISYSTEM_H_


#include "org/jinterop/dcom/common/JISystem.h"
#include "jexception.h"

class JISystem
{
public:
  static void setInBuiltLogHandler(bool b)
  {
    jex_cast([&b] {org::jinterop::dcom::common::JISystem::setInBuiltLogHandler(b); });
  }
};

#endif

