#ifndef __ITEM_H_
#define __ITEM_H_


#include <java/lang/Integer.h>

#include "org/openscada/opc/lib/da/Item.h"



#include "jivariant.h"
#include "itemstate.h"
#include "tools.h"
#include "jexception.h"

#include <iostream>

using std::cout;

class Item
{
public:
  Item ():item (nullptr)
  {
  }
  Item (org::openscada::opc::lib::da::Item * i):item (i)
  {
//    i->setActive(true);
//    cout << "item: !!!\n";
    
  }
  
  //virtual ::org::openscada::opc::lib::da::Group * getGroup();
  int getServerHandle()
  {
    return jex_cast<int>([this] {return item->getServerHandle(); });
  }
  int getClientHandle()
  {
    return jex_cast<int>([this] {return item->getClientHandle(); });
  }
  string getId()
  {
    return jex_cast<string>([this] {return jstring_to_string(item->getId()); });
  }
  void setActive(bool b)
  {
    jex_cast([this, &b]{ item->setActive(b); });
  }
  ItemState read(bool b)
  {
    return jex_cast<ItemState> ([this, &b] {return ItemState( item->read(b) ); });
  }
  int write( JIVariant v)
  {
     return jex_cast<int> ( [this, &v] {return item->write(v.javaObject())->intValue(); });
  }
  
  org::openscada::opc::lib::da::Item * javaObject() {return item;}
private:
  org::openscada::opc::lib::da::Item * item;
};

#endif
