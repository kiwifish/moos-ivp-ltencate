
#include <string> 
#include <vector> 
#include <cstdint>
#include <math.h>

#define PI 3.14159265
#define sind(x) (sin((x)*PI / 180))
#define cosd(x) (cos((x)*PI / 180))

 class calcAngle
 {

 public: 

   calcAngle();{
     epsilon = 0.00737;
   };
   ~calcAngle() {};

   bool transmissionPath();
   void setReceivedIndex(unsigned int v)    {m_received_index=v;};
   void setCalculatedIndex(unsigned int v)  {m_calculated_index=v;};
   void setDone(bool v)                     {m_done=v;};

   bool   done() {return(m_done);};

   bool   factor(unsigned long int max_steps);

   std::string  getReport();

 protected:
   double        epsilon; //think we don't need this with epsilon
   int           transmission_angle; //int? not sure??
   
   uint64_t      m_start_index;
   uint64_t      m_orig;
   bool          m_done;
   unsigned int  m_received_index;

 }
