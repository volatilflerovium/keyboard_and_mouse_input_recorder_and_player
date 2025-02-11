/*********************************************************************
* class UDPClient                                                    *
*         	                                                         *
* Version: 1.0                                                       *
* Date:    09-02-2025                                                *
* Author:  Dan Machado                                               *
**********************************************************************/
#include "TinyUSB_Link_Lib/udp_client.h"

//====================================================================

UDPClient::UDPClient(const char* ip, uint16_t port)
: m_listeningThreadPtr(nullptr)
, m_fd(::socket(AF_INET, SOCK_DGRAM, 0))
, m_lastError(errno)
, m_running(true)
{
	if(m_fd>-1 && std::strlen(ip)>0){
		m_addr.sin_family = AF_INET;
		m_addr.sin_port = htons(port);
		m_addr.sin_addr.s_addr = inet_addr(ip);

		int cn=::connect(m_fd,	reinterpret_cast<sockaddr*>(&m_addr), sizeof(m_addr));
		if(cn==-1){
			m_lastError=errno;
			m_running=false;
		}
		else{
			m_listeningThreadPtr=new std::thread([this](){
				char buffer[64]; 
				while(m_running){
					receive(buffer, 64);
					m_cv.notify_one();
				}
			});
		}
	}
}

//--------------------------------------------------------------------

UDPClient::~UDPClient()
{
	m_running=false;
	closeConnection();
	if(m_listeningThreadPtr){
		if(m_listeningThreadPtr->joinable()){
			m_listeningThreadPtr->join();
		}
		delete m_listeningThreadPtr;
		m_listeningThreadPtr=nullptr;
	}
}

//--------------------------------------------------------------------

ConnectorI* UDPClient::getConnector(const char* ip, uint16_t port)
{
	static UDPClient* clientPtr=new UDPClient(ip, port);
	static SocketManager manager(clientPtr);
	
	if(!clientPtr->isSameSocket(ip, port)){
		delete clientPtr;
		clientPtr=new UDPClient(ip, port);
		manager.m_client=clientPtr;
	}
	return clientPtr;
}

//--------------------------------------------------------------------

bool UDPClient::isSameSocket(const char* ip, uint port)
{
	if(port==htons(m_addr.sin_port)){
		size_t l=std::strlen(ip);
		if(l!=std::strlen(inet_ntoa(m_addr.sin_addr))){
			return false;
		}
		return std::memcmp(ip, inet_ntoa(m_addr.sin_addr), l)==0;
	}
	return false;
}

//====================================================================
