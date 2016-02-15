#ifndef __INIT_H_
#define __INIT_H_

#include "entries.h"
#include "tag.h"
#include "netmodule.h"
#include "inputdevice.h"
#include "outputdevice.h"

#include <string>

using std::string;

class IoServicePool;

class EntriesParser
{
public:
  EntriesParser(entries_t& e ) : entries(e){}
  bool parse(tags_t&, io_service & );
private:
  entries_t & entries;
};

class InputModulesParser
{
public:
  InputModulesParser(const tags_t& t):tags(t)
  {
  }
  void parse(NetModules_t& );
private:
  const tags_t & tags;
};

class TcpInputDevicesParser
{
public:
  TcpInputDevicesParser( tags_t& t) : tags(t)
  {
  }
  
  void parse (TcpInputDevices_t&, IoServicePool&  );
  
private:
  tags_t& tags;
};

class OutputModulesParser
{
public:
  OutputModulesParser(const tags_t& t):tags(t)
  {
  }
  void parse(NetModules_t& );
private:
  const tags_t & tags;
};


class OutputDriversParser
{
public:
  OutputDriversParser(const tags_t& t):tags(t)
  {
  }
  void parse(OutputDrivers_t& );
private:
  const tags_t & tags;
};

#endif

