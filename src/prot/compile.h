#ifndef __COMPILE_CPP
#define __COMPILE_CPP

#include "configreader.h"
#include "mysqldriver.h"
#include "basesaver.h"
#include "xmlconfigreader.h"

namespace prot
{
  namespace stub_config
  {
    //typedef BaseConfigReader<ReaderStub> ConfigReader;
    typedef BaseConfigReader<XmlConfigReader> ConfigReader;
    typedef BaseSaver<MySqlDriver>       Saver;
  }
}

#endif
