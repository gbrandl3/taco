#ifndef TACO_BASIC_ERRORS_H
#define TACO_BASIC_ERRORS_H

#include <DevErrors.h>

#define DevErr_ExecutionDenied 4010
#define DevErr_BadDynamicCast 4011
#define DevErr_UnexpectedException 4012
#define DevErr_RangeError 4017
#define DevErr_InvalidValue 4018
#define DevErr_RuntimeError 4019
#define DevErr_InternalError 4020
#define DevErr_TypeError 4023
#define DevErr_IOError 4024
#define DevErr_SystemError 4025

#ifdef __cplusplus

namespace TACO {
	//! Contains all %TACO errors
	namespace Error {
		/**
		 * Runtime error.
		 *
		 * General runtime error which is not covered by any other error.
		 */
		const DevLong RUNTIME_ERROR = DevErr_RuntimeError;

		/**
		 * Range error.
		 *
		 * A value is out of range, for example:
		 * <pre>
		 * if (value < MIN || MAX < value) {
		 *         throw Exception( %Error::%RANGE_ERROR);
		 * }
		 * </pre>
		 */
		const DevLong RANGE_ERROR = DevErr_RangeError;

		/**
		 * Invalid value.
		 *
		 * For example:
		 * <pre>
		 * enum MyEnum { A, B, C, D };
		 *  ...
		 * switch (enumValue) {
		 * case A:
		 *         // Do something
		 *         break;
		 * case B:
		 *         // Do something else
		 *         break;
		 * default:
		 *         throw Exception( %Error::%INVALID_VALUE, "only enum values A and B are allowed");
		 * }
		 * </pre>
		 */
		const DevLong INVALID_VALUE = DevErr_InvalidValue;

		/**
		 * Memory allocation failed.
		 */
		const DevLong NO_MEMORY = DevErr_InsufficientMemory;

		/**
		 * Unexpected exception.
		 */
		const DevLong UNEXPECTED_EXCEPTION = DevErr_UnexpectedException;

		/**
		 * Input or output error.
		 *
		 * This error is appropriate if received data is invalid or
		 * incomplete, and so on.
		 */
		const DevLong IO_ERROR = DevErr_IOError;

		/**
		 * System error.
		 *
		 * A system call failed, for example:
		 * <pre>
		 * ssize_t r = read( buf, size);
		 * if (r == -1) {
		 *         throw Exception( %Error::%SYSTEM_ERROR, strerror( errno));
		 * }
		 * </pre>
		 */
		const DevLong SYSTEM_ERROR = DevErr_SystemError;

		/**
		 * Type error.
		 *
		 * It is not possible to convert an object into a representation of the required type.
		 */
		const DevLong TYPE_ERROR = DevErr_TypeError;

		/**
		 * Internal error.
		 *
		 * The program is incorrect (has a bug), for example:
		 * <pre>
		 * enum MyEnum { A, B };
		 *  ...
		 * switch (enumValue) {
		 * case A:
		 *         // Do something
		 *         break;
		 * case B:
		 *         // Do something else
		 *         break;
		 * default:
		 *         throw Exception( %Error::%INTERNAL_ERROR, "unexpected enum value");
		 * }
		 * </pre>
		 * If you change the enum and forget to alter the switch statement, the error
		 * might be discoverd at runtime.
		 */
		const DevLong INTERNAL_ERROR = DevErr_InternalError;

		/**
		 * The command execution is not allowed in the current device state.
		 */
		const DevLong EXECUTION_DENIED = DevErr_ExecutionDenied;

		/**
		 * %Command is not implemented.
		 */
		const DevLong COMMAND_NOT_IMPLEMENTED = DevErr_CommandNotImplemented;

		//! Bad dynamic cast
		const DevLong BAD_DYNAMIC_CAST = DevErr_BadDynamicCast;
	}
}

#endif /* __cplusplus */

#endif /* TACO_BASIC_ERRORS_H */
