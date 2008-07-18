#include <log4cpp/AppendersFactory.hh>
#include <log4cpp/BasicConfigurator.hh>
#include <log4cpp/PropertyConfigurator.hh>
#include <log4cpp/Category.hh>

#include <MsgServerAppender.h>
#include <cstdlib>

int main(int argc, char **argv)
{
        log4cpp::AppendersFactory &af = log4cpp::AppendersFactory::getInstance();
        af.registerCreator("MsgserverAppender", log4taco::create_msgserver_appender);

	const char  *logpath = getenv("LOGCONFIG");
        try
        {
                if (!logpath)
                        throw 0;
                log4cpp::PropertyConfigurator::configure(logpath);
        }
        catch (const log4cpp::ConfigureFailure &e)
        {
                std::cerr << e.what() << std::endl;
                logpath = "no";
                log4cpp::BasicConfigurator::configure();
        }
        catch (...)
        {
                logpath = "no";
                log4cpp::BasicConfigurator::configure();
        }
	log4cpp::Category *logStream = &log4cpp::Category::getInstance("taco.system.MsgServerTest");
        logStream->noticeStream() << "using " << logpath << " configuration file" << log4cpp::eol;
	logStream->infoStream() << "finished" << log4cpp::eol;
}

