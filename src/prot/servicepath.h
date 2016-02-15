#ifndef __SERVICE_PATH_H_
#define __SERVICE_PATH_H_

#include "amigofs.h"
#include <string>
#include <iostream>

namespace a {
  namespace fs {
    inline string prot_sql_path()
    {
      return etc_path() + slash + "prot" + slash + "sql" + slash;
    }
  }
}

#endif
