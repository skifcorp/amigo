#ifndef __OUTPUT_DRIVER_BASE_H_
#define __OUTPUT_DRIVER_BASE_H_

#include <memory>

using std::shared_ptr;

class OutputDriverBase : public std::enable_shared_from_this<OutputDriverBase>
{
public:
  typedef shared_ptr<OutputDriverBase> pointer;
  
  OutputDriverBase()
  {
  }
  
  virtual ~OutputDriverBase()
  {
  }
};

#endif
