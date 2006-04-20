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
		static DevType type()
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

	template <> class ArgPut<std::vector<short> > : public ArgBase<DevVarShortArray> {
	public:
		explicit ArgPut( const std::vector<short>& i) : v( i)
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
		const std::vector<short>& v;
		DevVarShortArray dv;
	};

	template <> class ArgPut<std::vector<unsigned short> > : public ArgBase<DevVarUShortArray> {
	public:
		explicit ArgPut( const std::vector<unsigned short>& i) : v( i)
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
		const std::vector<unsigned short>& v;
		DevVarUShortArray dv;
	};

	template <> class ArgPut<std::vector<long> > : public ArgBase<DevVarLongArray> {
	public:
		explicit ArgPut( const std::vector<long>& i) : v( i)
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
		const std::vector<long>& v;
		DevVarLongArray dv;
	};

	template <> class ArgPut<std::vector<unsigned long> > : public ArgBase<DevVarULongArray> {
	public:
		explicit ArgPut( const std::vector<unsigned long>& i) : v( i)
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
		const std::vector<unsigned long>& v;
		DevVarULongArray dv;
	};

	template <> class ArgPut<std::vector<float> > : public ArgBase<DevVarFloatArray> {
	public:
		explicit ArgPut( const std::vector<float>& i) : v( i)
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
		const std::vector<float>& v;
		DevVarFloatArray dv;
	};

	template <> class ArgPut<std::vector<double> > : public ArgBase<DevVarDoubleArray> {
	public:
		explicit ArgPut( const std::vector<double>& i) : v( i)
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
		const std::vector<double>& v;
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
				v.assign( t.sequence, t.sequence + t.length);
			} else {
				v.clear();
			}
			return v;
		}

	private:
		std::vector<std::string> v;
	};

	template <> class ArgGet<std::vector<short> > : public ArgGetBase<DevVarShortArray> {
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

		std::vector<short> object()
		{
			if (t.sequence != 0) {
				v.assign( t.sequence, t.sequence + t.length);
			} else {
				v.clear();
			}
			return v;
		}

	private:
		std::vector<short> v;
	};

	template <> class ArgGet<std::vector<unsigned short> > : public ArgGetBase<DevVarUShortArray> {
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

		std::vector<unsigned short> object()
		{
			if (t.sequence != 0) {
				v.assign( t.sequence, t.sequence + t.length);
			} else {
				v.clear();
			}
			return v;
		}

	private:
		std::vector<unsigned short> v;
	};

	template <> class ArgGet<std::vector<long> > : public ArgGetBase<DevVarLongArray> {
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

		std::vector<long> object()
		{
			if (t.sequence != 0) {
				v.assign( t.sequence, t.sequence + t.length);
			} else {
				v.clear();
			}
			return v;
		}

	private:
		std::vector<long> v;
	};

	template <> class ArgGet<std::vector<unsigned long> > : public ArgGetBase<DevVarULongArray> {
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

		std::vector<unsigned long> object()
		{
			if (t.sequence != 0) {
				v.assign( t.sequence, t.sequence + t.length);
			} else {
				v.clear();
			}
			return v;
		}

	private:
		std::vector<unsigned long> v;
	};

	template <> class ArgGet<std::vector<float> > : public ArgGetBase<DevVarFloatArray> {
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

		std::vector<float> object()
		{
			if (t.sequence != 0) {
				v.assign( t.sequence, t.sequence + t.length);
			} else {
				v.clear();
			}
			return v;
		}

	private:
		std::vector<float> v;
	};

	template <> class ArgGet<std::vector<double> > : public ArgGetBase<DevVarDoubleArray> {
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

		std::vector<double> object()
		{
			if (t.sequence != 0) {
				v.assign( t.sequence, t.sequence + t.length);
			} else {
				v.clear();
			}
			return v;
		}

	private:
		std::vector<double> v;
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
