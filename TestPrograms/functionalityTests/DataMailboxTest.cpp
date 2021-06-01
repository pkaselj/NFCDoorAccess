#include "DataMailbox.hpp"
#include "Time.hpp"

#include<iostream>

#include <unistd.h>
#include <sys/wait.h>
#include <cstring>
#include <thread>

constexpr int cycles = 3;

using namespace std;

void sender();
void receiver();

int main(int argc, char** argv)
{
	std::thread tReceiver(receiver);
	std::thread tSender(sender);
	
	tSender.join();
	tReceiver.join();

	return 0;
}

void sender()
{

	sleep(1);
	cout << "SENDER START" << endl;
	Logger logger("sender.log");
	DataMailbox mailbox("sender", &logger);
	MailboxReference destination("receiver");

	sleep(2);

	/*
			DatabaseReply dbr[] = { 
			DatabaseReply(),
			DatabaseReply(DatabaseReply::enuStatus::INSUFFICIENT_PERMISSIONS),
			DatabaseReply(5),
			DatabaseReply(-1) 
		};


		for (auto& i : dbr)
		{
			mailbox.send(destination, &i);
			i.DumpSerialData("sender_" + std::to_string(&i - dbr));
			sleep(1);
		}
	*/

	CommandMessage cmd(CommandMessage::enuCommand::ADD);
	InputParameter param(InputParameter::enuType::KeypadPIN, "220100");

	cmd.addParameter(param);

	for (int i = 0; i < cycles; i++)
	{
		mailbox.send(destination, &cmd);
		sleep(1);
	}

	cout << "SENDER END" << endl;
}

void receiver()
{
	cout << "RECEIVER START" << endl;
	Logger logger("receiver.log");
	DataMailbox mailbox("receiver", &logger);
	
	DataMailboxMessage* pReceivedMessage = nullptr;

	mailbox.setRTO_s(2);

	for (int i = 0; i < cycles + 2; i++)
	{
		pReceivedMessage = mailbox.receive(enuReceiveOptions::TIMED);
		std::cout << "INFO: " << pReceivedMessage->getInfo() << std::endl;

		if (pReceivedMessage->getDataType() != MessageDataType::enuType::DataMailboxErrorMessage)
		{
			pReceivedMessage->DumpSerialData("receiver_" + std::to_string(i));
		}
		

		delete pReceivedMessage;
	}
	

	cout << "RECEIVER END" << endl;
}

