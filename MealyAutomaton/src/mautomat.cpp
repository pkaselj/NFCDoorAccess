


#include "mautomat.h"

#ifndef TRACE_W
#define TRACE_W(ID, MSG) *m_pLogger << std::to_string(ID) + " - " + MSG
#endif
/*!
* --------------------------------------------------------------------
	MAutState
* --------------------------------------------------------------------
*/
MAutTransition * MAutState::getTransition(int iEventID)
{
    for (const auto& p_transition : m_vecTransitions) 
	{
		if(p_transition != NULL)		
			if(p_transition->getEventId() == iEventID)
			{	
				return p_transition;
			}
	}

	return NULL;
}





/*!
* --------------------------------------------------------------------
	MAUTOMAT
* --------------------------------------------------------------------
*/

bool MAutomat::test()
{
  //Test the initial automat state

	if (m_vecAuthStates.size() < 1) 
    {
        TRACE_W(m_siInstId, "MAutomat::init Undefined m_vecTransitions list!");
        m_iErrorStatus = AUT_TABLE_NOK;
		return false;
    }
    
    if (m_iInitialStateID < 0)
    {
        TRACE_W(m_siInstId, "MAutomat::init Undefined m_iInitialStateID < 0!");
        m_iErrorStatus = AUT_INITIAL_STATE_NOK;
		return false;
    }

	m_iCurrentStateID = m_iInitialStateID;
   
    m_bRecursion = false;

	m_bAutInitialized = true;
    m_iErrorStatus = AUT_OK;
	return true;
}





bool MAutomat::processEvent(MAutEvent * event)
{
	if(!isAutInitialized())	 //if something wrong with the automat definition
	{ 
		//TRACE_W(m_siInstId, "ERROR : MAutomat::sendEvent AUTOMAT: not initialized! Call test() before using automat!");
		m_iErrorStatus = AUT_NOK;
		return false;
	}


	//Test if the event is OK
	if (event == NULL )
    {
		TRACE_W(m_siInstId, "ERROR : MAutomat::sendEvent Received event==NULL. m_iCurrentStateID = " + std::to_string(m_iCurrentStateID) );
        this->reset();
        
		m_iErrorStatus = AUT_EVT_UNKNOWN;
		return false;
    }


  	m_iCurrentEventID = event->getEventId();

	saveEvent(event); //Save the current event data (virtual)
	
	//Get the current state object
	MAutState * pCurrentState = getCurrentState();
	
	//Test if the state is OK 
	if (pCurrentState == NULL )
    {
		std::string sMsg = "The automat state cannot be found!";
		TRACE_W(m_siInstId, sMsg);
        this->reset();

		m_iErrorStatus = AUT_TABLE_NOK;
		return false;
    }


	//Find the transition in the current state that corresponds for the currently received event 
	MAutTransition * pTransition = NULL;
	pTransition = pCurrentState->getTransition(m_iCurrentEventID);

	
	//Test if the state is OK 
	if (pTransition == NULL )
    {
		std::string sMsg = "TRANSITION NOT FOUND!";
		TRACE_W(m_siInstId, sMsg);
		
		m_iErrorStatus = AUT_NEXT_STATE_NOK;
		return false;
    }

	
	//Change the current state than call the transition function
	this->m_iCurrentStateID = pTransition->getNextStateId();


   
    // Execute the provided action 
	if(pTransition->getFunctor()!= NULL)
	{	MAutomatFunctor <MAutomat> *pFunctor = pTransition->getFunctor();
		pFunctor->Call(event);
    }
	
	if(event!=NULL) delete event;
     
	m_iErrorStatus = AUT_OK;
	return true;

}

MAutState * MAutomat::getCurrentState()
{
	MAutState * pCurrentState = nullptr;
	for(const auto& pState : m_vecAuthStates)
	{
		if(pState != nullptr)
		{
			if(pState->getStateID() == m_iCurrentStateID)
			{
				pCurrentState = pState;
				break;
			}
		}
	}

	return pCurrentState;
}


/*
	Set the automat to initial state
* --------------------------------------------------------------------



/*!
//Copy the data of the current event so it can be used in automat transitions
*/
void MAutomat::saveEvent(MAutEvent * event)
{
	if(m_pCurrentEvent != nullptr) 
			delete m_pCurrentEvent;
	
	m_pCurrentEvent = new MAutEvent(event->getEventId(),event->getEventDesc(),event->hasData());

	return;
}

//Keeps the pointer to the current Input package
// and delete the old one when the new one come
void MAutomat::saveLastInputPck(std::string sInPCK)
{
	m_sLastInputPCK = sInPCK;
}


//Keeps the pointer to the current Output package
// and delete the old one when the new one come
void MAutomat::saveLastOutputPck(std::string sOutPCK)
{

	
	m_sLastOutputPCK = sOutPCK;		
}



MAutomat::~MAutomat()
{ 
	 while (m_vecAuthStates.empty() == false)
	 {
		delete m_vecAuthStates.back();
		m_vecAuthStates.pop_back();
	 }
			

	if(m_pCurrentEvent!=NULL) 
			delete m_pCurrentEvent;

};

MAutEvent::MAutEvent()
	:	m_iEventID(0), m_sDesc(""), m_bHasData(false)
{}

MAutEvent::MAutEvent(int iEventID, std::string sDesc, bool bHasData)
	:m_iEventID(iEventID), m_sDesc(sDesc), m_bHasData(bHasData) {};// //int IDEvent, std::string sDesc, bool bArgs

MAutEvent::~MAutEvent() {}

int MAutEvent::getEventId() { return m_iEventID; };
std::string MAutEvent::getEventDesc() { return m_sDesc; };
bool MAutEvent::hasData() { return m_bHasData; };

MAutTransition::MAutTransition(int IDEvent, int IDNextState, MAutomat* pAut, bool (MAutomat::* pAction) (MAutEvent* pEvent))
{
	m_iEventID = IDEvent;
	m_iNextStateID = IDNextState;

	pActionFunctor = new MAutomatFunctor <MAutomat>(pAut, pAction);
};

MAutTransition::~MAutTransition() { delete pActionFunctor; };


int MAutTransition::getEventId() { return m_iEventID; };
int MAutTransition::getNextStateId() { return m_iNextStateID; };

//This is the functor that is used for the action funtion call
MAutomatFunctor <MAutomat>* MAutTransition::getFunctor() { return pActionFunctor; };

MAutState::MAutState(int iState)
	: m_iStateID(iState)
{};

MAutState::~MAutState() {
	while (m_vecTransitions.empty() == false)
	{
		delete m_vecTransitions.back();
		m_vecTransitions.pop_back();
	}

};

void MAutState::addTransition(MAutTransition* pT) { m_vecTransitions.push_back(pT); };

int MAutState::getStateID() { return m_iStateID; };

MAutomat::MAutomat(short int siInstId, ILogger* pLogger)
	:	m_siInstId(siInstId), m_pLogger(pLogger), m_pCurrentEvent(NULL),
	m_sLastInputPCK(), m_sLastOutputPCK(), m_bAutInitialized(false), m_bTrace(false), m_iErrorStatus(AUT_OK)
{
	// tmTraceStart.start();
};

bool MAutomat::isAutInitialized() { return m_bAutInitialized; }; //is the MAutomat class defined well?
int MAutomat::getCurrentStateId() { return m_iCurrentStateID; };

int MAutomat::getCurrentEventId() { return m_iCurrentEventID; };
bool MAutomat::isInRecursion() { return m_bRecursion; };

void MAutomat::addAutState(MAutState* p) { m_vecAuthStates.push_back(p); };
