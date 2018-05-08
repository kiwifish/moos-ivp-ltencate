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
  postMessage("XWEX", xwex);
  string sval = getBufferStringVal("UCTD_MSMNT_REPORT", ok1);
  postMessage("STRING", sval);
  handleMeasurementReport(sval);
  postMessage("TEMP", m_temp);

  
  

  // if ((m_temp - m_temp_prev) > 5) {
  //   m_wave_x =
  //   m_wave_y =
  //     }

  //do a figure 8?

  // if ((m_temp - m_temp_prev) > 5) {
    
  // }
  // m_prev_temp = m_temp;
  

  //hang out stationary
  


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
  
  // double rel_ang_to_wpt = relAng(m_osx, m_osy, m_nextpt.x(), m_nextpt.y());
  ZAIC_PEAK crs_zaic(m_domain, "course");
  crs_zaic.setSummit(10);
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
