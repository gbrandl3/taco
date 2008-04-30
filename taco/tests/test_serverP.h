#ifndef TEST_SERVER_P_H
#define TEST_SERVER_P_H
#include <Device.h>
#include <string>

class TestDevice : public Device
{
public:
	TestDevice(const std::string name, DevLong &error);
	~TestDevice();
protected:
	long tacoSendByteArray(void *argin, void *argout, DevLong *error);
};

class TestDevice2 : public Device
{
public:
       TestDevice2(const std::string name, DevLong &error);
       ~TestDevice2();
protected:
       long tacoRead(void *argin, void *argout, DevLong *error);
};

#endif
