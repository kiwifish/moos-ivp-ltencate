/************************************************************/
/*    NAME: Lauren TenCate                                              */
/*    ORGN: MIT                                             */
/*    FILE: PrimeFactor.h                                          */
/*    DATE:                                                 */
/************************************************************/

#ifndef PrimeFactor_HEADER
#define PrimeFactor_HEADER

#include "MOOS/libMOOS/MOOSLib.h"
#include <list>
#include "PrimeEntry.h" 


using namespace std;

class PrimeFactor : public CMOOSApp
{
 public:
   PrimeFactor();
   ~PrimeFactor();

 protected: // Standard MOOSApp functions to overload  
   bool OnNewMail(MOOSMSG_LIST &NewMail);
   bool Iterate();
   bool OnConnectToServer();
   bool OnStartUp();
   
 protected:
   void RegisterVariables();

 private: // Configuration variables
  
 private: // State variables
   list<PrimeEntry> m_num_val_messages; //list entries
   unsigned int m_recieved_index;
   unsigned int m_calculated_index; 
};

#endif 
