#ifndef MAUTOMAT_H
#define MAUTOMAT_H

#include "mautomatfunctor.h"

#include "Time.hpp"
#include "Kernel.hpp"
#include "NulLogger.hpp"

#include<string>
#include<vector>
#include<mutex>


class MAutomat;
/*!

*/
class MAutEvent
{

public:
	MAutEvent();
	MAutEvent(int iEventID, std::string sDesc, bool bHasData); //int IDEvent, std::string sDesc, bool bArgs
	
	virtual ~MAutEvent();

	int getEventId();
	std::string getEventDesc();
	bool hasData();

protected:
	int m_iEventID;
	std::string m_sDesc;
	bool m_bHasData;
	std::mutex automatonMutex;
};



/*
	A transition is defined by:
		event , 
		next state 
		and the function to be called when the state is to be changed 
*/
class MAutTransition
{
public:

	MAutTransition(int IDEvent, int IDNextState, MAutomat * pAut , bool (MAutomat::*pAction ) (MAutEvent * pEvent));
	~MAutTransition();
	
	
	int getEventId();
	int getNextStateId();

	//This is the functor that is used for the action funtion call
	MAutomatFunctor <MAutomat> * getFunctor(); 
	
private:
	int m_iEventID;
	int m_iNextStateID;
	MAutomatFunctor <MAutomat> *pActionFunctor;//The Functor nedeed for the function pointer
	
};




/*
	The state is identified by ID
		
	Contains a list of all possible transitions that can happend
*/
class MAutState
{
public:
	MAutState(int iState);

	~MAutState();

	void addTransition(MAutTransition * pT);
	MAutTransition * getTransition(int iEventID);

	int getStateID();
private:
	int m_iStateID;
	std::vector <MAutTransition*> m_vecTransitions; //list of MAutTransitions
};



/*

*/
class MAutomat
{
public:
	MAutomat(short int siInstId, ILogger* pLogger = NulLogger::getInstance());
	
//	MAutomat(std::string name, int iType):m_bAutInitialized(false),m_sName(name),m_iAutomatType(iType) {};
	
	virtual ~MAutomat();

	typedef enum
	{
		AUT_OK,
		AUT_TABLE_NOK,
		AUT_INITIAL_STATE_NOK,
		AUT_ALLOC_NOK,
		AUT_FREE_NOK,
		AUT_AUTOMATE_ID_NOK,
		AUT_EVT_UNKNOWN,
		AUT_NEXT_STATE_NOK,
		AUT_DEBUG_NOK,
		AUT_NOK //something else gone wrong
		
	} enumErrorStatusTypes;


	bool test();
	virtual bool reset() = 0;
	virtual bool processEvent(MAutEvent * event);


	bool isAutInitialized(); //is the MAutomat class defined well?
	int getCurrentStateId();
	
	int getCurrentEventId();
	bool isInRecursion ();

	enumErrorStatusTypes getErrorStatus() const { return static_cast<enumErrorStatusTypes>(m_iErrorStatus); }
	void clearErrorStatus() { m_iErrorStatus = AUT_OK; }
	
	
	void setTracing(bool bTrace); //the automat will not trace unles you initialize log file first 
	

	void logToFile(const std::string &s);
		
protected:
	void setTracingFile(const std::string& file_name);

	void addAutState(MAutState * p);
	MAutState * getCurrentState();

	virtual void saveEvent(MAutEvent * event);

	virtual void saveLastInputPck(std::string sInPCK);
	virtual void saveLastOutputPck(std::string sOutPCK);

	short int m_siInstId;

	ILogger * m_pLogger;

	int m_iInitialStateID;
	int m_iCurrentStateID;
	
	int m_iCurrentEventID;

	MAutEvent * m_pCurrentEvent;

	std::string m_sLastInputPCK;
	std::string m_sLastOutputPCK;

	
	
private:
	bool m_bRecursion;
	
	//Lista stanja po kojoj cemo trazit trenutno stanje
	std::vector <MAutState*> m_vecAuthStates; 

	int m_iErrorStatus;
	bool m_bAutInitialized;

	std::string m_sTraceFileNameBase;
	std::string m_sTraceFileNamePath;
	std::string m_datLogFileDate = "";

	bool m_bTrace;
	std::string tmTraceStart = Time::getTime();
	
};







#endif



