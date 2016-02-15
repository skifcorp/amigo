#include "votlis.h"
#include "tag.h"
#include "amigofs.h"
#include "tagconf.h"
#include "linker.h"
#include "modiface.h"

#include <memory>
#include <iostream>
#include <vector>
#include <memory>
#include <functional>
#include <list>

using std::cout;
using std::shared_ptr;
using std::vector;
using std::shared_ptr;
using std::function;
using std::list;

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/optional.hpp>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/bind/protect.hpp>
#include "monotone_timer.hpp"
#include <boost/lexical_cast.hpp>
#include <boost/assert.hpp>

using boost::property_tree::ptree;
using boost::property_tree::read_xml;
using boost::optional;
using boost::bind;
using boost::protect;
using boost::asio::io_service;
using boost::asio::monotone_timer;
using boost::lexical_cast;


class TagConfParser
{
public:
  TagConfParser (Linkers_t& l):linkers(l)
  {
  }

  ~TagConfParser ()
  {
  }
  
  void parse (tags_t & tags, TagConfs_t & tag_confs, io_service & ios)
  {
    ptree tree;
    read_xml(tags.projectFolder() + a::fs::slash + "votlis.xml", tree);
    
    for ( auto host : tree ) {
      linkers.push_back( Linker::pointer(new  Linker(ios, host.second.get<string>("<xmlattr>.h"), 
                                                          host.second.get<short>("<xmlattr>.p"),  
                                                          host.second.get<unsigned>("<xmlattr>.prd")) ) );
      
      for (auto node : host.second) {
        if (node.first == "<xmlattr>") continue;
        
        optional<ptree& > p = node.second.get_child_optional("<xmlattr>.p");
        if (p) {
          //parseTagChildren((*p).data(), tags);
        }
        else {
          tag_confs.push_back(TagConf());

          TagConf & tc = tag_confs.back();
          
          optional<ptree& > n = node.second.get_child_optional("<xmlattr>.n");
          assert (n);
          tc.assignTag (tags, (*n).data());
          optional<ptree&> accept = node.second.get_child_optional("<xmlattr>.accept");
          if (accept) {
            if ((*accept).data() == "by_name") {
              tc.setAcceptMethod(TagConf::ByName);
            }
            else if ( (*accept).data() == "by_id" ) {
              tc.setAcceptMethod(TagConf::ById);
            }
          }
        
          optional<ptree&> remote_name = node.second.get_child_optional("<xmlattr>.remote_name");
          if (remote_name) {
            tc.setRemoteName((*remote_name).data());
//cout << "--------------->remote_name: "<< (*remote_name).data() <<"\n";
          }

          optional<ptree&> remote_id = node.second.get_child_optional("<xmlattr>.remote_id");
          if (remote_id ) {
            tc.setRemoteId(lexical_cast<tag_id_t>((*remote_id).data()));
          }
          
//          cout << "1: \n";
          connectTag (tc, std::ref(*linkers.back().get()));
//          cout << "2: \n";
        }
      }
    }
  }
private:
  void parseTagChildren( const string& par_name, tags_t& tags )
  {
    cout << "par_name: "<<par_name<<"\n";
  }
  
  void connectTag ( TagConf & tc, Linker & l )
  {
//    cout << "connect tag: "<<tc.remoteName()<<"\n";
    tc.connect(l);
    tc.checkExistingConnections(l);
//    cout << "connect tag: "<<tc.remoteName()<<" finished\n";
  }
  
  Linkers_t& linkers;
};


/*
void checkExistingLinkCount()
{
  
}
*/

/*
void exitFunc(io_service & ios, Linkers_t & linkers, TagConfs_t & tag_confs)
{
  for (auto tc : tag_confs) {
    tc.disconnect();
  }
  ios.stop();
}*/


class VotlisRmtIface : public ModIface
{
public:
  VotlisRmtIface (tags_t & t):tags(t),work_(service)  {}
  ~VotlisRmtIface () {}
  
  virtual void init () 
  {
    TagConfParser parser(linkers);
    parser.parse(tags, tag_confs, service);
  }
  
  virtual void start ()
  {
    service.run();
  }
  
  virtual void stop ()
  {
    cout << "stopping VotlisRmt\n";
    for (auto tc : tag_confs) {
      tc.disconnect();
    }
    cout << "after sdisconnect before stop: \n";
    
    for ( auto l : linkers ) {
      l->stop();
    }
    service.stop();
    cout << "after stop\n";
  }
  
private:
  tags_t & tags;
  Linkers_t linkers;
  TagConfs_t tag_confs;
  
  io_service service;
  io_service::work work_;
  
};

extern "C" shared_ptr<ModIface> executeModule( tags_t& tags )
{
  return shared_ptr<ModIface>(new VotlisRmtIface(tags));
#if 0
  cout << "v.o.t.l.i.s. started\n";
  Linkers_t linkers;
  TagConfs_t tag_confs;
  
  io_service service;
  io_service::work w(service);
  
//  *stop_module_func = bind ( exitFunc, std::ref(service), std::ref(linkers), std::ref(tag_confs) );
    
  {
    TagConfParser parser(linkers);
    parser.parse(tags, tag_confs, service);
  }

  service.run();
  
  cout << "votlis_rmt exiting!!!!!!\n";
#endif
}

