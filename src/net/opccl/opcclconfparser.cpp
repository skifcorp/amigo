#include "opcclconfparser.h"
#include "amigofs.h"

#include <iostream>
#include <string>

using std::cout;
using std::string;

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/optional.hpp>

using boost::property_tree::ptree;
using boost::property_tree::read_xml;
using boost::optional;


template <class Tree,class RetType>
inline bool get_attr (Tree & t, const string& attr_name, RetType& ret, const string& msg)
{
  optional<RetType> s = t.template get_optional<RetType>("<xmlattr>."+attr_name); 
    //t.get_optional("<xmlattr>."+attr_name);
  if (s) { 
    ret = *s; return true;
  }
  
  
  cerr << msg <<"\n"; 
  return false;
}


//#define check_is_attr(x) {if (x.first == "<xmlattr>") continue;}

OpcHosts_t OpcclConfParser:: parse(tags_t & tags)
{
  ptree tree;
  read_xml(tags.projectFolder() + a::fs::slash + "opccl.xml", tree);
  OpcHosts_t hosts;
  
  
  
  for ( auto base : tree ) {
    if (base.second.empty()) continue;
    for (auto host : base.second) {
      if (host.first == "<xmlattr>") continue;
      
      OpcHost opc_host;

      if (!get_attr(host.second, "host"    , opc_host.host,  "Cant find host when parsing opccl.xmll!!! in node: " + host.first)) continue;
      if (!get_attr(host.second, "domain"  , opc_host.domain,  "Cant find domain when parsing opccl.xmll!!! in node: " + host.first)) continue;
      if (!get_attr(host.second, "user"    , opc_host.user,  "Cant find user when parsing opccl.xmll!!! in node: " + host.first)) continue;
      if (!get_attr(host.second, "password", opc_host.password,  "Cant find password when parsing opccl.xmll!!! in node: " + host.first)) continue;
      if (!get_attr(host.second, "clsid"   , opc_host.clsid,  "Cant find clsid when parsing opccl.xmll!!! in node: " + host.first)) continue;
      
      for (auto group : host.second)  {
        if (group.first == "<xmlattr>") continue;
        OpcGroup opc_group;
        
        if (!get_attr(group.second, "name"    , opc_group.name,  "Cant find name when parsing opccl.xmll!!! node:" + group.first)) continue;
        
        for(auto tag : group.second) {
          if (tag.first == "<xmlattr>") continue;
          tag_id_t id;
          if (!get_attr(tag.second,"id", id, "Cant find ID when parsing opccl.xmll!!! node: " + tag.first) ) continue;

          opc_group.push_back( OpcTag(tags, id) );
        }
        
        opc_host.push_back(opc_group);
      }
      hosts.push_back(opc_host);
    }
  }
  
  return hosts;
}
