/************************************************************/
/*    NAME: Lauren TenCate                                              */
/*    ORGN: MIT                                             */
/*    FILE: GenPath.cpp                                        */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "ACTable.h"
#include "GenPath.h"

using namespace std;

//---------------------------------------------------------
// Constructor

GenPath::GenPath()
{
}

//---------------------------------------------------------
// Destructor

GenPath::~GenPath()
{
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool GenPath::OnNewMail(MOOSMSG_LIST &NewMail)
{
  AppCastingMOOSApp::OnNewMail(NewMail);

  MOOSMSG_LIST::iterator p;
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;
    string key    = msg.GetKey();
    string sval  = msg.GetString();
    reportEvent(sval);


#if 0 // Keep these around just for template
    string comm  = msg.GetCommunity();
    double dval  = msg.GetDouble();
    string msrc  = msg.GetSource();
    double mtime = msg.GetTime();
    bool   mdbl  = msg.IsDouble();
    bool   mstr  = msg.IsString();
#endif

     
    if(key == "VISIT_POINT"){
      Point m_point;
      string sval = msg.GetString();
      m_visit_pts_str.push_back(sval);
    }

    
     else if(key != "APPCAST_REQ") // handled by AppCastingMOOSApp
       reportRunWarning("Unhandled Mail: " + key);
   }
	
   return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool GenPath::OnConnectToServer()
{
   registerVariables();
   return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool GenPath::Iterate()
{
  AppCastingMOOSApp::Iterate();
  
  //creating list of points
  std::list<string>::iterator it;
  for (it = m_visit_pts_str.begin(); it != m_visit_pts_str.end();++it) {
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
    m_points.push_back(m_point);
  }

AR  //handling points now

  // Point bestpoint = *it;
  
  for (std::vector<Point>::iterator it = m_points.begin(); it != m_points.end();it++){
    Point pval = *it;

  //   int min_distance = 10000;
    
  //   for (std::vector<Point>::iterator it = m_points.begin(); it != m_points.end();it++){
  //     Point ipval = *it;

  //     int distance = sqrt((pval.getX()-ipval.getX())^2+(pval.getY()-ipval.getY())^2)


  // 	if (distance < min_distance) {
  // 	  min_distance = distance;
  // 	  bestpoint = ipval;
  // 	}

  //     m_points.
  //   }
    
    m_visit_points.add_vertex(pval.getX(), pval.getY());
   }

   string update_str = "points = ";
   update_str+= m_visit_points.get_spec();

   Notify("UPDATES_VAR", update_str);

   
     
  // Do your thing here!
  AppCastingMOOSApp::PostReport();
  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool GenPath::OnStartUp()
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

    bool handled = false;
    if(param == "foo") {
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

void GenPath::registerVariables()
{
  AppCastingMOOSApp::RegisterVariables();
  Register("VISIT_POINT", 0);
}


//------------------------------------------------------------
// Procedure: buildReport()

bool GenPath::buildReport() 
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




