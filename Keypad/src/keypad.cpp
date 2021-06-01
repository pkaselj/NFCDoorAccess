#include"keypad.hpp"

#include<unistd.h>
#include<cstdio>
#include<cstdlib>
#include<linux/input.h>
#include<linux/input-event-codes.h>
#include<cstring>
#include<sys/stat.h>
#include<fcntl.h>
#include<errno.h>

#define ENTER 96
#define BCKSPC 14

const uint Keypad::m_buffer_size = GlobalProperties::Get().KEYPAD_BUFFER_SIZE;


Keypad::Keypad(const std::string& keypadPath, Pipe* pOutputPipt,ILogger* p_logger)
    :   m_pOutputPipe(pOutputPipt), m_pLogger(p_logger)
{
    if(m_pLogger == nullptr)
        m_pLogger = NulLogger::getInstance();

    if(keypadPath == "")
    {
        *m_pLogger << "Invalid keypad path (empty)!";
        Kernel::Fatal_Error("Invalid keypad path (empty)!");
    }

    if (m_pOutputPipe == nullptr)
    {
        *m_pLogger << keypadPath + " - pipe pointer invalid. (nullptr)!";
        Kernel::Fatal_Error(keypadPath + " - pipe pointer invalid. (nullptr)!");
    }

    /*keypadFile = fopen(keypadPath.c_str() , "r");

    if(keypadFile == NULL)
    {
        *m_pLogger << "Cannot open specified file on path: " + std::string(keypadPath);
        Kernel::Fatal_Error("Cannot open specified file on path: " + std::string(keypadPath));
    }*/

    m_keypadFile_fd = open(keypadPath.c_str(), O_RDONLY | O_NONBLOCK);
    if (m_keypadFile_fd < 0)
    {
        *m_pLogger << "Cannot find file descriptor of a file on path: " + std::string(keypadPath);
        Kernel::Fatal_Error("Cannot find file descriptor of a file on path: " + std::string(keypadPath));
    }

    clearBuffer();


    input_event = new IEVENT;
    if(input_event == NULL)
    {
        *m_pLogger << "Cannot create a new IEVENT!";
        Kernel::Fatal_Error("Cannot create a new IEVENT!");
    }

    std::cout << "Keypad ready!" << std::endl;
}

Keypad::~Keypad(void)
{
    close(m_keypadFile_fd);
    delete input_event;

    *m_pLogger << "Closing keypad!";

}

inline void Keypad::clearBuffer(void) {m_buffer = "";}
inline void Keypad::flushBuffer(void)
{
    if(m_buffer == "") return;

    // DEBUG
    // std::cout << "Keypad read" << std::endl;
    // DEBUG
    
    m_pOutputPipe->send(m_buffer);
    clearBuffer();
}

void Keypad::sendSpecialChar(char character)
{
    if (character == ENTER)
    {
        m_pOutputPipe->send("ENTER");
    }
    else if (character == BCKSPC)
    {
        m_pOutputPipe->send("BCKSPC");
    }
}

inline void Keypad::appendChar(char x) {m_buffer += x;}

void Keypad::readChar(void)
{
    *m_pLogger << "Buffer: " + m_buffer;
    checkBufferOverflow();


    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(m_keypadFile_fd, &readfds);

    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 500 * 1000;

    /*int status = select(1, &readfds, nullptr, nullptr, &timeout);
    std::cout << "MARK" << std::endl;
    if (status < 0)
    {
        *m_pLogger << "Keypad select() error in readChar()";
        Kernel::Fatal_Error("Keypad select() error in readChar()");
    }
    else if (status == 0)
    {
        std::cout << "MARK2" << std::endl;
        return;
    }*/

    
    
    //fread(input_event, sizeof(IEVENT), 1, keypadFile);
    ssize_t dataRead = read(m_keypadFile_fd, input_event, sizeof(IEVENT));

    if (dataRead <= 0)
    {
        return;
    }

    if(input_event->type != EV_KEY || input_event->value != 1) return; // Filter only Key Pressed events

    decodeChar();
}

inline void Keypad::checkBufferOverflow(void)
{
    if(m_buffer.size() >= Keypad::m_buffer_size)
        flushBuffer();
}


void Keypad::decodeChar(void)
{
    char currentCharacter = (char) input_event->code;


    /* switch-case for special characters*/
    switch (currentCharacter)
    {
    case 14: // BACKSPACE
    {
        if (m_buffer != "")
        {
            clearBuffer();
        }
        else
        {
            sendSpecialChar(currentCharacter);
        }
    }
        
    return;

    case 55:
        currentCharacter = '*';
    break;

    case 70 ... 83:
        currentCharacter = characterMap[currentCharacter - CHARACTER_MAP_OFFSET];
    break;

    case 96: // ENTER
        if (m_buffer != "")
        {
            flushBuffer();
        }
        else
        {
            sendSpecialChar(currentCharacter);
        }
    return;

    case 98:
        currentCharacter = '/';
    break;

    default:
        char* errorMessage = new char[100]; 
        sprintf(errorMessage, "Invalid character '%c' (int)%d", currentCharacter, (int) currentCharacter);
        *m_pLogger << errorMessage;
        delete[] errorMessage;
    return;
    }
    
    appendChar(currentCharacter);
}