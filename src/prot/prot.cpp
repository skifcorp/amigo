#include "tag.h"
#include "configreader.h"
#include "compile.h"
#include "modiface.h"

using prot::Variable;
using prot::VariablesContainer;
using prot::stub_config::ConfigReader;
using prot::stub_config::Saver;

#include <functional>
#include <memory>
using std::function;
using std::shared_ptr;

#include <boost/asio.hpp>
#include <boost/any.hpp>
#include <boost/bind/protect.hpp>

using boost::asio::io_service;
using boost::any;
using boost::any_cast;
using boost::protect;


void stopModule(io_service & ios)
{
  ios.stop();
}


typedef function<void (Tag &, const string& , const any& , Variable::Pointer  )> newValFunc_t;

int main()
{
  return 0;
}


void onNewValue (Tag & t, const string& nam, const any& val, Variable::Pointer var)
{  
  if (nam == "value") {
    float fval = static_cast<float>(any_cast<value_t>(val));
    var->appendValue( fval );
    
    function<value_t(void)> elr;
    if (!t.lookup("scale_elr", elr) ) {
      cerr<<"cant find scale_elr in tag: "<<t.fullName()<<"\n";
      elr = []{return 0.0f;};
    }

    function<value_t(void)> ehr;
    if (!t.lookup("scale_ehr", ehr) ) {
      cerr<<"cant find scale_ehr in tag: "<<t.fullName()<<"\n";
      ehr = []{return 100.0f;};
    }
    
    var->checkScale( elr(), ehr() );
  }
  else if (nam == "rel"){
    short rel = any_cast<short>(val);
    if (rel == 1) {
      var->appendValue(NAN);
    }
  }
}

void postNewValue (io_service & ios, newValFunc_t f, Tag & t, const string& nam, const any& val, Variable::Pointer var  )
{
  ios.post( bind(f, std::ref(t), nam, val, var) );
}

class ProtIface : public ModIface
{
public:
  ProtIface(tags_t & t):tags(t){}
  ~ProtIface(){}
  virtual void init()
  {
    ConfigReader config_reader;
    var_cont = config_reader.readConfig(tags);

    for (Variable::Pointer v : var_cont )   {
      newValFunc_t f = onNewValue;
    
      tags[v->tagId()]->connectPropChangeEvent( "prot_val", "rel", bind(postNewValue, std::ref(service), f, _1, _2, _3, v ), true, EventReader );
      tags[v->tagId()]->connectPropChangeEvent( "prot_val", "value", bind(postNewValue, std::ref(service), f, _1, _2, _3, v ), true, EventReader );
      tags[v->tagId()]->incLink();
    
      cout << "prot: "<<tags[v->tagId()]->fullName()<<"\n";
    }
  }
  virtual void start()
  {
    Saver saver( service, tags, var_cont );
    service.run();
  }
  virtual void stop()
  {
    service.stop();
  }
private:
  tags_t & tags;
  io_service service;
  VariablesContainer var_cont;
};

extern "C" shared_ptr<ModIface> executeModule( tags_t& tags )
{
//  cerr << "\n\n\n\nneed to add incLink and decLink!!!!\n\n\n\n";
  return shared_ptr<ModIface>(new ProtIface(tags));
#if 0
  cout << "p.r.o.t. started\n";
  io_service ios; 
  
  ConfigReader config_reader;
  VariablesContainer var_cont = config_reader.readConfig(tags) ;
  
  

  for (Variable::Pointer v : var_cont )   {
    newValFunc_t f = onNewValue;
    
    tags[v->tagId()]->connectPropChangeEvent( "prot_val", "rel", bind(postNewValue, std::ref(ios), f, _1, _2, _3, v ), true, EventReader );
    tags[v->tagId()]->connectPropChangeEvent( "prot_val", "value", bind(postNewValue, std::ref(ios), f, _1, _2, _3, v ), true, EventReader );
    tags[v->tagId()]->incLink();
    
    cout << "prot: "<<tags[v->tagId()]->fullName()<<"\n";
  }
  
  
  Saver saver( ios, tags, var_cont );
  
  
  f = bind ( stopModule, std::ref(ios) );
  
  ios.run();
#endif
}

