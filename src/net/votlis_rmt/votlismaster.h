#ifndef __VOTLIS_MASTER_H_
#define __VOTLIS_MASTER_H_

#include "tagconf.h"
#include "votlisprotocol.h"
#include "outputethernetconnection.h"
#include "amsync.h"

#include <string>
#include <vector>
#include <list>


using std::string;
using std::list;
using std::pair;
using std::make_pair;
using std::cerr;

#include <boost/asio.hpp>
#include <boost/assert.hpp>
#include "monotone_timer.hpp"

using boost::asio::io_service;
using boost::asio::monotone_timer;


class VotlisMaster
{
public:
  VotlisMaster(io_service & ios, const string& h, short p, unsigned prd) : algo_state(None), conn( &ios, wrap<io_service&>() ), 
                         period_(prd), linked_count(0), timer_service(), timer(timer_service), timer_work(timer_service), timer_thread(new thread(bind(&io_service::run, &timer_service)))
  {
    conn.setIp(h); conn.setPort(p);
    
  }
  ~VotlisMaster()
  {
    cout << "VOTLIS_MASTER destructed!!!!!!!!!!!!!!!!!!!!!!!!\n";
    unlinkAll();
    timer_service.stop();
    if (timer_thread->joinable())
      timer_thread->join();
  }
  
  void appendTag(const TagConf& tc)
  {
    tags.push_back( make_pair(tc, NotLinked) );
    
    if ( !conn.ready() && algo_state == None ) {
      connect();
    }
  }

  //function need syncronization
  void removeTag (const TagConf& tc)
  {
    //this function caled from tag linked by post

    for (auto iter = tags.begin(); iter != tags.end(); ++iter) {
      if ( (*iter).first == tc ) {
        unlink( tc, (*iter).second );
        tags.erase(iter);
      }
    }
  }

  io_service & get_io_service () {return conn.get_io_service();}
  void stopInternalService ()
  {
    timer.cancel();
    timer_service.stop();
    if ( timer_thread->joinable() ) {
      cout << "<<<<<<<<<<<<<<<before join timer_thread\n";
      timer_thread->join();
      cout << ">>>>>>>>>>>>>>>after join timer_thread\n";
    }
  }
private:
  enum TagState{NotLinked, Linked, NotFound};
  enum AlgoState{None, Connecting, Linking, ReadingNew, Writing};
  AlgoState algo_state;
  
  vector<char> buffer;
  OutputEthernetConnection<io_service&> conn;
  
  list<pair<TagConf, TagState>> tags;
  unsigned period_;
  unsigned linked_count;

  io_service timer_service;
  monotone_timer timer;
  io_service::work timer_work;

  shared_ptr<thread> timer_thread;
  
  bool timerStopped() const
  {
    return timer.expires_at().m < monotone_timer::traits_type::now().m;
  }
  
  void unlinkAll()
  {
    for ( auto t : tags ) {
      unlink(t.first, t.second);
    }
  }
  
  void startUnlink(tag_id_t t)
  {
    //this method called from another thread
    am_sync promis;
    
    get_io_service().post( bind(&VotlisMaster::handleStartUnlink, this, t, std::ref(promis)));
    
    cout << "------------->wait1\n";
    promis.wait();
    cout << "------------->after wait1\n";
  }
  
  void handleStartUnlink(tag_id_t t, am_sync & promis)
  {
    VotlisProtocol vp;
    
    buffer = vp.unlinkItem(t);
    conn.writeAsync(buffer, buffer.size(), bind(&VotlisMaster::unlinkWriteHandler, this, _1, std::ref(promis)));
  }
  
  void unlink(const TagConf& t, TagState st)
  {
    disconnectWriteEvent(t);
    
    if (st == Linked) {
      --linked_count;
      //cout << "s->"<<t.remoteId() << " in linked state!!!\n";
      timer_service.post( bind(&VotlisMaster::startUnlink, this, t.remoteId()) );
    }
    
//    cout << "linked count: "<<linked_count<<"\n";
    
    assert(linked_count>=0);
  }
  
  void unlinkWriteHandler(bool ok, am_sync& promis)
  {
    if (!ok) {cerr<<__func__<<" ";onError(promis);  return;}
    
    buffer.resize(sizeof(int));
    conn.readAsync(buffer, buffer.size(), bind(&VotlisMaster::unlinkReadHandler, this, _1, std::ref(promis)));
  }
  
  void unlinkReadHandler(bool ok, am_sync & promis) 
  {
    if (!ok){ cerr<<__func__<<" ";onError(promis); return;}
    
    promis.set();
  }
  
  void startConnect()
  {
    //this method called from another thread
//cout << "start connect called!!!\n";
    am_sync promis;
    get_io_service().post(bind(&VotlisMaster::handleStartConnect, this, std::ref(promis)));
cout << "------------------> wait 2\n";
    promis.wait();
cout << "------------------> after wait 2\n";
  }
  
  
  void handleStartConnect(am_sync & promis)
  {
//cout << "handle start connect called!!!\n";

    conn.asyncPrepare( bind(&VotlisMaster::connectHandler, this, _1, std::ref(promis)) );
    algo_state = Connecting;
  }
  
  void connect()
  {
//cout << "connect called\n";
    timer_service.post( bind(&VotlisMaster::startConnect, this));
  }
  
  void connectHandler (bool ok, am_sync & promis )
  {
    if (!ok) { cerr<<__func__<<" error";onError(promis); return;}
    
    //cout << "connected!!!\n";
    
    //startLink( /*boost::system::error_code(boost::system::errc::success, boost::system::get_system_category()) */);
    timer_service.post(bind(&VotlisMaster::handleTimer, this, boost::system::error_code(boost::system::errc::success, boost::system::get_system_category())));
    
    promis.set();
  }
  
  void startLink( /*const boost::system::error_code& er*/ am_sync & promis ) 
  {
//    if (!er)
      tryLink(tags.begin(), promis);
  }
  
  void tryLink( list<pair<TagConf, TagState>>::iterator it , am_sync & promis )
  {
    //cout << "tryLink: "<<(it == tags.end())<<"\n";
    
    for (;it != tags.end(); ++it) {
      if (it->second == NotLinked ) {
        conn.get_io_service().post(bind(&VotlisMaster::link, this, it, std::ref(promis)));
        return;
      }
    }

    conn.get_io_service().post( bind(&VotlisMaster::readNew, this, std::ref(promis)) );
  }
  
  void link ( list<pair<TagConf, TagState>>::iterator p, am_sync & promis  )
  {
    //cout << "link\n";
    algo_state = Linking;
    
    if (p->first.acceptMethod() == TagConf::ByName ) {
      VotlisProtocol vp;

//cout <<" linking name: "<<p->first.remoteName()<<"\n";

      buffer = vp.linkItemByName(p->first.remoteName(), 0.000001, 0);
    
      conn.writeAsync(buffer, buffer.size(),  bind(&VotlisMaster::linkByNameWriteHandler, this, _1, p, std::ref(promis) ));
    }
    else if (p->first.acceptMethod() == TagConf::ById ) {
      //BOOST_ASSERT_MSG(0, "Link by id not implemented\n");
      assert(0);
    }
  }
  
  void linkByNameWriteHandler(bool ok, list<pair<TagConf, TagState>>::iterator it, am_sync& promis)
  {
    //cout<<"handle writeLink\n";
    
    if (!ok) {cerr<<__func__<<" ";onError(promis); return ;} 
    
    buffer.resize(sizeof(tag_id_t));
    conn.readAsync(buffer, buffer.size(), bind(&VotlisMaster::linkByNameReadHandler, this, _1, it, std::ref(promis)));
  }
  
  void linkByNameReadHandler (bool ok, list<pair<TagConf, TagState>>::iterator it, am_sync & promis)
  {
    //cout<<"handle readLink\n";
    
    if (!ok) {cerr<<__func__<<" ";onError(promis); return;}
    
    tag_id_t id = *reinterpret_cast<tag_id_t*>(&buffer[0]);
    
    //cout << "----------->GOT ID: "<<id<<"\n";
    
    if (id > -1) {
      it->first.setRemoteId(id);
      it->second = Linked; 
      ++linked_count;
      connectWriteEvent( it->first );
    }
    else {
      it->second = NotFound;
    }
    
    tryLink(++it, promis);
  }
  
  void connectWriteEvent(TagConf & tc)
  {
    auto func = [this, &tc](Tag& t, const string& n, const any& v)  {
      this->get_io_service().post(bind(&VotlisMaster::onWrite, this, std::ref(tc), std::ref(t), std::cref(n), std::cref(v)));
    };
    
    tc.tag()->connectPropChangeEvent( tc.remoteName(), "value", func, false, EventWriter );
  }
  
  void disconnectWriteEvent(const TagConf& tc )
  {
    const_cast<TagConf&>(tc).tag()->disconnectPropChangeEvent( tc.remoteName(), "value" );
  }
  
  void handleTimer(const boost::system::error_code& er)
  {
    //this method is called from timer thread
    if ( er )
      return;
      
    am_sync promis;
    
    conn.get_io_service().post(bind(&VotlisMaster::startLink, this, std::ref(promis)));
cout << "----------------------> wait 3\n";
    promis.wait();
cout << "----------------------> after wait 3\n";
  }
  
  void readNew (am_sync & promis)
  {
    cout << "readNew\n";
    
    timer.expires_from_now( boost::posix_time::milliseconds(period_) );
//    timer.async_wait( bind(&VotlisMaster::startLink, this, _1 ) );
    timer.async_wait( bind(&VotlisMaster::handleTimer, this, _1 ) );
    
    algo_state = ReadingNew;
    
    VotlisProtocol vp;
    
    buffer = vp.readNew();
    conn.writeAsync(buffer, buffer.size(), bind(&VotlisMaster::handleWriteReadNew, this, _1, std::ref(promis)) );
  }
  
  void handleWriteReadNew(bool ok, am_sync & promis)
  {
    if (!ok){cerr<<__func__<<" ";onError(promis); return ;}
    
    buffer.resize(sizeof(GIV));
    unsigned giv_counter = 0;
    conn.readAsync(buffer, buffer.size(), bind(&VotlisMaster::handleReadReadNew, this, _1, giv_counter, std::ref(promis)) );
  }
  
  void handleReadReadNew(bool ok, unsigned giv_counter, am_sync & promis)
  {
    if (!ok) {cerr<<__func__<<" ";onError(promis); return;}
    
    ++giv_counter;
    
    GIV g = *reinterpret_cast<GIV*>(&buffer[0]);
    
    //cout << "receive giv--->: "<<g.Ident<< " value: "<<g.Value<<"\n";
    
    for (auto t : tags ) {
      if (t.first.remoteId() == g.Ident) {
//        cout << "in votlis_rmt------->>>setValue: "<< g.Ident<<"\n";
        t.first.tag()->setProp( "value", value_t(g.Value), EventReader );
//        cout << "in votlis_rmt------->>>setValue: "<<"after\n";
      }
    }
    
    if (g.Ident == -1 || giv_counter == linked_count) {
      algo_state = None;
      promis.set();
      return;
    }
    
    conn.readAsync(buffer, buffer.size(), bind(&VotlisMaster::handleReadReadNew, this, _1, giv_counter, std::ref(promis)) );
  }
  
//  void str
  
  void onError (am_sync & promis)
  {
    cerr<< "ON ERROR CALLED!!!\n";
    timer.cancel();
    
    linked_count = 0;
    
    for ( pair<TagConf,TagState> & ptag : tags ) {
      ptag.second = NotLinked;
    }
    
    unlinkAll();
    
    promis.set();
    
    connect();
  }
  
  void startWrite(const TagConf& tg, const any& val)
  {
    //this method called from another thread
    am_sync promis;
    
    get_io_service().post(bind(&VotlisMaster::handleStartWrite, this, std::ref(tg), val, std::ref(promis)));
cout << "-----------------------> wait 4\n";
    promis.wait();
cout << "-----------------------> after wait 4\n";
  }
  
  void handleStartWrite(const TagConf& tg, const any& val, am_sync& promis)
  {
    VotlisProtocol vp;
    
    //cout << "handleStartWrite: id: "<<tg.remoteId() << " val: "<<static_cast<float>(any_cast<value_t>(val))<<"\n";
    
    buffer = vp.writeValue( tg.remoteId(), static_cast<float>(any_cast<value_t>(val))  );
    
    conn.writeAsync(buffer, buffer.size(), bind(&VotlisMaster::handleOnWrite, this, _1, std::ref(promis)));  
  }
  
  void onWrite (const TagConf& tg_conf, const Tag& tg, const string& , const any& val)
  {
      
    //cout << __func__<<" "<<"CALLED!: "<<tg.fullName()<<" value: "<< static_cast<float>(any_cast<value_t>(val))  << " algo_state: "<< algo_state <<"\n";

    VotlisProtocol vp;
    
    //buffer = vp.writeValue( tg.id(), static_cast<float>(any_cast<value_t>(val))  );
    
    //conn.writeAsync(buffer, buffer.size(), bind(&VotlisMaster::handleOnWrite, this, _1));
    //auto func = [&buffer](){ conn.writeAsync(bufer, buffer.size(), bind(&VotlisMaster::handleOnWrite, this, _1)) };
    
    timer_service.post(bind(&VotlisMaster::startWrite, this, std::ref(tg_conf), val));
  }
  
  void handleOnWrite(bool ok, am_sync & promis)
  {
    if (!ok) {cerr<<__func__<<" ";onError(promis);return;}
    
    //cout << __func__<<" "<<"CALLED\n";
    
    promis.set();
  }
};



#endif
