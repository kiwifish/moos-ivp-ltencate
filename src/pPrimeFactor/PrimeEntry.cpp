
// #include <iostream>   // For terminal I/O using cout                           
// #include <math.h>
// #include <vector>
// #include <string>
#include "stdint.h"
#include <cstdlib>
#include "PrimeEntry.h"

using namespace std;

bool  PrimeEntry::factor(unsigned long int max_steps)
{

  cout << max_steps << endl;
  uint64_t numerical_val;
  
  int i;
  if (this->started())
    {
      numerical_val = this->getFactor();
      this->m_factors.pop_back();
      i = this->m_start_index;
    }
  else
    {
      numerical_val = this->getOriginalVal(); // don't really need the accessor here
      cout << "msg in iterate" << endl;
      i = 2;
    }


  
    for (i; i <= int(sqrt(numerical_val)) && max_steps > 0; i++) {
    max_steps = max_steps -1;
    // cout << max_steps << endl;
    // cout << i << endl;
    cout << i << endl;
    cout << numerical_val << endl;
    // cout << sqrt(numerical_val) << endl;

    if (numerical_val % i ==0 && i == int(sqrt(numerical_val))){
      this->setFactor(i);
      this->setFactor(i);
      this->setDone(true);
      return false;
    }
    else if (i == int(sqrt(numerical_val))){ //switch these functions 
      this->setFactor(numerical_val);
      this->setDone(true);
      return false;
    }
    else if (numerical_val % i == 0) { // oh god why
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

  double calcTime = (this->m_ctime - this->m_rtime)*1000;
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
