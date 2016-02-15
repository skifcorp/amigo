//#include <boost/extension/extension.hpp>

#include "amigofs.h"
#include "tag.h"
#include "amodule.h"
#include "baseprojparser.h"

#include <iostream>
#include <string>
#include <memory>
#include <functional>
#include <vector>
#include <map>
#include <list>
#include <thread>

using std::cout;
using std::cerr;
using std::string;
using std::shared_ptr;
using std::function;
using std::vector;
using std::map;
using std::list;
using std::thread;

#include <typeinfo>



void ffff()
{
  ptree p;
//  read_xml ("aaa", p);
  
//  if ( typeid(ptree::key_type::value_type) == typeid(char) ) {
//    cout << "got char\n";
//  }
  
  cout <<"name: "<< typeid(ptree::key_type::value_type).name()<<"\n";
}


int main (int argc, char ** argv)
{ 
  tags_t tags;
  tags.setArgcArgv(argc, argv);

  if ( argc < 2 ) {
    //cout << "You must specify path to project in first parameter\n"; return 0;
    tags.showProjMan();
  }
  else {
    tags.setProjectFolder(argv[1] + a::fs::slash);

    tags.run();

//    tags.join();
  }
  
  tags.clear();

  return 0;
}
