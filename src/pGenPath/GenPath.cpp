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
#include <exception>
#include <cmath>

bool debug = true;

using namespace std;

//---------------------------------------------------------
// Constructor

GenPath::GenPath()
{
  currentbest = 1e6;
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
  // if (debug) cout << "I'm in Iterate()\n";
  AppCastingMOOSApp::Iterate();
  
  //creating list of points
  std::list<string>::iterator it;
  for (it = m_visit_pts_str.begin(); it != m_visit_pts_str.end();) {
    Point m_point;
    string sval = *it;
    Notify("STRING", sval);
    
    //if (debug) cout << "sval = " << sval << endl;
    vector<string> myvector =parseString(sval, ',');
    //if (debug) cout << "myvector = " << myvector << endl;
    
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
  	m_point.setID(idint);
      }
      // if (debug) cout << "completed logic block\n";
      
    }

    it = m_visit_pts_str.erase(it);
    m_points.push_back(m_point);
  }

  closePoints();


  for(unsigned int i=0; i< m_visit_points.size(); i++) {
    string update_str = "points = ";
    update_str+= m_visit_points.get_spec();
    Notify("UPDATES_VAR", update_str);
    Notify("DEBUG", m_visit_points.get_spec());

  }

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

// Point GenPath::closePointsNew(Point pt)
// {

//   std::vector<Point>::iterator it;
//   for(it = m_points.begin(); it != m_points.end(); it++){
//     Point pt2 = *it;

//     dist = sqrt(pow(pt.getX()-close.getX(),2) + pow(pt.getY()-close.getY(),2));


//   closePointsNew(
// }

void GenPath::closePoints()
{
  
  for (std::vector<Point>::iterator it = m_points.begin(); it != m_points.end();){
    //if (debug) cout << "iterating through list\n";
    vector<Point>::iterator pval = it;
    for (std::vector<Point>::iterator it2 = m_points.begin()+1; it2 != m_points.end();it2++){
      // if (debug) cout << "iterating through 2nd list\n";
      Point close = *it2;
      dist = sqrt(pow(pval->getX()-close.getX(),2) + pow(pval->getY()-close.getY(),2));
      if (debug) cout << "dist" << dist << endl;
      if (debug) cout << "currentbest" << currentbest << endl;
      
      if(dist < currentbest){
	m_x = pval->getX();
	m_y = pval->getY();//save point location
	currentbest = dist;
	if (debug) cout << "found new bestpoint\n";
	it = it2;
      }
      
    }
    
    m_visit_points.add_vertex(m_x, m_y);
    // if (pval != *it)
      m_points.erase(pval); //this is the wrong way to delete things
  }
}

