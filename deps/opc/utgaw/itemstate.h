#ifndef __ITEM_STATE_H_
#define __ITEM_STATE_H_

#include <string>

using std::string;

#include <java/lang/Short.h>

#include "org/openscada/opc/lib/da/ItemState.h"

#include "tools.h"
#include "jivariant.h"
#include "jexception.h"

class ItemState
{
public:
  ItemState() : item_state(nullptr){}
  ItemState(org::openscada::opc::lib::da::ItemState * is) : item_state(is){}
  
  string toString()
  {
    return jex_cast<string>([this] {return jstring_to_string(item_state->toString()); } );
  }
  short  getQuality()
  {
    return jex_cast<short> ([this] {return  item_state->getQuality()->shortValue(); });
  }
  void setQuality(short v)
  {
    jex_cast ([this, &v] {item_state->setQuality( new ::java::lang::Short(v) ) ;});
  }
  //virtual ::java::util::Calendar * getTimestamp();
  //virtual void setTimestamp(::java::util::Calendar *);
  JIVariant getValue()
  {
    return jex_cast<JIVariant> ([this] {return JIVariant(item_state->getValue()) ;});
  }
  
  void setValue( JIVariant v )
  {
    jex_cast ([this, &v]{item_state->setValue( v.javaObject() ); });
  }
  
  int getErrorCode()
  {
    return jex_cast<int>( [this] {return item_state->getErrorCode(); });
  }
  
  void setErrorCode (int e)
  {
    jex_cast ([this, &e] {item_state->setErrorCode(e); } );
  }
  
  int hashCode()
  {
    return jex_cast<int> ( [this] {return item_state->hashCode();});
  }
  
//  bool equals(::java::lang::Object *);
  
  org::openscada::opc::lib::da::ItemState * javaObject() {return item_state;}
private:
  org::openscada::opc::lib::da::ItemState * item_state;
};

#endif
