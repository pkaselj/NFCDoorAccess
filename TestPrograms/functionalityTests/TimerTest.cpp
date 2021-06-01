/*
*	 Copyright (C) Petar Kaselj 2021
*
*	 This file is part of NFCDoorAccess.
*
*	 NFCDoorAccess is written by Petar Kaselj as an employee of
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

#include "Timer.hpp"

#include<iostream>

class A
{
public:
	void print(void* v_pTimer)
	{ 
		Timer* pTimer = (Timer*)v_pTimer;
		std::cout << Time::getTime() << " " << pTimer->getName() << " - Timed out!" << std::endl; 
	}
};

int main()
{
	A testAobject;
	TimerCallbackFunctor<A> callback(&testAobject, (TimerCallbackFunctor<A>::TfPointer) &A::print);

	Logger logger("timer.log");

	Timer timer1("timer1", &logger);

	timer1.setTimeout_s(2);

	timer1.setTimeoutCallback(&callback);

	std::cout << Time::getTime() << " - Timer started" << std::endl;
	timer1.Start();

	sleep(10);

}