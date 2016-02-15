#ifndef __LINKER_H_
#define __LINKER_H_

#include "votlismaster.h"
#include "outputethernetconnection.h"

#include <string>
#include <memory>
#include <vector>

using std::string;
using std::vector;
using std::shared_ptr;

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include "monotone_timer.hpp"

using boost::asio::io_service;
using boost::asio::monotone_timer;
using boost::bind;

class Linker
{
public:
  typedef shared_ptr<Linker> pointer;
  
  Linker(io_service & ios, const string& h, short p, unsigned prd):votlis_master(ios,h, p, prd)/*, period_(prd), timer(ios)*/
  {
  }
  
  Linker(const Linker& ) = delete;
  
  ~Linker(){}
  
  void linkFunc(const TagConf& t)
  { 
//    cout << "linkFunc from votlis_rmt: "<<t.remoteName()<<"\n";
//    auto tt = t;
    
    votlis_master.get_io_service().post(bind(&VotlisMaster::appendTag, &votlis_master, t));

/*    if ( timerIsStopped() ) {
      startTimer();
    } */
  }
  void unlinkFunc(const TagConf& t)
  {
    cout << "got the unlink Event!!!\n";
    votlis_master.get_io_service().post(bind(&VotlisMaster::removeTag, &votlis_master, t));
  }
  
  void stop ()
  {
    votlis_master.stopInternalService();
  }
/*  
  void run(const boost::system::error_code& e)
  {*/
    
/*    if (!e) {
      startTimer();
      votlis_master.exec();
    }
    else if ( e == boost::asio::error::operation_aborted ) {

    }
    else {
      cerr<<"error: "<<e.message()<<"\n"; assert(0);
    }*/
//  }
  
private:
/*  bool timerIsStopped() const
  {
    return timer.expires_at().m < monotone_timer::traits_type::now().m;
  }*/
/*  
  void startTimer()
  {
    timer.expires_from_now(boost::posix_time::milliseconds(period_));
    timer.async_wait(bind(&Linker::run, this, _1));
  }*/
  
  VotlisMaster votlis_master;
  //unsigned period_;
  //monotone_timer timer;
};

typedef vector<Linker::pointer> Linkers_t;


#endif
