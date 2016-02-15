#ifndef __JEXCEPTION_H_
#define __JEXCEPTION_H_

#include <string>
#include <iostream>

using std::string;

#include <java/lang/Throwable.h>

//#include "tools.h"



class JException 
{
public:
  JException ( java::lang::Throwable * t );
  string getMessage();
  string getLocalizedMessage();  
  string toString();  
  void printStackTrace();
private:
  java::lang::Throwable * throwable;
};

//#pragma GCC push_options
//#pragma GCC java_exceptions
//#pragma GCC pop_options

template <class T>
void jex_cast (T t)
{
    try {
//      std::cout << "1: before {\n";
      t();
//      std::cout << "1: after  }\n";
    }
    catch (java::lang::Throwable * t) {
//      std::cout << "1: WOW! Exception...\n";
//      t->printStackTrace();
      //throw JException(t);      
      throw t;      
    }
}

template <class R, class T>
R jex_cast (T t)
{
    R ret;
    try {
      ret = t();
    }
    catch (java::lang::Throwable * t) {
//      std::cout << "2: WOW! Exception...\n";
//      t->printStackTrace();
      //throw JException(t);
      throw t;
    }
  return ret;
}


#endif
