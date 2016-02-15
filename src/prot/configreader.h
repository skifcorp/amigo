#ifndef __CONFIG_READER_H_
#define __CONFIG_READER_H_


#include "configdata.h"

using std::string;

namespace prot
{


#if 0
class ReaderStub 
{
public:
 
  ReaderStub () 
  {
  }

  ~ReaderStub ()
  {
  }

  VariablesContainer readConfig()
  {
    VariablesContainer cont;

    Variable v1("127.0.0.1", "C.K.G.item1", "KOTEL1");

    cont.push_back (v1);

    return cont;
  }
};

#endif


template <class reader_type>
class BaseConfigReader
{
public:
  
  BaseConfigReader ()
  {
  }

  ~BaseConfigReader()
  {
  }

  VariablesContainer readConfig (const tags_t & t)
  {
    return reader.readConfig(t);
  }
private:
  reader_type reader;
};

}

#endif
