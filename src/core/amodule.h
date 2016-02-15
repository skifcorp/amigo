#ifndef __A_MODULE_H_
#define __A_MODULE_H_

#include <memory>
#include <iostream>
#include <list>
#include <string>

using std::shared_ptr;
using std::cerr;
using std::cout;
using std::list;
using std::string;

#include <boost/extension/shared_library.hpp>

using boost::extensions::shared_library;

class AModule
{
public:
  AModule(const string& p) : path (p), lib (p, true) {}
  ~AModule() {close();}
  
  void open ()
  {
    if ( !lib.open() ) {
      cerr << "Cant open!: "<<path<<"\n";
#ifdef __linux__
      cerr << dlerror() << "\n";
#endif
#ifdef _WIN32
      DWORD error = GetLastError();
      char BufErr[80];
      FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, error, /*LANG_SYSTEM_DEFAULT*/MAKELANGID(LANG_ENGLISH,SUBLANG_ENGLISH_US), BufErr, 80, NULL);
      cerr<<BufErr<<"\n";
#endif
    }
  }
  
  void close ()
  {
    lib.close();
  }
#if 0
  template <class RetVal, class... Params >
  void execute (const string& n, Params... p)
  {
    lib.get<RetVal, Params ...>(n)(p ...);
  }
  
  template <class RetVal, class... Params >
  RetVal execute (const string& n, Params... p)
  {
    return lib.get<RetVal, Params ...>(n)(p ...);
  }
#endif
  
  template <class RetVal, class... Params >
  RetVal execute (const string& n, Params... p)
  {
    return lib.get<RetVal, Params ...>(n)(p ...);
  }
  
  template <class... Params >
  void execute (const string& n, Params... p)
  {
    lib.get<void, Params ...>(n)(p ...);
  }
  
  
  typedef shared_ptr<AModule> pointer;
  string libPath() const {return path;}
private:  
  string path;
  shared_library lib;
};

typedef list<AModule::pointer> AModules_t;

#endif
