/*
 * This code is part of the CPISync project developed at Boston University:
 *      {@link http://nislab.bu.edu}
 * 
 * The code, explanation, references, API, and a demo can be found on this
 * web page.  The main theoretical basis for the approach can be found at
 * {@link http://ipsit.bu.edu/documents/ieee-it3-web.pdf}
 *   Y. Minsky, A. Trachtenberg, and R. Zippel,
 *   "Set Reconciliation with Nearly Optimal Communication Complexity",
 *   IEEE Transactions on Information Theory, 49:9.
 * 
 * Elements of the CPISync project code have been worked on, at various points, by:
 * @author Ari Trachtenberg
 * @author Sachin Agarwal 
 * @author Paul Varghese
 * @author Jiaxi Jin
 * @author Jie Meng
 * @author Alexander Smirnov

 */

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
