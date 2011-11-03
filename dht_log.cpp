/*   This lib is designed to provide a simple log/print-out mechanism for diagnosing .
 *
 *   Created by Tony.
 */
#include <iostream>
#include <stdio.h>
#include <stdarg.h>
#include <iostream>
#include <fstream>
#include<string>
#include<cstring>

FILE *log_open(string fileName) {
	FILE *logfile;

	// very first thing, open up the logfile and mark that we got in
	// here.  If we can't open the logfile, we're dead.
	logfile = fopen("bbfs.log", "w");
	if (logfile == NULL) {
		perror("logfile");
		exit( EXIT_FAILURE);
	}

	// set logfile to line buffering
	setvbuf(logfile, NULL, _IOLBF, 0);

	return logfile;
}

void log_msg(string fileName, string logInfo) {

	ofstream myfile;
	myfile.open(fileName, ios::app);

	myfile << logInfo;
	myfile.close();

}
