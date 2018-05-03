/************************************************************/
/*    NAME: Lauren TenCate                                              */
/*    ORGN: MIT                                             */
/*    FILE: BHV_ZigLeg.h                                      */
/*    DATE:                                                 */
/************************************************************/

#ifndef Pulse_HEADER
#define Pulse_HEADER

#include <string>
#include "IvPBehavior.h"
#include "XYRangePulse.h"
#include "ZAIC_PEAK.h"

class BHV_Pulse : public IvPBehavior {
public:
  BHV_Pulse(IvPDomain);
  ~BHV_Pulse() {};
  
  bool         setParam(std::string, std::string);
  void         onSetParamComplete();
  void         onCompleteState();
  void         onIdleState();
  void         onHelmStart();
  void         postConfigStatus();
  void         onRunToIdleState();
  void         onIdleToRunState();
  IvPFunction* onRunState();

protected: // Local Utility functions
  IvPFunction* buildFunctionWithZAIC();
  IvPFunction* buildFunctionWithReflector();
  IvPFunction* buildFunctionSpeed();
  
protected: // Configuration parameters
  double m_arrival_radius;
  double m_desired_speed;
  std::string m_ipf_type;

protected: // State variables
  XYRangePulse m_pulse;
  double m_osy;
  double m_osx;
  double m_time;
  double m_curr_time;
  double m_waypoint;
  double posted_waypoint;
  double diff;
  double post_time;
  int wpt_pulse;
  bool waypoint_new;
  double m_zig_duration;
  double m_zig_angle;
  bool posted;
  double bhv_start;
  double m_heading;
  double m_angle;
  bool first_waypoint;
};

#define IVP_EXPORT_FUNCTION

extern "C" {
  IVP_EXPORT_FUNCTION IvPBehavior * createBehavior(std::string name, IvPDomain domain) 
  {return new BHV_Pulse(domain);}
}
#endif


