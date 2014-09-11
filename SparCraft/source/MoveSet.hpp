#pragma once

#include <stdio.h>
#include "Common.h"

///////////////////////////////////////////////////////////////////////////////
//
// BitSet
//
///////////////////////////////////////////////////////////////////////////////

#ifdef WIN32
	#include <intrin.h>
	#pragma intrinsic(__ll_lshift)
	#pragma intrinsic(__ll_rshift)
	#define __lz(a) countLeadingZeros(a)
	#define __tz(a) countTrailingZeros(a)
	#define __ONE 1ull
	#define __ZERO 0ull
	#define __LSHIFT64(VAL, N) __ll_lshift(VAL, N)
	#define __RSHIFT64(VAL, N) __ll_rshift(VAL, N)
#else
	#define __lz(a) __builtin_clzll(a)
	#define __tz(a) __builtin_ctzll(a)
	#define __ONE 1LLU
	#define __ZERO 0LLU
	#define __LSHIFT64(VAL, N) VAL << N
	#define __RSHIFT64(VAL, N) VAL >> N
#endif

#define LBIT(N) (__LSHIFT64(__ONE, (63-N)))
#define RBIT(N) (__LSHIFT64(__ONE, N))
#define SINGLE_BIT(N) RBIT(N)

//#define REVERSE_ACTION_ITERATOR

class BitSet {

	unsigned long long set;							// 64 bit unsigned int to represent set

public:

	BitSet() : set(0) {}							// default constructor sets to zero
	BitSet(long long unsigned s) : set(s) {}		// constructor which takes a uint64

	// pops the next action (bit from the right)
	int popAction()
	{	
		#ifdef REVERSE_ACTION_ITERATOR
			// get the number of trailing zeros
			int nextAction = 63 - __lz(set);
		#else
			// get the number of trailing zeros
			int nextAction = __tz(set);
		#endif
		
		// set that bit to a zero
		subtract(nextAction);

		return (Action)nextAction;
	}
	
	// peeks at the next action
	int nextAction()
	{	
		#ifdef REVERSE_ACTION_ITERATOR
			// get the number of trailing zeros
			int nextAction = 63 - __lz(set);
		#else
			// get the number of trailing zeros
			int nextAction = __tz(set);
		#endif

		return nextAction;
	}

	int  		operator [] 	(const int bit) 	const	{ return set & SINGLE_BIT(bit) ? 1 : 0; }	// get the bit at i
	BitSet 		operator +  	(const int bit) 	const	{ return BitSet(set | SINGLE_BIT(bit)); }	// member addition
	BitSet 		operator -  	(const int bit) 	const	{ return BitSet(set & ~SINGLE_BIT(bit)); }	// member subtraction
	BitSet 		operator |  	(const BitSet & a) 	const	{ return BitSet(set | a.set); }				// set union
	BitSet 		operator +  	(const BitSet & a)	const 	{ return BitSet(set | a.set); }				// set union
	BitSet 		operator -  	(const BitSet & a)	const 	{ return BitSet(set & ~a.set); }			// set subtraction
	BitSet 		operator &  	(const BitSet & a)	const 	{ return BitSet(set & a.set); }				// set intersection
	BitSet 		operator ~  	()					const 	{ return BitSet(~set); }					// set negation

	bool 		isEmpty			() 					const	{ return set == __ZERO; }					// the set is all zeros
	bool 		contains		(const BitSet a) 	const	{ return (set & a.set) == a.set; }			// completely contain another set
	bool		contains		(const int bit) 	const	{ return (set & SINGLE_BIT(bit)) != __ZERO;}// contains a bit set to 1
	bool		containsAny 	(const BitSet a)	const	{ return (set & ~a.set) != set; }			// does set contain any of
	bool		containsNone 	(const BitSet a)	const	{ return (set & ~a.set) == set; }			// does set contain none of 
	bool 		isSubsetOf		(const BitSet a) 	const	{ return (a.set & set) == set; }			// is a subset of another set
	int 		getBit			(const int bit)		const	{ return (set & SINGLE_BIT(bit)) ? 1 : 0; }	// identical to contains
	void 		add				(const int bit) 			{ set |= SINGLE_BIT(bit); }					// sets a bit to 1
	void 		add				(const BitSet a)			{ set |= a.set; }							// sets all of input set to 1
	void		subtract		(const int bit)				{ set &= ~SINGLE_BIT(bit); }				// set bit to zero
	void 		subtract		(const BitSet a)			{ set &= ~a.set; }							// sets all of input set to 0
	
	int countTrailingZeros(unsigned long long s) const
	{	
		int zeros = 0;
		
		while (!(s & __ONE))
		{
			s = __RSHIFT64(s,1);
			++zeros;
		}
		
		return zeros;
	}
	
	int countLeadingZeros(unsigned long long s) const
	{	
		int zeros = 0;
		unsigned long long __L_ONE = __LSHIFT64(__ONE, 63);
		
		while (!(s & __L_ONE))
		{
			s = __LSHIFT64(s,1);
			++zeros;
		}
		
		return zeros;
	}
	
	int numActions() const
	{
		BitSet t(set);
		int count(0);
		
		while (!t.isEmpty())
		{
			t.popAction();
			++count;
		}
		
		return count;
	}
	
	int randomAction() const
	{
        BitSet s(set);
        int num = s.numActions();
        if (num == 1)
        {
            return s.popAction();
        }
        
        int r = (rand() % (s.numActions() - 1));
        
        Action a = s.popAction();
        
        for (int i=0; i<r; i++)
        {
            a = s.popAction();
        }
        
	    return a;
	}

	void print() const
	{
		// print the bits from left to right
		printf("%ull\n", set);
		for (int i=0; i<64; ++i) { 	printf("%d", (63-i)%10); }
		printf("\n");
		for (int i=0; i<64; ++i) { 	printf("%d", (set & (__LSHIFT64(__ONE, (63 - i))) ? 1 : 0)); }
		printf("\n");
	}
};

typedef BitSet MoveSet;
