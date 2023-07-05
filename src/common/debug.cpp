#include <iostream>
#include "debug.h"
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")

using namespace std;
void debug_log(string log) {
	cout << log << endl;
};