#ifndef __J_INITIALIZER_H_
#define __J_INITIALIZER_H_

#include <gcj/cni.h>

class JInitializer
{
public:
  JInitializer()
  {

    JvAttachCurrentThread(0, 0);
  }
  ~JInitializer()
  {
    JvDetachCurrentThread();
  }
  
  static void init()
  {
     JvCreateJavaVM(0);
  }  
};

#endif
