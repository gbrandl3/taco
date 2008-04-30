#ifndef TEST_EXT_SERVER_H
#define TEST_EXT_SERVER_H

#include <TACOServer.h>

namespace Test {
        namespace Ext {
                class Server;
        }
        const unsigned int DEVICE_MAX = 0;
}


class Test::Ext::Server : public ::TACO::Server
{
public:
        Server( const std::string &name, DevLong &error) throw (::TACO::Exception);

        ~Server() throw ();
protected:
	const char *GetClassName();

	void deviceOff(void) throw (::TACO::Exception);

	void deviceOn(void) throw (::TACO::Exception);

	short deviceState( void ) throw (::TACO::Exception);

	void deviceUpdate(void) throw (::TACO::Exception);

	void deviceReset(void) throw (::TACO::Exception);

	void v_Init(void) throw (::TACO::Exception);
};

#endif
