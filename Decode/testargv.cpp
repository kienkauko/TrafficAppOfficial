#include <stdio.h>
#include <iostream> 
#include <stdlib.h>
#include <string.h>
using namespace std; 
  
int main(int argc, char* argv[]) 
{ 
int num;
    cout << "You have entered " << argc 
         << " arguments:" << "\n"; 
 	cout << argv[1] << endl;
       // cout << argv[i] << "\n";
	num = atoi(argv[2]); 
	cout << num << endl;
	num = atoi(argv[3]);
	cout << num << endl;
    return 0; 
} 
