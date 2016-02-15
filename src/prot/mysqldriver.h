#ifndef __MYSQL_DRIVER_H_
#define __MYSQL_DRIVER_H_

#include <boost/tuple/tuple.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/algorithm/string.hpp>

#include <string>
#include <vector>
#include <iostream>

#include <mysql-connector/cppconn/driver.h>
#include <mysql-connector/cppconn/exception.h>
#include <mysql-connector/cppconn/connection.h>
#include <mysql-connector/cppconn/statement.h>
#include <mysql-connector/cppconn/resultset.h>
#include <mysql-connector/cppconn/prepared_statement.h>

#include "configdata.h"

using std::string;
using std::vector;

using boost::tuple;

using namespace boost::posix_time;


namespace prot 
{
class MySqlDriver
{
public:
  MySqlDriver( )
  {
  }

  ~MySqlDriver()
  {
    disconnect();
  }
  

  void connect() {connect("127.0.0.1", "operator", "");}
  void disconnect ();
  
  void save ( const VariableParams& , const vector<VariableValue>  &, const vector<ScaleValue>&  );
  tuple <float, float, bool> getScale ( const string& equi, const string& var_name );
private:
  
  typedef boost::shared_ptr<sql::Connection> SqlConnPtr;
  typedef boost::shared_ptr<sql::Statement > SqlStmtPtr;
  typedef boost::shared_ptr<sql::ResultSet > SqlResultPtr;
  typedef boost::shared_ptr<sql::PreparedStatement> SqlPreparedPtr;
  
  SqlConnPtr conn_ptr;
  SqlStmtPtr stmt_ptr;
  
  void connect ( const string & , const string& , const string&  );
  void prepareDatabase    ( const string& db_name );
  bool checkDbExists      ( const string& db_name );
  bool checkTbExists      ( const string& db_name, const string& tb_name );
  void createDatabase     ( const string& db_name );
  void setCurrentDatabase ( const string& db_name );
  void loadScript (const string &);
  
  string scaleTableName (const string& var_name)
  {
    return "Scale_" + boost::replace_all_copy (var_name, ".", "$");
  }
  void insertValues (const string& table_name, const vector<VariableValue>& data);
  void insertScales (const string& table_name, const vector<ScaleValue>   & data);
  
  string preparedTableName( const string & );
};

};
#endif
