#ifndef __SAVER_H_
#define __SAVER_H_

#include "configdata.h"
#include "tag.h"

#include <string>
#include <vector>
#include <thread>
#include <atomic>

using std::atomic_bool;
using std::string;
using std::vector;
using std::thread;

#include <boost/bind.hpp>
#include "monotone_timer.hpp"
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

using boost::tuple;
using boost::posix_time::seconds;
using boost::asio::io_service;
using boost::asio::monotone_timer;
using boost::bind;

template <  class T >
class Disconnector
{
public:
  Disconnector (T & t) : dis(t)
  {
  }
  
  ~Disconnector ()
  {
    dis.disconnect();
  }
private:
  T & dis;
};

namespace prot 
{

template <class Driver>
class BaseSaver
{
public:
  typedef Driver DriverType;
  typedef tuple < VariableParams, vector<VariableValue>, vector<ScaleValue> > OneVariableData;
 
  BaseSaver(io_service & ios, tags_t& t, VariablesContainer& v) : driver_ (), timer(ios), tags(t), variables(v), saving_now(false)
  {
    timer.expires_from_now( seconds(20) );
    timer.async_wait( bind(&BaseSaver<Driver>::handleSaveTimer, this, _1) );
  }
  
  ~BaseSaver()
  {
    
  }
  
  /*
  tuple <float, float, bool> getScale ( const string& equi, const string& var_name )
  {
    Disconnector<Driver> dis (driver_);
    driver_.connect ( "127.0.0.1", "operator", "" );
    
    return driver_.getScale (equi, var_name);
  }*/
  
private:
  Driver driver_;
  monotone_timer timer;
  tags_t & tags;
  VariablesContainer & variables;
  atomic_bool saving_now;
  
  void handleSaveTimer(const boost::system::error_code & ec)
  {
    if ( !ec ) {                                           //if cancelled then exit
      timer.expires_from_now(seconds(20));
      timer.async_wait(bind(&BaseSaver<Driver>::handleSaveTimer, this, _1));
    }
    
    if ( !saving_now.load() ) {
      saving_now.store(true);
      vector < OneVariableData > data_to_save;
      data_to_save.reserve ( variables.size() );
  
      for ( auto v : variables ) {
        data_to_save.push_back ( std::move(v->values()) ); v->clear();
      }

      thread save_thr ( bind(&BaseSaver<DriverType>::doSave, this, data_to_save ) );
      save_thr.detach();
    }
    else {
      cout << "want storing but previous storing not finished yet ... so will retry in 20 seconds\n";
    }
  }

  void doSave( vector<OneVariableData> data_to_save )
  {
    Disconnector<Driver> dis(driver_);
    driver_.connect ();

    for (  auto iter = data_to_save.begin(); iter != data_to_save.end(); ++iter ) {
      OneVariableData & ovd = *iter;
      driver_.save ( ovd.get<0>(), ovd.get<1>(), ovd.get<2>() );
    }
    saving_now.store(false);
  }

};

}

#endif
