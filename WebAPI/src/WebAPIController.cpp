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

#include <QCoreApplication>
#include <QTcpServer>
#include <QTcpSocket>

#include <iostream>

const unsigned int PORT = 8080;

class ConnectionManager : public QObject
{
	Q_OBJECT

public:
	ConnectionManager(QObject* pParent = nullptr) : QObject(pParent) {}

private:

	QTcpSocket* m_pSocket = nullptr;

	void HandleConnection();

	std::string receiveData();
	void sendData(const std::string& data);

	std::string parseCommand(const std::string& command);

public slots:
	void ConnectionReceived();

};

int main(int argc, char** argv)
{
	QCoreApplication app(argc, argv);

	std::cout << "Started Qt" << std::endl;

	QTcpServer* pSocket = new QTcpServer(nullptr);
	ConnectionManager* pConnectionManager = new ConnectionManager;

	QObject::connect(pSocket, &QTcpServer::newConnection,
		pConnectionManager, &ConnectionManager::ConnectionReceived);

	if (!pSocket->listen(QHostAddress::Any, PORT))
	{
		std::cout << "Could not start listening on server!" << std::endl;
		return -1;
	};

	return app.exec();
}

void ConnectionManager::ConnectionReceived()
{
	QTcpServer* pServer = (QTcpServer*)sender();
	if (pServer == nullptr)
	{
		std::cout << "Invalid pointer to sender!" << std::endl;
		return;
	}

	m_pSocket = pServer->nextPendingConnection();

	std::cout << "Connected!" << std::endl;



	m_pSocket->close();
}

void HandleConnection()
{
	std::string dataString = "";

	while (dataString != "CLOSE")
	{
		m_pSocket->waitForReadyRead(10000);
		if (m_pSocket->bytesAvailable())
		{
			QByteArray data = pSocket->readAll();

			dataString = data.toStdString();

			std::cout << "Data received: " << dataString << std::endl;

		}
	}

}

void HandleConnection();

std::string receiveData();
void sendData(const std::string& data);

std::string parseCommand(const std::string& command);


#include "WebAPIController.moc"





