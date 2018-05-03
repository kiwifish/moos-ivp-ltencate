/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: HazardMgr.cpp                                        */
/*    DATE: Oct 26th 2012                                        */
/*                                                               */
/* This file is part of MOOS-IvP                                 */
/*                                                               */
/* MOOS-IvP is free software: you can redistribute it and/or     */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation, either version  */
/* 3 of the License, or (at your option) any later version.      */
/*                                                               */
/* MOOS-IvP is distributed in the hope that it will be useful,   */
/* but WITHOUT ANY WARRANTY; without even the implied warranty   */
/* of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See  */
/* the GNU General Public License for more details.              */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with MOOS-IvP.  If not, see                     */
/* <http://www.gnu.org/licenses/>.                               */
/*****************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "HazardMgr.h"
#include "XYFormatUtilsHazard.h"
#include "XYFormatUtilsPoly.h"
#include "ACTable.h"
#include <cstdlib>

using namespace std;

//---------------------------------------------------------
// Constructor

HazardMgr::HazardMgr()
{
  // Config variables
  m_swath_width_desired = 25;
  m_pd_desired          = 0.9;

  // State Variables 
  m_sensor_config_requested = false;
  m_sensor_config_set   = false;
  m_swath_width_granted = 0;
  m_pd_granted          = 0;

  m_sensor_config_reqs = 0;
  m_sensor_config_acks = 0;
  m_sensor_report_reqs = 0;
  m_detection_reports  = 0;

  m_summary_reports = 0;
  dtime = 0;
  rendevous = false;
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool HazardMgr::OnNewMail(MOOSMSG_LIST &NewMail)
{
  AppCastingMOOSApp::OnNewMail(NewMail);

  MOOSMSG_LIST::iterator p;
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;
    string key   = msg.GetKey();
    string sval  = msg.GetString();
    string comm  = msg.GetCommunity();
    double dval  = msg.GetDouble();
    m_ownship = comm;

#if 0 // Keep these around just for template
    string msrc  = msg.GetSource();
    double mtime = msg.GetTime();
    bool   mdbl  = msg.IsDouble();
    bool   mstr  = msg.IsString();
#endif
    
    if(key == "UHZ_CONFIG_ACK") 
      handleMailSensorConfigAck(sval);

    else if(key == "UHZ_OPTIONS_SUMMARY") 
      handleMailSensorOptionsSummary(sval);

    else if(key == "UHZ_DETECTION_REPORT") 
      handleMailDetectionReport(sval);

    else if(key == "HAZARDSET_REQUEST") 
      handleMailReportRequest();

    else if(key == "UHZ_MISSION_PARAMS") 
      handleMailMissionParams(sval);

    else if(key == "NODE_MESSAGE")
      handleMailNodeMessage(sval);

    else if (key == "UHZ_HAZARD_REPORT"){ 
      if (m_host_community=="jake"){
        handleMailHazardReportJake(sval);
      }
      else if (m_host_community=="kasper"){ //Lauren
        handleMailHazardReportKasper(sval);
      }
    }

    else if(key == "NAV_X")
      m_nav_x = dval;

    else if(key == "NAV_Y")
      m_nav_y = dval;

    else if(key == "RENDEVOUS"){
      reportEvent("handle rendevous mail");
      rendevous = strBegins(sval, "true");
    }
    
    else 
      reportRunWarning("Unhandled Mail: " + key);
  }
	
   return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool HazardMgr::OnConnectToServer()
{
   registerVariables();
   return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool HazardMgr::Iterate()
{
  AppCastingMOOSApp::Iterate();
  
  if(!m_sensor_config_requested)
    postSensorConfigRequest();

  if(m_sensor_config_set)
    postSensorInfoRequest();

  // put 
  // std::list<string>::iterator it;
  // for (it = m_hazards.begin(); it != m_hazards.end();++it) {

  //   string sval = *it;
  //   handleMailHazardReportJake(sval); // just add it on here
  //   it = m_hazards.erase(it);
	    
  // }
  
  
  if((MOOSTime() - dtime) > 100){


    if (m_hazfriend.length() < 100 && (rendevous)){
      NodeMessage node_message;
      node_message.setSourceNode("jake");
      node_message.setDestNode("kasper");
      node_message.setVarName("WAYPOINT");
      node_message.setStringVal(m_hazfriend);
      string msg = node_message.getSpec();
      reportEvent("rendeovous + low message");

      dtime = MOOSTime();
      Notify("NODE_MESSAGE_LOCAL", msg);
      m_hazfriend = "";
      
    }

    else if (rendevous) { // NAV_X > 135
      //check for #
      std::string m_hazfriend_trunc = m_hazfriend.substr(0,99);
      NodeMessage node_message;
      node_message.setSourceNode("jake");
      node_message.setDestNode("kasper");
      node_message.setVarName("WAYPOINT");
      node_message.setStringVal(m_hazfriend_trunc);
      string msg = node_message.getSpec();
      reportEvent("rendevous");

      dtime = MOOSTime();
      Notify("NODE_MESSAGE_LOCAL", msg); 
      m_hazfriend.erase(0,99);
    }
  }

  string update_str = "points= ";
  update_str += waypoint_list.get_spec();
  Notify("UPDATES_VAR", update_str);

  AppCastingMOOSApp::PostReport();
  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool HazardMgr::OnStartUp()
{
  AppCastingMOOSApp::OnStartUp();

  STRING_LIST sParams;
  m_MissionReader.EnableVerbatimQuoting(true);
  if(!m_MissionReader.GetConfiguration(GetAppName(), sParams))
    reportConfigWarning("No config block found for " + GetAppName());

  STRING_LIST::iterator p;
  for(p=sParams.begin(); p!=sParams.end(); p++) {
    string orig  = *p;
    string line  = *p;
    string param = tolower(biteStringX(line, '='));
    string value = line;

    bool handled = false;
    if((param == "swath_width") && isNumber(value)) {
      m_swath_width_desired = atof(value.c_str());
      handled = true;
    }
    else if (param == "ship_name"){
      m_othership = value;
      handled = true;
    }
      
    else if(((param == "sensor_pd") || (param == "pd")) && isNumber(value)) {
      m_pd_desired = atof(value.c_str());
      handled = true;
    }
    else if(param == "report_name") {
      value = stripQuotes(value);
      m_report_name = value;
      handled = true;
    }
    else if(param == "region") {
      XYPolygon poly = string2Poly(value);
      if(poly.is_convex())
	m_search_region = poly;
      handled = true;
    }

    if(!handled)
      reportUnhandledConfigWarning(orig);
  }
  
  m_hazard_set.setSource(m_host_community);
  m_hazard_set.setName(m_report_name);
  m_hazard_set.setRegion(m_search_region);
  
  registerVariables();	
  return(true);
}

//---------------------------------------------------------
// Procedure: registerVariables

void HazardMgr::registerVariables()
{
  AppCastingMOOSApp::RegisterVariables();
  Register("UHZ_DETECTION_REPORT", 0);
  Register("UHZ_CONFIG_ACK", 0);
  Register("UHZ_OPTIONS_SUMMARY", 0);
  Register("UHZ_MISSION_PARAMS", 0);
  Register("HAZARDSET_REQUEST", 0);
  Register("NODE_MESSAGE", 0);
  Register("UHZ_HAZARD_REPORT",0);
  Register("NAV_X",0);
    Register("NAV_Y",0);//Jocelyn
    Register("RENDEVOUS",0);
}

//---------------------------------------------------------
// Procedure: postSensorConfigRequest

void HazardMgr::postSensorConfigRequest()
{
  string request = "vname=" + m_host_community;
  
  request += ",width=" + doubleToStringX(m_swath_width_desired,2);
  request += ",pd="    + doubleToStringX(m_pd_desired,2);

  m_sensor_config_requested = true;
  m_sensor_config_reqs++;
  Notify("UHZ_CONFIG_REQUEST", request);
}

//---------------------------------------------------------
// Procedure: postSensorInfoRequest

void HazardMgr::postSensorInfoRequest()
{
  string request = "vname=" + m_host_community;

  m_sensor_report_reqs++;
  Notify("UHZ_SENSOR_REQUEST", request);
}

//---------------------------------------------------------
// Procedure: handleMailSensorConfigAck

bool HazardMgr::handleMailSensorConfigAck(string str)
{
  // Expected ack parameters:
  string vname, width, pd, pfa, pclass;
  
  // Parse and handle ack message components
  bool   valid_msg = true;
  string original_msg = str;

  vector<string> svector = parseString(str, ',');
  unsigned int i, vsize = svector.size();
  for(i=0; i<vsize; i++) {
    string param = biteStringX(svector[i], '=');
    string value = svector[i];

    if(param == "vname")
      vname = value;
    else if(param == "pd")
      pd = value;
    else if(param == "width")
      width = value;
    else if(param == "pfa")
      pfa = value;
    else if(param == "pclass")
      pclass = value;
    else
      valid_msg = false;       

  }


  if((vname=="")||(width=="")||(pd=="")||(pfa=="")||(pclass==""))
    valid_msg = false;
  
  if(!valid_msg)
    reportRunWarning("Unhandled Sensor Config Ack:" + original_msg);

  
  if(valid_msg) {
    m_sensor_config_set = true;
    m_sensor_config_acks++;
    m_swath_width_granted = atof(width.c_str());
    m_pd_granted = atof(pd.c_str());
  }

  return(valid_msg);
}

//---------------------------------------------------------
// Procedure: handleMailDetectionReport
//      Note: The detection report should look something like:
//            UHZ_DETECTION_REPORT = vname=betty,x=51,y=11.3,label=12 

bool HazardMgr::handleMailDetectionReport(string str)
{
  m_detection_reports++;

  XYHazard new_hazard = string2Hazard(str);
  // new_hazard.setType("hazard");

  string hazlabel = new_hazard.getLabel();
  
  // if(hazlabel == "") {
  //   reportRunWarning("Detection report received for hazard w/out label");
  //   return(false);
  // }

  // int ix = m_hazard_set.findHazard(hazlabel);
  // if(ix == -1)
  //   m_hazard_set.addHazard(new_hazard);
  // else
  //   m_hazard_set.setHazard(ix, new_hazard);

  // string event = "New Detection, label=" + new_hazard.getLabel();
  // event += ", x=" + doubleToString(new_hazard.getX(),1);
  // event += ", y=" + doubleToString(new_hazard.getY(),1);

  // reportEvent(event);

  string req = "vname=" + m_host_community + ",label=" + hazlabel;

  Notify("UHZ_CLASSIFY_REQUEST", req);

  return(true);
}

//---------------------------------------------------------
// Procedure: handleMailHazardReportKasper(string str)
//      Note: handles incoming UHZ_HAZARD_REPORTs for Kasper
//       e.g. UHZ_HAZARD_REPORT = x=51,y=11.3,type=hazard,label=12
//      If classification=hazard, want to add to the hazardset report

void HazardMgr::handleMailHazardReportKasper(string str){ //Lauren
  XYHazard new_hazard = string2Hazard(str); 

  string hazType  = new_hazard.getType(); 
  string hazlabel = new_hazard.getLabel();
  
  if(hazlabel == "") {
    reportRunWarning("Detection report received for hazard w/out label");
  }

  if (hazType == "hazard"){
    int ix = m_hazard_set.findHazard(hazlabel);
    if(ix == -1)
      m_hazard_set.addHazard(new_hazard);
    else
      m_hazard_set.setHazard(ix, new_hazard);
  }
}
//---------------------------------------------------------
// Procedure: handleMailReportRequest

void HazardMgr::handleMailReportRequest()
{
  m_summary_reports++;

  m_hazard_set.findMinXPath(20);
  //unsigned int count    = m_hazard_set.findMinXPath(20);
  string summary_report = m_hazard_set.getSpec("final_report");
  
  Notify("HAZARDSET_REPORT", summary_report);

  NodeMessage node_message;

  //add in hazard info here

  node_message.setSourceNode(m_host_community); //This vehicle
  node_message.setDestNode(m_othership); //Other vehicle
  node_message.setVarName("HAZARDSET_REPORT"); //"WAYPOINT_LIST"
  node_message.setStringVal(summary_report);
  string msg = node_message.getSpec();
  Notify("NODE_MESSAGE_LOCAL", msg);
}


//---------------------------------------------------------
// Procedure: handleMailMissionParams
//   Example: UHZ_MISSION_PARAMS = penalty_missed_hazard=100,               
//                       penalty_nonopt_hazard=55,                
//                       penalty_false_alarm=35,                  
//                       penalty_max_time_over=200,               
//                       penalty_max_time_rate=0.45,              
//                       transit_path_width=25,                           
//                       search_region = pts={-150,-75:-150,-50:40,-50:40,-75}


void HazardMgr::handleMailMissionParams(string str)
{
  vector<string> svector = parseStringZ(str, ',', "{");
  unsigned int i, vsize = svector.size();
  for(i=0; i<vsize; i++) {
    string param = biteStringX(svector[i], '=');
    string value = svector[i];
    // This needs to be handled by the developer. Just a placeholder.
  }
}


//------------------------------------------------------------
// Procedure: buildReport()

bool HazardMgr::buildReport() 
{
  m_msgs << "Config Requested:"                                  << endl;
  m_msgs << "    swath_width_desired: " << m_swath_width_desired << endl;
  m_msgs << "             pd_desired: " << m_pd_desired          << endl;
  m_msgs << "   config requests sent: " << m_sensor_config_reqs  << endl;
  m_msgs << "                  acked: " << m_sensor_config_acks  << endl;
  m_msgs << "------------------------ "                          << endl;
  m_msgs << "Config Result:"                                     << endl;
  m_msgs << "       config confirmed: " << boolToString(m_sensor_config_set) << endl;
  m_msgs << "    swath_width_granted: " << m_swath_width_granted << endl;
  m_msgs << "             pd_granted: " << m_pd_granted          << endl << endl;
  m_msgs << "--------------------------------------------" << endl << endl;

  m_msgs << "               sensor requests: " << m_sensor_report_reqs << endl;
  m_msgs << "             detection reports: " << m_detection_reports  << endl << endl; 

  m_msgs << "   Hazardset Reports Requested: " << m_summary_reports << endl;
  m_msgs << "      Hazardset Reports Posted: " << m_summary_reports << endl;
  m_msgs << "                   Report Name: " << m_report_name << endl;

  return(true);
}

void HazardMgr::handleMailNodeMessage(string str) {
  std::cout << "NOde message" << std::endl;
  vector<string> myvector = parseStringQ(str, ',');
  for(unsigned int i=0; i<myvector.size();i++){ 
      string param = biteString(myvector[i], '=');
      string value = myvector[i];
      if(tolower(param) == "string_val")
	handleMailWaypoint(value);
    }
 
}

// Jocelyn
void HazardMgr::handleMailHazardReportJake(string str) {
  
  XYHazard new_hazard = string2Hazard(str);

  if (new_hazard.getType() == "hazard"){
      m_hazfriend += "#" + doubleToString(new_hazard.getX(),1);
      m_hazfriend += "," + doubleToString(new_hazard.getY(),1);
    }

  reportEvent(m_hazfriend);
}


//Jocelyn


void HazardMgr::handleMailHazardSetReport(string str) {

  string event = "recieved a hazardset!" + str;

  // reportEvent(event);
  XYHazard new_hazard;
  new_hazard.setType("hazard");

  vector<string> myvector = parseString(str, '#');
  for(unsigned int i=0; i<myvector.size();i++){
    vector<string> myvector2  = parseString(myvector[i], ',');
    for(unsigned int i=0; i<myvector2.size(); i++) {
      string param = biteString(myvector2[i], '=');
      string value = myvector2[i];
      if(tolower(param) == "x"){
	new_hazard.setX(atof(value.c_str()));
      }
      else if(tolower(param) == "y")
        new_hazard.setY(atof(value.c_str()));
      else if(tolower(param) == "label")
        new_hazard.setLabel(value);
    }
    
     m_hazard_set.addHazard(new_hazard);
  }
}

void HazardMgr::handleMailWaypoint(string str) {

  
  double x;
  double y;

  vector<string> myvector = parseString(str, '#');
  for(unsigned int i=0; i<myvector.size();i++){
    string param = biteString(myvector[i], ',');
    string value = myvector[i];
    x = atof(value.c_str()); // check if x & y are flipped
    y = atof(param.c_str());
    
    waypoint_list.add_vertex(x,y);
  }
}






