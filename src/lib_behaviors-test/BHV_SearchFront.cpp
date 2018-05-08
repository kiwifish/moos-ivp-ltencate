/************************************************************/
/*    NAME: Undergrad girls                                              */
/*    ORGN: MIT                                             */
/*    FILE: BHV_SearchFront.cpp                                    */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include <cstdlib>
#include "MBUtils.h"
#include "BuildUtils.h"
#include "AngleUtils.h"
#include "BHV_SearchFront.h"
#include "ZAIC_PEAK.h"
#include "OF_Coupler.h"

using namespace std;

//---------------------------------------------------------------
// Constructor

BHV_SearchFront::BHV_SearchFront(IvPDomain domain) :
    IvPBehavior(domain)
{
  IvPBehavior::setParam("name", "searchfront");
  m_domain = subDomain(m_domain, "course,speed");

  // All distances are in meters, all speed in meters per second
  // Default values for configuration parameters 
  m_desired_speed  = 0; 
  m_arrival_radius = 10;
  m_ipf_type       = "zaic";

  // Default values for behavior state variables
  m_x  = 0;
  m_y  = 0;
  post_time = 0;
  m_new_x = 0;
  m_prev_x = 0;
  m_new_y = 50;
  m_prev_y = 0;

  addInfoVars("UCTD_MSMNT_REPORT,NAV_X, NAV_Y");
}

//---------------------------------------------------------------
// Procedure: setParam()

bool BHV_SearchFront::setParam(string param, string val)
{
  // Convert the parameter to lower case for more general matching
  param = tolower(param);

  // Get the numerical value of the param argument for convenience once
  double double_val = atof(val.c_str());
  
  if((param == "foo") && isNumber(val)) {
    // Set local member variables here
    return(true);
  }
  else if (param == "bar") {
    // return(setBooleanOnString(m_my_bool, val));
  }
  else if(param == "ipf_type") {
    val = tolower(val);    
    if((val == "zaic") || (val == "reflector")) {
      m_ipf_type = val;
      return(true);
    }

  // If not handled above, then just return false;
  return(false);
  }
}

//---------------------------------------------------------------
// Procedure: onSetParamComplete()
//   Purpose: Invoked once after all parameters have been handled.
//            Good place to ensure all required params have are set.
//            Or any inter-param relationships like a<b.

void BHV_SearchFront::onSetParamComplete()
{
}

//---------------------------------------------------------------
// Procedure: onHelmStart()
//   Purpose: Invoked once upon helm start, even if this behavior
//            is a template and not spawned at startup

void BHV_SearchFront::onHelmStart()
{
}

//---------------------------------------------------------------
// Procedure: onIdleState()
//   Purpose: Invoked on each helm iteration if conditions not met.

void BHV_SearchFront::onIdleState()
{
}

//---------------------------------------------------------------
// Procedure: onCompleteState()

void BHV_SearchFront::onCompleteState()
{
}

//---------------------------------------------------------------
// Procedure: postConfigStatus()
//   Purpose: Invoked each time a param is dynamically changed

void BHV_SearchFront::postConfigStatus()
{
}

//---------------------------------------------------------------
// Procedure: onIdleToRunState()
//   Purpose: Invoked once upon each transition from idle to run state

void BHV_SearchFront::onIdleToRunState()
{
}

//---------------------------------------------------------------
// Procedure: onRunToIdleState()
//   Purpose: Invoked once upon each transition from run to idle state

void BHV_SearchFront::onRunToIdleState()
{
}


//---------------------------------------------------------------
// Procedure: onRunState()
//   Purpose: Invoked each iteration when run conditions have been met.

IvPFunction* BHV_SearchFront::onRunState()
{
  // Part 1: Build the IvP function
  IvPFunction *ipf = 0;

  bool ok1;
  bool ok2;

  double xwex = getBufferDoubleVal("NAV_X", ok2);
  string sval = getBufferStringVal("UCTD_MSMNT_REPORT", ok1);
  handleMeasurementReport(sval);

  temps.push_back(m_temp);
  x_cords.push_back(m_x);
  y_cords.push_back(m_y);

  //feed this data into a list
  //check the agaisnt the beginning of the list
  //clear point when necessary

  postMessage("FIRST_TEMP",temps.front());
  postMessage("CURRENT_TEMP", m_temp);
  postMessage("XX", x_cords.front());
  postMessage("YY", y_cords.front());
  
  if ((m_temp - temps.front()) > 3){
      postMessage("LOOP", "true");
      bool ok1;
      
      m_new_x = m_x;
      m_prev_x = x_cords.front();
      postMessage("NEW_X", m_new_x);
      postMessage("PREV_X", m_prev_x);
      m_new_y = m_y;
      m_prev_y = y_cords.front();
      postMessage("NEW_Y", m_new_y);
      postMessage("PREV_Y", m_prev_x);

      postMessage("LOOP", "false");
      
       temps.clear();
      // postMessage("TEMP_FRONT", temps.front());
      x_cords.clear();
      y_cords.clear();
      
  }

  std::list<double>::iterator it;
  for (it=temps.begin(); it!=temps.end(); ++it)
    postMessage("UGH", *it);

  
     // realtime -> prev

  // Part N: Prior to returning the IvP function, apply the priority wt
  // Actual weight applied may be some value different than the configured
  // m_priority_wt, depending on the behavior author's insite.
  if(m_ipf_type == "zaic")
    ipf = buildFunctionWithZAIC();
  if(ipf)
    ipf->setPWT(m_priority_wt);

  return(ipf);
}

IvPFunction *BHV_SearchFront::buildFunctionWithZAIC() {
  ZAIC_PEAK spd_zaic(m_domain, "speed");
  spd_zaic.setSummit(10);
  spd_zaic.setPeakWidth(0.5);
  spd_zaic.setBaseWidth(1.0);
  spd_zaic.setSummitDelta(0.8);  
  if(spd_zaic.stateOK() == false) {
    string warnings = "Speed ZAIC problems " + spd_zaic.getWarnings();
    postWMessage(warnings);
    return(0);
  }
  
  double rel_ang_to_wpt = relAng(m_new_x, m_new_y, m_prev_x, m_prev_y);
  postMessage("RELANG", rel_ang_to_wpt);
  ZAIC_PEAK crs_zaic(m_domain, "course");
  crs_zaic.setSummit(rel_ang_to_wpt);
  crs_zaic.setPeakWidth(0);
  crs_zaic.setBaseWidth(180.0);
  crs_zaic.setSummitDelta(0);  
  crs_zaic.setValueWrap(true);
  if(crs_zaic.stateOK() == false) {
    string warnings = "Course ZAIC problems " + crs_zaic.getWarnings();
    postWMessage(warnings);
    return(0);
  }

  IvPFunction *spd_ipf = spd_zaic.extractIvPFunction();
  IvPFunction *crs_ipf = crs_zaic.extractIvPFunction();

  OF_Coupler coupler;
  IvPFunction *ivp_function = coupler.couple(crs_ipf, spd_ipf, 50, 50);

  return(ivp_function);
}

void BHV_SearchFront::handleMeasurementReport(std::string str){
  vector<string> myvector2 = parseString(str, ',');
  for(unsigned int i=0; i<myvector2.size(); i++) {
    string param = biteString(myvector2[i], '=');
    string value = myvector2[i];
    if(tolower(param) == "x"){
      postMessage("X", m_x);
      m_x = atof(value.c_str());}
    else if(tolower(param) == "y"){
      postMessage("Y", m_y);
      m_y = atof(value.c_str());}
    else if(tolower(param) == "temp"){
      m_temp = atof(value.c_str());
      postMessage("temp", m_temp);}
  }
}
