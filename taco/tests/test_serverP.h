
#include <Device.H>
#include <string>

class TestDevice : public Device
{
public:
	TestDevice(const std::string name, long &error);
	~TestDevice();
protected:
	long tacoSendByteArray(void *argin, void *argout, long *error);
};
