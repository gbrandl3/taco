/*
 * Extensions for the convenient access to TACO
 * Copyright (C) 2002-2013 Sebastian Huber <sebastian-huber@web.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */
#ifndef TACO_ARG_PUT_GET_H
#define TACO_ARG_PUT_GET_H

#include <string>
#include <vector>

#include <stdlib.h>

#include <API.h>

#include <TACOTypes.h>

namespace TACO {
	template<typename T> class ArgBase {
	public:
		static DevArgType type()
		{
			return ::TACO::type<T>();
		}
	};

	template<typename T> class ArgPut : public ArgBase<T> {
	public:
		explicit ArgPut( T i) : t( i)
		{
			// VOID
		}
	
		void* address()
		{
			return static_cast<void*>( &t);
		}

	protected:
		T t;
	};

	template <> class ArgPut<void> : public ArgBase<DevVoid> {
	public:
		explicit ArgPut()
		{
			// VOID
		}
	
		void* address()
		{
			return 0;
		}
	};

	template <> class ArgPut<bool> : public ArgBase<DevBoolean> {
	public:
		explicit ArgPut( bool i) : t( static_cast<DevBoolean>( i))
		{
			// VOID
		}
	
		void* address()
		{
			return static_cast<void*>( &t);
		}

	protected:
		DevBoolean t;
	};

	template <> class ArgPut<std::string> : public ArgBase<DevVarCharArray> {
	public:
		explicit ArgPut( const std::string& i) : s( i)
		{
			// VOID
		}
	
		void* address()
		{
			dv.length = s.size();
			dv.sequence = (dv.length == 0) ? 0 : const_cast<char*>( s.data());
			return static_cast<void*>( &dv);
		}
	
	private:
		const std::string& s;
		DevVarCharArray dv;
	};

	template <> class ArgPut<std::vector<std::string> > : public ArgBase<DevVarStringArray> {
	public:
		explicit ArgPut( const std::vector<std::string>& i) : v( i)
		{
			dv.sequence = 0;
		}
	
		virtual ~ArgPut()
		{
			free( dv.sequence);
		}

		void* address()
		{
			free( dv.sequence);
			dv.sequence = static_cast<DevString*>( malloc( sizeof( DevString*) * v.size()));
			if (dv.sequence == 0) {
				return 0;
			}
			dv.length = v.size();
			for (std::vector<std::string>::size_type i = 0; i < v.size(); ++i) {
				dv.sequence [i] = const_cast<DevString>( v [i].c_str());
			}
			return static_cast<void*>( &dv);
		}
	
	private:
		const std::vector<std::string>& v;
		DevVarStringArray dv;
	};

	template <> class ArgPut<std::vector<DevShort> > : public ArgBase<DevVarShortArray> {
	public:
		explicit ArgPut( const std::vector<DevShort>& i) : v( i)
		{
			// VOID
		}
	
		void* address()
		{
			dv.length = v.size();
			dv.sequence = (dv.length == 0) ? 0 : const_cast<DevShort*>( &v.front());
			return static_cast<void*>( &dv);
		}
	
	private:
		const std::vector<DevShort>& v;
		DevVarShortArray dv;
	};

	template <> class ArgPut<std::vector<DevUShort> > : public ArgBase<DevVarUShortArray> {
	public:
		explicit ArgPut( const std::vector<DevUShort>& i) : v( i)
		{
			// VOID
		}
	
		void* address()
		{
			dv.length = v.size();
			dv.sequence = (dv.length == 0) ? 0 : const_cast<DevUShort*>( &v.front());
			return static_cast<void*>( &dv);
		}
	
	private:
		const std::vector<DevUShort>& v;
		DevVarUShortArray dv;
	};

	template <> class ArgPut<std::vector<DevLong> > : public ArgBase<DevVarLongArray> {
	public:
		explicit ArgPut( const std::vector<DevLong>& i) : v( i)
		{
			// VOID
		}
	
		void* address()
		{
			dv.length = v.size();
			dv.sequence = (dv.length == 0) ? 0 : const_cast<DevLong*>( &v.front());
			return static_cast<void*>( &dv);
		}
	
	private:
		const std::vector<DevLong>& v;
		DevVarLongArray dv;
	};

	template <> class ArgPut<std::vector<DevULong> > : public ArgBase<DevVarULongArray> {
	public:
		explicit ArgPut( const std::vector<DevULong>& i) : v( i)
		{
			// VOID
		}
	
		void* address()
		{
			dv.length = v.size();
			dv.sequence = (dv.length == 0) ? 0 : const_cast<DevULong*>( &v.front());
			return static_cast<void*>( &dv);
		}
	
	private:
		const std::vector<DevULong>& v;
		DevVarULongArray dv;
	};

	template <> class ArgPut<std::vector<DevFloat> > : public ArgBase<DevVarFloatArray> {
	public:
		explicit ArgPut( const std::vector<DevFloat>& i) : v( i)
		{
			// VOID
		}
	
		void* address()
		{
			dv.length = v.size();
			dv.sequence = (dv.length == 0) ? 0 : const_cast<DevFloat*>( &v.front());
			return static_cast<void*>( &dv);
		}
	
	private:
		const std::vector<DevFloat>& v;
		DevVarFloatArray dv;
	};

	template <> class ArgPut<std::vector<DevDouble> > : public ArgBase<DevVarDoubleArray> {
	public:
		explicit ArgPut( const std::vector<DevDouble>& i) : v( i)
		{
			// VOID
		}
	
		void* address()
		{
			dv.length = v.size();
			dv.sequence = (dv.length == 0) ? 0 : const_cast<DevDouble*>( &v.front());
			return static_cast<void*>( &dv);
		}
	
	private:
		const std::vector<DevDouble>& v;
		DevVarDoubleArray dv;
	};

	template<typename T> class ResourcePut : public ArgPut<T> {
	public:
		explicit ResourcePut( const T& i) : ArgPut<T>( i)
		{
			// VOID
		}
	};

	template <> class ResourcePut<std::string> : public ArgPut<DevString> {
	public:
		explicit ResourcePut( const std::string& i) : ArgPut<DevString>( 0), s( i)
		{
			// VOID
		}
	
		void* address()
		{
			t = const_cast<char*>( s.c_str());
			return static_cast<void*>( &t);
		}
	
	private:
		const std::string& s;
	};

	template<typename T> class ArgGetBase : public ArgBase<T> {
	public:
		explicit ArgGetBase() : t()
		{
			// VOID
		}
	
		void* address()
		{
			return static_cast<void*>( &t);
		}
	
		T object()
		{
			return t;
		}

	protected:
		T t;
	};

	template <> class ArgGetBase<void> : public ArgBase<DevVoid> {
	public:
		explicit ArgGetBase()
		{
			// VOID
		}

		void* address()
		{
			return 0;
		}

		void object()
		{
			// VOID
		}
	};

	template <> class ArgGetBase<bool> : public ArgBase<DevBoolean> {
	public:
		explicit ArgGetBase() : b()
		{
			// VOID
		}
	
		void* address()
		{
			return static_cast<void*>( &b);
		}
	
		bool object()
		{
			return static_cast<bool>( b);
		}

	protected:
		DevBoolean b;
	};

	template<typename T> class ArgGet : public ArgGetBase<T> {
	public:
		explicit ArgGet() : ArgGetBase<T>()
		{
			// VOID
		}
	};

	template <> class ArgGet<std::string> : public ArgGetBase<DevVarCharArray> {
	public:
		explicit ArgGet() : ArgGetBase<DevVarCharArray>()
		{
			t.length = 0;
			t.sequence = 0;
		}
		
		~ArgGet()
		{
			if (t.sequence != 0) {
				DevLong e;
				dev_xdrfree( type(), &t, &e);
			}
		}

		void* address()
		{
			if (t.sequence != 0) {
				DevLong e;
				dev_xdrfree( type(), &t, &e);
				t.length = 0;
				t.sequence = 0;
			}
			return static_cast<void*>( &t);
		}

		std::string object()
		{
			if (t.sequence != 0) {
				s.assign( t.sequence, t.sequence + t.length);
			} else {
				s.erase();
			}
			return s;
		}

	private:
		std::string s;
	};

	template <> class ArgGet<std::vector<std::string> > : public ArgGetBase<DevVarStringArray> {
	public:
		explicit ArgGet() : ArgGetBase<DevVarStringArray>()
		{
			t.length = 0;
			t.sequence = 0;
		}
		
		~ArgGet()
		{
			if (t.sequence != 0) {
				DevLong e;
				dev_xdrfree( type(), &t, &e);
			}
		}

		void* address()
		{
			if (t.sequence != 0) {
				DevLong e;
				dev_xdrfree( type(), &t, &e);
				t.length = 0;
				t.sequence = 0;
			}
			return static_cast<void*>( &t);
		}

		std::vector<std::string> object()
		{
			if (t.sequence != 0) {
				v.clear();
				for (unsigned int i = 0; i < t.length; ++i)
					v.push_back(t.sequence[i]);
			} else {
				v.clear();
			}
			return v;
		}

	private:
		std::vector<std::string> v;
	};

	template <> class ArgGet<std::vector<DevShort> > : public ArgGetBase<DevVarShortArray> {
	public:
		explicit ArgGet() : ArgGetBase<DevVarShortArray>()
		{
			t.length = 0;
			t.sequence = 0;
		}
		
		~ArgGet()
		{
			if (t.sequence != 0) {
				DevLong e;
				dev_xdrfree( type(), &t, &e);
			}
		}

		void* address()
		{
			if (t.sequence != 0) {
				DevLong e;
				dev_xdrfree( type(), &t, &e);
				t.length = 0;
				t.sequence = 0;
			}
			return static_cast<void*>( &t);
		}

		std::vector<DevShort> object()
		{
			if (t.sequence != 0) {
				v.assign( t.sequence, t.sequence + t.length);
			} else {
				v.clear();
			}
			return v;
		}

	private:
		std::vector<DevShort> v;
	};

	template <> class ArgGet<std::vector<DevUShort> > : public ArgGetBase<DevVarUShortArray> {
	public:
		explicit ArgGet() : ArgGetBase<DevVarUShortArray>()
		{
			t.length = 0;
			t.sequence = 0;
		}
		
		~ArgGet()
		{
			if (t.sequence != 0) {
				DevLong e;
				dev_xdrfree( type(), &t, &e);
			}
		}

		void* address()
		{
			if (t.sequence != 0) {
				DevLong e;
				dev_xdrfree( type(), &t, &e);
				t.length = 0;
				t.sequence = 0;
			}
			return static_cast<void*>( &t);
		}

		std::vector<DevUShort> object()
		{
			if (t.sequence != 0) {
				v.assign( t.sequence, t.sequence + t.length);
			} else {
				v.clear();
			}
			return v;
		}

	private:
		std::vector<DevUShort> v;
	};

	template <> class ArgGet<std::vector<DevLong> > : public ArgGetBase<DevVarLongArray> {
	public:
		explicit ArgGet() : ArgGetBase<DevVarLongArray>()
		{
			t.length = 0;
			t.sequence = 0;
		}
		
		~ArgGet()
		{
			if (t.sequence != 0) {
				DevLong e;
				dev_xdrfree( type(), &t, &e);
			}
		}

		void* address()
		{
			if (t.sequence != 0) {
				DevLong e;
				dev_xdrfree( type(), &t, &e);
				t.length = 0;
				t.sequence = 0;
			}
			return static_cast<void*>( &t);
		}

		std::vector<DevLong> object()
		{
			if (t.sequence != 0) {
				v.assign( t.sequence, t.sequence + t.length);
			} else {
				v.clear();
			}
			return v;
		}

	private:
		std::vector<DevLong> v;
	};

	template <> class ArgGet<std::vector<DevULong> > : public ArgGetBase<DevVarULongArray> {
	public:
		explicit ArgGet() : ArgGetBase<DevVarULongArray>()
		{
			t.length = 0;
			t.sequence = 0;
		}
		
		~ArgGet()
		{
			if (t.sequence != 0) {
				DevLong e;
				dev_xdrfree( type(), &t, &e);
			}
		}

		void* address()
		{
			if (t.sequence != 0) {
				DevLong e;
				dev_xdrfree( type(), &t, &e);
				t.length = 0;
				t.sequence = 0;
			}
			return static_cast<void*>( &t);
		}

		std::vector<DevULong> object()
		{
			if (t.sequence != 0) {
				v.assign( t.sequence, t.sequence + t.length);
			} else {
				v.clear();
			}
			return v;
		}

	private:
		std::vector<DevULong> v;
	};

	template <> class ArgGet<std::vector<DevFloat> > : public ArgGetBase<DevVarFloatArray> {
	public:
		explicit ArgGet() : ArgGetBase<DevVarFloatArray>()
		{
			t.length = 0;
			t.sequence = 0;
		}
		
		~ArgGet()
		{
			if (t.sequence != 0) {
				DevLong e;
				dev_xdrfree( type(), &t, &e);
			}
		}

		void* address()
		{
			if (t.sequence != 0) {
				DevLong e;
				dev_xdrfree( type(), &t, &e);
				t.length = 0;
				t.sequence = 0;
			}
			return static_cast<void*>( &t);
		}

		std::vector<DevFloat> object()
		{
			if (t.sequence != 0) {
				v.assign( t.sequence, t.sequence + t.length);
			} else {
				v.clear();
			}
			return v;
		}

	private:
		std::vector<DevFloat> v;
	};

	template <> class ArgGet<std::vector<DevDouble> > : public ArgGetBase<DevVarDoubleArray> {
	public:
		explicit ArgGet() : ArgGetBase<DevVarDoubleArray>()
		{
			t.length = 0;
			t.sequence = 0;
		}
		
		~ArgGet()
		{
			if (t.sequence != 0) {
				DevLong e;
				dev_xdrfree( type(), &t, &e);
			}
		}

		void* address()
		{
			if (t.sequence != 0) {
				DevLong e;
				dev_xdrfree( type(), &t, &e);
				t.length = 0;
				t.sequence = 0;
			}
			return static_cast<void*>( &t);
		}

		std::vector<DevDouble> object()
		{
			if (t.sequence != 0) {
				v.assign( t.sequence, t.sequence + t.length);
			} else {
				v.clear();
			}
			return v;
		}

	private:
		std::vector<DevDouble> v;
	};

	template<typename T> class ResourceGet : public ArgGet<T> {
	public:
		explicit ResourceGet() : ArgGet<T>()
		{
			// VOID
		}
	};

	template <> class ResourceGet<std::string> : public ArgGet<DevString> {
	public:
		explicit ResourceGet() : ArgGet<DevString>()
		{
			t = 0;
		}
		
		~ResourceGet()
		{
			if (t != 0) {
				DevLong e;
				dev_xdrfree( type(), &t, &e);
			}
		}

		void* address()
		{
			if (t != 0) {
				DevLong e;
				dev_xdrfree( type(), &t, &e);
				t = 0;
			}
			return static_cast<void*>( &t);
		}

		std::string object()
		{
			if (t != 0) {
				s = t;
			} else {
				s.erase();
			}
			return s;
		}

	private:
		std::string s;
	};
}

#endif // TACO_ARG_PUT_GET_H
