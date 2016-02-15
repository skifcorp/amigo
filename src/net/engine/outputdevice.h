#ifndef __OUTPUT_DEVICE_H_
#define __OUTPUT_DEVICE_H_

#include <memory>
#include <string>
#include <map>

using std::shared_ptr;
using std::string;
using std::map;

typedef int device_id_t;

class OutputDevice : public map<string, string>
{
public:
  OutputDevice(device_id_t i):id_ (i) 
  {
  }
  ~OutputDevice(){}
  
  device_id_t id() const
  {
    return id_;
  }
private:
  device_id_t id_;
};

typedef vector<OutputDevice> OutputDevices_t;

class OutputDriver : public map<string, string>
{
public:
  OutputDriver(const string& p) : protocol_(p)
  {
  }
  
  ~OutputDriver()
  {
  }
  
  void setProtocol (const string& p)
  {
    protocol_ = p;
  }
  string protocol() const {return protocol_;}
  
  void setTransport (const string& t)
  {
    transport_ = t;
  }
  string transport() const {return transport_;}
  
  OutputDevices_t & devices() {return devices_;}
  const OutputDevices_t & devices() const {return devices_;}
private: 
  string protocol_;
  string transport_;

  OutputDevices_t devices_;
};

typedef vector<OutputDriver> OutputDrivers_t;

#endif
