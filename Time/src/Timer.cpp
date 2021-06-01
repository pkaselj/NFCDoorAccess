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

#include<memory.h>

#include<sstream>

#include<iostream>
#include<limits>

#include "Kernel.hpp"

// void default_timeout_callback_function (int signal, siginfo_t* pInfo, void* uu);

// UNIX_SignalHandler::signal_handler_function Timer::global_timeout_callback = nullptr;

std::string toString(const timespec& obj)
{
    return "{ s: " + std::to_string(obj.tv_sec) + ", ns: " + std::to_string(obj.tv_nsec) + " }";
}

const itimerspec Timer::timeout_settings_all_zero = { {0,0},{0,0} };

Timer::Timer(const std::string& name, ILogger* pLogger)
    : m_name(name)

{// Handles nullptr


    // Set logger and name
    m_pLogger = pLogger;
    if(m_pLogger == nullptr)
        m_pLogger = NulLogger::getInstance();

    if(m_name == "")
    {
        *m_pLogger << "Timer name cannot be empty!";
        Kernel::Fatal_Error("Timer name cannot be empty!");
    }


    // Timer (signal) settings
    sigevent timer_signal_settings = {};
    memset(&timer_signal_settings, 0, sizeof(sigevent));

    timer_signal_settings.sigev_value.sival_ptr = reinterpret_cast<void*>(this);
    timer_signal_settings.sigev_notify = SIGEV_THREAD;
    timer_signal_settings.sigev_notify_attributes = nullptr;
    timer_signal_settings.sigev_notify_function = &Timer::executeCallback;

    // Create timer
    int success = timer_create(CLOCK_REALTIME, &timer_signal_settings, &m_timer);
    int _errno = errno;


    if(success != 0)
    {
        *m_pLogger << "Timer could not be created. ERRNO: " + std::to_string(_errno);
        Kernel::Fatal_Error("Timer could not be created. ERRNO: " + std::to_string(_errno));
    }

    // Set current timeout settings to 0
    timeout_settings = timeout_settings_all_zero;

    // Stop timer (set all fields to 0)
    Stop();

    m_status = enuTimerStatus::Stopped;

    *m_pLogger << "Timer: " + name + " successfully created!";
}

Timer::~Timer()
{
    Stop();

    int success = timer_delete(m_timer);
    if (success != 0)
    {
        *m_pLogger << "Timer could not be deleted: " + m_name;
        // DEBUG
        std::cout << "ERROR DELETION";
    }
}


void Timer::executeCallback(sigval sigev_value)
{
    Timer* pTimer = reinterpret_cast<Timer*>(sigev_value.sival_ptr);

    if (pTimer->hasCallback() == false)
    {
        // *(pTimer->m_pLogger) << pTimer->getName() + " Timer - Could not call timeout callback (nullptr)!";
        // Kernel::Warning(pTimer->getName() + " Timer - Could not call timeout callback (nullptr)!");
        return;
    }

    void* callbackArg = sigev_value.sival_ptr;
    (*(pTimer->m_pTimeoutCallback))(callbackArg);
}

void Timer::setTimeout_s(time_t seconds)
{

    timespec interval = Time::getTimespecFrom_s(seconds);
    if (interval == (timespec) {0,0})
    {
        *m_pLogger << m_name + " - Timer timeout set to {0,0} from: " + std::to_string(seconds) + " s";
        Kernel::Warning(m_name + " - Timer timeout set to {0,0} from: " + std::to_string(seconds) + " s");
    }

    timeout_settings.it_value = interval;
    *m_pLogger << "Timer set to: " + toString(interval);
}

void Timer::setTimeout_ns(long nanoseconds)
{

    timespec interval = Time::getTimespecFrom_ns(nanoseconds);
    if (interval == (timespec) { 0, 0 })
    {
        *m_pLogger << m_name + " - Timer timeout set to {0,0} from: " + std::to_string(nanoseconds) + " ns";
        Kernel::Warning(m_name + " - Timer timeout set to {0,0} from: " + std::to_string(nanoseconds) + " ns");
    }

    timeout_settings.it_value = interval;
    *m_pLogger << "Timer set to: " + toString(interval);
}

void Timer::setTimeout_ms(long milliseconds)
{

    timespec interval = Time::getTimespecFrom_ms(milliseconds);
    if (interval == (timespec) { 0, 0 })
    {
        *m_pLogger << m_name + " - Timer timeout set to {0,0} from: " + std::to_string(milliseconds) + " ms";
        Kernel::Warning(m_name + " - Timer timeout set to {0,0} from: " + std::to_string(milliseconds) + " ms");
    }

    timeout_settings.it_value = interval;
    *m_pLogger << "Timer set to: " + toString(interval);
}

void Timer::clearTimeoutSettings()
{
    timeout_settings = timeout_settings_all_zero;
    *m_pLogger << "Timer set to: " + toString(timeout_settings.it_value);
}

void Timer::Start()
{
    if(timeout_settings.it_value.tv_nsec == 0 &&
        timeout_settings.it_value.tv_sec == 0)
        {
            *m_pLogger << "Timer: " + m_name + ", timeout set to 0. Ignoring Timer::Start()";
            Kernel::Warning("Starting timer with timeout set to 0. Check: " + m_pLogger->getName());
        }


    int success = timer_settime(m_timer, 0, &timeout_settings, nullptr);
    if(success != 0)
    {
        std::stringstream error_msg_stream;

        error_msg_stream << "Could not start the timer: "  << m_name << "\n"
                         << "With the following timeout_settings: \n"
                         << "\t it_interval.tv_nsec: " << timeout_settings.it_interval.tv_nsec << "\n"
                         << "\t it_interval.tv_sec: " << timeout_settings.it_interval.tv_sec << "\n"
                         << "\t it_value.tv_nsec: " << timeout_settings.it_value.tv_nsec << "\n"
                         << "\t it_value.tv_sec: " << timeout_settings.it_value.tv_sec << "\n";

        *m_pLogger << error_msg_stream.str();
        Kernel::Fatal_Error(error_msg_stream.str());
    }

    m_status = enuTimerStatus::Started;

    std::stringstream log_msg_stream;

    log_msg_stream   << "Started the timer with the name: "  << m_name << "\n"
                     << "With the following timeout_settings: \n"
                     << "\t it_interval.tv_nsec: " << timeout_settings.it_interval.tv_nsec << "\n"
                     << "\t it_interval.tv_sec: " << timeout_settings.it_interval.tv_sec << "\n"
                     << "\t it_value.tv_nsec: " << timeout_settings.it_value.tv_nsec << "\n"
                     << "\t it_value.tv_sec: " << timeout_settings.it_value.tv_sec << "\n";

    *m_pLogger << log_msg_stream.str();
}

void Timer::Stop()
{
    int status = timer_settime(m_timer, 0, &timeout_settings_all_zero, nullptr);

    if(status != 0)
    {
        *m_pLogger << "Could not stop the timer with the name: " + m_name;
        Kernel::Fatal_Error("Could not stop the timer! Check: " + m_pLogger->getName());
    }

    m_status = enuTimerStatus::Stopped;
    *m_pLogger << "Timer: " + m_name + ", stopped!";
}

itimerspec Timer::Pause()
{
    int status = timer_gettime(m_timer, &timeout_settings);
    Stop();

    if(status != 0)
    {
        *m_pLogger << "Could not pause the timer with the name: " + m_name;
        Kernel::Fatal_Error("Could not pasue the timer! Check: " + m_pLogger->getName());
    }

    m_status = enuTimerStatus::Paused;
    *m_pLogger << "Timer: " + m_name + ", paused!";

    return timeout_settings;
}

void Timer::Reset()
{

    int status = timer_settime(m_timer, 0, &timeout_settings, nullptr);
    int _errno = errno;

    if(status != 0)
    {
        // timeout settings
        *m_pLogger << "Could not reset the timer with the name: " + m_name + ". Errno: " + std::to_string(_errno);
        Kernel::Fatal_Error("Could not reset the timer with the name: " + m_name + ". Errno: " + std::to_string(_errno));
    }
    
    Start();
    *m_pLogger << "Timer: " + m_name + ", restarted with: " + toString(timeout_settings.it_value);
    
}

Timer::enuTimerStatus Timer::getTimerStatus()
{
    if (m_status != enuTimerStatus::Stopped && m_status != enuTimerStatus::Paused)
    {
        itimerspec time_till_timeout = timeout_settings_all_zero;
        int result = timer_gettime(m_timer, &time_till_timeout);
        if (result < 0)
        {
            Kernel::Fatal_Error(m_name + " - timer - could not get time_until_timeout in Timer::getTimerStatus()");
        }

        if (time_till_timeout.it_value == (timespec){0, 0})
        {
            m_status = enuTimerStatus::Expired;
        }
    }
    return m_status;
}

std::string Timer::getName() const
{
    return m_name;
}

void Timer::ignoreAlarmSignals()
{
    UNIX_SignalHandler::IgnoreSignal(UNIX_SignalHandler::enuSignal::enuSIGALRM);
}