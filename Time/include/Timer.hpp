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

#ifndef TIMER_HPP
#define TIMER_HPP

#include"Time.hpp"

#include "UNIX_SignalHandler.hpp"
#include "NulLogger.hpp"

template<class T> class TimerCallbackFunctor;

class Timer
{

    public:

    typedef enum
    {
        Started = 0,
        Paused,
        Stopped,
        Expired,
        Error
    } enuTimerStatus;

    typedef enum
    {
        User_Defined_First,
        Global_First
    } enuCallbackCallOrder;


    /**
     * @brief Create a Timer object
     * @param name GLOBALLY UNIQUE timer name
     * @param pLogger Pointer to an ILogger derived class to log messages to
    */
    Timer(const std::string& name, ILogger* pLogger = NulLogger::getInstance());
    Timer(const Timer&) = delete;

    ~Timer();


    /// Sets the timer timeout callback as defined in `pCallback` functor. Sets callback globally (all timers in current process) - must be taken into account in callback function
    template<class T> void setTimeoutCallback(TimerCallbackFunctor<T>* pCallback)
    {
        m_pTimeoutCallback = pCallback;
    }


    /// Ignore all timer timeout signals (SIGALRM) in current process. DEPRECATED
    static void ignoreAlarmSignals();


    /// Sets timer timeout to `time_t seconds`
    void setTimeout_s(time_t seconds);

    /// Sets timer timeout to `long milliseconds`
    void setTimeout_ms(long milliseconds);

    /// Sets timer timeout to `long nanoseconds`
    void setTimeout_ns(long nanoseconds);


    /// Sets timer settings to 0.
    void clearTimeoutSettings();

    /// Start the timer with settings specified with one of `setTimeout_*()` functions
    void Start();

    /// Stop the timer
    void Stop();

    /// Reset and immediately start the timer.
    void Reset();

    /// Pause the timer and get the `itimerspec` structure which holds time left until timeout
    itimerspec Pause();

    /// Get the timer status. \see enuTimerStatus
    enuTimerStatus getTimerStatus();

    std::string getName() const;

    bool hasCallback() const { return m_pTimeoutCallback != nullptr; }


    private:

    static void executeCallback(sigval sigev_value);

    timer_t m_timer;
    ILogger* m_pLogger;

    const std::string m_name;

    enuTimerStatus m_status;

    itimerspec timeout_settings;
    static const itimerspec timeout_settings_all_zero;


    SignalHandlerFunctor* m_pTimeoutCallback = nullptr;

};

template<class T>
class TimerCallbackFunctor : public SignalHandlerFunctor
{
public:
    typedef void (T::*TfPointer)(void*);
    TimerCallbackFunctor(T* pObject, TfPointer pFunction) : m_pObject(pObject), m_pFunction(pFunction)
    {
        if (m_pObject == nullptr || m_pFunction == nullptr)
        {
            Kernel::Fatal_Error("Invalid pointers in TimerCallback functor");
        }
    };

    virtual ~TimerCallbackFunctor() = default;

    virtual void operator()(void* args)
    {
        (m_pObject->*m_pFunction)(args);
    };


protected:
    T* m_pObject;
    TfPointer m_pFunction;

};

#endif