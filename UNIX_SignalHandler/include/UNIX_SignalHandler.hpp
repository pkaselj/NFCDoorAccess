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

#ifndef UNIX_SIGNAL_HANDLER_HPP
#define UNIX_SIGNAL_HANDLER_HPP


#include<signal.h>

#include<unordered_map>

#include"Kernel.hpp"

class SignalHandlerFunctor;

// TODO Make MT safe
class UNIX_SignalHandler
{
public:
    typedef enum : int // TODO add real-time signals
    {
        enuSIGABRT = SIGABRT,
        enuSIGALRM = SIGALRM,
        enuSIGBUS = SIGBUS,
        enuSIGCHLD = SIGCHLD,
        enuSIGCLD = SIGCLD,
        enuSIGCONT = SIGCONT,
        // enuSIGEMT = SIGEMT,
        enuSIGFPE = SIGFPE,
        enuSIGHUP = SIGHUP,
        enuSIGILL = SIGILL,
        // enuSIGINFO = SIGINFO,
        enuSIGPWR = SIGPWR,
        enuSIGINT = SIGINT,
        enuSIGIO = SIGIO,
        enuSIGIOT = SIGIOT,
        enuSIGKILL = SIGKILL,
        // enuSIGLOST = SIGLOST,
        enuSIGPIPE = SIGPIPE,
        enuSIGPOLL = SIGPOLL,
        enuSIGPROF = SIGPROF,
        enuSIGQUIT = SIGQUIT,
        enuSIGSEGV = SIGSEGV,
        enuSIGSTKFLT = SIGSTKFLT,
        enuSIGSTOP = SIGSTOP,
        enuSIGTSTP = SIGTSTP,
        enuSIGSYS = SIGSYS,
        enuSIGTERM = SIGTERM,
        enuSIGTRAP = SIGTRAP,
        enuSIGTTIN = SIGTTIN,
        enuSIGTTOU = SIGTTOU,
        // enuSIGUNUSED = SIGUNUSED,
        enuSIGURG = SIGURG,
        enuSIGUSR1 = SIGUSR1,
        enuSIGUSR2 = SIGUSR2,
        enuSIGVTALRM = SIGVTALRM,
        enuSIGXCPU = SIGXCPU,
        enuSIGXFSZ = SIGXFSZ,
        enuSIGWINCH = SIGWINCH
    } enuSignal;

    /// typedef for signal handler callback function pointer
    typedef void (*signal_handler_function)(int, siginfo_t*, void*);
    typedef struct sigaction signal_handler_properties;

    public:
    /// Set `handler` to handle `signal`
    static void SetSignalHandle(enuSignal signal, signal_handler_function handler); // Unblock signal first TODO?

    /// Set `pFunctor` to handle `signal`
    static void SetSignalHandle(enuSignal signal, SignalHandlerFunctor* pFunctor);

    /// Reset signal handler options to system default for `signal`
    static void ResetSignalHandlerToDefault(enuSignal signal);

    /// Ignore `signal` (Not all signals can be ignored. \see [man signal(7)](https://man7.org/linux/man-pages/man7/signal.7.html) )
    static void IgnoreSignal(enuSignal signal);

    /// Get the pointer to functor which defines handling for `signal` (if none is set the function returns nullptr)
    static SignalHandlerFunctor* getFunctorForSignal(enuSignal signal);

    static std::string getSignalName(enuSignal signal);

    /// Raises/Throws/Sends the `signal` to the process with `PID`
    static void sendSignal(unsigned int PID, enuSignal signal);

    /// Block all blockable signals
    static void BlockAll();

    /// Unblock all blockable signals
    static void UnblockAll();

    /// Block `signal` (thread local?)
    static void BlockSignal(enuSignal signal);

    /// Unblock `signal` (thread local?)
    static void UnblockSignal(enuSignal signal);

    /// Restore last set signal mask (thread local?)
    static void RestoreLastSignalMask();

    // add block and unblock for multiple signals

    //static bool isSignalBlocked(int signal);

    /// On signal raise, value pointed to by pFlag is set to 1
    static void bindSignalToFlag(enuSignal signal, volatile sig_atomic_t* pFlag);

private:
    /// ************ Private (static class) **********************
    UNIX_SignalHandler();
    UNIX_SignalHandler& operator= (UNIX_SignalHandler&);
    UNIX_SignalHandler(UNIX_SignalHandler&);
    /// **********************************************************

    static sigset_t* m_pSignalMask;
    static std::unordered_map<enuSignal, SignalHandlerFunctor*> m_SignalToFunctorMap;

    static void setSignalHandlerFunction(enuSignal signal, signal_handler_function pFunction);
};


/// Base class for Signal handler functors
class SignalHandlerFunctor
{
public:
    virtual void operator()(void* params) = 0;
    void Call(void* params) { (*this)(params); }
};

#endif


