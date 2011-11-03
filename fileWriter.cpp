/*
 * fileWriter.cpp
 *
 *  Created on: Mar 27, 2011
 *      Author: tony
 *  This program is designed to try race condition of file writing.
 *
 */

#include <iostream>
#include <fstream>
using namespace std;

int main () {
  ofstream myfile;
  myfile.open ("example.txt", ios::app);
  myfile << "Writing this to a file.\n";
  myfile.close();
  return 0;
}
