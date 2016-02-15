
// DO NOT EDIT THIS FILE - it is machine generated -*- c++ -*-

#ifndef __javax_activation_URLDataSource__
#define __javax_activation_URLDataSource__

#pragma interface

#include <java/lang/Object.h>
extern "Java"
{
  namespace java
  {
    namespace net
    {
        class URL;
        class URLConnection;
    }
  }
  namespace javax
  {
    namespace activation
    {
        class URLDataSource;
    }
  }
}

class javax::activation::URLDataSource : public ::java::lang::Object
{

public:
  URLDataSource(::java::net::URL *);
  virtual ::java::lang::String * getContentType();
  virtual ::java::lang::String * getName();
  virtual ::java::io::InputStream * getInputStream();
  virtual ::java::io::OutputStream * getOutputStream();
  virtual ::java::net::URL * getURL();
private:
  ::java::net::URL * __attribute__((aligned(__alignof__( ::java::lang::Object)))) url;
  ::java::net::URLConnection * connection;
public:
  static ::java::lang::Class class$;
};

#endif // __javax_activation_URLDataSource__