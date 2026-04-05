#include "LoggerManager.h"
//#include "../FederationManager.h"
#include <iostream>
using namespace SDL_RTI;


LoggerManager::LoggerManager() 
{

}

LoggerManager::~LoggerManager()
{

}

void LoggerManager::setFederationManager(ClientPitch* federationManager)
{
	m_FederationManager = federationManager;
}


void LoggerManager::notifyNewLogger(const std::string message, enumsCommon_Client::levelLoggerEnum level)
{
	//if (m_FederationManager)
	//{
	//	m_FederationManager->notifyNewLogger(message, level);
	//}	
	//else
	//{
	//	//Delete this line after checking the operation of the logger on IOS
	//	std::cout << "LoggerManager::notifyNewLogger --> Check that the logs arrive, message: " << message << std::endl;
	//}
}

void LoggerManager::notifyNewLogger(const std::wstring message, enumsCommon_Client::levelLoggerEnum level)
{
	//std::string messageNew;
	//for (wchar_t wchar : message) {
	//	messageNew.push_back(static_cast<char>(wchar));
	//}

	//if (m_FederationManager)
	//{
	//	m_FederationManager->notifyNewLogger(messageNew, level);
	//}
	//else
	//{
	//	//Delete this line after checking the operation of the logger on IOS
	//	std::cout << "LoggerManager::notifyNewLogger --> Check that the logs arrive, message: " << messageNew << std::endl;
	//}
}