#ifndef __TOOLS_H_
#define __TOOLS_H_

#include <memory>
#include <list>
#include <thread>

using std::thread;
using std::shared_ptr;
using std::list;

typedef shared_ptr<thread> ThreadPtr;
typedef list<ThreadPtr> ThreadList;

inline void join_threads (ThreadList& l)
{
  for (auto iter = l.begin(); iter != l.end(); ++iter) {
    (*iter)->join();
  }
}


#endif
