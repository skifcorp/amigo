#ifndef __XML_CONFIG_READER_H_
#define __XML_CONFIG_READER_H_

#include "configdata.h"
#include "tag.h"

#include <string>

namespace prot 
{

using std::string;

class XmlConfigReader
{
public:
  XmlConfigReader () {}
  ~XmlConfigReader () {}

  VariablesContainer readConfig (const tags_t & );
private:
  

};

}

#endif
