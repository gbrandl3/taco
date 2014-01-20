/*
 * log4taco.h
 *
 * Copyright 2010-2014, Jens Krueger <jens.krueger@frm2.tum.de>. All rights reserved.
 *
 * See the COPYING file for the terms of usage and distribution.
 */

#ifndef LOG4TACO_H
#define LOG4TACO_H

#include <log4cpp/Category.hh>

extern log4cpp::Category *logStream;

#define ENDLOG          log4cpp::eol
#define DEBUG_STREAM    logStream->debugStream()
#define INFO_STREAM     logStream->infoStream()
#define NOTICE_STREAM   logStream->noticeStream()
#define WARN_STREAM     logStream->warnStream()
#define ERROR_STREAM    logStream->errorStream()
#define FATAL_STREAM    logStream->fatalStream()

#endif
