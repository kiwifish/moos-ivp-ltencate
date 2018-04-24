/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: PointAssign.h                                          */
/*    DATE: December 29th, 1963                             */
/************************************************************/

#ifndef PointAssign_HEADER
#define PointAssign_HEADER

#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"
#include <list>
#include <stdlib.h>
#include <vector>
#include "Point.h"
#include "MOOS/libMOOS/MOOSLib.h"

using namespace std;

class PointAssign : public AppCastingMOOSApp
{
 public:
   PointAssign();
   ~PointAssign();

 protected: // Standard MOOSApp functions to overload  
   bool OnNewMail(MOOSMSG_LIST &NewMail);
   bool Iterate();
   bool OnConnectToServer();
   bool OnStartUp();

 protected: // Standard AppCastingMOOSApp function to overload 
   bool buildReport();

 protected:
   void registerVariables();
   void postViewPoint(double x, double y, string label, string color);

 private: // Configuration variables
   bool assign_by_region;
   
 private: // State variables
   list<string> m_travel_points;
   vector<Point> m_points;
};

#endif 
