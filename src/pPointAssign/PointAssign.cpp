/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: PointAssign.cpp                                        */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "ACTable.h"
#include "XYPoint.h"
#include "PointAssign.h"

using namespace std;

//---------------------------------------------------------
// Constructor

PointAssign::PointAssign()
{
  assign_by_region = false;
  firstpoint = true;
  lastpoint  = true;
}

//---------------------------------------------------------
// Destructor

PointAssign::~PointAssign()
{
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool PointAssign::OnNewMail(MOOSMSG_LIST &NewMail)
{
  AppCastingMOOSApp::OnNewMail(NewMail);

  MOOSMSG_LIST::iterator p;
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;
    string key    = msg.GetKey();

 

#if 0 // Keep these around just for template
    string comm  = msg.GetCommunity();
    double dval  = msg.GetDouble();
    string sval  = msg.GetString(); 
    string msrc  = msg.GetSource();
    double mtime = msg.GetTime();
    bool   mdbl  = msg.IsDouble();
    bool   mstr  = msg.IsString();
#endif

     if(key == "VISIT_POINT"){
       string sval = msg.GetString();
       m_travel_points.push_back(sval);
       m_Comms.Notify("VISIT_POINT_CHECK", sval);
     }
     
     else if(key != "APPCAST_REQ") // handled by AppCastingMOOSApp
       reportRunWarning("Unhandled Mail: " + key);
   }
	
   return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool PointAssign::OnConnectToServer()
{
   registerVariables();
   return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool PointAssign::Iterate()
{
  AppCastingMOOSApp::Iterate();
  // Do your thing here!
  m_Comms.Notify("UTS_PAUSE", "false");
  
  //message processing I'd love to do in onnewmail
  std::list<string>::iterator it;
  for (it = m_travel_points.begin(); it != m_travel_points.end();++it) {
    Point m_point;
    string sval = *it;

    m_Comms.Notify("STRING_VAL", sval);
    
    vector<string> myvector = parseString(sval, ',');
    for(unsigned int i=0; i<myvector.size(); i++) {
      string param = biteStringX(myvector[i], '=');
      string value = myvector[i];
      if(tolower(param) == "x"){
	double xdouble = atof(value.c_str());
	m_point.setX(xdouble);
      }
      else if(tolower(param) == "y"){
	double ydouble = atof(value.c_str());
	m_point.setY(ydouble);
      }
      else if(tolower(param) == "id"){
	int idint = atoi(value.c_str());
        m_Comms.Notify("INT", idint);
	m_point.setID(idint);
      }      
    }
    it = m_travel_points.erase(it); 
    m_points.push_back(m_point);
  }

  //message processing end 

  //assign by region is false
  if (assign_by_region == false) {

    m_Comms.Notify("ASSIGN_BY_REGION", "false");
    m_Comms.Notify("VISIT_POINT_HENRY", "firstpoint");
    m_Comms.Notify("VISIT_POINT_GILDA", "firstpoint");

    for (std::vector<Point>::iterator it = m_points.begin(); it != m_points.end();it++) {
      
      Point& point = *it;
      std::cout << point.getReport() << std::endl;

      if ((point.getID() % 2) == 0) {
	    
	string id = intToString(point.getID());
	string color = "green";
	postViewPoint(point.getX(),point.getY(),id,color);
	m_Comms.Notify("VISIT_POINT_HENRY", point.getReport());
      }
      else {
	    
	string id = intToString(point.getID());
	string color = "blue";
	postViewPoint(point.getX(),point.getY(),id,color);
	m_Comms.Notify("VISIT_POINT_GILDA", point.getReport());
      }
    }
    
    m_Comms.Notify("VISIT_POINT_HENRY", "lastpoint");
    m_Comms.Notify("VISIT_POINT_GILDA", "lastpoint");
  }


  //assign by region is true
  else if(assign_by_region == true) {
    if(firstpoint){
    m_Comms.Notify("VISIT_POINT_HENRY", "firstpoint");
    m_Comms.Notify("VISIT_POINT_GILDA", "firstpoint");
    firstpoint = false;
    }
    
    for (std::vector<Point>::iterator it = m_points.begin(); it != m_points.end();) {
      Point& point = *it;
      
      if (point.getX() < 65) { 

  	string id = intToString(point.getID());
  	string color = "blue";
  	postViewPoint(point.getX(),point.getY(),id,color);
  	m_Comms.Notify("VISIT_POINT_HENRY", point.getReport());
      }
      else {
  	string id = intToString(point.getID());
  	string color = "red";
  	postViewPoint(point.getX(),point.getY(),id,color);
  	m_Comms.Notify("VISIT_POINT_GILDA", point.getReport());
      }

      it = m_points.erase(it); 

    }

    if (m_points.size() == 0 && lastpoint) {
    m_Comms.Notify("VISIT_POINT_HENRY", "lastpoint");
    m_Comms.Notify("VISIT_POINT_GILDA", "lastpoint");
    lastpoint = false;
    }
  }
  

  AppCastingMOOSApp::PostReport();
  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool PointAssign::OnStartUp()
{
  AppCastingMOOSApp::OnStartUp();

  STRING_LIST sParams;
  m_MissionReader.EnableVerbatimQuoting(false);
  if(!m_MissionReader.GetConfiguration(GetAppName(), sParams))
    reportConfigWarning("No config block found for " + GetAppName());

  STRING_LIST::iterator p;
  for(p=sParams.begin(); p!=sParams.end(); p++) {
    string orig  = *p;
    string line  = *p;
    string param = tolower(biteStringX(line, '='));
    string value = line;
    bool bvalue;

    bool handled = false;
    if(param == "assign_by_region") {
      setBooleanOnString(bvalue,value);
      assign_by_region = bvalue;
      handled = true;
    }
    else if(param == "bar") {
      handled = true;
    }

    if(!handled)
      reportUnhandledConfigWarning(orig);

  }
  
  registerVariables();	
  return(true);
}

//---------------------------------------------------------
// Procedure: registerVariables

void PointAssign::registerVariables()
{
  AppCastingMOOSApp::RegisterVariables();
  Register("VISIT_POINT",0);
}


//------------------------------------------------------------
// Procedure: buildReport()

bool PointAssign::buildReport() 
{
  m_msgs << "============================================ \n";
  m_msgs << "File:                                        \n";
  m_msgs << "============================================ \n";

  ACTable actab(4);
  actab << "Alpha | Bravo | Charlie | Delta";
  actab.addHeaderLines();
  actab << "one" << "two" << "three" << "four";
  m_msgs << actab.getFormattedString();

  return(true);
}

void PointAssign::postViewPoint(double x, double y, string label, string color)
 {
   XYPoint point(x, y);
   point.set_label(label);
   point.set_color("vertex", color);
   point.set_param("vertex_size", "2");

   string spec = point.get_spec();
   Notify("VIEW_POINT", spec);
 }


