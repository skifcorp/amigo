#include "tagconf.h"
#include "linker.h"


void TagConf::connect(Linker& l)
{
  assert(tag_);
    
  tag_->connectLinkEvent("votlis_rmt", bind(&Linker::linkFunc, &l, std::ref(*this)));
  tag_->connectUnlinkEvent("votlis_rmt", bind(&Linker::unlinkFunc, &l, std::ref(*this)));
}
  
void TagConf::checkExistingConnections(Linker & l)
{
  if (tag_->linksCount() > 0) {
    l.linkFunc(*this);
  }
}
