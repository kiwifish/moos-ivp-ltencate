#include <string>
#include <cstdlib>
#include <iostream>
#include <sstream>

class Point
{
 public:
  Point();
  ~Point() {};

  void setX (double v)                     {m_x=v;};
  void setY (double v)                     {m_y=v;}; 
  void setID (int v)                       {m_id=v;};
  double getX ()                             {return m_x;};
  double getY ()                             {return m_y;};
  int getID ()                             {return m_id;};

  std::string getReport();
  
 protected: 
  int        m_id;
  double     m_x;
  double     m_y;
};

