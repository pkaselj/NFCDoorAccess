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