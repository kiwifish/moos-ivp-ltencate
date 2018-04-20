/************************************************************/
/*    NAME: Lauren TenCate                                              */
/*    ORGN: MIT                                             */
/*    FILE: BHV_Pulse.cpp                                    */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include <cstdlib>
#include "MBUtils.h"
#include "BuildUtils.h"
#include "BHV_Pulse.h"

using namespace std;

//---------------------------------------------------------------
// Constructor

BHV_Pulse::BHV_Pulse(IvPDomain domain) :
  IvPBehavior(domain)
{
  // Provide a default behavior name
  IvPBehavior::setParam("name", "defaultname");

  // Declare the behavior decision space
  m_domain = subDomain(m_domain, "course,speed");

  // Add any variables this behavior needs to subscribe for
  addInfoVars("WPT_INDEX, NAV_X, NAV_Y", "no_warning");

  //initialize variables
  wpt_pulse = 1;
  post_time = 0;
  posted_waypoint = 1;
  waypoint_new = false;
 
}

//---------------------------------------------------------------
// Procedure: setParam()

bool BHV_Pulse::setParam(string param, string val)
{
  // Convert the parameter to lower case for more general matching
  param = tolower(param);

  // Get the numerical value of the param argument for convenience once
  double double_val = atof(val.c_str());
  
  if((param == "pulse_range") && isNumber(val)) {
    m_pulse.set_rad(double_val);// Set local member variables here
    return(true);
  }
  else if (param == "pulse_duration") {
    m_pulse.set_duration(double_val);
    return(true);// return(setBooleanOnString(m_my_bool, val));
  }

  // If not handled above, then just return false;
  return(false);
}

//---------------------------------------------------------------
// Procedure: onSetParamComplete()
//   Purpose: Invoked once after all parameters have been handled.
//            Good place to ensure all required params have are set.
//            Or any inter-param relationships like a<b.

void BHV_Pulse::onSetParamComplete()
{
}

//---------------------------------------------------------------
// Procedure: onHelmStart()
//   Purpose: Invoked once upon helm start, even if this behavior
//            is a template and not spawned at startup

void BHV_Pulse::onHelmStart()
{
}

//---------------------------------------------------------------
// Procedure: onIdleState()
//   Purpose: Invoked on each helm iteration if conditions not met.

void BHV_Pulse::onIdleState()
{
}

//---------------------------------------------------------------
// Procedure: onCompleteState()

void BHV_Pulse::onCompleteState()
{
}

//---------------------------------------------------------------
// Procedure: postConfigStatus()
//   Purpose: Invoked each time a param is dynamically changed

void BHV_Pulse::postConfigStatus()
{
}

//---------------------------------------------------------------
// Procedure: onIdleToRunState()
//   Purpose: Invoked once upon each transition from idle to run state

void BHV_Pulse::onIdleToRunState()
{
}

//---------------------------------------------------------------
// Procedure: onRunToIdleState()
//   Purpose: Invoked once upon each transition from run to idle state

void BHV_Pulse::onRunToIdleState()
{
}

//---------------------------------------------------------------
// Procedure: onRunState()
//   Purpose: Invoked each iteration when run conditions have been met.

IvPFunction* BHV_Pulse::onRunState()
{
  // Part 1: Build the IvP function
  IvPFunction *ipf = 0;

 
  bool ok3;
  m_waypoint = getBufferDoubleVal("WPT_INDEX", ok3);

  m_curr_time = getBufferCurrTime();
  diff = getBufferCurrTime()-post_time;
  waypoint_new = (m_waypoint != posted_waypoint);
  
  postMessage("WAYPOINTZ", m_waypoint);
  postMessage("DIFFTIME", diff);
  postMessage("WAYPOINT_POST", posted_waypoint);
  postMessage("WAYPOINT_NEW", waypoint_new);

  
  if(diff >= 20){
    //change time on new waypoint, not post
    //post time, but don't keep reposting
    if(waypoint_new) {
      posted_waypoint = m_waypoint;
      bool ok1, ok2;
      m_x = getBufferDoubleVal("NAV_X", ok1); //what is ok1?
      m_y = getBufferDoubleVal("NAV_Y", ok2); // what is ok2?
      m_time = getBufferCurrTime();
      
      m_pulse.set_x(m_x);
      m_pulse.set_y(m_y);
      m_pulse.set_time(m_time);
      
      
      m_pulse.set_label("bhv_pulse");
      m_pulse.set_color("edge", "yellow");
      m_pulse.set_color("fill","yellow");
      
      string spec = m_pulse.get_spec();
      postMessage("VIEW_RANGE_PULSE",spec);
    }
    post_time = getBufferCurrTime();
    //updates at new waypoint, but overall behavior doesn't update
    
  }
  
  // if (int(m_waypoint) = wpt_pulse){
   //    if(diff >= 5) {
	
   // 	bool ok1, ok2;
   // 	m_x = getBufferDoubleVal("NAV_X", ok1); // what is ok1?
   // 	m_y = getBufferDoubleVal("NAV_Y", ok2); // what is ok2?
   // 	m_time = getBufferCurrTime();
  
   // 	m_pulse.set_x(m_x);
   // 	m_pulse.set_y(m_y);
   // 	m_pulse.set_time(m_time);

  
   // 	m_pulse.set_label("bhv_pulse");
   // 	m_pulse.set_color("edge", "yellow");
   // 	m_pulse.set_color("fill","yellow");

   // 	string spec = m_pulse.get_spec();
   // 	postMessage("VIEW_RANGE_PULSE",spec);

   // 	wpt_pulse = m_waypoint;
   // 	post_time = getBufferCurrTime();
   //    }
  // }
  
  // Part N: Prior to returning the IvP function, apply the priority wt
   // Actual weight applied may be some value different than the configured
  // m_priority_wt, depending on the behavior author's insite.
  if(ipf)
    ipf->setPWT(m_priority_wt);

  return(ipf);
}

