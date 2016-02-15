#ifndef __TOOLS_H_
#define __TOOLS_H_


#include <java/lang/String.h>
#include <gcj/cni.h>

#include <string>
using std::string;


#include "jexception.h"

inline string jstring_to_string(::java::lang::String * js)
{
  return jex_cast<string> ( [js] (){
    int len = JvGetStringUTFLength(js);
    char buffer[len + 1];
    jsize real_len = JvGetStringUTFRegion( js, 0, len, buffer );
    buffer[real_len] = '\0';
    return string(buffer);
//    char * buffer = new char[len];
//    JvGetStringUTFRegion( js, 0, len, buffer );
//    return string(buffer, len);
    
  });
}

inline ::java::lang::String * string_to_jstring(const string& s)
{
  return jex_cast< ::java::lang::String *> ([&s] {
    return new ::java::lang::String( JvNewStringLatin1(s.c_str(), s.size()) );
  });
}

#endif
