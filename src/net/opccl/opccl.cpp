#include <memory>

using std::shared_ptr;


#include "tag.h"
#include "amigofs.h"
#include "opcmodiface.h"


extern "C" shared_ptr<ModIface> executeModule( tags_t& tags )
{
  return shared_ptr<ModIface>(new OpcModIface(tags));
}
