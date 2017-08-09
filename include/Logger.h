/* This code is part of the CPISync project developed at Boston University.  Please see the README for use and references. */

/* 
 * File:   Logger.h
 *
 * Created on August 17, 2011, 4:49 PM
 */

#ifndef LOGGER_H
#define	LOGGER_H

#ifndef DEFAULT_LOGLEVEL
// default log level ... should be otherwise specified as a preprocessor directive.
// Logger::enabled in Logger.cpp controls whether logger is enabled
#define DEFAULT_LOGLEVEL METHOD_DETAILS
#endif

#include <string>
#include <cstdio>
#include <iostream>
#include <unistd.h>

/**
 * A simplified logging class.
 */
class Logger {

public:
        // Constants
    // ... logging levels
    enum LOG_TYPES {
    NONE = 0,    /** No logging (the default). */
    METHOD = 1, /** Logging information within method calls. */
    METHOD_DETAILS = 2, /** More detailed information related to method calls. */
    COMM = 3, /** Low-level communications logs. */
    COMM_DETAILS = 4 /** More detailed information related to communication logs. */
    };
    
    // Variables
    static const LOG_TYPES logLevel = DEFAULT_LOGLEVEL; /** A number representing the level logging desired.  All logs of level <= logLevel are outputted.*/ 
    
    /**
     * Default constructor
     */
    Logger() {} /** A number representing the level logging desired.  All logs of level <= logLevel are outputted.*/ 
    
    /**
     * Outputs a text string to the log with a given logging level.
     * @param level The logging level.
     * @param text The text to be logged.
     */
    inline static void gLog(int level, string text) {
    if (level <= logLevel) {
        clog << (::getpid()%2==1?">":"") << string(level+1,' ') << "(level=" << level << ")  " << text << endl;
    }
}

    /**
     * Outputs and error message and continues.
     * @param msg The error message.
     */
    static void error(string msg);
    
    /**
     * Outputs an error message and stops execution.
     * @param msg The error message.
     */
    static void error_and_quit(string msg);
 };
#endif	/* LOGGER_H */
