#ifndef __BASE_PROJ_PARSER_H_
#define __BASE_PROJ_PARSER_H_

#include "tag.h"
#include "amigofs.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

using boost::property_tree::ptree;
using boost::property_tree::read_xml;

class parse_core_modules
{
public:
  parse_core_modules()
  {
  }
  
  void parse (list<string>& l, const string& base_path)
  {
//    ptree trr;
//    read_xml("aaaaa", trr);

    ptree tree;
    read_xml(base_path + "core.xml", tree);


    if ( typeid(ptree::key_type::value_type) == typeid(char) ) {
      cout << "got char\n";
    }
    else {
      cout << "not char\n";
    }

    for (auto n : tree) {
      for (auto p : n.second ) {
        l.push_back(a::fs::lib_path() + a::fs::slash + a::fs::lib_name (p.first) );
      }
    }
  }
};


class parse_tree 
{
public:
  parse_tree () : cur_id (0){}
/*  std::size_t count ( const ptree & tree  )
  {
    std::size_t counter = 0;

    for ( auto n : tree ) {
      if ( n.first != "<xmlattr>"  ) {
        ++counter;
        if ( !n.second.empty() )
          counter += count(n.second);
      }
    }

    return counter;
  }*/
  void parse (tags_t & tags)
  {
    ptree tree;
    read_xml( tags.projectFolder() + "tags.xml", tree );
    parse (tree, tags);
    
    parse_core_modules p;
    p.parse( tags.modulesPaths(), tags.projectFolder() );
  }
private:
  void parse( const ptree & tree, tags_t & tags )
  {
    list<string> keys;
    
    parse_tree_internal (tree, tags, keys, tags.tagRoot());
  }

  inline string join_list (const list<string>& l)
  {
    list<string>::const_iterator iter = l.begin();
    if (iter == l.end()) return string();

    string ret(*iter); ++iter;

    for (;iter != l.end(); ++iter) {
      ret += "." + *iter;
    }

    return std::move(ret);
  }
  
  template <class T>
  inline void parse_tree_internal ( const ptree & tree, tags_t & tags, list<string>& keys, T & base_tag )
  {
    for ( auto n : tree ) {
      if ( n.first == "<xmlattr>"  ) {
        //int id = n.second.get<int>("id");
      }
      else {
        keys.push_back(n.first);
        Tag * t = new Tag ( n.first );
        t->setId(cur_id++);
        base_tag.appendChild( t );
        tags.appendTag ( join_list(keys), t );
        if ( !n.second.empty() )
          parse_tree_internal ( n.second, tags, keys, *t );

        keys.pop_back();
      }
    }
  }
  tag_id_t cur_id;
};


#endif
