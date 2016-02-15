#ifndef __TAG_H_
#define __TAG_H_

#include "amtree.h"
#include "amodule.h"
#include "visiface.h"
#include "modiface.h"

#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <atomic>
#include <list>
#include <functional>
#include <cassert>
#include <mutex>
#include <memory>
#include <thread>

using std::string;
using std::map;
using std::vector;
using std::cout;
using std::atomic_int;
using std::atomic_flag;
using std::list;
using std::function;
using std::mutex;
using std::lock_guard;
using std::shared_ptr;
using std::thread;
using std::pair;

#include <boost/any.hpp>
#include <boost/bind.hpp>
using boost::any;
using boost::any_cast;
using boost::bind;

typedef int tag_id_t;

struct value_t {
  value_t() : init_(false), fval(0.0){}
  value_t(float v) : init_(false), fval(v){}
  value_t(int v) : init_(false), ival(v){}
  value_t(unsigned v) : init_(false), uval(v){}
  
  bool init() const {return init_;}
  
  value_t& operator=(const float& other)
  {
    fval = other; init_ = true; return *this;
  }
  value_t& operator=(const int& other)
  {
    ival = other; init_ = true; return *this;
  }
  value_t& operator=(const unsigned& other)
  {
    uval = other; init_ = true; return *this;
  }
  operator float() const
  {
    return fval;
  }
  operator int() const 
  {
    return ival;
  }
  operator unsigned() const
  {
    return uval;
  }
private:
  bool init_;
  union {
    float        fval;
    unsigned int uval;
    int          ival;
  };

};

enum EventSubscriber
{
  EventNone, EventReader, EventWriter
};

class Tag : public TreeNode<Tag>
{
public:
  typedef function<void(Tag &, const string&, const any& )> PropEventFunc;
  typedef map<string, any>::iterator iterator;
  typedef map<string, any>::const_iterator const_iterator;
  
  Tag (const string& n= string(), Tag * p = 0) : TreeNode<Tag>(p), name_(n), id_(impossible()), links_(0)
  {
    
  }
  
  ~Tag ()
  {
    
  }
  
  Tag (const Tag&) = delete;
  string name() const
  {
    return name_;
  }
  string fullName() const
  {
    if ( parent() ) {
      return parent()->fullName() + "." + name_;
    }
    
    return name_;
  }
  static tag_id_t impossible() {return -1;}
  void setId (tag_id_t i){id_ = i;}
  tag_id_t id() const {return id_;}
  
  void incLink() 
  {
    if ( links_.fetch_add(1) == 0 ) {
      lock_guard<mutex> g(link_mutx);
      for ( auto f : link_events ) {
        f.second();
      }
    }
  }

  void decLink() 
  {
    while (1) {
      int cur_val = links_.load();
      int exp_val = cur_val - 1;
      if (cur_val <= 0)  break;

      if (links_.compare_exchange_strong(cur_val, exp_val)) {
        if (exp_val == 0) {
          lock_guard<mutex> g(link_mutx);
          for ( auto f : unlink_events ) {
            f.second();
          }
        }
        break;
      } 
    }
  }
  
  int linksCount() const
  {
    return links_.load();
  }
  
  typedef function<void ()> eventFunc;

  void connectLinkEvent(const string& n, eventFunc e, bool emit = false)
  {
    lock_guard<mutex> g(link_mutx);
    auto res = link_events.insert( make_pair(n, e) );
    
    assert (res.second);
    
    if ( emit && links_.load() > 0 ) {
      e();
    }
  }
  void disconnectLinkEvent(const string& n)
  {
    lock_guard<mutex> g(link_mutx);
    link_events.erase(n);
  }

  void connectUnlinkEvent(const string& n, eventFunc e)
  {
    lock_guard<mutex> g(link_mutx);
    auto res = unlink_events.insert(make_pair(n, e));
    assert(res.second);
  }
  void disconnectUnlinkEvent(const string& n)
  {
    lock_guard<mutex> g(link_mutx);
    unlink_events.erase(n);
  }
  void connectPropChangeEvent(const string& ev_name, const string& prop_name, PropEventFunc f, bool emit, EventSubscriber subsc )
  {
    //here we can connect properties which dont exists because they can appear later
    lock_guard<mutex> g(prop_mutx);
    prop_funcs[prop_name][ev_name] = make_pair(f, subsc);
    if ( emit ) {
      auto val_iter = props_.find(prop_name);
      if ( val_iter == props_.end() ) {
        //cerr<<"cant emit!!! prop: "<<prop_name<<" not found\n"; 
        return;
      }

      f(*this, prop_name, val_iter->second);
    }
  }
  void disconnectPropChangeEvent(const string& ev_name, const string& prop_name)
  {
    lock_guard<mutex> g(prop_mutx);
    auto iter = prop_funcs.find(prop_name);
    
    if ( iter == prop_funcs.end() ) return;
    
    (*iter).second.erase(ev_name);
    if ( (*iter).second.empty() )
      prop_funcs.erase(iter);
  }
  
/*  void blockEvent(bool bl, const string& prop_name)
  {
    lock_guard<mutex> g(prop_mutx);
    if ( bl )
      block_prop_events.insert(prop_name);
    else {
      block_prop_events.erase(prop_name);
    }  
  } */

  void erase (const string& k)
  {
    lock_guard<mutex> g(prop_mutx);
    props_.erase(k);
  }
  
  
  template<class T>
  void setProp(const string& k, const T& v, EventSubscriber subsc)
  {
    lock_guard<mutex> g(prop_mutx);
    
//    if ( block_prop_events.find(k) != block_prop_events.end() ) {
//      cout << "ev: "<<k<<"blocked!!!\n";
//    }
    
    any & vref = props_[k];
    vref = v;
    auto m = prop_funcs.find(k);
    if (m == prop_funcs.end()) return;

    for ( map<string, pair<PropEventFunc, EventSubscriber>>::iterator iter = m->second.begin(); iter != m->second.end(); ++iter) {
      //fi->second(*this, k, vref);
      if ( (subsc == EventWriter) || (subsc == EventReader && iter->second.second == subsc) ) {
        iter->second.first(*this, k, vref);
      }
    }
  }
  
  template <class T>
  bool lookup(const string& k, T& v) const
  {
    lock_guard<mutex> g(prop_mutx);
    
//    if (k == "elr" ) assert(0);
    
    auto iter = props_.find(k);
    if ( iter == props_.end() )
      return false;
    v = any_cast<T>((*iter).second);

    return true;
  }
  
  
  /*
  iterator find(const string& k)
  {
   return props_.find(k);
  }
  
  const_iterator find(const string& k) const
  {
   return props_.find(k);
  }
  
  iterator end() 
  {
    return props_.end();
  }
  
  const_iterator end() const
  {
    return props_.end();
  }*/
private:
#if 0
  void emitPropEvent(const string& ev_name, const string& prop_name )
  {
    lock_guard<mutex> g(prop_mutx);
    auto prop_iter = prop_funcs.find(prop_name);
    if ( prop_iter == prop_func.end( ) ) {
      cerr<<"cant reinit!!! events map for prop: "<<prop_name<<" not found!"; return;
    }
    
    auto ev_iter = prop_iter->second.find(ev_name);
    if ( ev_iter == prop_iter->second.end() ) {
      cerr<<"cant reinit!!! event for prop: "<<ev_name<<" not found\n"; return;
    }
    
    auto val_iter = props_.find(k);
    if ( val_iter == props_.end() ) {
      cerr<<"cant reinit!!! prop: "<<prop_name<<" not found\n"; return;
    }
    
    ev_iter->second(*this, prop_name, *val_iter);
  }
#endif

  string name_;
  tag_id_t id_;
  atomic_int links_;
  map<string, eventFunc> link_events;
  map<string, eventFunc> unlink_events;
  mutable mutex link_mutx, prop_mutx;

  map<string, any> props_;
  
  typedef map<string, map<string, pair<PropEventFunc, EventSubscriber>>> PropEventFuncs;
  PropEventFuncs prop_funcs;
  
  //set<string> block_prop_events;
};

class tags_t;

class ModuleContext
{
public:
  ModuleContext(AModule::pointer m):module_(m) {}
  ModuleContext() {}
  ModuleContext(const ModuleContext& ) = delete;
  ~ModuleContext()
  {
    stop();
    
    cout << "destructing module: "<<module_->libPath()<<"\n";
    if (  thread_.joinable() ) thread_.join();

    modiface_.reset();
    module_->close();
    module_.reset();
  }

  void setModule(AModule::pointer m) {module_ = m;}
  AModule::pointer module() {return module_;}
  const AModule::pointer module() const {return module_;}
  void stop() {if (modiface_)modiface_->stop();}

  typedef shared_ptr<ModuleContext> pointer;
  typedef shared_ptr<thread> thread_ptr;
  //void setThread(thread && th) {thread_ = std::move(th);}
  //const thread& getThread() const {return thread_;}
  void join ()
  {
    thread_.join();
  }
  
  void open ()
  {
    module_->open();
  }
  
  void init(tags_t & tags)
  {
    assert (module_.get());
    //module_->open();
    
    modiface_ = module_->execute<shared_ptr<ModIface>, tags_t& >("executeModule", tags);
    
    assert (modiface_.get());
    modiface_->init();
  }
  
  void start()
  {
    assert(modiface_);
    //modiface->start();
    thread_ = std::move(thread(bind(&ModIface::start, modiface_.get())));
  }
private:
  AModule::pointer module_;
  thread           thread_;
  shared_ptr<ModIface> modiface_;
};

class TagRoot : public TreeRoot<Tag>
{
public:
  TagRoot ( size_type s ) : TreeRoot<Tag>(s) {}
  TagRoot (  ) : TreeRoot<Tag>() {}
  //TagRoot(const TagRoot& ) = delete;
  void appendChild (Tag * t)
  {
    push_back(t);
  }
};

class tags_t
{
public:
  typedef vector<Tag *> tags_vector;
  typedef map<string, Tag*> tags_map;
  
  typedef tags_vector::iterator ordered_iterator;
  typedef tags_vector::const_iterator const_ordered_iterator;
  
  typedef tags_map::iterator name_iterator;
  typedef tags_map::const_iterator const_name_iterator;
  
  ordered_iterator begin() 
  {
    return indexes.begin();
  }
  
  const_ordered_iterator begin() const
  {
    return indexes.begin();
  }

  ordered_iterator end() 
  {
    return indexes.end();
  }
  
  const_ordered_iterator end() const
  {
    return indexes.end();
  }
  
  name_iterator nameEnd() 
  {
    return names.end();
  }
  
  const_name_iterator nameEnd() const
  {
    return names.end();
  }
  
  tags_t (tags_vector::size_type s): _proj_state(proj_closed)
  {
    indexes.reserve (s);
  }

  tags_t (): _proj_state(proj_closed)
  {
  }

  
  tags_t (const tags_t& ) = delete;
  
  ~tags_t ()
  {
    //visif.reset();
    //join();
  }
  
  Tag * operator[] (tags_vector::size_type id)
  {
    assert (id < indexes.size());
    return indexes[id];
  }
  
  const Tag * operator[] (tags_vector::size_type id) const
  {
    assert (id < indexes.size());
    return indexes[id];
  }

  Tag * operator[] (const string& k)
  {
    tags_map::iterator iter = names.find(k);
    if (iter == names.end())
      return nullptr;
    
    return (*iter).second;
  }
  
  const Tag * operator[] (const string& k) const
  {
    tags_map::const_iterator iter = names.find(k);
    if (iter == names.end())
      return nullptr;
    
    return (*iter).second;
  }
  
  name_iterator find(const string& n)
  {
    return names.find(n);
  }
  
  const_name_iterator find (const string& n) const
  {
    return names.find(n);
  }
  

  
  void appendTag ( const string& full_name, Tag * t )
  {
    indexes.push_back (t);
    
//    cout << "parsing tags: size: "<<indexes.size() << " id: "<<t->id()<<"\n";
    
    names.insert ( tags_map::value_type(full_name, t) );
  }
  
  void setProjectFolder(const string& f);
  string projectFolder() const 
  {
    return project_folder;
  }
  void printNames () const
  {
    for (auto t : names) {
      cout << t.first << "\n";
    }
  }
  
  void setArgcArgv(int & ac, char ** av)
  {
    argc_ = &ac; argv_ = av;
  }
  int& argc()
  {
    return *argc_;
  }
  char ** argv()
  {
    return argv_;
  }
  
  TagRoot& tagRoot()
  {
    return tag_root;
  }
  
  list<string>& modulesPaths()
  {
    return modules_paths;
  }

  const TagRoot& tagRoot() const
  {
    return tag_root;
  }
  
  const list<string>& modulesPaths() const
  {
    return modules_paths;
  }
  
  void clear()
  {
//    props.clear();
    indexes.clear();
    names.clear();
    tag_root.clear();
    modules_paths.clear();
  }
  
  void run ();
  
  void stop()
  {
    for (auto p : modules_contexts) {
      cout << "before stop: "<<p->module()->libPath()<<"\n";
      p->stop();
      p->join();
    }
//    stop_funcs.clear();
    _proj_state = proj_stopped;
  }  
  void closeProject()
  {
/*    for (auto m : modules_contexts) {
      m.close();
    } */
//    cout << "1\n";
    modules_contexts.clear();
//    cout << "2\n";    
    clear();
//    cout << "3\n";
    _proj_state = proj_closed;
  }
  
/*  void join()
  {
//    cout << "before join: \n";
    for (auto t : threads) {
      cout << "joining thread: "<<t->get_id()<<"\n";
      t->join();
    }
  }*/
  
  /*
  vector<function<void()>> & stopFuncs()
  {
    return stop_funcs;
  }
  */
  void showProjMan();
  enum proj_state {
    proj_closed, proj_opened, proj_running, proj_stopped
  };


  proj_state projState() const {
    return _proj_state;
  }
  
private:
  tags_vector indexes;
  tags_map names;
  
  string project_folder;
  shared_ptr<VisIface> visif;
  AModule::pointer visimod;
  
  int * argc_;
  char ** argv_;
  
  TagRoot tag_root;
  list<string> modules_paths;
  
  //typedef shared_ptr<thread> thread_ptr;
  //typedef vector<thread_ptr> threads_t;
  
  //AModules_t modules;
  //threads_t threads;
  
  //list<pair<string, function<void()>>> stop_funcs;
  list<ModuleContext::pointer> modules_contexts;
  
  bool initVisimod();
  
  proj_state _proj_state;
  
};


#endif
