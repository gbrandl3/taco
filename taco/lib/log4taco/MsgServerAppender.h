#ifndef LOG4TACO_MSGSERVERAPPENDER_H
#define LOG4TACO_MSGSERVERAPPENDER_H

#include <log4cpp/LayoutAppender.hh>
#include <log4cpp/FactoryParams.hh>

namespace log4taco
{
	class MsgServerAppender;
	std::auto_ptr<log4cpp::Appender> create_msgserver_appender(const log4cpp::FactoryParams &);
}

class LOG4CPP_EXPORT log4taco::MsgServerAppender : public log4cpp::LayoutAppender
{
public:
	MsgServerAppender(const std::string &name);
	~MsgServerAppender();

	void close();

protected:
	void _append(const log4cpp::LoggingEvent&);
};


#endif // LOG4TACO_MSGSERVERAPPENDER_H
