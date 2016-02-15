
#include <iostream>
#include <thread>
#include <chrono>

using std::cout;
using std::cerr;


#include "server.h"
#include "group.h"
#include "item.h"
#include "jivariant.h"
#include "jisystem.h"
#include "jexception.h"
#include "connectioninformation.h"
#include "jinitializer.h" 

//#pragma GCC reset_options

/* 
192.168.0.251 
192.168.0.251 
Administrator 
marhabaten 
F822DE8F-207C-11D1-BAD4-006097385086
*/


int main (int argc , char ** argv)
{
  //int a = 0b1101;
  
//  if (argc < 6 ) {
//    cerr<< "to few arguments!"; return 0;
//  }

  

  
  JInitializer init;
  cout << "before JISystem\n";
  JISystem::setInBuiltLogHandler(false);
  cout << "after JISystem\n";
  ConnectionInformation  ci;

  
//  cout << "running with: " << "host: "<<argv[1] << " domain: "<<argv[2] << " user: "<<argv[3] << " password: "<<argv[4]<< " clsid: "<<argv[5]<<"\n";
  cout << "---->1\n";
  ci.setHost ("192.168.0.251" );
  ci.setDomain ("192.168.0.251" );
  ci.setUser ("Administrator" );
  ci.setPassword ( "marhabaten" );
  ci.setClsid ( "F822DE8F-207C-11D1-BAD4-006097385086" );  /*abb server*/

  cout << "---->2\n";
//  ci.setHost ( argv[1] );
//  ci.setDomain ( argv[2] );
//  ci.setUser ( argv[3] );
//  ci.setPassword ( argv[4] );
//  ci.setClsid ( argv[5] );
  
  Server server(ci);
  cout << "---->3\n";
//  try {  
    server.connect();
	cout << "---->4\n";
  //}
//  catch (JException & ex ) {
//    std::cerr<< "got exception!!!!\n";
//    ex.printStackTrace();
//  }

    Group gr  = server.addGroup("group");
  Item item;

      item = gr.addItem ("variable1");

    while (1) {
      ItemState st = item.read(false);
      JIVariant v  = st.getValue();
      
      cout << "value: "<<v.getObjectAsShort() << " quality: "<<st.getQuality()<<"\n";
      
      std::this_thread::sleep_for(std::chrono::seconds(1));
    }
  //}
 
  return 0;
}
