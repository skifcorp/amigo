
#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/optional/optional.hpp>

#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <map>
#include <list>
#include <cassert>
//#include <pair>

using std::cout;
using std::string;
using std::fstream;
using std::vector;
using std::pair;
using std::map;
using std::make_pair;
using std::list;

using boost::property_tree::ptree;
using boost::property_tree::xml_writer_settings;
using boost::algorithm::split;
using boost::trim;
using boost::trim_right_if;
using boost::lexical_cast;
using boost::optional;

namespace fs = boost::filesystem;

typedef vector<pair<string, string>> keyval_t;


typedef vector<string> name_by_id_t;
typedef map<string, size_t> id_by_name_t;

class conf_parser
{
public:
  conf_parser()
  {
  }
  virtual ~conf_parser()
  {
  }
  
  void removeSemicolon (string& s)
  {
    trim_right_if( s, boost::is_any_of(";") );
  }

  void trimVector(vector<string>& v)
  {
    for ( std::size_t i = 0; i<v.size(); ++i) {
      trim (v[i]);
    }
  }

  void parseProps ( const string& s, keyval_t& k  ) const
  {
    vector<string> keyvals;
    split (keyvals, s, boost::is_any_of(" "));
    
    if (keyvals.size() == 0) return;
    
    for (auto keyval : keyvals) {
      vector<string> kv;
      split(kv, keyval, boost::is_any_of("="));
      k.push_back( std::make_pair(kv[0], kv[1]) );
    }
  }

  string propValue ( const string& s, const string& prop_name ) const
  {
    keyval_t kv;
    parseProps (s, kv);
    
    for ( auto v : kv) {
      if (v.first == prop_name) return v.second;
    }

    return string();
  }

  ptree findByAtt (const ptree& par, const string& att_name, const string& att_value)
  {
    for (auto n : par) {
      for (auto a : n.second) {
        if (a.first == "<xmlattr>") {
          optional<ptree&> ret = a.second.get_child_optional (att_name);
          if ( ret && (*ret).data() == att_value) {
            return n.second;
          }
        }
      }
    }
    
    return ptree();
  }
  
};

class unit_conf_parser : public conf_parser
{
public:
  unit_conf_parser() : cur_depth (0), was_internal(false) {}
  ~unit_conf_parser() 
  {
    if ( stream.is_open() )
      stream.close();
  }
  
  void parse (const string& s)
  {
    stream.open ( s, fstream::in );
    string l;
    
    while ( !stream.eof() ) {
      std::getline (stream, l);
      parseLine(l);
    }
    
    finish();
    
    stream.close();
  }

  void parseLine( string& s )
  {
    removeSemicolon(s);
    
    vector<string> v;
    split(v, s, boost::is_any_of(","));

    trimVector(v);

    if (v.size () == 0) return;

    fillCurItem (v[2], v[1]);
    
    trim(v[5]);

    if ( curDepth() == 1 ) {
      was_internal = (propValue(v[5], "mode") == "internal");
    }
    
/*    if (was_internal && curDepth() > 0) {
      return;
    }*/
    
    processLine(v);
  }

  void fillCurItem (const string& typ, const string& nam)
  {
    if (typ == "ITEM") {
      cur_depth = 3; item_name = nam;
    }
    else if (typ == "GROUP") {
      cur_depth = 2; group_name = nam;
    }
    else if (typ == "DEVICE") {
      cur_depth = 1; device_name = nam;
    }
    else if (typ == "DRIVER" ) {
      cur_depth = 0; driver_name = nam;
    }
  }
  
  inline string curItemName() const 
  {
    switch (cur_depth)
    {
    case 3:
      return driver_name + "." + device_name + "." + group_name + "." + item_name;
    case 2:
      return driver_name + "." + device_name + "." + group_name;
    case 1:
      return driver_name + "." + device_name;
    case 0:
      return driver_name;
    };
    
    assert (0);
    
    return string();
  }
  
  virtual void processLine (const vector<string>& ) = 0;
  virtual void finish () {}
  
  int curDepth () const {return cur_depth;}
  
  
  bool wasInternal () const
  {
    return was_internal;
  }
  
private:
  fstream stream;
    
  int cur_depth;
  string device_name;
  string driver_name;
  string group_name;
  string item_name;
  
  bool was_internal;
/*  bool isInternal (const string& s) const 
  {
    return propValue(s, "mode") == "internal";
  } */
};



class tag_tree_parser : public unit_conf_parser
{
public:
  tag_tree_parser (name_by_id_t & n, id_by_name_t& i) : name_by_id(n), id_by_name(i){}
  
  virtual void processLine (const vector<string>& v )
  {
    string n = curItemName();

    name_by_id.push_back(n);
    id_by_name.insert( make_pair(n, name_by_id.size() - 1) );
  }
  
private:
  name_by_id_t & name_by_id;
  id_by_name_t & id_by_name;
};

class unit_conf : public unit_conf_parser
{
public:
  unit_conf(ptree& pt, const id_by_name_t & i, id_by_name_t & uid ):tree(pt), id_by_name(i), unit_conf_id_by_name(uid) ,id (0)
  {
  }
  
  ~unit_conf ()
  {
  }
  
  virtual void processLine ( const vector<string>& v  )
  {
    if ( v[2]  != "ITEM" ) return;
    
    keyval_t props;
    parseProps( v[5] , props );
    
    ptree atts;
    makePropsTree(props, atts);

    atts.put ("<xmlattr>.tag_id", idByName( curItemName() ) );
    atts.put ("<xmlattr>.id", lexical_cast<int>(id) );
    tree.add_child ( "n" , atts );
    
    unit_conf_id_by_name.insert ( make_pair(curItemName(), id) );
    
    ++id;
  }
  
  string idByName (const string& n) const
  {
    //id_by_name_t::const_iterator iter = id_by_name.find (curItemName());
    id_by_name_t::const_iterator iter = id_by_name.find (n);
    assert ( iter != id_by_name.end() );
    return lexical_cast<string>((*iter).second) ;
  }
  
  bool isUnitProp (const string& p) const 
  {
    return p == "ilr" || p == "ihr" || p == "elr" || p == "ehr" || p == "type" || p == "use_var_scales" || p == "use_inp_scales" || p == "multip_inp_scales";
  }
  
  void makePropsTree ( const keyval_t & k, ptree & t ) const
  {
    for (auto p : k) {
      if ( p.first == "scale_min" || p.first == "scale_max" || p.first == "inp_scale_min" || p.first == "inp_scale_max" ) {

//cout << "nnnn: "<<p.second<<" "<<"<xmlattr>." + p.first <<" "<< idByName(p.second)<<"\n";

        t.put( "<xmlattr>." + p.first, idByName(p.second) );
      }
      else if (isUnitProp (p.first)){
        t.put("<xmlattr>." + p.first, p.second );
      }
    }

    if (!propExists(k, "ilr")) t.put("<xmlattr>.ilr", "0");
    if (!propExists(k, "ihr")) t.put("<xmlattr>.ihr", "64000");
    if (!propExists(k, "elr")) t.put("<xmlattr>.elr", "0");
    if (!propExists(k, "ehr")) t.put("<xmlattr>.ehr", "100");
  }
  
  bool propExists (const keyval_t& k, const string& n) const 
  {
    for (auto p : k) if (p.first == n) return true;
    
    return false;
  }
private:
  ptree & tree;
  const id_by_name_t & id_by_name;
  id_by_name_t & unit_conf_id_by_name;
  int id;
};

class swap_parser : public unit_conf_parser
{
public:
  swap_parser( ptree& t, const id_by_name_t& uid, const id_by_name_t& did ) : tree (t), unit_conf_id_by_name(uid), file_counter(0), device_id_by_name(did)
  {
  }
  
  virtual void processLine (const vector<string>& v)
  {
    if ( !wasInternal() && curDepth() > 0 ) {
      return;
    }
    else if( curDepth() == 0 ) {
      if ( !cur_file.empty() ) {
        tree.add_child("file", cur_file);
        cur_file.clear();
      }
      cur_file.add ( "<xmlattr>.name", "swap" + lexical_cast<string>(file_counter++) );

/*      auto nam_iter = device_id_by_name.find(curItemName());
      assert(nam_iter != device_id_by_name.end());

      cur_file.add ( "<xmlattr>.dev_id", (*nam_iter).second );*/
    }
    else if (curDepth() == 1) {
      auto nam_iter = device_id_by_name.find(curItemName());
      assert(nam_iter != device_id_by_name.end());

      cur_file.add ( "<xmlattr>.dev_id", (*nam_iter).second );
    }
    else if( curDepth() == 3 ) {
      auto iter = unit_conf_id_by_name.find( curItemName() );
      assert ( iter != unit_conf_id_by_name.end() );

      ptree ent_id;
      ent_id.put( "<xmlattr>.ent_id", (*iter).second );
      ent_id.put( "<xmlattr>.offset", propValue(v[5], "offset") );
      cur_file.add_child ("i", ent_id);
    }
  }
  
  virtual void finish ()
  {
    if ( !cur_file.empty() ) {
      tree.add_child("file", cur_file);
      cur_file.clear();
    }
  }
private:
  ptree & tree;
  ptree cur_file;
  const id_by_name_t & unit_conf_id_by_name;
  int file_counter;
  const id_by_name_t & device_id_by_name;
//  string driver_name;
};


class byrt_based_parser : public unit_conf_parser
{
public:
  byrt_based_parser(ptree & t, const id_by_name_t& uid, const id_by_name_t& dev_id) : tree (t), unit_conf_id_by_name(uid), prev_depth(0), device_id_by_name(dev_id), read_always(false)
  {
  }
  
  virtual void processLine (const vector<string>& v)
  {
    if ( wasInternal() && curDepth() > 0 ) {
      return;
    }

    if ( curDepth() == 1 ) {
      addDevice(curItemName());
    }
    else if( curDepth() == 2 ) {
      period = propValue( v[5], "period" );
      read_always = !propValue(v[5], "read_always").empty();
    }
    else if ( curDepth() == 3 ) {
      if ( prev_depth == 2 ) {
        addGroup (propValue (v[5], "offset"), period );
        addItem ( curItemName () );
      }
      else {
        addItem ( curItemName() );
      }
    }
    prev_depth = curDepth();
  }
  virtual void finish ()
  {
    addCurrentGroup();
    appendCurrentDevice();
  }
private:
  void appendCurrentDevice ()
  {
    if ( !cur_dev.empty() ) {
      tree.add_child("d", cur_dev);
    }
  }
  void addDevice(const string& n)
  {
    appendCurrentDevice();
    
    auto iter = device_id_by_name.find(n);
    if ( iter == device_id_by_name.end() ) {
      cout << "("<<n<<")\n";
      assert(0);
    }
    
    cur_dev.clear();
    cur_dev.put("<xmlattr>.dev_id", (*iter).second);
  }
  
  void addCurrentGroup()
  {
    //tree props;
    if ( !cur_group.empty() ) {
      //tree.add_child("g", cur_group);
      cur_dev.add_child("g", cur_group);
    }

  }
  
  void addGroup(const string& offset, const string& period)
  {
    addCurrentGroup();
    
    cur_group.clear();
    cur_group.put( "<xmlattr>.offset", offset );
    if (!period.empty() && period != "1")
      cur_group.put( "<xmlattr>.period", period + "00");
    if (read_always)
      cur_group.put( "<xmlattr>.read_always", true );
  }

  void addItem (const string& cur_item_name)
  {
    auto iter = unit_conf_id_by_name.find(cur_item_name);
    if ( iter == unit_conf_id_by_name.end() ) {
      cout << "cur_item_name: ("<<cur_item_name<<")\n";
      assert( 0 );
    }

    ptree item;
    item.put( "<xmlattr>.ent_id",  (*iter).second );
    
    cur_group.add_child ("i", item);
  }

  ptree & tree;
  const id_by_name_t & unit_conf_id_by_name;
  int prev_depth;
  string period;
  ptree cur_group, cur_dev;
  const id_by_name_t & device_id_by_name;
  bool read_always;
};


class driver_device_parser : public unit_conf_parser
{
public:
  driver_device_parser (ptree & d, ptree & m, id_by_name_t& dev_id) : devices (d), modules(m), was_ethernet(false), device_id_by_name(dev_id), dev_counter(0)
  {
    
  }
  
  virtual void processLine (const vector<string>& v)
  {
    if (curDepth() == 0 ) {
      drv.clear();
      
      drv.put("<xmlattr>.transport", transportName(v[5]));
      drv.put("<xmlattr>.protocol" , protocolName (v[5]));
      
      fillSpecificParams(drv, v[5]);
      
      if ( isEthernetProtocol(propValue(v[5], "net")) ) {
        was_ethernet = true; last_ip = propValue(v[5], "host"); drv.put( "<xmlattr>.ip", last_ip );
      }
      else {
        was_ethernet = false;
      }
      
      fillModules (v[5]);
    }
    else if ( curDepth() == 1 ) {
      if ( wasInternal() ) {
        if (internal_drv.empty()) {
          internal_drv.put("<xmlattr>.protocol", "swap");
        }
        ptree dev;
        dev.put( "<xmlattr>.id", lexical_cast<string>(dev_counter) );
        internal_drv.add_child("dev", dev);
      }
      else {
        ptree dev;
        if (was_ethernet) {
          //dev.put( "<xmlattr>.ip", last_ip );
        }
        else {
          dev.put( "<xmlattr>.station", propValue(v[5], "station") );
        }
      
        dev.put("<xmlattr>.proc", procType(v[5]));
        dev.put("<xmlattr>.id", dev_counter);
        drv.add_child("dev", dev);

        devices.add_child ("drv", drv);

      }
      device_id_by_name.insert( make_pair(curItemName(), dev_counter ) );
      ++dev_counter;
    }
  }
  virtual void finish()
  {
    if (!internal_drv.empty()) {
      devices.add_child ("drv", internal_drv);
    }
  }
private:
  string moduleName (const string& s)
  {
    if (s == "udp_net") return "byrt0";
    if (s == "udp_net2" || s == "tcp_net") return "byrt";
    if (s == "mbsr_udp" || s == "mb_tcpip" || s == "mb_serial") return "modbus";
    if (s == "mininet") return "mininet";
    if (s == "oven_ac2") return "oven_ac2";
    
    assert (0);
    
    return string();
  }
  
  void fillModules (const string& s)
  {
    string module_name = moduleName( propValue(s, "net") );
    
    if (modules.find(module_name) == modules.not_found()) {
      modules.put("modules." + module_name, string());
    }
    
    modules.put("modules.swap", string());
  }
  
  string procType(const string& s) const
  {
    string proc = propValue (s, "proc_type");

    if (proc == "intel") return "big_endian";
    if (proc == "motorola" || proc == "akon") return "small_endian";
    if (proc == "schneider") return "pdp11";
    
    assert (0);
    
    return string();
  }
  
  void fillSpecificParams (ptree & t, const string& s) const
  {
    string protocol = propValue (s, "net");
    if ( isEthernetProtocol(protocol) ) {
      string port = propValue (s, "port");
      if (!port.empty()) t.put("port", port);
    }
    else if (isSerialProtocol (s) ) {
      string dev = propValue (s, "device");
      if (!dev.empty()) t.put("dev", dev);
        
      string baud = propValue (s, "baud");
      if (!baud.empty()) t.put("baud", baud);
        
      string parity = propValue (s, "parity");
      if (!parity.empty()) t.put("parity", parity);
        
      string stop_bits = propValue(s, "stop_bits");
      if (!stop_bits.empty()) t.put("stop_bits", stop_bits);
    }
  }
  
  bool isEthernetProtocol(const string& s) const
  {
    return s == "udp_net" || s == "mb_tcpip" || s == "udp_net2" ||  s == "mbsr_udp" || s == "tcp_net";
  }
  
  bool isSerialProtocol(const string& s) const
  {
    return s == "mininet" || s == "mb_serial" || s == "oven_ac2";
  }
  
  string transportName (const string& s) const
  {
    string protocol = propValue(s, "net");
    if (protocol == "udp_net" || protocol == "udp_net2" || protocol == "mbsr_udp") return "udp";
    if (protocol == "mb_tcpip" || protocol == "tcp_net") return "tcp";
    if (protocol == "mininet" || protocol == "mb_serial" || protocol == "oven_ac2") return "tty";
    
    std::cerr << s << " protocol: ("<<protocol<<")" <<"\n";
    
    assert (0);
  }
  
  string protocolName (const string& s) const 
  {
    string protocol = propValue(s, "net");
    
    if (protocol == "udp_net") return "byrt0";
    if (protocol == "udp_net2" || protocol == "tcp_net") return "byrt";
    if (protocol == "mbsr_udp" || protocol == "mb_tcpip" || protocol == "mb_serial") return "modbus";
    if (protocol == "mininet") return "mininet";
    if (protocol == "oven_ac2") return "oven_ac2";
    
    assert (0);
  }
  
  string last_ip;

  ptree & devices;
  ptree & modules;

  bool was_ethernet;  
  
  ptree drv;
  ptree internal_drv;
  
  id_by_name_t & device_id_by_name;
  int dev_counter;
};


class prot_conf : public conf_parser
{
public:
  prot_conf(ptree& pt, const id_by_name_t & i) : tree(pt), tag_id_by_name(i)
  {
    
  }
  
  void parse (const string& s)
  {
    stream.open(s, fstream::in);
    string l;
    
    while ( !stream.eof() ) {
      std::getline (stream, l);
      parseLine(l);
    }
    
    //finish();
    
    stream.close();
  }
private:
  void parseLine( string& s )
  {
    removeSemicolon(s);
    
    vector<string> v;
    split(v, s, boost::is_any_of(","));

    trimVector(v);

    if (v.size () == 0) return;
    
    processCols(v);
    //tree.put_child()
  }
  
  void processCols (const vector<string>& v)
  {
    enum ColType{Db=0, Var, CyrVar, Props, Grp};
    
    ptree & db_tree = dbByName(v[Db]);
    ptree & n       = db_tree.add("v", string());
    
    n.put("<xmlattr>.tag_id"  , lexical_cast<string>( (*tag_id_by_name.find( v[Var] )).second ) );
    if ( !v[CyrVar].empty() )
      n.put("<xmlattr>.cn" , v[CyrVar] );
    if ( !v[Grp].empty() )
      n.put("<xmlattr>.grp", v[Grp] );
    
    processProps(n, v[Props]);
  }
  
  void processProps(ptree & t, const string& prp)
  {
    keyval_t props;
    parseProps(prp, props);
    
    for ( keyval_t::value_type&  p : props ) {
      t.put( "<xmlattr>." + p.first, p.second );
    }
  }
  
  ptree& dbByName (const string& nam)
  {
    optional<ptree&> db_tree =  tree.get_child_optional(nam);
    if (db_tree) return *db_tree;
    
    return tree.put_child(nam, ptree());
    
    //t.data() = nam;

//    db_list.push_back( t );
//    tree.put_child(nam, )
    //return db_list.back();

  }

  //list<ptree> db_list;

  ptree & tree;
  fstream stream;
  const id_by_name_t & tag_id_by_name;
};

void write_tags (const string& p, const name_by_id_t& n)
{
  ptree tree;
  
  
  for ( size_t i = 0; i<n.size(); ++i ) {
    ptree id_tree;
    id_tree.put("<xmlattr>.id" , lexical_cast<string>(i) );
    tree.put_child(n[i], id_tree);
  }
  
  write_xml( p, tree , std::locale(), xml_writer_settings<char>(' ', 2) );
}


void saveInputModules (const string& p)
{
  fstream s;
  s.open (p, fstream::out);
  
  s<<"<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
   <<"<modules>\n"
   <<"  <votlis id=\"0\"/>\n"
   <<"</modules>\n";
 
  s.close();
}

void saveInputDevices (const string& p)
{
  fstream s;
  s.open(p, fstream::out);
  
  s<<"<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
   <<"<drv transport=\"tcp\">\n"
   <<"  <dev protocol=\"votlis\" port=\"4242\"/>\n"
   <<"</drv>\n";
  
  s.close();
}


int main (int argc, char ** argv)
{
  if ( argc != 3 ) {
    cout << "You need to path \"from\" catalog as first parameter and \"to\" catalog as second parameter\n"; 
    return 0;
  }
  
  fs::path from_path(argv[1]), to_path(argv[2]);
  fs::directory_iterator end;
  ptree tree;
  
  const string slash(1, '/'/*fs::slash<fs::path>::value*/);

  name_by_id_t name_by_id;
  id_by_name_t id_by_name, unit_conf_id_by_name, device_id_by_name;

  for (fs::directory_iterator iter(from_path); iter != end; ++iter  ) {
    if ( (*iter).path().filename() == "unit_conf.txt" ) {
      tag_tree_parser tags(name_by_id, id_by_name);
      tags.parse( (*iter).path().string() );
      
      unit_conf c( tree, id_by_name, unit_conf_id_by_name );
      
      c.parse((*iter).path().string());
      write_xml( to_path.string() + slash + "entries.xml", tree , std::locale(), xml_writer_settings<char>(' ', 2) );
      
      ptree devices, modules;
      
      driver_device_parser ddm(devices, modules, device_id_by_name);
      
      ddm.parse ((*iter).path().string());
      write_xml( to_path.string() + slash + "out_devices.xml", devices , std::locale(), xml_writer_settings<char>(' ', 2) );
      write_xml( to_path.string() + slash + "out_modules.xml", modules , std::locale(), xml_writer_settings<char>(' ', 2) );
      
      ptree byrt_tree;
      byrt_based_parser byrt_parser (byrt_tree, unit_conf_id_by_name, device_id_by_name);
      byrt_parser.parse((*iter).path().string());
      write_xml( to_path.string() + slash + "byrt.xml", byrt_tree , std::locale(), xml_writer_settings<char>(' ', 2) );
      
      ptree swap_tree;
      swap_parser sw_parser (swap_tree, unit_conf_id_by_name, device_id_by_name);
      sw_parser.parse((*iter).path().string());
      write_xml( to_path.string() + slash + "swap.xml", swap_tree , std::locale(), xml_writer_settings<char>(' ', 2) );
    }
    else if ( (*iter).path().filename() == "prot_conf.txt" ) {
      ptree prot_tree;
      prot_conf pc(prot_tree, id_by_name);
      pc.parse((*iter).path().string());
      write_xml(to_path.string() + slash + "prot_conf.xml", prot_tree , std::locale(), xml_writer_settings<char>(' ', 2));
    }
    
  }

  //write_xml( to_path.string() + slash + "tags.txt", tree , std::locale(), xml_writer_settings<char>(' ', 2) );

  write_tags ( to_path.string() + slash + "tags.xml", name_by_id );
  
  saveInputModules(to_path.string() + slash + "inp_modules.xml");
  saveInputDevices( to_path.string() + slash + "inp_devices.xml" );
    
  return 0;
}

