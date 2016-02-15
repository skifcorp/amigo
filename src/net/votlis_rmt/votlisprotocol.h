#ifndef __VOTLIS_PROTOCOL_H_
#define __VOTLIS_PROTOCOL_H_

#include "votlis.h"

#include <string>
#include <vector>

using std::string;
using std::vector;

class VotlisProtocol
{
public:
  VotlisProtocol ()
  {
  }
  ~VotlisProtocol ()
  {
  }
  vector<char> linkItemByName(const string& n, float dz, unsigned short dt) const
  {
    cout << "sizeof rd:"<<sizeof(RD)<<" len: "<<n.size()<<"sizeof(s_info) "<<sizeof(s_info)<<"\n";
    
    RD rd{LINK_ITEM_BY_NAME, n.size() + 1};

    vector<char> v(sizeof(RD) + rd.work + sizeof(s_info));

    *reinterpret_cast<RD*>(&v[0]) = rd;
    std::copy(n.begin(), n.end(), (v.begin() += sizeof(RD)));
    v[sizeof(RD) + n.size()] = 0;
    
    s_info si;
    si.DeadZone = dz;
    si.DeadTime = dt;
    si.res = 0;
    
    *reinterpret_cast<s_info*>(&v[sizeof (RD) + rd.work]) = si;
    return std::move(v);
  }
  vector<char> readNew () const
  {
    RD rd{READ_NEW, 0};
    vector<char> v(sizeof(RD));
    *reinterpret_cast<RD*>(&v[0]) = rd;
    return std::move(v);
  }
  vector<char> unlinkItem(tag_id_t id) const
  {
    RD rd{FREE_EXT_ITEM, id};
    vector<char> v(sizeof(RD));
    *reinterpret_cast<RD*>(&v[0]) = rd;
    return std::move(v);
  }
  vector<char> endSession() const
  {
    RD rd{END_SESSION, 0};
    vector<char> v(sizeof(RD));
    *reinterpret_cast<RD*>(&v[0]) = rd;
    return std::move(v);
  }
  
  vector<char> writeValue(tag_id_t id, float val ) const
  {
    RD rd{WRITE_BY_IDENT, id};
    vector<char> v(sizeof(RD)+sizeof(float));
    *reinterpret_cast<RD*>(&v[0]) = rd;
    *reinterpret_cast<float*>(&v[sizeof(RD)]) = val;
    
    return std::move(v);
  }
private:
  
};



#endif
