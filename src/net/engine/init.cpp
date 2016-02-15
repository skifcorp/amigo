
#include "entries.h"
#include "amigofs.h"
#include "init.h"
#include "engine.h"

#include <iostream>

using std::cout;
using std::cerr;

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/optional/optional.hpp>
#include <boost/filesystem.hpp>

using boost::property_tree::ptree;
using boost::lexical_cast;
using boost::optional;


bool EntriesParser::parse (tags_t& t, io_service& ios)
{
  ptree tree;
  
  read_xml(t.projectFolder() + a::fs::slash + "entries.xml", tree);
//  read_xml( t.projectFolder() +  + "entries.xml", tree );
  
  entries.reserve ( tree.size() );
  
  for (auto n : tree) {
    entry * e = new entry (t, ios);
    e->setTagId(n.second.get<tag_id_t>("<xmlattr>.tag_id"));
    e->setId(n.second.get<tag_id_t>("<xmlattr>.tag_id"));
    
    {
      optional<float> ilr = n.second.get_optional<float>("<xmlattr>.ilr");
      if (ilr) e->setIlr(*ilr);
    }
    {
      optional<float> ihr = n.second.get_optional<float>("<xmlattr>.ihr");
      if (ihr) e->setIhr(*ihr);
    }
    {
      optional<float> elr = n.second.get_optional<float>("<xmlattr>.elr");
      if (elr) e->setElr(*elr);
    }
    {
      optional<float> ehr = n.second.get_optional<float>("<xmlattr>.ehr");
      if (ehr) e->setEhr(*ehr);
    }
    
    {
      optional<string> typ = n.second.get_optional<string>("<xmlattr>.type");
      if (typ) {
        if (*typ == "USINT") e->setType(A_USINT);
        else if (*typ == "SINT") e->setType(A_SINT);
        else if (*typ == "UINT") e->setType(A_UINT);
        else if (*typ == "INT") e->setType(A_INT);
        else if (*typ == "UDINT") e->setType(A_UDINT);
        else if (*typ == "DINT") e->setType(A_DINT);
        else if (*typ == "REAL") e->setType(A_REAL);
      }
    }
    
    {
      optional<tag_id_t> scale_min = n.second.get_optional<tag_id_t>("<xmlattr>.scale_min");
      if (scale_min) e->setScaleMin(*scale_min);
    }
    {
      optional<tag_id_t> scale_max = n.second.get_optional<tag_id_t>("<xmlattr>.scale_max");
      if (scale_max) e->setScaleMax(*scale_max);
    }

    {
      optional<tag_id_t> inp_scale_min = n.second.get_optional<tag_id_t>("<xmlattr>.inp_scale_min");
      if (inp_scale_min) e->setInpScaleMin(*inp_scale_min);
    }
    {
      optional<tag_id_t> inp_scale_max = n.second.get_optional<tag_id_t>("<xmlattr>.inp_scale_max");
      if (inp_scale_max) e->setInpScaleMax(*inp_scale_max);
    }
    {
      optional<bool> use_var_scales = n.second.get_optional<bool>("<xmlattr>.use_var_scales");
      if (use_var_scales) e->setUseVarScales(*use_var_scales);
    }
    {
      optional<bool> use_inp_scales = n.second.get_optional<bool>("<xmlattr>.use_inp_scales");
      if (use_inp_scales) e->setUseInpScales(*use_inp_scales);
    }
    {
      optional<bool> multip_inp_scales = n.second.get_optional<bool>("<xmlattr>.multip_inp_scales");
      if (multip_inp_scales) e->setMultipInpScales(*multip_inp_scales);
    }
    
    entries.push_back (e);
  }
  
  return true;
}

void InputModulesParser::parse(NetModules_t & modules)
{
  ptree tree;
  read_xml(tags.projectFolder() + a::fs::slash + "inp_modules.xml", tree);
  
  for(auto m : tree) {
    for ( auto n : m.second ) {
      string path = a::fs::input_modules_path() + a::fs::slash + a::fs::lib_name(n.first);
      cout << "input_module: "<<path<<"\n";
      NetModule::pointer p = NetModule::pointer(new NetModule(path));
      modules.insert (make_pair(n.first, p));
      p->open();
    }
  }
}

void TcpInputDevicesParser::parse (TcpInputDevices_t& tcpd, IoServicePool & ios)
{
  ptree tree;
  read_xml(tags.projectFolder() + a::fs::slash + "inp_devices.xml", tree);
  
  for (auto drv : tree) {
    if (drv.first == "<xmlattr>") continue;

    for (auto dev : drv.second) {
      if (dev.first == "<xmlattr>") continue;
      
      string protocol = dev.second.get<string>("<xmlattr>.protocol");
      if (protocol == "votlis") {
        short port = dev.second.get<short>("<xmlattr>.port");
        tcpd.insert(make_pair(protocol, TcpInputDevice::pointer(new TcpInputDevice(port, tags, ios))));
      }
    }
  }
}


void OutputModulesParser::parse(NetModules_t & modules)
{
  ptree tree;
  read_xml(tags.projectFolder() + a::fs::slash + "out_modules.xml", tree);
  
  for(auto m : tree) {
    for ( auto n : m.second ) {
      string path = a::fs::output_modules_path() + a::fs::slash + a::fs::lib_name(n.first);
      cout << "out_module: "<<path<<"\n";
      NetModule::pointer p = NetModule::pointer(new NetModule(path));
      modules.insert(make_pair(n.first, p));
      p->open();
    }
  }
}


void OutputDriversParser::parse (OutputDrivers_t & drivers)
{
  ptree tree;
  read_xml(tags.projectFolder() + a::fs::slash + "out_devices.xml", tree);
  
  for ( auto drv : tree  ) {
    OutputDriver d(drv.second.get<string>( "<xmlattr>.protocol") );
    drivers.push_back(d);
    for ( auto drv_child : drv.second  ) {
      if ( drv_child.first == "<xmlattr>" ) {
        for (auto drv_attr : drv_child.second) {
          if ( drv_attr.first == "protocol"  ) {

          }
          else if ( drv_attr.first == "transport"  ) {
            drivers.back().setTransport(drv_attr.second.data());
          }
          else {
            drivers.back().insert( make_pair( drv_attr.first, drv_attr.second.data() ) );
          }
        }
      }
      else {
        for (auto dev_child : drv_child.second) {
          OutputDevice dev(lexical_cast<int>(dev_child.second.get<string>("id")) );
          drivers.back().devices().push_back(dev);
          for (auto dev_attr : dev_child.second) {
            if (dev_attr.first == "id") {
            }
            else {
              drivers.back().devices().back().insert(make_pair(dev_attr.first, dev_attr.second.data()));
            }
          }
        }
      }
    }
  }
}


