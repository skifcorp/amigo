
#include "votlis.h"
#include "entries.h"
#include "tag.h"
#include "inputdriver.h"
#include "inputconnection.h"

#include <functional>
#include <thread>

#include <string>
#include <algorithm>
#include <chrono>
#include <cmath>
#include <deque>
#include <algorithm>
#include <cmath>

#include <boost/any.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>

using boost::asio::ip::tcp;
using boost::asio::io_service;
using boost::any_cast;

using std::bind;
using std::string;
using std::equal_range;
using std::chrono::milliseconds;
using std::function;
using std::chrono::monotonic_clock;
using std::fabs;
using std::deque;
using std::cerr;

typedef tag_id_t ident_t;



struct Subscribe
{
  Subscribe (Tag * t) : reliability (-1), tag (t)
  {
    value = 0.0f;
    deadZone = 0.0f;
  }
  
  milliseconds deadTime;
  value_t deadZone;
  int reliability;
  value_t value;
  monotonic_clock::time_point accessTime;
  
  bool checkDeadZone( )
  {
/*    auto iter = tag->find("value");
    
    if (iter == tag->end())  {
      //cerr << "votlis: \"value\" not found\n";
      return false;
    }
    
    value_t val = any_cast<value_t>( (*iter).second );*/
    value_t val = 0.0f;
    if ( !tag->lookup("value", val) ) return false;
    
    if ( fabsf(static_cast<float>(val) - static_cast<float>(value)) > static_cast<float>(deadZone) ) {
      value = val;
      return true;
    }
    
    return false;
  }

  bool checkRel ()
  {
/*    auto iter = tag->find("rel");
    
    if (iter == tag->end())  {
      cerr << "votlis: \"rel\" not found\n";
      return false;
    }
    
    short new_rel = any_cast<short>( (*iter).second );*/
    short new_rel = 0;
    if ( !tag->lookup("rel", new_rel) ) return false;
    
    if (reliability != new_rel ) {
      reliability = new_rel;
      return true;
    }

    return false;
  }

  bool checkDeadTime( )
  {
#if 0
    monotonic_clock::time_point now = monotonic_clock::now();
    if (now - accessTime > deadTime ) {
      accessTime = now;
      return true;
    }
#endif

    return false;
  }
  
  void updateAccessTime ()
  {
    accessTime = monotonic_clock::now();
  }
  
  bool checkNeedSend ()
  {
    return checkDeadZone() /*||  checkDeadTime() */ || checkRel();
  }
  
  Tag * tag;
};


class VotlisConnector : public InputDriver
{
public:
  VotlisConnector (InputDrivers & dr, tags_t& t):InputDriver (dr), tags(t)
  {
  }
  ~VotlisConnector()
  {
    cout << "Votlis connector deleted\n";
  }
private:
  typedef deque<Subscribe> Subscribers;
  Subscribers subscribers;

  tags_t & tags;

  bool checkSubscribersItem (const Tag * t) const
  {
    for (auto s : subscribers ) {
      if ( t == s.tag){
        cout << "\n\n\nchekckSubscribersItem FAILED!!!\n\n\n";
        return false;
      }
    }
    
    return true;
  }
  
  bool removeSubscribersItem (const Tag * t)
  {
    for (auto iter = subscribers.begin(); iter != subscribers.end(); ++iter) {
      cout << (*iter).tag->fullName() <<"\n";
      if ( (*iter).tag == t ){
        subscribers.erase (iter);
        return true;
      }
    }
    cout << "\n\n\nremoveSubscribersItem FAILED!!!\n\n\n";
    return false;
  }

  virtual void stop_()
  {
    cout << ">>>>>>>>>>>>STOPED!\n";
  }

  virtual void start_ (InputConnection::pointer c)
  {
    conn = c;
    read_buffer.resize( sizeof (RD) );

    readHelper();
  }

  void readHandler (bool ok)
  {
    if (!ok) {
      stop(); return;
    }

    if (read_buffer.empty()) { cerr<<"read buffer empty\n"; return;}
    
    const RD & rd = reinterpret_cast<const RD&>(read_buffer.front());
    
    switch (rd.command)
    {
    case GET_UNIT_IDENT_BY_NAME:
      read_buffer.resize(rd.work);
      conn->readAsync(read_buffer, rd.work, bind(&VotlisConnector::handleGetUnitItemByName, this, _1));   break;
    //case ACCEPT_ITEM:
      //acceptItem(static_cast<ident_t>(cmd.work) ); break;
    //case FREE_ITEM:
      //freeItem(static_cast<ident_t>(cmd.work) ); break;
    case FREE_EXT_ITEM:
      freeExtItem(static_cast<ident_t>(rd.work));
      break;
    case READ_BY_IDENT:
      readByIdent(static_cast<ident_t>(rd.work));
      break;
    case WRITE_BY_IDENT:
      read_buffer.resize(sizeof(float));
      conn->readAsync(read_buffer, sizeof(float), bind( &VotlisConnector::handleWriteByIdent, this, static_cast<ident_t>(rd.work), _1) ); 
      break;
    case END_SESSION:
      endSession (); 
      break;
    case READ_ALL:
      readAll (); 
      break;
    case READ_NEW:
      readNew (); 
      break;
    case LINK_ITEM_BY_NAME:
      read_buffer.resize(rd.work + sizeof(s_info));
      conn->readAsync( read_buffer, rd.work + sizeof(s_info), bind(&VotlisConnector::handleLinkItem, this, _1, static_cast<std::size_t>(rd.work)) ); 
      break;
    case BEGIN_SESSION:
      read_buffer.resize(rd.work);
      conn->readAsync(read_buffer, rd.work, bind(&VotlisConnector::beginSession, this, _1)); 
      break;
    default:
      std::cerr<<"Unrecognized command "<<std::hex<<rd.command<<" \n";
      break;
    }
  }

  void writeHandler (bool ok)
  {
    if (!ok) {
      stop(); return;
    }
    
    //read_buffer.resize(sizeof (RD));
    //readHelper();
  }
  
  vector<char> read_buffer;
  vector<char> write_buffer;
  
  InputConnection::pointer conn;
  
  void handleGetUnitItemByName(bool ok)
  {
    if (!ok) { 
      stop(); return;
    }
    
    string n(&read_buffer[0], read_buffer.size() - 1);
    
    auto iter = tags.find(n);
    if ( iter != tags.nameEnd() ) {
      sendHelper((*iter).second->id());
    }
    else {
      sendHelper(Tag::impossible());
    }
    readHelper();
  }
  
  void freeExtItem( ident_t id )
  {
    Tag *  t = tags[id];

    if (!removeSubscribersItem(t)) {
      std::cerr<<" item: "<<t->fullName() <<" removeSubscribersItem failed!!!\n"; return;
    }
    else {
      std::cout << "removing: "<<t->fullName()<<"\n";
    }
    
    t->decLink();

    sendHelper(static_cast<int>(1));
    readHelper();
  }
  
  void readByIdent( ident_t id )
  {
    Tag * t = tags[id];
    sendHelper( tagToGiv(t) );
    readHelper();
  }
  
  void handleWriteByIdent ( ident_t id, bool ok ) 
  {
    if (!ok) {
      stop(); return;
    }

    Tag * t = tags[id];
    if (!t) {
      cerr<<"can find tag with id: "<<id<<"\n"; return;
    }
    
    value_t v = *reinterpret_cast<float*>(&read_buffer[0]);

/*    auto iter = t->find("write");
    if ( iter == t->end() ) {
      cerr << "writeNotFound for ident: "<<id<<"\n";
    }
    else if ( (*iter).second.empty() ) {
      cerr << "writeFunction is empty!!!!\n";
    }
    else {
      //cout << "before call\n";
      auto f = any_cast<std::function<void(const value_t&)>>((*iter).second);
      f(v);
    } */
    
//    std::function<void(const value_t&)> f;
    t->setProp( "value", v, EventWriter );
//    else {
//      f(v);
//    }
    readHelper();
  }
  
  void endSession ()
  {
    cout << "end session\n";
    //readHelper();
    stop();
  }
  
  void readAll ()
  {
    for (auto iter = subscribers.begin(); iter != subscribers.end(); ++iter) {
      Tag  * t = (*iter).tag;
      
      (*iter).updateAccessTime();
      
      sendHelper( tagToGiv(t) );
    }
    readHelper();
  }

  void readNew ()
  {
cout << "readNew\n";
    std::size_t counter = 0;
    for(auto iter = subscribers.begin(); iter != subscribers.end(); ++iter) {
      Tag * t = (*iter).tag;
      if ( (*iter).checkNeedSend() ) {
        sendHelper( tagToGiv(t) );
        ++counter;
      }
    }
    
    if (counter < subscribers.size()) {
      sendHelper ( GIV {-1,-1, 0.0, 0.0, 0.0} );
    }
    
    readHelper();
  }

  void handleLinkItem (bool ok, std::size_t buffer_len)
  {
    if (!ok) {
      stop(); return;
    }

    string n(&read_buffer[0], buffer_len - 1);


    s_info si = *reinterpret_cast<s_info*>(&read_buffer[buffer_len]);

    cout<<"link: " << n << " len: "<<buffer_len<<" deadZone: "<<si.DeadZone<<" deadTime: "<<si.DeadTime<<"\n";

    auto iter = tags.find(n);
    if ( iter != tags.nameEnd() && checkSubscribersItem((*iter).second) ) {
      sendHelper((*iter).second->id());

      (*iter).second->incLink();                            //server accept :)

      subscribers.push_back( Subscribe( (*iter).second ) ); //connector accept :)
    }
    else {
cout << "send id: -2\n";
      sendHelper(-2);

    }
    
    readHelper();
  }
  
  void beginSession (bool ok)
  {
    cout << "beginSession: "; printBuffer (read_buffer);
    
    if (!ok) {
      stop(); return;
    }
    readHelper();
  }

  GIV tagToGiv (const Tag * t) const
  {
    short rel_ = -1; value_t val_ = 0.0f;
    function<value_t(void)> elr_, ehr_;
    
    if ( !t->lookup("rel", rel_) ) {
      cerr<<"cant lookup rel "<<t->fullName()<<"\n";
    }
    else if ( !t->lookup("value", val_)  ) {
      cerr<<"cant lookup value "<<t->fullName()<<"\n";
    }
    else if ( !t->lookup("scale_elr", elr_) ) {
      cerr<<"cant lookup scale_elr "<<t->fullName()<<"\n";
    }
    else if ( !t->lookup("scale_ehr", ehr_)  ) {
      cerr<<"cant lookup scale_ehr "<<t->fullName()<<"\n";
    }
    else {  //all ok
      return GIV{t->id(), rel_, val_,  elr_(), ehr_()};
    }
    
    //not all ok
    return GIV{t->id(), -1, 0.0, 0.0, 100.0};
  }
  
  template <class T>
  void sendHelper (const T& v)
  {
    write_buffer.resize(sizeof (T));
    *reinterpret_cast<T*>(&write_buffer[0]) = v;
    conn->writeAsync(write_buffer, bind(&VotlisConnector::writeHandler, this, _1) );
  }
  void readHelper()
  {
    read_buffer.resize(sizeof(RD));
    conn->readAsync( read_buffer, sizeof (RD), bind(&VotlisConnector::readHandler, this, _1) );
  }

  void printBuffer (const vector<char> & v) const
  {
    for (auto c : v) {
      cout << c;
    }
    cout<<"\n";
  }
};



extern "C" InputDriver::pointer createDriver (InputDrivers & dr, tags_t & tags)
{
  cout << "votlis input protocol->createDriver!!!\n";
  return InputDriver::pointer( new VotlisConnector(dr, tags) );
}
