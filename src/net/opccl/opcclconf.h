#ifndef __OPCCLCONF_H_
#define __OPCCLCONF_H_

#include <vector>
#include <string>

using std::vector;
using std::string;

//#include "connectioninformation.h"
#include "tag.h"

template <class KeyT>
class BaseOpcTag
{
public:
  BaseOpcTag ():tags(nullptr),id_(Tag::impossible())
  {
     cout << "opctag 1\n";
  }
  
  BaseOpcTag (tags_t & t, const KeyT & id__):tags(&t),id_(id__)
  {
     cout << "opctag 2\n";    
  }

  BaseOpcTag (tags_t & t):tags(&t), id_(Tag::impossible())
  {
     cout << "opctag 3\n";
  }
  
  ~BaseOpcTag (){}
  
  void setId(const KeyT& id__)
  {
    id_ = id__;
  }
  KeyT id() const {return id_;}
  string name() const
  {
    assert (tags);
    return (*tags)[id_]->fullName();
  }
private:
  tags_t * tags;
  KeyT id_;
};

typedef BaseOpcTag<tag_id_t> OpcTag;


class OpcGroup : public vector<OpcTag>
{
public:
  OpcGroup() 
  {}

  ~OpcGroup() 
  {}
  
  string name;
private:
  
};

struct OpcHostProps
{
  string host;
  string domain;
  string user;
  string password;
  string clsid;
};

class OpcHost : public OpcHostProps,
                  public vector<OpcGroup>
{
public:
  OpcHost(){}
  ~OpcHost(){}
  
private:
  
};

class OpcHosts_t : public vector<OpcHost>
{
public:
  OpcHosts_t()
  {
  }

  ~OpcHosts_t()
  {
  }
  
  OpcHosts_t(const OpcHosts_t& other):vector<OpcHost>(other)
  {
    cout << "copy of OpcHosts_t!!!!\n";
  }
  
  OpcHosts_t(OpcHosts_t&& other):vector<OpcHost>(other)
  {
    cout << "move of OpcHosts_t!!!!\n";
  }
  
private:
  
};



#endif
