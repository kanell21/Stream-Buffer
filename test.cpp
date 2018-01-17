#include <iostream>
#include "stream_buffer.h"
#include <math.h>
using namespace std;
typedef unsigned int ADDRINT; 
int main(){
	
	StreamBufferModule<ADDRINT> sbm(10,10,64);
	ADDRINT address;
	while(1){
	
		cin >> address;
		cout << sbm.access(address) << '\n' ;
	
	}
}

