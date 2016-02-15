#include <boost/shared_ptr.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
//#include <boost/algorithm/string.hpp>
#include <boost/tuple/tuple.hpp>

#include "mysqldriver.h"

#include <fstream>
#include <sstream>

#include "servicepath.h"

//prot::MySqlDriver::MySqlDriver( )
//{
//  try {
//  } 
//}

void prot::MySqlDriver::connect ( const string & host, const string& user, const string& pass  )
{
  cout << "if mysql_driver-->connect\n";
  
  try {
    sql::Driver * drv = get_driver_instance();
    
    printf ("\n\nmysql_driver: %p\n\n", conn_ptr.get());
    
    conn_ptr.reset( drv->connect(host, user, pass ) );
    stmt_ptr.reset( conn_ptr->createStatement() );
  }
  catch ( sql::SQLException& ex ) {
    std::cerr<<"sqlconnect ERROR!!!!( "<<ex.what()<<" ) state ("<<  ex.getSQLState() <<")\n" ;
    throw std::runtime_error ("error connecting to my sql");
  }
}

void prot::MySqlDriver::disconnect()
{
  if ( stmt_ptr ) {
    stmt_ptr->close ();
    stmt_ptr.reset();
  }
  
  if ( conn_ptr ) {
    conn_ptr->close ();
    conn_ptr.reset();
    std::cout<<"MYSQL CLOSED!!!\n";
  }
}

void prot::MySqlDriver::save ( const VariableParams& params, const vector<VariableValue>  & values, const vector<ScaleValue>& scales )
{
  try {
//std::cout << "1\n";
    prepareDatabase ( params.get<0>() );
//std::cout << "2\n";
    string table_name = preparedTableName ( params.get<1>() );
//std::cout << "3\n";
    insertValues  ( table_name, values );
//std::cout << "4\n";
    insertScales  ( params.get<1>(), scales );
//std::cout << "5\n" ;   
  }
  catch ( sql::SQLException& ex ) {
    std::cerr<<"sqlexecution ERROR!!!!( "<<ex.what()<<" ) state ("<<  ex.getSQLState() <<")\n" ;
  }
}

void prot::MySqlDriver::insertScales (const string & var_name, const vector<ScaleValue>& scales )
{
  if ( scales.size() == 0 ) 
    return;
  
  string q = "INSERT INTO " + scaleTableName (var_name) + "(Time, MinS, MaxS) VALUES ";
  
  for ( vector<ScaleValue>::const_iterator iter = scales.begin(); iter != scales.end(); ++iter ) {

//    std::cout<<"------->"<< (q +  "('" + boost::posix_time::to_iso_string((*iter).get<0>() ) + "', "
//                                      + boost::lexical_cast<string>((*iter).get<1>() ) + ", "
//                                      + boost::lexical_cast<string>((*iter).get<2>() ) + ") "  ) <<"\n" ;
    
    stmt_ptr->execute ( q + "('" + boost::posix_time::to_iso_string((*iter).get<0>() ) + "', "
                                 + boost::lexical_cast<string>((*iter).get<1>() ) + ", "
                                 + boost::lexical_cast<string>((*iter).get<2>() ) + ") " );
    

  }
}
void prot::MySqlDriver::insertValues ( const string& table_name, const vector<VariableValue>& values  )
{
  if ( values.size() == 0 ) 
    return;
    
  string iq = "INSERT INTO " + table_name + "(Time, IsDst, Value, NScale) VALUES ";
  string uq = " ON DUPLICATE KEY UPDATE IsDst=VALUES(IsDst), Value=VALUES(Value), NScale=VALUES(NScale) ";
  vector<VariableValue>::const_iterator iter = values.begin();
  
  string val ( std::isnan((*iter).get<1>()) ? "NULL" : boost::lexical_cast<string>((*iter).get<1>()) );
  iq += "('" + boost::posix_time::to_iso_string ( (*iter).get<0>() ) + "', 1, " +  val + ", 1 )";
  ++iter;
  for ( ;iter != values.end(); ++iter  ) {
    val = std::isnan((*iter).get<1>()) ? "NULL" : boost::lexical_cast<string>((*iter).get<1>());
    
    iq += ", ('" + boost::posix_time::to_iso_string ( (*iter).get<0>() ) + "', 1, " + val + ", 1 )";
  }
  
//  std::cout << iq + uq;
  
//  cout << iq + uq<<"\n";
//  wdde
  stmt_ptr->execute ( iq + uq );
}

void prot::MySqlDriver::createDatabase ( const string& db_name  )
{
  //stmt_ptr->execute ( "CREATE DATABASE " + db_name  );
  stmt_ptr->execute ( "CREATE DATABASE " + db_name + " DEFAULT COLLATE utf8_general_ci" );

  setCurrentDatabase (db_name);
  stmt_ptr->execute ( "CREATE TABLE FilesTable (VName CHAR(60), STName INT, TBegin TIMESTAMP DEFAULT 0, TEnd TIMESTAMP DEFAULT 0)"  );

//std::cout << "1\n";
  
  cout << "creatng database: "<<db_name<<"\n";
  
  using a::fs::prot_sql_path;
  
  loadScript(prot_sql_path() + "01.sql");
  loadScript(prot_sql_path() + "02.sql");
  loadScript(prot_sql_path() + "03.sql");
  loadScript(prot_sql_path() + "04.sql");
  loadScript(prot_sql_path() + "05.sql");
  loadScript(prot_sql_path() + "06.sql");
  loadScript(prot_sql_path() + "07.sql");
  loadScript(prot_sql_path() + "08.sql");
  loadScript(prot_sql_path() + "09.sql");
  loadScript(prot_sql_path() + "10.sql");
  loadScript(prot_sql_path() + "11.sql");
  loadScript(prot_sql_path() + "12.sql");
  loadScript(prot_sql_path() + "14.sql");
  loadScript(prot_sql_path() + "15.sql");  
  loadScript(prot_sql_path() + "16.sql");
  loadScript(prot_sql_path() + "17.sql");
  loadScript(prot_sql_path() + "schedule.sql");
}

void prot::MySqlDriver::loadScript ( const string& file_name )
{
  std::ifstream sql_script(file_name.c_str());

  if ( !sql_script ) {
    std::cerr<<"----------------->> Cant load sql script!!!! "<<file_name<<"\n";
  }
  else {
    std::ostringstream sql_data;
    sql_data<<sql_script.rdbuf();
    stmt_ptr->execute ( sql_data.str() );
  }
}

void prot::MySqlDriver::prepareDatabase ( const string& db_name  )
{
  if ( checkDbExists(db_name)  ) {
    setCurrentDatabase (db_name);
  }
  else {
    createDatabase ( db_name );
  }
}

bool prot::MySqlDriver::checkDbExists ( const string& db_name )
{
  string q = "SELECT count(*) FROM INFORMATION_SCHEMA.SCHEMATA WHERE SCHEMA_NAME='"+db_name+"';" ;

  SqlResultPtr res ( stmt_ptr->executeQuery (q) );
  res->next();

  return res->getInt(1);
}

bool prot::MySqlDriver::checkTbExists ( const string& db_name, const string& tb_name )
{
  string q = "SELECT count(*) FROM INFORMATION_SCHEMA.TABLES WHERE TABLE_SCHEMA='" + db_name + "' AND TABLE_NAME='"+tb_name + "'";
  SqlResultPtr res(stmt_ptr->executeQuery (q) );
  res->next();
  return res->getInt(1);  
}

void prot::MySqlDriver::setCurrentDatabase ( const string& db_name  )
{
  stmt_ptr->execute ( "USE " + db_name );
}


string prot::MySqlDriver::preparedTableName( const string & var_name )
{
  SqlResultPtr res( stmt_ptr->executeQuery( "CALL prepare_table('" + var_name + "')" ) );
  res->next();
  string table_name = res->getString (1);
  
  // some stupid code for free results
  while (res->next() ) {}
  do {
    res.reset ( stmt_ptr->getResultSet() );
    while (res && res->next() ) {}
  } while (stmt_ptr->getMoreResults() );
  
  return table_name;
}

tuple <float, float, bool> prot::MySqlDriver::getScale (const string& equi,  const string& var_name)
{
  prepareDatabase (equi);
  
  string table_name = scaleTableName( var_name ); 

  if ( !checkTbExists (equi, table_name ) ) {
//std::cout<<"scale_1\n";
    return boost::make_tuple (0.0, 0.0, false);
  }
  
  SqlResultPtr res( stmt_ptr->executeQuery ("SELECT count(*) FROM " + table_name) );
  res->next();
  if ( res->getInt(1) == 0 ) {
//std::cout<<"scale_2\n";  
    return boost::make_tuple (0.0, 0.0, false);
  }
  
  res.reset( stmt_ptr->executeQuery("SELECT MinS, MaxS FROM " + table_name + " WHERE Time = (SELECT MAX(Time) FROM " + table_name + " )") );
  res->next();

//std::cout<<"scale_3\n";  
  return boost::make_tuple( static_cast<float>(res->getDouble("MinS")), static_cast<float>(res->getDouble("MaxS")), true );
}
