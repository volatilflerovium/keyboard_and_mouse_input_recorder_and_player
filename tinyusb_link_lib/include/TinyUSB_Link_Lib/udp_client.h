/*********************************************************************
* class UDPClient                                                    *
*         	                                                         *
* Version: 1.0                                                       *
* Date:    09-02-2025                                                *
* Author:  Dan Machado                                               *
**********************************************************************/
#ifndef UDP_CLIENT_H
#define UDP_CLIENT_H

#include "TinyUSB_Link_Lib/connector_interface.h"

#include <cstring>
#include <thread>
#include <mutex>
#include <condition_variable>

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

//--------------------------------------------------------------------

class UDPClient : public ConnectorI
{
	struct SocketManager
	{
		SocketManager(UDPClient* client)
		:m_client(client)
		{}

		~SocketManager()
		{
			if(m_client){
				delete m_client;
				m_client=nullptr;
			}
		}

		UDPClient* m_client;
	};

	public:
		UDPClient()=delete;

		UDPClient(const char* ip, uint16_t port);

		virtual ~UDPClient();

		static ConnectorI* getConnector(const char* ip, uint16_t port);

		virtual ssize_t receive(void* buffer, const size_t bufferSize) override;

		virtual ssize_t send(const void* data, const size_t dataSize) override;

		virtual bool receive(int timeout=50) override;

		virtual bool isActive() const override;

		const char* getLastError() const;

		void closeConnection();

	private:
		sockaddr_in m_addr;
		std::mutex m_mtx;
		std::condition_variable m_cv;
		std::thread* m_listeningThreadPtr;
		int m_fd;
		int m_lastError;
		bool m_running;
		bool isSameSocket(const char* ip, uint port);
};

//--------------------------------------------------------------------

inline ssize_t UDPClient::receive(void* buffer, const size_t bufferSize)
{
	auto r=::recv(m_fd, buffer, bufferSize, 0);
	m_lastError=errno;
	//dbg("r: ", r, " ", (char*)buffer);
	return r;
}

//--------------------------------------------------------------------

inline ssize_t UDPClient::send(const void* data, const size_t dataSize)
{
	auto r=::send(m_fd, data, dataSize, 0);
	//m_lastError=errno;
	//dbg("send: ", r, " error: ", errno, " ", strerror(m_lastError));
	return r;
}

//--------------------------------------------------------------------

inline bool UDPClient::receive(int timeout)
{
	std::unique_lock<std::mutex> lck(m_mtx);
	if(m_cv.wait_for(lck, std::chrono::milliseconds(timeout))==std::cv_status::timeout){
		return false;
	}
	return true;
}

//--------------------------------------------------------------------

inline bool UDPClient::isActive() const
{
	return m_fd!=-1 && m_running;
}

//--------------------------------------------------------------------

inline const char* UDPClient::getLastError() const
{
	return strerror(m_lastError);
}

//--------------------------------------------------------------------

inline void UDPClient::closeConnection()
{
	if(m_fd>0){
		shutdown(m_fd, SHUT_RDWR);
		close(m_fd);
		m_fd=-1;
	}
}

//====================================================================

#endif
