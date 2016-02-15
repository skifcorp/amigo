#ifndef __NET_MODULE_H_
#define __NET_MODULE_H_

#include "amodule.h"

#include <map>
#include <string>

using std::map;
using std::string;

typedef AModule NetModule;

typedef map<string, NetModule::pointer> NetModules_t;

#endif
