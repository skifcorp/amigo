#ifndef __BYRT_H_
#define __BYRT_H_

#include "entries.h"

#include <vector>
//#include <memory>

using std::vector;
//using std::shared_ptr;

#include "monotone_timer.hpp"
#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

using boost::asio::monotone_timer;
using boost::asio::io_service;
using boost::posix_time::milliseconds;

class ByrtItem
{
public:
  ByrtItem() : ent_id(){}
  ~ByrtItem(){}
  void setEntryId(tag_id_t id){ent_id = id;}
  tag_id_t entryId() const {return ent_id;}
private:
  tag_id_t ent_id;
};

class ByrtGroup : public vector<ByrtItem>
{
public:
  ByrtGroup() : offset_(0), period_(1000), read_always(false), timer_(nullptr)
  {
  }
  ~ByrtGroup()
  {
    stopTimer();
  }
  
  void setOffset( unsigned short o)  {offset_ = o;}
  unsigned short offset() const {return offset_;}
  
  void setPeriod(int p) {period_ = p;}
  int period() const {return period_;}
  
  void setReadAlways(bool r) {read_always = r;}
  bool readAlways() const {return read_always;}
  
  bool isCyclicReading() const
  {
    return period_ > 0 && read_always;
  }
  //monotone_timer timer;
  //bool readingActive;
  void initTimer(io_service& ios)
  {
    timer_ = new monotone_timer(ios);
  }
  
  template <class T>
  void asyncWaitTimer(T func, io_service& ios)
  {
    if (timer_ == nullptr) {
      initTimer(ios);
    }
    
    //cout << "period: "<<period_<<"\n";
    
    timer_->expires_from_now(milliseconds(period_));
    timer_->async_wait(func);
  }
  void stopTimer()
  {
    if (timer_) {
      timer_->cancel(); delete timer_; timer_ = nullptr;
    }
  }
private:
  unsigned short offset_;
  int period_;
  bool read_always;
  
  monotone_timer * timer_;
};

class ByrtDevice : public vector<ByrtGroup>
{
public:
  ByrtDevice()
  {
  }
  ~ByrtDevice()
  {
  }
  
  void setId (device_id_t i)
  {
    id_ = i;
  }
  device_id_t id() const
  {
    return id_;
  }
private:
  device_id_t id_;
};

typedef vector<ByrtDevice> ByrtDevices_t;


#endif
