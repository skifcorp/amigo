#ifndef __TAG_CONF_H_
#define __TAG_CONF_H_

//#include "linker.h"
#include "tag.h"


#include <string>
#include <list>

using std::string;
using std::list;

#include <boost/optional.hpp>

using boost::optional;

class Linker;

class TagConf
{
public:
  enum AcceptMethod{ById, ByName};
  
  TagConf () : accept_method(ById), tag_(nullptr)
  {
  }
  ~TagConf()
  {
  }
  void setAcceptMethod (AcceptMethod am) 
  {
    accept_method = am;
  }
  AcceptMethod acceptMethod () const 
  {
    return accept_method;
  }
  void assignTag(tags_t& tags, const string& tn)
  {
    auto iter = tags.find(tn);
    assert (iter != tags.nameEnd());
    tag_ = (*iter).second;
  }
  void setRemoteName (const string& tn)
  {
    remote_tag_name = tn;
  }
  string remoteName () const 
  {
    if (remote_tag_name)
      return remote_tag_name.get();

    return tag_->fullName();
  }
  
  void setRemoteId (tag_id_t id)
  {
    remote_id = id;
  }
  tag_id_t remoteId () const
  {
    if (remote_id)
      return remote_id.get();
    return tag_->id();
  }
  void connect(Linker& );
  
  void disconnect ()
  {
    assert(tag_);
    
    tag_->disconnectLinkEvent("votlis_rmt");
    tag_->disconnectUnlinkEvent("votlis_rmt");
  }
  
  void checkExistingConnections(Linker & );
  
  bool operator==(const TagConf& other) const
  {
    return other.tag_ == tag_;
  }
  Tag * tag() {return tag_;}
  const Tag * tag() const {return tag_;}
private:
  optional<string>   remote_tag_name;
  optional<tag_id_t> remote_id;
  
  AcceptMethod accept_method;

  Tag * tag_;
};

typedef list<TagConf> TagConfs_t;

#endif
