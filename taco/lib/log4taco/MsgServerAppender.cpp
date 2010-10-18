/*
 * MsgServerAppender.cpp
 *
 * Copyright 2007, Jens Krueger <jens.krueger@frm2.tum.de>. All rights reserved.
 *
 * See the COPYING file for the terms of usage and distribution.
 */

#include <log4cpp/Portability.hh>
#include <log4cpp/Category.hh>
#include <log4cpp/FactoryParams.hh>

#include "MsgServerAppender.h"
#include <API.h>

namespace log4taco 
{
	MsgServerAppender::MsgServerAppender(const std::string& name) 
		: LayoutAppender(name)
	{
	}

	MsgServerAppender::~MsgServerAppender() 
	{
    	}

	void MsgServerAppender::close()
	{
	}

	void MsgServerAppender::_append(const log4cpp::LoggingEvent &event)
	{
		std::string message(_getLayout().format(event));
		dev_printdiag(CLEAR, "");
		while (message.length() > 255)
		{
			dev_printdiag(WRITE, message.substr(0, 255).c_str());
			message.erase(0, 255);
		}
		dev_printdiag(SEND, message.c_str());
	}	

	std::auto_ptr<log4cpp::Appender> create_msgserver_appender(const log4cpp::FactoryParams& params)
	{
		std::string name;
		params.get_for("MsgserverAppender").required("name", name);
		return std::auto_ptr<log4cpp::Appender>(new MsgServerAppender(name));
	}
}

