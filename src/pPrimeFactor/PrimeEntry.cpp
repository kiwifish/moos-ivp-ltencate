
#include <iostream>   // For terminal I/O using cout                           
#include <math.h>
#include <vector>
#include <cstdlib>

 using namespace std;

 int main(int argc, char **argv)
 {
   int n = atoi(argv[1]);
   std::vector<int> primes;
   


   int i = 2;
   for (i; i <= sqrt(n); i++){
     if (n % i == 0) {
       primes.push_back(i);
       n = n/i;
       i = 2;
       }
     else
       i++;

   }

   return(0);
 }

