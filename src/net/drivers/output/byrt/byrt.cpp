#include "entries.h"
#include "tag.h"
#include "outputdevice.h"
#include "outputconnection.h"
#include "amigofs.h"
#include "protocol.h"
#include "byrt.h"
#include "confparser.h"
#include "outputdriverbase.h"
#include "amsync.h"

#include <memory>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>

using std::queue;
using std::thread;
using std::shared_ptr;
using std::mutex;
using std::unique_lock;
using std::condition_variable;
using std::lock_guard;

/*
class am_sync
{
public:
  am_sync() : ready(false)
  {
  }
  
  am_sync(am_sync const& ) = delete;
  
  void wait()
  {
    unique_lock<mutex> lock(mutx);
    if ( !isReady() ) {
      cond.wait(lock);
    }
  }
  
  void set()
  {
    lock_guard<mutex> grd(mutx);
    ready = true;
    cond.notify_all();
  }
private:
  bool isReady () const {return ready;}
  bool ready;
  mutex mutx;
  condition_variable cond;
};

*/
#include <boost/bind.hpp>

using boost::bind;



class ByrtMaster : public OutputDriverBase
{
public:
  ByrtMaster(entries_t& ent, const tags_t& t, function<OutputDevice(device_id_t)> dev_func, OutputConnection::pointer c, io_service& ios ) 
    : entries(ent), tags(t),  get_device_func(dev_func),  conn(c), service(ios), frame_index(0), /*all_groups_count(0), */preparing_socket(false), connected(false),
    timer_work(timer_service)
  {
    ConfParser parser(tags);
    parser.parse(devices);
    
    start();
  }
  
  unsigned short currentFrameIndex() const
  {
    return frame_index;
  }

  unsigned short nextFrameIndex() 
  {
    return ++frame_index;
  }
  
/*  
  void OneItem::valueToRaw( const value_t& val, data_t typ,  unsigned char * to ) const
  {
    switch (typ)
    {
    case USINT:
     *reinterpret_cast<unsigned char*>(to) = val.fval;
     break;
    case SINT:
      *reinterpret_cast<signed char*>(to) = val.fval;
      break;
    case UINT:
      *reinterpret_cast<unsigned short*>(to) = val.fval;
      break;
    case INT:
      *reinterpret_cast<signed short*>(to) = val.fval;
      break;
    case DINT:
      *reinterpret_cast<signed int*>(to) = val.fval;
      break;
    case UDINT:
      *reinterpret_cast<unsigned int*>(to) = val.fval;
      break;
    case REAL:
      *reinterpret_cast<float*>(to) = val.fval;
      break;
    }
  }*/

  value_t valueFromRaw(const unsigned char * from, data_t typ) const
  {
    switch (typ)
    {
    case A_USINT:
      return static_cast<float>(*reinterpret_cast<const unsigned char*>(from));
    case A_SINT:
      return static_cast<float>(*reinterpret_cast<const signed char*>(from));
    case A_UINT:
      return static_cast<float>(*reinterpret_cast<const unsigned short*>(from));
    case A_INT:
      return static_cast<float>(*reinterpret_cast<const signed short*>(from)) ;
    case A_DINT:
      return static_cast<float>(*reinterpret_cast<const signed int*>(from));
    case A_UDINT:
      return static_cast<float>(*reinterpret_cast<const unsigned int*>(from));
    case A_REAL:
      //cout << "gotReal\n";
      return *reinterpret_cast<const float*>(from);
    }
    
    return value_t();
  }

  
  
  /*
  void onError (  ByrtDevices_t::iterator cur_dev  )
  {
    cerr<< "onError\n";
    
    conn->asyncPrepare( bind(&ByrtMaster::processDevice, this, cur_dev,  _1 ) );
  }
  
  void processDevice(ByrtDevices_t::iterator cur_dev, bool ok)
  {    
    if (!ok) {onError(cur_dev); return;}
  
    assert ( cur_dev != devices.end() );
  
    service.post( bind(&ByrtMaster::processGroup, this, cur_dev, cur_dev->begin(), true) );
  }
  */
  unsigned short groupLength(const ByrtGroup& gr) const
  {
    int counter = 0;
    for ( auto iter = gr.begin(); iter != gr.end(); ++iter ) {
      counter += data_t_length(entries[iter->entryId()]->type());
    }
    return counter;
  }

  vector<char> makeSendBuffer(const ByrtGroup& gr, bool read, unsigned short & grp_length)
  {
    grp_length = groupLength(gr);
  
    //vector<char> buffer( cmd_tcp_header_len + grp_length );
    vector<char> buffer( sizeof (TCP_CMD) );
    TCP_CMD cmd{1, read ? MustRead : MustWrite, grp_length, gr.offset(), 0, nextFrameIndex(), 0};
    if (!read) {
      //need to copy data from group to buffer
    }
    
    //cout << "makeSendBuffer: offset: "<< cmd.offset << " length: "<<cmd.length<<"\n";
    
    *reinterpret_cast<TCP_CMD*>(&buffer[0]) = cmd;
  
    return std::move(buffer);
  }

/*  void handleSendGroup(ByrtDevices_t::iterator cur_dev, ByrtDevice::iterator cur_gr, bool read, bool ok)
  {
    if (!ok) {onError(cur_dev); return;}
  
    const unsigned short frame_length = (read ? groupLength(*cur_gr) : 0 ) + cmd_tcp_header_len;
    recv_buffer.resize(frame_length);
    
    conn->readAsync(recv_buffer, frame_length, bind(&ByrtMaster::handleReadGroup, this, cur_dev, cur_gr, read, _1) );
  }*/
  
  bool checkAnswer (const TCP_CMD& cmd, const ByrtGroup& gr,  bool read ) const
  {
    if ( cmd.version != 1 ) {
      cerr << "bad version "<<(int)cmd.version<<"\n"; return false;
    }
    
    if ( cmd.counter != currentFrameIndex() ) {
      cerr << "bad frame index! counter: " << cmd.counter << " current: "<<currentFrameIndex()<<"\n";  return false;
    }
    
    if ( (cmd.cmd == MustRead && !read) || (cmd.cmd == MustWrite && read) ) {
      cerr << "frame.cmd and cur_cmd are different!\n"; return false;
    }
    
    if (gr.offset() != cmd.offset ) {
      cerr << "frame.offset and cur_offset are different!\n"; return false;
    }
    
    if ( (read && cmd.length != groupLength(gr) ) || (!read && cmd.length != 0) ) {
      cerr << "bad frame length: "<<cmd.length<<"\n" ; return false;
    }
    
    return true;
  }
  /*
  void nextDevGroup(ByrtDevices_t::iterator & cur_dev, ByrtDevice::iterator & cur_gr)
  {
    ++cur_gr;
    if ( cur_gr == cur_dev->end() ) {
      ++cur_dev;
      if ( cur_dev == devices.end() ) {
        cur_dev = devices.begin();
        cur_gr = cur_dev->begin();
      }
    }
  }
  */
  /*
  void handleReadGroup(ByrtDevices_t::iterator cur_dev, ByrtDevice::iterator cur_gr, bool read, bool ok)
  {
    if (!ok) {onError(cur_dev); return;}
    
    TCP_CMD cmd;
    
    cmd = *reinterpret_cast<TCP_CMD*>(&recv_buffer[0]);
    
    if (!checkAnswer(cmd, cur_gr, read)) {
      cerr<< "bad answer\n"; return;
    }
    
    if ( read ) {
      unsigned short gr_offset = 0;
      for (ByrtGroup::iterator itm = cur_gr->begin(); itm != cur_gr->end(); ++itm) {
        //value_t val = valueFromRaw(&cmd.buf[gr_offset], itm->type());
        entry * e = entries[itm->entryId()];
        
        value_t val = valueFromRaw(&cmd.buf[gr_offset], e->type());
        e->setValue(val);
        e->setRel(1);
        
        gr_offset += data_t_length(e->type());
      }
    }
    
    nextDevGroup(cur_dev, cur_gr);
    
    service.post(bind(&ByrtMaster::processGroup, this, cur_dev, cur_gr, true));
  }*/

/*
  void processGroup(ByrtDevices_t::iterator cur_dev, ByrtDevice::iterator cur_gr, bool ok)
  {
    cout << "processGroup CALLED!\n";
    
    if (!ok) {onError(cur_dev); return;}
  
    assert( cur_gr != cur_dev->end() );
    
    int counter = 0;
    while ( !cur_gr->isCyclicReading()  ) {
      nextDevGroup(cur_dev, cur_gr);
      if (counter >= all_groups_count) {
        //nothing found simply stop cyclic part;
        return;
      }
      ++counter;
    }
    
    //unsigned short grp_length = 
    vector<char> send_buffer = makeSendBuffer(*cur_gr, true);
  
    conn->writeAsync( send_buffer, cmd_tcp_header_len, bind(&ByrtMaster::handleSendGroup, this, cur_dev, cur_gr, true, _1) );
  }*/
  
  
  
  void start ()
  {
    timer_thread.reset( new thread( bind(&io_service::run, &timer_service) ) );
    
    for (ByrtDevices_t::iterator dev_iter = devices.begin(); dev_iter != devices.end(); ++dev_iter)  {
      for (ByrtDevice::iterator gr_iter = dev_iter->begin(); gr_iter != dev_iter->end(); ++gr_iter) {
        
        if ( gr_iter->readAlways() ) {
          startReadGroup(*gr_iter);
        }
        else {
          for (ByrtGroup::iterator itm_iter = gr_iter->begin(); itm_iter != gr_iter->end(); ++itm_iter) {
            entry * e = entries[itm_iter->entryId()];
            e->tag()->connectLinkEvent ("byrtmaster", bind(&ByrtMaster::startReadGroup, this, std::ref(*gr_iter)), true);
            e->tag()->connectUnlinkEvent("byrtmaster", bind(&ByrtMaster::stopReadGroup, this, std::ref(*gr_iter)));
          }
        }
      }
    }

    //conn->asyncPrepare( bind(&ByrtMaster::processDevice, this, devices.begin(), _1) );
  }
  
  void startReadGroup( ByrtGroup& gr )
  {
    //cout << "startReadGroup\n";
    
    gr.asyncWaitTimer( bind( &ByrtMaster::handleGroupTimer, this, _1, std::ref(gr), true ), timer_service );
  }
  
  void stopReadGroup(ByrtGroup& gr)
  {
    gr.stopTimer();
  }
  
  void handleGroupTimer (const boost::system::error_code& err, ByrtGroup& gr, bool read)
  {
    // this method called by timer from another thread
    
    if (err == boost::asio::error::operation_aborted ) return;                  //if timer stopped
    
    {
      am_sync promis;
      cout << "before post\n";
      service.post(bind(&ByrtMaster::dispatchFrame, this, std::ref(gr), read, std::ref(promis)));
      
      cout << "before block------------------------->>\n";
      promis.wait();
      cout << "after block------------------------->>\n";
    }
    
    if ( read )
      startReadGroup(gr);  //for cyclic reading
  }
  
  void writeGroup(ByrtGroup & gr)
  {
    timer_service.post(bind(&ByrtMaster::handleGroupTimer, this, boost::system::error_code(boost::system::errc::success, boost::system::get_system_category()), std::ref(gr), false));
  }
  
  void dispatchFrame( ByrtGroup & gr, bool read, am_sync& promis )
  {
    cout << "dispatch frame called\n";
    
    if ( conn->ready() && connected ) {
      cout << "connection is ready and connected!!!!\n";
      sendFrame(gr, read, promis);
    }
    else {
      prepareSocket();
      if ( read ) {
      
      }
      else {
        //append to queue to execute when connecting finished
      }
      
//      cout << "dispatchFrame - before releasePromise\n";
      //promis.set_value();  //releasing promise
      //service.post( bind(&ByrtMaster::releasePromise, this, std::ref(promis)) );
      cout << "before release promise in dispatch\n";
      releasePromise(promis);
      cout << "after release promise in dispatch\n";
//      cout << "dispatchFrame -after set_value()\n";
    }
  }

  void prepareSocket(  )
  {
    if ( !preparing_socket ) {
      cout << "prepareSocket\n";
      
      conn->asyncPrepare( bind(&ByrtMaster::handlePrepareSocket, this, _1) );
      preparing_socket = true;
    }
  }

  void handlePrepareSocket(bool ok)
  {
    cout << "handlePrepareSocket: res: "<<ok<<"\n";
    
    preparing_socket = false;
    connected  = ok;
    if ( ok ) {
      //sending write commands from queue
    }
    else {
      //clearing write commands queue
    }
  }

  
  void setGroupReliability(ByrtGroup & gr, short rel)
  {
    for (ByrtGroup::iterator iter = gr.begin(); iter != gr.end(); ++iter) {
      entries[iter->entryId()]->setRel(rel);
    }
  }
  
  void onSocketError(ByrtGroup& gr, bool read, am_sync & promis)
  {
    cerr << "onSocketError!!!!!\n\n\n";
    
    connected = false;
    setGroupReliability(gr, -1);
    if (read) {
      //startReadGroup(gr); //for cyclic reading and establishing  connection 
    }
    //promis.set_value();
    releasePromise(promis);
  }
  
  
  void sendFrame(ByrtGroup& gr, bool read, am_sync& promis)
  {
    unsigned short buf_len = 0;
    vector<char> send_buffer = makeSendBuffer(gr, read, buf_len);

    cout << "before writing2: ["<<std::hex;
    
    for (unsigned i = 0; i<cmd_tcp_header_len; ++i) { 
      cout << (unsigned short)(unsigned char)send_buffer[i] << " ";
    }
    
    cout << "]\n"<<std::dec;
    
    conn->writeAsync( send_buffer, cmd_tcp_header_len+(read?0:buf_len), bind(&ByrtMaster::handleSendGroup, this, std::ref(gr), read, _1, std::ref(promis)) );
  }

  void handleSendGroup(ByrtGroup & gr, bool read, bool ok, am_sync& promis)
  {
    if (!ok) {onSocketError(gr, read, promis); return;}
  
    const unsigned short frame_length = (read ? groupLength(gr) : 0 ) + cmd_tcp_header_len;
    recv_buffer.resize(frame_length);
    
    conn->readAsync(recv_buffer, frame_length, bind(&ByrtMaster::handleReadGroup, this, std::ref(gr), read, _1, std::ref(promis)) );
  }
  
  void handleReadGroup(ByrtGroup& gr, bool read, bool ok, am_sync& promis)
  {
    if (!ok) {onSocketError(gr, read, promis); return;}

    TCP_CMD cmd;
    
    cmd = *reinterpret_cast<TCP_CMD*>(&recv_buffer[0]);

    cout << "before reading2: ["<<std::hex;
    for (unsigned i = 0; i<cmd.length; ++i) { 
      cout << (unsigned short)(unsigned char)recv_buffer[i] << " ";
    }
    cout << "]\n"<<std::dec;

    
    if (!checkAnswer(cmd, gr, read)) {
      cerr<< "bad answer\n"; releasePromise(promis); return; //now returing but later I remove it
    }
    
    if ( read ) {
      unsigned short gr_offset = 0;
      for (ByrtGroup::iterator itm = gr.begin(); itm != gr.end(); ++itm) {
        entry * e = entries[itm->entryId()];
        
        value_t val = valueFromRaw(&cmd.buf[gr_offset], e->type());

        e->setValue(val);
        e->setRel(1);
        
        gr_offset += data_t_length(e->type());
      }
    }
    
    cout << "successfull operation!!!!\n";
    
    //promis.set_value();
    releasePromise(promis);
//    cout << "after set_value\n";
  }
  
//  void postReleasePromise()
//  {
//    
  //}
  void releasePromise(am_sync& promis)
  {
     cout << "before setValue\n";promis.set();cout << "after setValue\n";
  }
private:
  entries_t &                         entries;
  const tags_t&                       tags;
  function<OutputDevice(device_id_t)> get_device_func;
  OutputConnection::pointer           conn;
  io_service &                        service;
  ByrtDevices_t                       devices;

  unsigned char frame_index;
  vector<char> recv_buffer;
  //int all_groups_count;
  bool preparing_socket;
  bool connected;
  
  shared_ptr<thread> timer_thread;
  io_service timer_service;
  io_service::work timer_work;
  
  
  
//  mutex timer_mutex;
//  mutex dispatch_start_mutex;
//  condition_variable dispatch_start_cond;
  
  //mutext dispatch_start
};


extern "C" OutputDriverBase::pointer createDriver( entries_t & entries, const tags_t& tags, function<OutputDevice(device_id_t)> get_device_func, OutputConnection::pointer conn, io_service & ios )
{
  cout << "b.y.r.t. started\n";
  
  return OutputDriverBase::pointer(new ByrtMaster(entries, tags, get_device_func, conn, ios));
}

