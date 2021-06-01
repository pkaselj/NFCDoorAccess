/*
*	 Copyright (C) Petar Kaselj 2021
*
*	 This file is part of NFCDoorAccess.
*
*	 NFCDoorAccess was edited by Petar Kaselj as an employee of
*	 Emovis tehnologije d.o.o. which allowed its release under
*	 this license.
*
*    NFCDoorAccess is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    NFCDoorAccess is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with NFCDoorAccess.  If not, see <https://www.gnu.org/licenses/>.
*
*/

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
