/************************************************************/
/*    NAME: Lauren TenCate                                              */
/*    ORGN: MIT                                             */
/*    FILE: Odometry.cpp                                        */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "Odometry.h"

using namespace std;

//---------------------------------------------------------
// Constructor

Odometry::Odometry()
{
  //make sure variables are initliazed to the values we expect
   m_first_reading = false; 
    m_current_x = 0;
    m_current_y = 0;
    m_previous_x = 0;
    m_previous_y = 0;
    m_total_distance = 0;
}

//---------------------------------------------------------
// Destructor

Odometry::~Odometry()
{
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool Odometry::OnNewMail(MOOSMSG_LIST &NewMail)
{

  AppCastingMOOSApp::OnNewMail(NewMail);  
  MOOSMSG_LIST::iterator p;
   
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;

    string key = msg.GetKey();
    double dval = msg.GetDouble();

    
    if (key == "NAV_X")
      {
	m_first_reading = true; //this is annoyingly inefficient because it's reset every time through the loop
	m_previous_x = m_current_x;
	m_current_x  = dval;
      }

    if (key == "NAV_Y")
      {
	m_first_reading = true;
	m_previous_y = m_current_y;
	m_current_y  = dval;

      }
    
   }
	
   return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool Odometry::OnConnectToServer()
{
   RegisterVariables();
   return(true);
}

void Odometry::RegisterVariables()
{
  AppCastingMOOSApp::RegisterVariables();
  m_Comms.Register("NAV_X",0);
  m_Comms.Register("NAV_Y",0);
}
//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool Odometry::Iterate()
{

  AppCastingMOOSApp::Iterate();

  if (m_first_reading) //don't calculate unless you actually have a reading
    {
      m_total_distance += sqrt((m_current_x - m_previous_x)*(m_current_x - m_previous_x) + (m_current_y - m_previous_y)*(m_current_y - m_previous_y));
      m_Comms.Notify("ODOMETRY_DIST", m_total_distance);
    }
  else
    {m_total_distance = 0;}
  
  AppCastingMOOSApp::PostReport();
  return(true);
  
}

bool Odometry::buildReport()

{
  m_msgs << m_total_distance << endl;
  return(true);
}


//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool Odometry::OnStartUp()
{
  AppCastingMOOSApp::OnStartUp();  
  list<string> sParams;
  m_MissionReader.EnableVerbatimQuoting(false);
  if(m_MissionReader.GetConfiguration(GetAppName(), sParams)) {
    list<string>::iterator p;
    for(p=sParams.begin(); p!=sParams.end(); p++) {
      string original_line = *p;
      string param = stripBlankEnds(toupper(biteString(*p, '=')));
      string value = stripBlankEnds(*p);

      //boilerplate code for handling tricky configurations; not used here, but kept for reference
      if(param == "FOO") {
        //handled
      }
      else if(param == "BAR") {
        //handled
      }
    }
  }
  
  RegisterVariables();	
  return(true);
}


