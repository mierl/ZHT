/*
 * exec.cpp
 *
 *  Created on: Jan 16, 2012
 *      Author: tony
 */

#include <string>
#include <iostream>
#include <stdio.h>
using namespace std;

string exec(string str) {
    char* cmd = str.c_str();
	FILE* pipe = popen(cmd, "r");
    if (!pipe) return "ERROR";
    char buffer[128];
    string result = "";
    while(!feof(pipe)) {
        if(fgets(buffer, 128, pipe) != NULL)
                result += buffer;
    }
    pclose(pipe);
    return result;
}

int main(){

        const string cmd = "cat /proc/personality.sh | grep BG_PSETORG";
        string result = exec(cmd);
        cout << "Result: "<< result<<endl;

}



