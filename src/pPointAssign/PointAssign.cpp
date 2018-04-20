/************************************************************/
/*    NAME: Lauren TenCate                                  */
/*    ORGN: MIT                                             */
/*    FILE: PointAssign.cpp                                        */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "XYPoint.h"
#include "PointAssign.h"

using namespace std;

//---------------------------------------------------------
// Constructor

PointAssign::PointAssign()
{
  assign_by_region = false; //needs to be initialized to something
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
    string key  = msg.GetKey();

    if(key == "VISIT_POINT"){
      Point m_point;
      string sval = msg.GetString();
      // std::cout << sval << std::endl;

      m_travel_points.push_back(sval);
    }
    
#if 0 // Keep these around just for template
    string key   = msg.GetKey();
    string comm  = msg.GetCommunity();
    double dval  = msg.GetDouble();
    string sval  = msg.GetString(); 
    string msrc  = msg.GetSource();
    double mtime = msg.GetTime();
    bool   mdbl  = msg.IsDouble();
    bool   mstr  = msg.IsString();
#endif
  }
	
   return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool PointAssign::OnConnectToServer()
{
   // register for variables here
   // possibly look at the mission file?
   // m_MissionReader.GetConfigurationParam("Name", <string>);
   // m_Comms.Register("VARNAME", 0);
	
   RegisterVariables();
   return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool PointAssign::Iterate()
{
  AppCastingMOOSApp::Iterate();
  m_Comms.Notify("UTS_PAUSE", "false");
  
  std::cout << m_travel_points.size() << std::endl;

  //message processing I'd love to do in onnewmail
  std::list<string>::iterator it;
  for (it = m_travel_points.begin(); it != m_travel_points.end();++it) {
    Point m_point;
    string sval = *it;

 
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
      else if(tolower(param) == "unique_id"){
	int idint = atoi(value.c_str());
	m_point.setID(idint);
      }
      
    }

    it = m_travel_points.erase(it);
    
    m_points.push_back(m_point);

  
  }

  //
  if (assign_by_region == false) {

    m_Comms.Notify("ASSIGN_BY_REGION", "false");
    
    m_Comms.Notify("VISIT_POINT_HENRY", "firstpoint");
    m_Comms.Notify("VISIT_POINT_GILDA", "firstpoint");

    for (std::vector<Point>::iterator it = m_points.begin(); it != m_points.end();it++) {
      
          Point& point = *it;
	  std::cout << point.getReport() << std::endl;

	  if ((point.getID() % 2) == 0) {
	    
	    string id = intToString(point.getID());
	    string color = "red";
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

  else {

    m_Comms.Notify("ASSIGN_BY_REGION", "true");

    m_Comms.Notify("VISIT_POINT_HENRY", "firstpoint");
    m_Comms.Notify("VISIT_POINT_GILDA", "firstpoint");
     for (std::vector<Point>::iterator it = m_points.begin(); it != m_points.end();) {

        Point& point = *it;

	if (point.getX() < 65) { //south

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
	
    m_Comms.Notify("VISIT_POINT_HENRY", "lastpoint");
    m_Comms.Notify("VISIT_POINT_GILDA", "lastpoint");
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
  list<string> sParams;
  m_MissionReader.EnableVerbatimQuoting(false);
  if(m_MissionReader.GetConfiguration(GetAppName(), sParams)) {
    list<string>::iterator p;
    for(p=sParams.begin(); p!=sParams.end(); p++) {
      string line  = *p;
      string param = tolower(biteStringX(line, '='));
      string value = line;
      bool bvalue;
      
      if(param == "assign_by_region") {
        setBooleanOnString(bvalue,value);
	 bool assign_by_region = bvalue;
      }
        
      else if(param == "vname") {
        //handled
      }
    }
  }
  
  RegisterVariables();	
  return(true);
}

//---------------------------------------------------------
// Procedure: RegisterVariables

void PointAssign::RegisterVariables()
{
  AppCastingMOOSApp::RegisterVariables();
  Register("VISIT_POINT",0);  // Register("FOOBAR", 0);
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


bool PointAssign::buildReport()
{
  // m_msgs << "Number of good messages: " << m_good_message_count << endl;
  // m_msgs << "Number of bad  messages: " << m_bad_message_count  << endl;
    
  return(true);
}
