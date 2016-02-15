
#include "tag.h"
#include "baseprojparser.h"

//#include "visiface.h"
#include "amigofs.h"

class VisifaceKeeper {
public:
  VisifaceKeeper (  AModule::pointer & m, shared_ptr<VisIface> & vif ) : module(m), iface(vif) 
  {
//    cout << "VisiFace constructor\n";
//   assert (0);
  }
  ~VisifaceKeeper ()
  {
    iface.reset();
    module.reset();
  }
private:
  AModule::pointer     & module;
  shared_ptr<VisIface> & iface;
};

void tags_t::run ()
{
  for ( auto m : modules_contexts ) {
    m->init(*this);
  }
  
  for ( auto m : modules_contexts ) {
    cout<<m->module()->libPath()<<" ";
    //m->setThread( thread(&AModule::execute<void, tags_t&, function<void()>*>, m->module().get(), "executeModule", std::ref(*this), &m->stopFunc() ) );
    m->start();
    //cout << "starting module: "<<m->module()->libPath() << "\n";
  }

  if ( initVisimod() ) {
    VisifaceKeeper k(visimod, visif);
    visif->executeModule();
    visif->start();
  }
  
  _proj_state = proj_running;
}

void tags_t::setProjectFolder(const string& f)
{
  project_folder = f;
  parse_tree tree_parser;
  tree_parser.parse(*this);

  for ( auto path : modules_paths ) {
    cout<<path<<"\n";
    modules_contexts.push_back( ModuleContext::pointer (new ModuleContext(AModule::pointer(new AModule(path)))) );
    modules_contexts.back()->open();
    //modules_contexts.back()->module()->open();
    //modules_contexts.back()->init(*this);
    
//    stop_funcs.push_back( pair<string, function<void()>> (path, function<void()>())) ;
  }

  _proj_state = proj_opened;
}

bool tags_t::initVisimod()
{
  if ( !visimod ) {
    visimod.reset( new AModule(a::fs::lib_path() + a::fs::slash + a::fs::lib_name(string("qtapp"))) );
    visimod->open();
    
    visif = visimod->execute<shared_ptr<VisIface>, tags_t&>("executeModule", *this);
    return true;
  }
  
  return false;
}

void tags_t::showProjMan()
{
  if ( initVisimod() ) {
    VisifaceKeeper k(visimod, visif);
    visif->showProjWindow();
    visif->start();
  }
}
