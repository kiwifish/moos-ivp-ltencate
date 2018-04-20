#include "Point.h" 

using namespace std;

Point::Point()
{
  m_x = 0;
  m_y = 0;
  m_id = 0;
}
string Point::getReport()
{
  string report = "x=";

  ostringstream xstr;
  xstr << this->m_x;

  report += xstr.str();

  report += ",y=";
  
  ostringstream ystr;
  ystr << this->m_y;

  report += ystr.str();

  report += ",unique_id=";

  ostringstream idstr;
  idstr << this->m_id;

  report += idstr.str();
  
  return report;
}
