#ifndef __GROUP_H_
#define __GROUP_H_

//#pragma GCC java_exceptions

#include <iostream>

#include "org/openscada/opc/lib/da/Server.h"
#include "org/openscada/opc/lib/da/Group.h"

#include "tools.h"
#include "jexception.h"
#include "item.h"

class Group
{
public:

//public: // actually package-private
//  Group(::org::openscada::opc::lib::da::Server *, jint, ::org::openscada::opc::dcom::da::impl::OPCGroupStateMgt *);
public:
  //Group (const Group& ) = delete;
  
  Group () : group (nullptr)
  {
  }
  
  Group (org::openscada::opc::lib::da::Group* gr) : group (gr) 
  {
  }
/*  
  Group(Group&& other)
  {
    if (this == &other) return;
    group = other.group;
    other.group = nullptr;
  } */
  
  void setActive(bool b)
  {
    jex_cast( [this, &b]{ group->setActive(b); });
  }

  void remove()
  {
    jex_cast([this]{ group->remove(); });
  }
  bool isActive()
  {
    return jex_cast<bool>([this]{return group->isActive();});
  }
  string getName()
  {
    return jex_cast<string> ([this] { return jstring_to_string(group->getName()); });
  }
  void setName (const string& s)
  {
    jex_cast([this, &s]{ group->setName(string_to_jstring(s)); });
  }
  
  Item addItem(const string& n)
  {
    return jex_cast<Item>([this, &n]{ return group->addItem(string_to_jstring(n));  });
  }
  
//  virtual ::java::util::Map * validateItems(JArray< ::java::lang::String * > *);
//  virtual ::java::util::Map * addItems(JArray< ::java::lang::String * > *);
//  virtual void setActive(jboolean, JArray< ::org::openscada::opc::lib::da::Item * > *);
//  virtual ::java::util::Map * write(JArray< ::org::openscada::opc::lib::da::WriteRequest * > *);
//  virtual ::java::util::Map * read(jboolean, JArray< ::org::openscada::opc::lib::da::Item * > *);
/*  Server getServer()
  {
    return Server(group->getServer());
  } */
  
  void clear()
  {
    jex_cast ([this]{ group->clear(); });
  }
  //virtual ::org::openscada::opc::dcom::da::impl::OPCAsyncIO2 * getAsyncIO20();
  //virtual ::org::openscada::opc::dcom::common::EventHandler * attach(::org::openscada::opc::dcom::da::IOPCDataCallback *);
  //virtual ::org::openscada::opc::lib::da::Item * findItemByClientHandle(jint);
  int getServerHandle()
  {
    return jex_cast<int>( [this] {return group->getServerHandle(); } );
  }
  
  void removeItem(const string& s)
  {
    jex_cast([this, &s]{ group->removeItem(string_to_jstring(s)); });
  }

  org::openscada::opc::lib::da::Group * javaObject() {return group;}
private:
  org::openscada::opc::lib::da::Group * group;
};

#endif
