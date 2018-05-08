/************************************************************/
/*    NAME: Undergrad girls                                              */
/*    ORGN: MIT                                             */
/*    FILE: BHV_SearchFront.h                                      */
/*    DATE:                                                 */
/************************************************************/

#ifndef SearchFront_HEADER
#define SearchFront_HEADER

#include <string>
#include "IvPBehavior.h"

class BHV_SearchFront : public IvPBehavior {
public:
  BHV_SearchFront(IvPDomain);
  ~BHV_SearchFront() {};
  
  bool         setParam(std::string, std::string);
  void         onSetParamComplete();
  void         onCompleteState();
  void         onIdleState();
  void         onHelmStart();
  void         postConfigStatus();
  void         onRunToIdleState();
  void         onIdleToRunState();
  void         handleMeasurementReport(std::string);
  IvPFunction* onRunState();

protected: // Local Utility functions
  IvPFunction* buildFunctionWithZAIC();

protected: // Configuration parameters
  double       m_arrival_radius;
  double       m_desired_speed;
  std::string  m_ipf_type;
  
protected: // State variables
  double m_x;
  double m_y;
  double m_temp;
};

#define IVP_EXPORT_FUNCTION

extern "C" {
  IVP_EXPORT_FUNCTION IvPBehavior * createBehavior(std::string name, IvPDomain domain) 
  {return new BHV_SearchFront(domain);}
}
#endif
