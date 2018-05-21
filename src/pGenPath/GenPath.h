/************************************************************/
/*    NAME: Lauren TenCate                                              */
/*    ORGN: MIT                                             */
/*    FILE: GenPath.h                                          */
/*    DATE: December 29th, 1963                             */
/************************************************************/

#ifndef GenPath_HEADER
#define GenPath_HEADER

#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"
#include "Point.h"
#include <list>
#include <stdlib.h>
#include <vector>
#include "XYSegList.h"

using namespace std;

class GenPath : public AppCastingMOOSApp
{
 public:
   GenPath();
   ~GenPath();

 protected: // Standard MOOSApp functions to overload  
   bool OnNewMail(MOOSMSG_LIST &NewMail);
   bool Iterate();
   bool OnConnectToServer();
   bool OnStartUp();

 protected: // Standard AppCastingMOOSApp function to overload 
   bool buildReport();

 protected:
   void registerVariables();
   void closePoints();
   // Point closePointsNew(Point pt);

 private: // Configuration variables

 private:
    list<string> m_visit_pts_str; 
    vector<Point> m_points; 
    XYSegList m_visit_points; // State variables
    double currentbest;
    double dist;
    double m_x;
    double m_y;
};

#endif 
