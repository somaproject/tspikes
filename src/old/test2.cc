#include <iostream>
#include <stdlib.h>


int testfunc( char* test, int n, int * points, int m){
  for (int i = 0; i < m; i++) {
    test[points[i]] += 100; 
  }
}
using namespace std; 

#define POINTCNT 800000
int main() {
  char test[200*200]; 
  int * test2 = new int[POINTCNT]; 
  
  for (int i = 0; i < POINTCNT; i++) {
    test2[i] = i % (200*200);
  }
  testfunc(test, 200*200, test2, POINTCNT); 
  cout << test[random() % (200*200)] << endl; 
  return 0;

}
