#ifndef __ENTRIES_H_
#define __ENTRIES_H_


#include <iostream>
#include <vector>
#include <cassert>
#include <functional>

using std::cout;
using std::function;
using std::vector;
using std::function;
using std::cerr;

#include <boost/bind.hpp>
#include <boost/any.hpp>
#include <boost/asio.hpp>

using boost::bind;
using boost::any_cast;
using boost::asio::io_service;

#include "tag.h"

enum data_t
{
  A_USINT = 1,
  A_SINT,
  A_UINT,
  A_INT,
  A_DINT,
  A_UDINT,
  A_REAL
};

inline int data_t_length(data_t t)
{
  switch (t)
  {
  case A_USINT: case A_SINT:
    return 1;
  case A_UINT: case A_INT:
    return 2;
  case A_DINT: case A_UDINT:case A_REAL:
    return 4;
  }
  
  return 0;
}

class entry
{
public:
  entry (tags_t & t, io_service& ios) : tags(t),id(badTagId()), tag_id(badTagId()), scale_min(badTagId()), scale_max(badTagId()), inp_scale_min(badTagId()), inp_scale_max(badTagId()),
                         type_ (A_DINT), use_var_scales(false), use_inp_scales(false), multip_inp_scales(false), service (ios)
  {
  }
  
  ~entry()
  {
    Tag * t = tags[tag_id];

    t->disconnectPropChangeEvent("entries", "value");
    
    t->erase("value");
    t->erase("scale_elr");
    t->erase("scale_ehr");
    
    t->erase("rel");
  }
    
  entry(const entry& ) = delete;
  
  void setValue( const value_t& v )                      //<-- this must be called from driver on cyclic reading
  {
    assert (tag_id != badTagId());
    
    tags[tag_id]->setProp( "value", tryConvert(v), EventReader );
  }
  
  value_t value() const
  {
    value_t val(0.0f);
    
    if ( tags[tag_id]->lookup("value", val) ) {
      cerr<<"lookup for value failed: tag: "<<tags[tag_id]->fullName()<<"\n";
    }

    return val;
  }
  
  void setRel (short rel)
  {
    assert (tag_id != badTagId());
    
    tags[tag_id]->setProp("rel", rel, EventReader );
  }
  
  short rel() const
  {
    assert( tag_id != badTagId() );
    short rl = 0;
    if ( tags[tag_id]->lookup("rel", rl) ) ;
      return rl;
    return -1;
  }

  void registerWriteFunc(std::function<void(const value_t&)> drv_func)
  {
    assert (tag_id != badTagId());
    
    auto func = [&service, drv_func](Tag& , const string&, const any& aval)
      {
        value_t val = any_cast<value_t>(aval);
        service.post(bind(drv_func, val));
      };
    
    tags[tag_id]->connectPropChangeEvent("entries", "value", func, false, EventWriter);   //now we cant connect more than one writeFunc !!!!!!
  }
  
    
  value_t ilr() const;
  value_t ihr() const;
  value_t elr() const;
  value_t ehr() const;
  
  value_t scaleElr() const
  {
    return getTagValueForScale(true, scale_min, elr_);
  }
  value_t scaleEhr() const
  {
    return getTagValueForScale(true, scale_max, ehr_);
  }
  
  void setId (const tag_id_t& i) {id = i;}
  void setTagId (const tag_id_t& i) 
  {
    tag_id = i;
    function<value_t(void)> elrf = bind(&entry::scaleElr, this);
    function<value_t(void)> ehrf = bind(&entry::scaleEhr, this);
    
    tags[tag_id]->setProp( "scale_elr", elrf, EventNone );
    tags[tag_id]->setProp( "scale_ehr", ehrf, EventNone );
  }
  
  void setIlr(const value_t& v) {ilr_ = v;}
  void setIhr(const value_t& v) {ihr_ = v;}
  void setElr(const value_t& v) {elr_ = v;}
  void setEhr(const value_t& v) {ehr_ = v;}
  
  void processScaleChanges(Tag& , const string&, const any&)
  {
    setValue( value()  ); //scales changed so value also changed
  }
  
  void setScaleMin(tag_id_t v) 
  {
    if (scale_min != badTagId()) 
      tags[scale_min]->disconnectPropChangeEvent("entries_scale", "value" );
    
    scale_min = v;
    
    tags[scale_min]->connectPropChangeEvent("entries_scale", "value", bind(&entry::processScaleChanges, this, _1, _2, _3), true, EventReader);
  }
  
  void setScaleMax(tag_id_t v) 
  {
    if (scale_max != badTagId()) 
      tags[scale_max]->disconnectPropChangeEvent("entries_scale", "value" );

    scale_max = v;

    tags[scale_max]->connectPropChangeEvent("entries_scale", "value", bind(&entry::processScaleChanges, this, _1, _2, _3), true, EventReader); 
  }
  
  void setInpScaleMin(tag_id_t v) 
  {
    if (inp_scale_min != badTagId()) 
      tags[inp_scale_min]->disconnectPropChangeEvent("entries_scale", "value" );

    inp_scale_min = v; 
    
    tags[inp_scale_min]->connectPropChangeEvent("entries_scale", "value", bind(&entry::processScaleChanges, this, _1, _2, _3), true, EventReader);
  }
  
  void setInpScaleMax(tag_id_t v)
  {
    if (inp_scale_max != badTagId()) 
      tags[inp_scale_max]->disconnectPropChangeEvent("entries_scale", "value" );

    inp_scale_max = v;
    
    tags[inp_scale_max]->connectPropChangeEvent("entries_scale", "value", bind(&entry::processScaleChanges, this, _1, _2, _3), true, EventReader);
  }
  
  void setType (const data_t& dt ) {type_ = dt;}
  data_t type() const {return type_;}
  void setUseVarScales (bool u){use_var_scales = u;}
  void setUseInpScales (bool u){use_inp_scales = u;}
  void setMultipInpScales (bool m){multip_inp_scales = m;}
  
  const Tag * tag() const
  {
    return tags[tag_id];
  }
  Tag * tag()
  {
    return tags[tag_id];
  }
private:
  value_t tryConvert (const value_t& v) const
  {
//    cout << "in try convert\n";
    
    return !(ilr_.init()||ihr_.init()||elr_.init()||ehr_.init()) ? v : convert(v);
    
    
  }
  value_t convert(const value_t& ) const ;
  value_t safeConvert(const value_t& ) const;
  
  float convert (float, float, float, float, float) const ;
  float safeConvert(float, float, float, float, float) const ;
  
  value_t getTagValue(tag_id_t , bool * ok) const;
  value_t getTagValueForScale(bool , tag_id_t, value_t def_val) const;
  
  tag_id_t badTagId() const {return -1;}
  
  tags_t & tags;
  
  tag_id_t id;
  tag_id_t tag_id;
  
  tag_id_t scale_min, scale_max, inp_scale_min, inp_scale_max;
  
  data_t type_;
  value_t ilr_, ihr_, elr_, ehr_;

  bool use_var_scales;
  bool use_inp_scales;
  bool multip_inp_scales;
  io_service & service;
};

class entries_t : public vector<entry *>
{
public:
  entries_t()
  {
    //reserve (s);
  }
  ~entries_t()
  {

    
    for (auto p : *this)
      delete p;
    clear();
    cout << "entries deleted\n";
  }
  
  entries_t(const entries_t& other) = delete;
};

/*class OneDriver : public OneEntry
{
public:
  typedef shared_ptr<OneDriver> Pointer;
  
  OneDriver () : dr_work (nullptr)
  {}
  
  int (* dr_work)(group *, int);

  char use_custom_cnt;
  
  function<void(group& )> writeGroup;
};*/



/*
class OneDevice : public OneEntry
{
public:
  typedef shared_ptr<OneDevice> Pointer;
  
  enum ProcEndian {
    LittleEndian   = 0,
    BigEndian      = 1,
    Pdp11Endian    = 2
  };

  OneDevice () : mode(0), proc_type (BigEndian), pInfo(0), plc_area (0)
  {}

  char mode;
  ProcEndian proc_type;
  void *pInfo;
  unsigned char *plc_area;

  OneDriver * driver ()
  {
    return static_cast<OneDriver*>(parent());
  }

  const OneDriver * driver () const
  {
    return static_cast<const OneDriver*>(parent());
  }
};
*/

/*
inline int data_t_length(data_t t)
{
  switch (t)
  {
  case USINT: case SINT:
    return 1;
  case UINT: case INT:
    return 2;
  case DINT: case UDINT:case REAL:
    return 4;
  }
  
  return 0;
}*/

/*
class OneGroup : public OneEntry
{
public:
  typedef shared_ptr<OneGroup> Pointer;
  
  OneGroup ();
  OneGroup (const OneGroup& ) = delete;
  ~OneGroup ();
  
  unsigned int period;
  int status; 
  unsigned char read_cmd;
  unsigned char write_cmd;
  int startingDelay;
  int currentDelay;
  unsigned char readAlways;
  unsigned char initialized;
  char driver_state;
  
  OneDevice * device () 
  {
    return static_cast<OneDevice*>(parent());
  }

  const OneDevice * device () const
  {
    return static_cast<const OneDevice*>(parent());
  }
  template <class T1, class T2>
  void memoryGeometry (T1& offs, T2& len) const;
  
  void setSmallestOffsetItem (OneItem * itm)
  {
    smallest_offset_item_ = itm;
  }
  
  void setBiggestOffsetItem (OneItem * itm)
  {
    biggest_offset_item_ = itm;
  }

  inline void incClient() {++clients_;}
  inline void decClient() {--clients_;};
  int clients() const 
  {
    return clients_.load();
  }
private:
  OneItem * smallest_offset_item_;
  OneItem * biggest_offset_item_;
  atomic_int clients_;
};
*/


/*

class NotInitialized : public std::runtime_error 
{
public:
  NotInitialized (const string& txt) : std::runtime_error (txt){}
};

class BadReliability : public std::runtime_error
{
public:
  BadReliability (const string& txt) : std::runtime_error (txt){}
};
*/

/*
class OneItem : public OneEntry
{
public:
  typedef shared_ptr<OneItem> Pointer;
  
  OneItem ();
  OneItem (const OneItem & ) = delete;
  unsigned short counter;
  value_t ILRange;
  value_t IHRange;
  value_t ELRange;
  value_t EHRange;
  value_t InputValue;
  time_t TimeRel;
  unsigned int offset;
  data_t type;
  char save;
  char fix;
  
  string scaleMinName;
  string scaleMaxName;
  item * ptrScaleMin;
  item * ptrScaleMax;
  bool use_var_scales;
  item * ptrInpScaleMin;
  item * ptrInpScaleMax;
  string inpScaleMinName;
  string inpScaleMaxName;
  bool use_inp_scales;  
  bool multip_inp_scales;
  
  OneGroup * group () 
  {
    return static_cast<OneGroup *>(parent());
  }

  const OneGroup * group () const 
  {
    return static_cast<const OneGroup *>(parent());
  }
  
  value_t value(int * rel = 0) const;
  void writeValue (const value_t& );
  void moveValue (const value_t& );
  
  value_t ilr() const throw(NotInitialized);
  value_t ihr() const throw(NotInitialized);
  value_t elr() const throw(NotInitialized);
  value_t ehr() const throw(NotInitialized);
  void incLink() 
  {
    if (links == 0)
      group()->incClient();
    ++links;
  }
  
  void decLink()
  {
    if (links > 0) {
      if (links == 1) {
        group()->decClient();
      } 
      --links;
    }
    else {
      std::cerr<< " somebody try to decrease links wich is 0 already, item: "<<fullName()<<"\n";
    }
  }
  short reliability () const 
  {
    return group()->driver_state;
  }
private:
  void valueToRaw (const value_t &, data_t, unsigned char * to ) const;
  void valueFromRaw (const unsigned char * from , value_t & to, data_t ) const;
  void checkBytesOrder (char * v) const
  {
    switch (group()->device()->proc_type)
    {
    case OneDevice::LittleEndian:
      swapLittleBigEndian(v, data_t_length(type));
    break;
    case OneDevice::BigEndian:
    break;
    case OneDevice::Pdp11Endian:
      swapPdp11Endian(v, data_t_length(type));
    break;
    }
  }
  void swapLittleBigEndian (char * v, int size) const
  {
    assert (size == 2 || size == 4 || size == 1);
    
    for (int i = 4-size; i<size/2; ++i) {
      char t = v[i]; v[i] = v[size-1-i];
      v[size-1-i] = t;
    }
  }
  
  void swapPdp11Endian (char * v, int size) const
  {
    assert (size == 2 || size == 4 || size == 1);
    
    for (int i = 0; i<size; i+=2) {
      char v1 = v[i+1];v[i+1]=v[i];
      v[i] = v1;
    }
  }
  
  value_t getScaleValue (bool, OneItem * , const value_t& def ) const throw(NotInitialized);
  atomic_int links;
};
*/
/*
template <class T1, class T2>
inline void OneGroup::memoryGeometry (T1& offs, T2& len) const
{
  offs = smallest_offset_item_->offset;
  len = biggest_offset_item_->offset - offs + data_t_length(biggest_offset_item_->type);
}*/
/*
typedef enum
{
 MUST_WRITE=0,
 MUST_READ
} MainNetCommands;
*/

#endif
