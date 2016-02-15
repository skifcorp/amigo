#ifndef __CONNECTION_INFORMATION_H_
#define __CONNECTION_INFORMATION_H_

#include <string>

using std::string;

#include "org/openscada/opc/lib/common/ConnectionInformation.h"


#include "tools.h"

class ConnectionInformation
{
public:
  ConnectionInformation () 
  {
    connection_information = new org::openscada::opc::lib::common::ConnectionInformation();
  }
  ConnectionInformation (const string& user, const string& password)
  {
    connection_information = new org::openscada::opc::lib::common::ConnectionInformation(string_to_jstring(user), string_to_jstring(password));
  }
  ConnectionInformation(const ConnectionInformation& other)
  {
    if(this == &other) return;
    
    connection_information = new org::openscada::opc::lib::common::ConnectionInformation(other.connection_information);
  }
  ConnectionInformation (org::openscada::opc::lib::common::ConnectionInformation * conn) : connection_information (conn)
  {
  }
  
  string getDomain()
  {
    return jstring_to_string (connection_information->getDomain());
  }
  void setDomain(const string& s)
  {
    connection_information->setDomain(string_to_jstring(s));
  }
  string getHost()
  {
    return jstring_to_string(connection_information->getHost());
  }
  void setHost(const string& s)
  {
    connection_information->setHost(string_to_jstring(s));
  }
  string getPassword()
  {
    return jstring_to_string(connection_information->getPassword());
  }
  void setPassword(const string& s)
  {
    connection_information->setPassword(string_to_jstring(s));
  }
  string getUser()
  {
    return jstring_to_string(connection_information->getUser());
  }
  void setUser(const string& s)
  {
    connection_information->setUser(string_to_jstring(s));
  }
  string getClsid()
  {
    return jstring_to_string(connection_information->getClsid());
  }
  void setClsid(const string& s)
  {
    connection_information->setClsid(string_to_jstring(s));
  }
  string getProgId()
  {
    return jstring_to_string(connection_information->getProgId());
  }
  void setProgId(string& s)
  {
    connection_information->setProgId(string_to_jstring(s));
  }
  string getClsOrProgId()
  {
    return jstring_to_string(connection_information->getClsOrProgId());
  }
  
  org::openscada::opc::lib::common::ConnectionInformation * javaObject() {return connection_information;}
  const org::openscada::opc::lib::common::ConnectionInformation * javaObject() const {return connection_information;}
private:
  org::openscada::opc::lib::common::ConnectionInformation * connection_information;
};


#endif
