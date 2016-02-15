#include "entries.h"
#include "outputdevice.h"
#include "amigofs.h"
#include "outputconnection.h"
#include "outputdriverbase.h"

#include <iostream>
#include <functional>
#include <string>
#include <fstream>
#include <memory>


using std::cout;
using std::function;
using std::string;
using std::fstream;
using std::shared_ptr;
using std::ios_base;

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/bind.hpp>
#include <boost/asio.hpp>

using boost::property_tree::ptree;
using boost::property_tree::read_xml;
using boost::bind;
using boost::asio::io_service;

class SwapEntry
{
public:
  SwapEntry (tag_id_t i, int o) : id_(i), offset_ (o)
  {
  }

  ~SwapEntry ()
  {
  }
  
  tag_id_t id() const
  {
    return id_;
  }
  int offset() const
  {
    return offset_;
  }
private:
  tag_id_t id_;
  int offset_;
  static int counter;
};



typedef vector<SwapEntry> device_entries_t;

class SwapDevice
{
public:
  SwapDevice(device_id_t i, const string& fn) : id_(i), file_name(fn)
  {
  }
  ~SwapDevice()
  {
  }
  
  device_id_t id() const
  {
    return id_;
  }
  
  string fileName () const
  {
    return file_name;
  }
  
  device_entries_t & entries() {return entries_;}
  const device_entries_t & entries() const {return entries_;}
private:
  device_id_t id_;
  string file_name;
  //OutputDevice & output_device;

  device_entries_t entries_;
};

typedef vector<SwapDevice> SwapDevices_t;

class ConfParser
{
public:
  ConfParser (const tags_t& t):tags(t)
  {
    
  }
  ~ConfParser()
  {
  }

  void parse (SwapDevices_t & devs)
  {
    ptree tree;
    read_xml(tags.projectFolder() + a::fs::slash + "swap.xml", tree);
          
    for (auto file : tree) {
      //cout << "file->"<<file.first;

      SwapDevice dev( file.second.get<device_id_t>("<xmlattr>.dev_id"), file.second.get<string>("<xmlattr>.name") );
          
      for ( auto en : file.second) {
        if (en.first == "<xmlattr>") {
          
        }
        else {
          SwapEntry ent(en.second.get<tag_id_t>("<xmlattr>.ent_id"), en.second.get<int>("<xmlattr>.offset") );
          dev.entries().push_back(ent);
        }
      }
          
      devs.push_back(dev);
    }
  }
private:
  const tags_t& tags;
};

template <class T> inline void readOneValue(fstream& strm, entry& e)
{
  T val; 
  strm.read(reinterpret_cast<char *> (&val), sizeof(val)); 


  e.setValue(static_cast<float>(val));


  e.setRel(1);

}

template <class T> inline void writeOneValue(fstream& strm, const value_t& v)
{
//  cout << "writeOneValue: " << static_cast<float>(v);
  

  //strm<<static_cast<T>(static_cast<float>(v));
  T val = static_cast<T>(static_cast<float>(v));



  strm.write(reinterpret_cast<char *> (&val), sizeof(val));
}

void writeValue (const value_t& v, SwapEntry e, const string& path, data_t typ)
{
  fstream strm;
  strm.open(path, fstream::out | fstream::binary | fstream::in);
  
//cout <<"s.w.a.p. write: "<<static_cast<float>(v)<<" offset: "<<e.offset()<<"\n";
  
  strm.seekp( e.offset(), ios_base::beg );
  
  //strm<<static_cast<float>(v);
  
  switch (typ) {
  case A_USINT: writeOneValue<unsigned char>(strm, v); break;
  case A_SINT: writeOneValue<char>(strm, v); break;
  case A_UINT: writeOneValue<unsigned short>(strm, v); break;
  case A_INT: writeOneValue<short>(strm, v); break;
  case A_UDINT: writeOneValue<unsigned>(strm, v); break;
  case A_DINT: writeOneValue<int>(strm, v); break;
  case A_REAL: writeOneValue<float>(strm, v); break;
  }
  
  strm.close();
}


extern "C" OutputDriverBase::pointer createDriver( entries_t & entries, const tags_t& tags, function<OutputDevice(device_id_t)> get_device_func, OutputConnection::pointer, io_service &  )
{
  cout << "s.w.a.p. started\n";

  SwapDevices_t swap_devices;
  {
    ConfParser parser(tags);
    parser.parse(swap_devices);
  }


  
  for ( auto swd : swap_devices ) {
    OutputDevice od = get_device_func( swd.id() );//dont use now
    
    fstream strm;
    string path(tags.projectFolder() + a::fs::slash + swd.fileName());
    strm.open(path, fstream::in);

    for ( auto ent : swd.entries() ) {
      strm.seekg(ent.offset());

      entry * e = entries[ent.id()];


      switch (e->type()) {
      case A_USINT: readOneValue<unsigned char>(strm, *e); break;
      case A_SINT:  readOneValue<char>(strm, *e); break;
      case A_UINT:  readOneValue<unsigned short>(strm, *e); break;
      case A_INT:   readOneValue<short>(strm, *e); break;
      case A_UDINT: readOneValue<unsigned>(strm, *e); break;
      case A_DINT:  readOneValue<int>(strm, *e); break;
      case A_REAL:  readOneValue<float>(strm, *e); break;
      };

      e->registerWriteFunc( bind(writeValue, _1, ent, path, e->type()) );
    }
    
    strm.close();
  }
  
  return OutputDriverBase::pointer(); //empty now
}

