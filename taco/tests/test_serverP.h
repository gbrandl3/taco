#ifndef TEST_SERVER_P_H
#define TEST_SERVER_P_H
#include <Device.h>
#include <string>

class TestDevice : public Device
{
public:
	TestDevice(const std::string name, long &error);
	~TestDevice();
protected:
	long tacoSendByteArray(void *argin, void *argout, long *error);
};

#endif
