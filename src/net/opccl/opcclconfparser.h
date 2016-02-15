#ifndef __OPCCLPARSER_H_
#define __OPCCLPARSER_H_




#include "opcclconf.h"

class OpcclConfParser
{
public:
  OpcclConfParser()
  {
  }
  
  ~OpcclConfParser()
  {
  }
  
  OpcHosts_t parse(tags_t & tags);
  
};


#endif
