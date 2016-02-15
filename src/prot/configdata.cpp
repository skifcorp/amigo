#include "configdata.h"
#include <cmath>

void prot::Variable::reset ()
{
  clear();
}

void prot::Variable::clear ()
{
  values_.clear();
  scales_.clear();
}

void prot::Variable::appendValue (float v )
{
  values_.push_back ( make_tuple (second_clock::universal_time(), v) );
  
//  std::cout<<"id: "<<ident_<<" "<<second_clock::universal_time()<<" "<<v<<" cnt:"<<values_.size()<<" v: "<<v<<"\n";
}


void prot::Variable::initScale ( const tuple<float, float, bool>& s  )
{
  cur_scale = s;
  std::cout<<"initScale: "<<cur_scale.get<0>()<<" "<<cur_scale.get<1>()<<" initialized: "<< cur_scale.get<2>() << "\n";
}

void prot::Variable::checkScale ( float low, float high )
{
  if ( std::abs(low - cur_scale.get<0>()) > 0.01 || std::abs (high - cur_scale.get<1>()) > 0.01 || !cur_scale.get<2>() ) {
    scales_.push_back ( make_tuple(second_clock::universal_time(), low, high) );
    cur_scale.get<0>() = low;
    cur_scale.get<1>() = high;
    cur_scale.get<2>() = true;
    
    //std::cout<<"checkScale: "<<cur_scale.get<0>()<<" "<<cur_scale.get<1>()<< "\n";
  }
}

void prot::Variable::setReliability(int rel)
{
  if ( rel == 1 && rel != reliability_ ) {
    appendValue(NAN);
  }

  reliability_ = rel;
}
