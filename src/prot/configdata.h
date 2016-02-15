#ifndef __CONFIG_DATA_H_
#define __CONFIG_DATA_H_

#include "tag.h"

#include <string>
#include <vector>
#include <iostream>
#include <memory>

using std::string;
using std::vector;
using std::shared_ptr;

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/tuple/tuple.hpp>



using boost::tuple;
using namespace boost::posix_time;

namespace prot 
{
typedef tuple<string, string> VariableParams;
typedef tuple<ptime, float>   VariableValue;
typedef tuple<ptime, float, float> ScaleValue;

class Variable 
{
public:
  typedef shared_ptr<Variable> Pointer;
  
  Variable (const tags_t& t, const string& h, const string& n, const string& equi) : tags(t),
                       equipment_ (equi),
                       depth_(1000),
                       dead_zone (0.001f),
                       reliability_(-1),
                       cur_scale(0.0, 0.0, false)
  {
  }
  
  Variable(const tags_t& t) : tags(t), depth_(1000), dead_zone (0.001f), reliability_ (-1),
               cur_scale(0.0, 0.0, false)
  {
  }
  
  ~Variable ()
  {
  }
  
  Variable(const Variable& ) = delete;
  
/*  
  Variable (const Variable& o) : tags  (o.tags),
                                 tag_id (o.tag_id),
                                 equipment_ (o.equipment_),
                                 ru_name  (o.ru_name),
                                 depth_  (o.depth_),
                                 dead_zone  (o.dead_zone),
                                 reliability_ (o.reliability_),
                                 values_  (o.values_),
                                 scales_  (o.scales_),
                                 cur_scale  (o.cur_scale)
  {
  } */
  
  
/*  void setName (const string& s)
  {
    name_ = s;
  } */
  
  
  
  
  string name () const
  {
    return tags[tag_id]->fullName();
  }

  void setRuName (const string& s)
  {
    ru_name = s;
  }
  string ruName() const
  {
    return ru_name;
  }
  void setDepth ( int d )
  {
    depth_ = d;
  }
  int depth () const
  {
    return depth_;
  }
  void setDeadZone ( float dz )
  {
    dead_zone = dz;
  }
  float deadZone () const
  {
    return dead_zone;
  }
  void setEquipment ( const string& equi )
  {
    equipment_ = equi;
  }
  string equipment ( ) const 
  {
    return equipment_;
  }
  void reset ();
  void clear ();
  void appendValue (float v );
  void checkScale  (float, float);
  void initScale   (const tuple<float, float, bool>& );
  tuple< VariableParams, vector < VariableValue >, vector<ScaleValue> >values()                  //by copy!!!
  {
    //auto tags.find();
    return boost::make_tuple (  boost::make_tuple (equipment_, tags[tag_id]->fullName()), values_ , scales_ );
  }
  void setReliability (int);
  int reliability () 
  {
    return reliability_;
  }
  
  void setTagId(tag_id_t tid)
  {
    tag_id = tid;
  }
  
  tag_id_t tagId() const
  {
    return tag_id;
  }
  
  float curElr() const 
  {
    if ( !cur_scale.get<2>() )
      return 0.0;
    
    return cur_scale.get<0>();
  }
  float curEhr() const 
  {
    if ( !cur_scale.get<2>() )
      return 0.0;
    
    return cur_scale.get<1>();
  }
private:
  //string  name_;
  const tags_t & tags;
  tag_id_t tag_id;
  string  equipment_;
  string  ru_name;
  int     depth_;
  float   dead_zone;

  int     reliability_;

  vector< VariableValue > values_;
  vector< ScaleValue > scales_;
  tuple<float, float, bool> cur_scale;
};

typedef vector<Variable::Pointer> VariablesContainer;
}


#endif
