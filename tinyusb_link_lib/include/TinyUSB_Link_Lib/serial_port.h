/*********************************************************************
* class SerialPort                                                   *
* (base on https://blog.mbedded.ninja/programming/operating-systems/linux/linux-serial-ports-using-c-cpp/)        	                                                         *
*         	                                                         *
* Version: 1.0                                                       *
* Date:    09-02-2025                                                *
* Author:  Dan Machado                                               *                                         *
**********************************************************************/
#ifndef _SERIAL_PORT_H
#define _SERIAL_PORT_H

#include "TinyUSB_Link_Lib/connector_interface.h"

#include <string>

#include <termios.h> // Contains POSIX terminal control definitions
#include <unistd.h>  // write(), read(), close()

//====================================================================

class SerialPort : public ConnectorI
{
	public:
		enum BAUD_RATE
		{
			B_0=B0,
			B_50=B50,
			B_75=B75,
			B_110=B110,
			B_134=B134,
			B_150=B150,
			B_200=B200,
			B_300=B300,
			B_600=B600,
			B_1200=B1200,
			B_1800=B1800,
			B_2400=B2400,
			B_4800=B4800,
			B_9600=B9600,
			B_19200=B19200,
			B_38400=B38400,
			B_57600=B57600,
			B_115200=B115200,
			B_230400=B230400,
			B_460800=B460800
		};

	public:
		SerialPort(unsigned int readTimeout, unsigned int baudRate)
		:m_serialPort(-1)
		, m_readTimeout(readTimeout)
		, m_baudRate(baudRate)
		, m_isActive(false)
		{}

		SerialPort(unsigned int readTimeout, BAUD_RATE baudRate=BAUD_RATE::B_9600)
		:SerialPort(readTimeout, static_cast<unsigned int>(baudRate))
		{}

		SerialPort()
		:SerialPort(10)
		{}

		virtual ~SerialPort();

		static ConnectorI* getConnector(const char* serialPortPath, unsigned int baudRate);

		virtual ssize_t receive(void* buffer, const size_t bufferSize) override;

		virtual ssize_t send(const void* data, const size_t dataSize) override;

		virtual bool receive(int timeout=50) override;

		virtual bool isActive() const override;

		bool connect(const char* serialPortPath);

		bool reset(const char* serialPortPath, unsigned int readTimeout, unsigned int baudRate);
		bool isSameSerial(const char* serialPortPath, unsigned int baudRate);

	private:
		std::string m_portPath;
		struct termios m_tty;
		int m_serialPort;
		unsigned int m_readTimeout; // in deciseconds
		unsigned int m_baudRate;
		bool m_isActive;

		void closeSerial();
		void setTimeoutAndBaudRate();
};

//--------------------------------------------------------------------

inline SerialPort::~SerialPort()
{
	closeSerial();
}

//--------------------------------------------------------------------

inline bool SerialPort::isActive() const
{
	return m_isActive;
}

//--------------------------------------------------------------------

inline ssize_t SerialPort::send(const void* data, const size_t dataSize)
{
	return write(m_serialPort, data, dataSize);
}

//--------------------------------------------------------------------

inline ssize_t SerialPort::receive(void* buffer, const size_t bufferSize)
{
	/*
	// Read bytes. The behaviour of read() (e.g. does it block?,
	// how long does it block for?) depends on the configuration
	// settings above, specifically VMIN and VTIME
	// */
	return read(m_serialPort, buffer, bufferSize);
}

//--------------------------------------------------------------------

inline bool SerialPort::receive(int timeout)
{
	static char recBuffer[64];
	return 0<receive(recBuffer, 64);
}

//====================================================================

#endif
