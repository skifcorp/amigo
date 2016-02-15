#ifndef __CONF_PARSER_H_
#define __CONF_PARSER_H_

#include "tag.h"
#include "amigofs.h"
#include "byrt.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/optional/optional.hpp>


using boost::property_tree::ptree;
using boost::property_tree::read_xml;
using boost::optional;


class ConfParser
{
public:
  ConfParser (const tags_t& t):tags(t)
  {
    
  }
  ~ConfParser()
  {
  }

  void parse (ByrtDevices_t & devs)
  {
    ptree tree;
    read_xml(tags.projectFolder() + a::fs::slash + "byrt.xml", tree);
          
    for (auto file : tree) {
      //cout << "file->"<<file.first;

      ByrtDevice dev;
      dev.setId ( file.second.get<device_id_t>("<xmlattr>.dev_id") ); //( file.second.get<device_id_t>("<xmlattr>.dev_id"), file.second.get<string>("<xmlattr>.name") );
          
      for ( auto gr : file.second) {
        if (gr.first == "<xmlattr>") { /*nothing... dev_id we already used */ }
        else {
          ByrtGroup bgr;
//          cout << "before group params\n";
          
          {
            optional<int> offset = gr.second.get_optional<int>("<xmlattr>.offset");
            if (offset) {
              bgr.setOffset(*offset);
            }
          }
          
          {
            optional<int> period = gr.second.get_optional<int>("<xmlattr>.period");
            
            if (period) {        
              bgr.setPeriod(*period);
            }
          }
          
          {
            optional<bool> read_always = gr.second.get_optional<bool>("<xmlattr>.read_always");
            if (read_always) {
              bgr.setReadAlways(*read_always);
            }
          }

          for (auto itm : gr.second ) {
            if (itm.first == "<xmlattr>") {/*nothing... all attributes already parsed*/}
            else {
              ByrtItem bitm;
              bitm.setEntryId(itm.second.get<tag_id_t>("<xmlattr>.ent_id"));
              bgr.push_back(bitm);
            }
          }
          dev.push_back(bgr);
        }
      }
          
      devs.push_back(dev);
    }
  }
private:
  const tags_t& tags;
};


#endif
