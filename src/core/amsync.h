#ifndef __AM_SYNC_H_
#define __AM_SYNC_H_

#include <condition_variable>
#include <mutex>

using std::condition_variable;
using std::mutex;
using std::unique_lock;
using std::lock_guard;

class am_sync
{
public:
  am_sync() : ready(false)
  {
  }
  
  am_sync(am_sync const& ) = delete;
  
  void wait()
  {
    unique_lock<mutex> lock(mutx);
    if ( !isReady() ) {
      cond.wait(lock);
    }
  }
  
  void set()
  {
    lock_guard<mutex> grd(mutx);
    ready = true;
    cond.notify_all();
  }
private:
  bool isReady () const {return ready;}
  bool ready;
  mutex mutx;
  condition_variable cond;
};


#endif
