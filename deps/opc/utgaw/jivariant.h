#ifndef __JI_VARIANT_H_
#define __JI_VARIANT_H_

#include "tools.h"

//#include "org/jinterop/dcom/core/JIVariant.h"
#include "nativejivariantwrapper.h"
#include "jexception.h"

#include <string>

using std::string;

class JIVariant 
{
public:
  JIVariant ( ) : variant (nullptr) {}
  JIVariant (org::jinterop::dcom::core::JIVariant * v) : variant(v)
  {
  }
  JIVariant( const JIVariant& other ) 
  {
    jex_cast ([this, &other] {variant = new org::jinterop::dcom::core::JIVariant(const_cast<JIVariant*>(&other)->javaObject()); });
  }

  void setFlag(int f)
  {
    jex_cast ([this, &f] {variant->setFlag (f); });
  }
  int getFlag()
  {
    return  jex_cast<int>([this] { return variant->getFlag(); });
  }
  bool isNull()
  {
    return jex_cast<bool>([this] {return variant->isNull(); });
  }
  JIVariant(int v, bool b)
  {
    jex_cast([this, &v, &b] { variant = new org::jinterop::dcom::core::JIVariant(static_cast<jint>(v), b); });
  }
  JIVariant(long v, bool b)
  {
    jex_cast([this, &v, &b] { variant = new org::jinterop::dcom::core::JIVariant(static_cast<jlong>(v), b); });
  }
  
  JIVariant(float v, bool b)
  {
    jex_cast([this, &v, &b] { variant = new org::jinterop::dcom::core::JIVariant(static_cast<jfloat>(v), b); });
  }
  
  JIVariant(bool v, bool b)
  {
    jex_cast([this, &v, &b] { variant = new org::jinterop::dcom::core::JIVariant(static_cast<jboolean>(v), b); });
  }
  
  JIVariant(double v, bool b)
  {
    jex_cast([this, &v, &b] { variant = new org::jinterop::dcom::core::JIVariant(static_cast<jdouble>(v), b); });
  }
  
  JIVariant(short v, bool b)
  {
    jex_cast([this, &v, &b] { variant = new org::jinterop::dcom::core::JIVariant(static_cast<jshort>(v), b); });
  }
  
  JIVariant(char v, bool b)
  {
    jex_cast([this, &v, &b] { variant = new org::jinterop::dcom::core::JIVariant(static_cast<jchar>(v), b); });
  }
  
  //JIVariant(:org::jinterop::dcom::core::JIString *, jboolean);
  JIVariant(const string& v,bool b)
  {
    jex_cast([this, &v, &b] { variant = new org::jinterop::dcom::core::JIVariant(string_to_jstring(v), b); });
  }
  
  JIVariant(const string & v)
  {
     jex_cast([this, &v] { variant = new org::jinterop::dcom::core::JIVariant(string_to_jstring(v)); });
  }
  
//  JIVariant(::org::jinterop::dcom::core::IJIComObject *, jboolean);
//  JIVariant(::org::jinterop::dcom::core::JIVariant$SCODE *, jint, jboolean);
  JIVariant(int v)
  {
    jex_cast([this, &v] { variant = new org::jinterop::dcom::core::JIVariant(static_cast<jint>(v)); });
  }

  JIVariant(float v)
  {
    jex_cast([this, &v] { variant = new org::jinterop::dcom::core::JIVariant(static_cast<jfloat>(v));  });
  }
  
  JIVariant(bool v)
  {
    jex_cast([this, &v] { variant = new org::jinterop::dcom::core::JIVariant(static_cast<jboolean>(v));  });
  }
  
  JIVariant(double v)
  {
    jex_cast([this, &v] { variant = new org::jinterop::dcom::core::JIVariant(static_cast<jdouble>(v)); });
  }
  
  JIVariant(short v)
  {
    jex_cast([this, &v] { variant = new org::jinterop::dcom::core::JIVariant(static_cast<jshort>(v)); });
  }
  
  JIVariant(char v)
  {
    jex_cast([this, &v] { variant = new org::jinterop::dcom::core::JIVariant(static_cast<jchar>(v)); });
  }
  
  
//  JIVariant(::org::jinterop::dcom::core::JIString *);
//  JIVariant(::org::jinterop::dcom::core::IJIComObject *);
//  JIVariant(::java::util::Date *);
//  JIVariant(::java::util::Date *, jboolean);
//  JIVariant(::org::jinterop::dcom::core::JICurrency *);
//  JIVariant(::org::jinterop::dcom::core::JICurrency *, jboolean);
//public:
//  JIVariant(::org::jinterop::dcom::core::JIVariant$SCODE *, jint);
//  JIVariant(::org::jinterop::dcom::core::JIArray *, jint);
//  JIVariant(::org::jinterop::dcom::core::JIArray *, jboolean, jint);
//  JIVariant(::org::jinterop::dcom::core::JIArray *, jboolean);
//private:
//  void initArrays(::org::jinterop::dcom::core::JIArray *, jboolean, jint);
//  JIVariant(::org::jinterop::dcom::core::JIArray *);
//  JIVariant(::org::jinterop::dcom::core::IJIUnsigned *);
//  JIVariant(::org::jinterop::dcom::core::IJIUnsigned *, jboolean);
//  ::java::lang::Object * getObject();
  int getObjectAsInt()
  {
    return jex_cast<int>([this] {return variant->getObjectAsInt(); });
  }

  float getObjectAsFloat()
  {
    return jex_cast<float>([this] {return variant->getObjectAsFloat(); });
  }
  //int getObjectAsSCODE();
  double getObjectAsDouble()
  {
    return jex_cast<double>([this] {return variant->getObjectAsDouble(); });
  }
  short getObjectAsShort()
  {
    return jex_cast<short>([this] {return variant->getObjectAsShort(); });
  }
  bool getObjectAsBoolean()
  {
    return jex_cast<bool>([this] {return variant->getObjectAsBoolean(); });
  }
  //::org::jinterop::dcom::core::JIString * getObjectAsString();
  string getObjectAsString2()
  {
    return jex_cast<string>([this] {return jstring_to_string(variant->getObjectAsString2()); });
  }
  //::java::util::Date * getObjectAsDate();
  char getObjectAsChar()
  {
    return jex_cast<char>([this] {return variant->getObjectAsChar(); });
  }
  //::org::jinterop::dcom::core::IJIComObject * getObjectAsComObject();
  //::org::jinterop::dcom::core::JIVariant * getObjectAsVariant();
  //::org::jinterop::dcom::core::JIArray * getObjectAsArray();
  long getObjectAsLong()
  {
    return jex_cast<long>([this] {return variant->getObjectAsLong(); });
  }
  //::org::jinterop::dcom::core::IJIUnsigned * getObjectAsUnsigned();
  int getType()
  {
    return jex_cast<int>([this] {return variant->getType(); });
  }
  string toString()
  {
    return jex_cast<string>([this] {return jstring_to_string(variant->toString()); });
  }
  static const int VT_NULL = 1;
  static const int VT_EMPTY = 0;
  static const int VT_I4 = 3;
  static const int VT_UI1 = 17;
  static const int VT_I2 = 2;
  static const int VT_R4 = 4;
  static const int VT_R8 = 5;
  static const int VT_VARIANT = 12;
  static const int VT_BOOL = 11;
  static const int VT_ERROR = 10;
  static const int VT_CY = 6;
  static const int VT_DATE = 7;
  static const int VT_BSTR = 8;
  static const int VT_UNKNOWN = 13;
  static const int VT_DECIMAL = 14;
  static const int VT_DISPATCH = 9;
  static const int VT_ARRAY = 8192;
  static const int VT_BYREF = 16384;
  static const int VT_BYREF_VT_UI1 = 16401;
  static const int VT_BYREF_VT_I2 = 16386;
  static const int VT_BYREF_VT_I4 = 16387;
  static const int VT_BYREF_VT_R4 = 16388;
  static const int VT_BYREF_VT_R8 = 16389;
  static const int VT_BYREF_VT_BOOL = 16395;
  static const int VT_BYREF_VT_ERROR = 16394;
  static const int VT_BYREF_VT_CY = 16390;
  static const int VT_BYREF_VT_DATE = 16391;
  static const int VT_BYREF_VT_BSTR = 16392;
  static const int VT_BYREF_VT_UNKNOWN = 16397;
  static const int VT_BYREF_VT_DISPATCH = 16393;
  static const int VT_BYREF_VT_ARRAY = 24576;
  static const int VT_BYREF_VT_VARIANT = 16396;
  static const int VT_I1 = 16;
  static const int VT_UI2 = 18;
  static const int VT_UI4 = 19;
  static const int VT_I8 = 20;
  static const int VT_INT = 22;
  static const int VT_UINT = 23;
  static const int VT_BYREF_VT_DECIMAL = 16398;
  static const int VT_BYREF_VT_I1 = 16400;
  static const int VT_BYREF_VT_UI2 = 16402;
  static const int VT_BYREF_VT_UI4 = 16403;
  static const int VT_BYREF_VT_I8 = 16404;
  static const int VT_BYREF_VT_INT = 16406;
  static const int VT_BYREF_VT_UINT = 16407;
  static const int FADF_AUTO = 1;
  static const int FADF_STATIC = 2;
  static const int FADF_EMBEDDED = 4;
  static const int FADF_FIXEDSIZE = 16;
  static const int FADF_RECORD = 32;
  static const int FADF_HAVEIID = 64;
  static const int FADF_HAVEVARTYPE = 128;
  static const int FADF_BSTR = 256;
  static const int FADF_UNKNOWN = 512;
  static const int FADF_DISPATCH = 1024;
  static const int FADF_VARIANT = 2048;
  static const int FADF_RESERVED = 61448;
  
  org::jinterop::dcom::core::JIVariant * javaObject() {return variant;}
  //const org::jinterop::dcom::core::JIVariant * javaObject() const {return variant;}
private:
  org::jinterop::dcom::core::JIVariant * variant;
};

#endif
