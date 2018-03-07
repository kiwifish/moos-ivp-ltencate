
#include "stdint.h"
#include <cstdlib> // potentially should've just included these in primeEntry.h but when I was doing that I was getting weird errors about uint64 not being a type
#include "PrimeEntry.h"

using namespace std;

bool  PrimeEntry::factor(unsigned long int max_steps)
{

  uint64_t numerical_val;
  
  int i; //incrementer for counting through loop
 
  if (this->m_started == true && this->m_factors.size()>0){ //started & not prime
    numerical_val = this->getOriginalVal()/this->m_factors.back();
    i = this->m_part_way;
  }
  else if (this->m_started == true) //started & prime
    {
      numerical_val = this->getOriginalVal();
      i = this->m_part_way;
    }
  else //first iteration
    {
      numerical_val = this->getOriginalVal(); // don't really need the accessor here
      i = 2;
    }


  
  for (;i <= int(sqrt(numerical_val)) && max_steps > 0; i++) {

    max_steps = max_steps -1;
    if (numerical_val % i ==0 && i == int(sqrt(numerical_val))){
      this->setFactor(i);
      this->setFactor(i);
      this->setDone(true);
      return false;
    }
    else if (i == int(sqrt(numerical_val))){
      this->setFactor(numerical_val);
      this->setDone(true);
      return false;
    }
    else if (numerical_val % i == 0) { 
      this->setFactor(i);
      cout << i << endl;
      numerical_val = numerical_val/i;
      i = 2;
    }
  
    else
      {
        this->m_start_index = i;
      }

  
    
  }

  return false;
}


string PrimeEntry::getReport()
{
  
  string num_result = "orig=";
  
  ostringstream origss;
  origss << this->m_orig;

  num_result += origss.str();

  num_result += ",received=";

    ostringstream recvss;
  recvss << this->m_recieved_index;

  num_result += recvss.str();

  num_result += ",calculated=";

  ostringstream calcss;
  calcss << this->m_calculated_index;

  num_result += calcss.str();

  num_result += ",solve_time=";

  double calcTime = (this->m_ctime - this->m_rtime);
  ostringstream time;

  time << calcTime;

  num_result += time.str();
  

    //solve time??

  num_result += "primes=";

  for (std::vector<uint64_t>::iterator it = m_factors.begin(); it != m_factors.end(); ++ it)
    {
      ostringstream factor;
      factor << *it;
      
      num_result += factor.str();
      if (it < m_factors.end()-1) {    num_result += ":";}

    }

  num_result += ",username=ltencate";

  return num_result;
    
}
