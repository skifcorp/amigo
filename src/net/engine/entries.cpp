#include "entries.h"

#include <cmath>

using std::fabs;

#include <boost/any.hpp>
#include <boost/lexical_cast.hpp>
using boost::any_cast;



float const PRECISION = 0.0001;

float entry::convert ( float inp, float min_inp, float max_inp, float min_out, float max_out ) const 
{
  float temp = max_inp - min_inp;
  if (fabsf (temp) < PRECISION)
    return min_out;
  else
    return (max_out - min_out) * (inp - min_inp) / temp + min_out;
}

float entry::safeConvert ( float inp, float min_inp, float max_inp, float min_out, float max_out ) const
{
  float temp = max_inp - min_inp;
  if (fabsf (temp) < PRECISION) {
    return min_out;
  }
  
  float out = (max_out - min_out) * (inp - min_inp) / temp + min_out;
  if (max_out > min_out) {
    if (out > max_out) return max_out;
    if (out < min_out) return min_out;
  }
  else {
    if (out > min_out) return min_out;
    if (out < max_out) return max_out;
  }
  
  return out;
}

value_t entry::convert(const value_t& v) const
{
  return convert(v, elr(), ehr(), ilr(), ihr());
}

value_t entry::safeConvert(const value_t& v) const
{  
  return safeConvert(v, ilr(), ihr(), elr(), ehr());
}


value_t entry::getTagValue (tag_id_t tid, bool * ok) const
{
  bool ok_; if (!ok) ok = &ok_;
  value_t ret; ret = 0.0f;
  
  Tag * t = tags[tid];
  *ok = t->lookup("value", ret);
  return ret;
}

value_t entry::getTagValueForScale(bool use, tag_id_t tid, value_t def_val) const
{
  if (!use || tid == badTagId() ) return def_val;
  
  bool ok = false;
  return getTagValue(tid, &ok);
}

value_t entry::ilr() const
{
  value_t ret = getTagValueForScale(use_inp_scales, inp_scale_min, ilr_);
  if (multip_inp_scales)
    ret = static_cast<float>(pow(10, static_cast<float>(ret)) * static_cast<float>(ilr_));
    
  return ret;
}

value_t entry::ihr() const
{
  value_t ret = getTagValueForScale(use_inp_scales, inp_scale_max, ihr_);
  
  if (multip_inp_scales)
    ret = static_cast<float>(pow(10, static_cast<float>(ret)) * static_cast<float>(ihr_));
  
  return ret;
}

value_t entry::elr() const
{
  return getTagValueForScale(use_var_scales, scale_min, elr_);
}

value_t entry::ehr() const
{
  return getTagValueForScale(use_var_scales, scale_max, ehr_);
}

/*
void entry::registerWriteFunc(std::function<void(const value_t&)> f)
{
  assert (tag_id != badTagId());
    
  (*tags[tag_id])["write"] = f;
    
//    cout <<"entry: "<< id  << " for tag_id "<<tag_id<<" registered write func\n";

}*/


/*
OneItem::OneItem ()
        :counter (0), TimeRel(1), offset(0), type(USINT),
        save(0), fix(0), ptrScaleMin(nullptr), ptrScaleMax(nullptr), use_var_scales(0), ptrInpScaleMin(nullptr), ptrInpScaleMax(nullptr), use_inp_scales(0),  multip_inp_scales(0)
{

  ILRange.fval = 0.0, IHRange.fval = 64000.0, ELRange.fval = 0.0, EHRange.fval = 100.0;

  InputValue.fval = 0.0;
}

void OneItem::writeValue(const value_t& val)
{
  moveValue(val);
  for (OneEntry * en = group()->firstChild();en;en=en->nextSibling()) {
    item * it = static_cast<OneItem *>(it);
    
    if (it->fix) {
      moveValue (it->InputValue);
    }
  }
  group()->device()->driver()->writeGroup( *group() );
  
  //++group()->ChangesCounter;
}
*/
/*
void OneItem::moveValue(const value_t& val)
{
  value_t v = val;
  try {
    v.fval = converts2(v.fval, ilr().fval, ihr().fval, elr().fval, ehr().fval);
  }
  catch (NotInitialized& ex) {  
    std::cerr<<ex.what()<<"\n"; 
    return;
  }
  

  checkBytesOrder ( reinterpret_cast<char *>(&v) );
  
  valueToRaw( val, type, &group()->device()->plc_area[offset] );
}*/
/*
value_t OneItem::value ( int * rel ) const
{
  value_t ret;
  ret.fval = 0.0;
  valueFromRaw ( &group()->device()->plc_area[offset], ret, type );
  //swapBytes(ret);
  checkBytesOrder ( reinterpret_cast<char *>(&ret) );
  try {
    ret.fval = converts(ret.fval, ilr().fval, ihr().fval, elr().fval, ehr().fval);
  }
  catch (NotInitialized & ex) {
    std::cerr<<ex.what() << "\n";
  }
  
  return ret;
}


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
}

void OneItem::valueFromRaw(const unsigned char * from, value_t & val, data_t typ) const
{
  switch (typ)
  {
  case USINT:
    val.fval = *reinterpret_cast<const unsigned char*>(from);
    break;
  case SINT:
    val.fval = *reinterpret_cast<const signed char*>(from);
    break;
  case UINT:
    val.fval = *reinterpret_cast<const unsigned short*>(from);
    break;
  case INT:
    val.fval = *reinterpret_cast<const signed short*>(from) ;
    break;
  case DINT:
    val.fval = *reinterpret_cast<const signed int*>(from);
    break;
  case UDINT:
    val.fval = *reinterpret_cast<const unsigned int*>(from);
    break;
  case REAL:
    val.fval = *reinterpret_cast<const float*>(from) ;
    break;
  }  
}
*/

/*
value_t OneItem::getScaleValue (bool use_scale, OneItem * itm, const value_t& def) const throw (NotInitialized)
{
  if (!use_scale)
    return def;
    
  if ( !itm->group()->initialized ) {
    throw NotInitialized ("item" + itm->fullName() + " group not initialized!");
    //return def;
  }
  
  int rel = -1;
  value_t ret = itm->value(&rel);
  

  return ret;
}

value_t OneItem::ilr() const  throw(NotInitialized)
{
  value_t ret = getScaleValue(use_inp_scales, ptrInpScaleMin, ILRange);
  if (multip_inp_scales)
    ret.fval = pow(10, (ret.fval)) * ILRange.fval;
    
  return ret;
}

value_t OneItem::ihr() const  throw(NotInitialized)
{
  value_t ret = getScaleValue(use_inp_scales,ptrInpScaleMax, IHRange);
  
  if (multip_inp_scales)
    ret.fval = pow(10, (ret.fval)) * IHRange.fval;
  
  return ret;
}

value_t OneItem::elr() const  throw(NotInitialized)
{
  return getScaleValue(use_var_scales, ptrScaleMin, ELRange);
}

value_t OneItem::ehr() const  throw(NotInitialized)
{
  return getScaleValue(use_var_scales, ptrScaleMax, EHRange);
}
*/

