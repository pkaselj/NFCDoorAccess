#ifndef MAUTOMATFUNCTOR_H
#define MAUTOMATFUNCTOR_H

#include "mautomat.h"


class MAutEvent;
// abstract base class
class MFunctor
{
public:
	
	// two possible functions to call member function. virtual cause derived
	// classes will use a pointer to an object and a pointer to a member function
	// to make the function call
	virtual void operator()(MAutEvent * pEvent)=0;  // call using operator
	virtual void Call(MAutEvent * pEvent)=0;        // call using function

	//virtual ~MFunctor(){}
};


// derived template class
template <class TClass> class MAutomatFunctor : public MFunctor
{
private:
	bool (TClass::*fpt)(MAutEvent * pEvent);   // pointer to member function
	TClass* pt2Object;                  // pointer to object
	
public:
	
	// constructor - takes pointer to an object and pointer to a member and stores
	// them in two private variables
	MAutomatFunctor(TClass* _pt2Object, bool(TClass::*_fpt)(MAutEvent * pEvent))
	{ pt2Object = _pt2Object;  fpt=_fpt; };
	
	// override operator "()"
	virtual void operator()(MAutEvent * pEvent)
    { (*pt2Object.*fpt)(pEvent);};              // execute member function
	
	// override function "Call"
	virtual void Call(MAutEvent * pEvent)
	{ (*pt2Object.*fpt)(pEvent);};             // execute member function

	virtual ~MAutomatFunctor() = default;
};

#endif
