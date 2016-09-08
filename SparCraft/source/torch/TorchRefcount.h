#pragma once

#include <atomic>

/*
*  Reference counting for Frames and Replayers.
*
*  Enables frames and replayers to be referenced by Lua variables and other
*  C++ objects at the same time.
*
*  We cannot use a C++ shared_pointer because that doesn't help us with the
*  interaction with Lua.
*/
class RefCounted {
private:
	std::atomic_int refs;

public:
	RefCounted() {
		refs = 1;
	}
	// Destructor needs to be virtual for delete this to work correctly.
	virtual ~RefCounted() {}

	void incref() {
		refs++;
	}
	void decref() {
		if (--refs == 0) delete this;
	}
};