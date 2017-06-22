/* This code is part of the CPISync project developed at Boston University.  Please see the README for use and references. */

// GenLog.h -- The class for record the log 
#ifndef GEN_LOG_H
#define GEN_LOG_H

#include <stdio.h>
#include <stdarg.h>
#include <iostream>
#include <unistd.h>

class GenLog
{
public:
	// the no-arg constructor
	GenLog();

	GenLog(int newfd);
	// destructor
	~GenLog();
	
	void gensync_Log();
	void gLog(const char* Format, ...);
	void set_fd(int newfd);
	int get_fd();
	string logbuf;

private:
	int ofd;
};

#endif
