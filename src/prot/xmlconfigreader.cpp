#include "xmlconfigreader.h"
#include "amigofs.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/optional.hpp>

using boost::optional;
using boost::property_tree::ptree;
using boost::lexical_cast;
using prot::VariablesContainer;

VariablesContainer prot::XmlConfigReader::readConfig (const tags_t & t)
{
  ptree tree;
  
  read_xml(t.projectFolder() + a::fs::slash + "prot_conf.xml", tree);
  VariablesContainer variables;
  
  
  for (auto db : tree ) {
    for ( pair<string, ptree> v : db.second ) {
      Variable::Pointer var = Variable::Pointer(new Variable(t));

//cout << "equi: "<<db.first;

      var->setEquipment(db.first);
      var->setTagId( v.second.get<tag_id_t>("<xmlattr>.tag_id") );
      
      optional<string> cyr_name = v.second.get_optional<string>("<xmlattr>.cn");
      if (cyr_name)
        var->setRuName (*cyr_name);
      
      optional<float> dz = v.second.get_optional<float>("<xmlattr>.DeadZone");
      if (dz) {
        var->setDeadZone(*dz);
      }

      optional<int> depth = v.second.get_optional<int>("<xmlattr>.Depth");
      if (depth) {
        var->setDepth(*depth);
      }
      
      variables.push_back(var);
    }
  }
  
  return std::move(variables);
}

