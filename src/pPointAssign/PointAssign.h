/************************************************************/
/*    NAME: Lauren TenCate                                              */
/*    ORGN: MIT                                             */
/*    FILE: PointAssign.h                                          */
/*    DATE:                                                 */
/************************************************************/

#ifndef PointAssign_HEADER
#define PointAssign_HEADER

#include "MOOS/libMOOS/MOOSLib.h"
#include <list>
#include <stdlib.h>
#include <vector>
#include "Point.h"
#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"

using namespace std;

class PointAssign : public AppCastingMOOSApp
{
 public:
  PointAssign(); // initialize assign by region as false
   ~PointAssign();

 protected: // Standard MOOSApp functions to overload  
   bool OnNewMail(MOOSMSG_LIST &NewMail);
   bool Iterate();
   bool OnConnectToServer();
   bool OnStartUp();
   bool buildReport();

 protected:
   void RegisterVariables();
   void postViewPoint(double x, double y, string label, string color);
   
 private: // Configuration variables
   bool assign_by_region;
   
   
 private: // State variables
   list<string> m_travel_points; //list entries
   vector<Point> m_points;

  
};

#endif 
