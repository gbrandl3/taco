#ifndef	TACO_EXCEPTION_H
#define	TACO_EXCEPTION_H

#include <string>
#include <exception>

#include <TACOBasicErrors.h>

namespace TACO {
	//! Returns the error description string
	std::string errorString( DevLong errorNumber) throw ();

	//! Returns the error description string without date and time
	std::string plainErrorString( DevLong errorNumber) throw ();

	/**
	 * Pushes a message onto the error stack.
	 *
	 * @warning
	 * This functions is only useful for server.
	 * Do not use it directly, use Exception instead.
	 */
	void pushErrorMessage( const std::string& msg) throw ();

	class Exception;
}

//! Extends the error description from the left side (new = msg + old)
TACO::Exception& operator>>( const std::string& msg, TACO::Exception& e) throw ();

/**
 * Standard exception for server and clients.
 *
 * The exception object can be interchanged between multiple server and client objects provided they are derived from
 * TACO::Server or TACO::Client.  You can extend the error description of the exception in order improve the error detection:
 * <pre>
 * try {
 *         setSomething( value);
 * } catch (%TACO::%Exception& e) {
 *         throw "set something failed: " >> e;
 * }
 * </pre>
 */
class TACO::Exception : public std::exception {
public:
	friend Exception& (::operator >>) ( const std::string& msg, Exception& e) throw ();

	//! Creates an exception object and sets the error number of the exception
	Exception( DevLong errorNumber = 0) throw ();

	//! Creates an exception object and sets the error number and description of the exception
	Exception( DevLong errorNumber, const std::string& errorDescription) throw ();

	//! Copy constructor
	Exception( const Exception& e) throw ();

	//! Assignment operator
	Exception& operator=( const Exception& e) throw ();

	//! Destoys the exception object
	~Exception() throw ()
	{
		// VOID
	}

	//! Converts to DevLong automatically
	operator DevLong() const throw ()
	{
		return mErrorNumber;
	}

	//! Extends the error description from the right side (new = old + msg)
	Exception& operator<<( const std::string& msg) throw ();

	//! Returns the error description to the exception
	const char *what() const throw ();

private:
	//! %TACO error number
	DevLong mErrorNumber;

	//! Error description
	std::string mErrorDescription;
};

#endif	// TACO_EXCEPTION_H
