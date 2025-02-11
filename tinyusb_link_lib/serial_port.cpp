/*********************************************************************
* class SerialPort                                                   *
* (base on https://blog.mbedded.ninja/programming/operating-systems/linux/linux-serial-ports-using-c-cpp/)        	                                                         *
*         	                                                         *
* Version: 1.0                                                       *
* Date:    09-02-2025                                                *
* Author:  Dan Machado                                               *                                         *
**********************************************************************/
#include "TinyUSB_Link_Lib/serial_port.h"

#include <string.h>

#include <fcntl.h>   // Contains file controls like O_RDWR
#include <errno.h>   // Error integer and strerror() function

//====================================================================

bool SerialPort::connect(const char* serialPortPath)
{
	m_portPath=serialPortPath;
	m_serialPort=open(serialPortPath, O_RDWR);
	m_isActive=true;

	if(tcgetattr(m_serialPort, &m_tty) != 0) {
		m_isActive=false;
		return false;
	}

	m_tty.c_cflag &= ~PARENB; // Clear parity bit, disabling parity (most common)
	m_tty.c_cflag &= ~CSTOPB; // Clear stop field, only one stop bit used in communication (most common)
	m_tty.c_cflag &= ~CSIZE; // Clear all bits that set the data size
	m_tty.c_cflag |= CS8; // 8 bits per byte (most common)
	m_tty.c_cflag &= ~CRTSCTS; // Disable RTS/CTS hardware flow control (most common)
	m_tty.c_cflag |= CREAD | CLOCAL; // Turn on READ & ignore ctrl lines (CLOCAL = 1)

	m_tty.c_lflag &= ~ICANON;
	m_tty.c_lflag &= ~ECHO; // Disable echo
	m_tty.c_lflag &= ~ECHOE; // Disable erasure
	m_tty.c_lflag &= ~ECHONL; // Disable new-line echo
	m_tty.c_lflag &= ~ISIG; // Disable interpretation of INTR, QUIT and SUSP
	m_tty.c_iflag &= ~(IXON | IXOFF | IXANY); // Turn off s/w flow ctrl
	m_tty.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL); // Disable any special handling of received bytes

	m_tty.c_oflag &= ~OPOST; // Prevent special interpretation of output bytes (e.g. newline chars)
	m_tty.c_oflag &= ~ONLCR; // Prevent conversion of newline to carriage return/line feed
	// m_tty.c_oflag &= ~OXTABS; // Prevent conversion of tabs to spaces (NOT PRESENT ON LINUX)
	// m_tty.c_oflag &= ~ONOEOT; // Prevent removal of C-d chars (0x004) in output (NOT PRESENT ON LINUX)

	setTimeoutAndBaudRate();

	return m_isActive;
}

//--------------------------------------------------------------------

void SerialPort::setTimeoutAndBaudRate()
{
	if(!m_isActive){
		return;
	}

	m_tty.c_cc[VMIN] = 0; // read doesn't block
	m_tty.c_cc[VTIME] = m_readTimeout; // deciseconds to wait for but returning as soon as any data is received.

	// Set in/out baud rate to be 9600
	cfsetispeed(&m_tty, m_baudRate);
	cfsetospeed(&m_tty, m_baudRate);
	// Save tty settings, also checking for error
	
	if(tcsetattr(m_serialPort, TCSANOW, &m_tty) != 0) {
		m_isActive=false;
	}
}

//--------------------------------------------------------------------

bool SerialPort::reset(const char* serialPortPath, unsigned int readTimeout, unsigned int baudRate)
{
	if(m_isActive){
		closeSerial();
	}
	m_portPath=serialPortPath;
	m_readTimeout=readTimeout;
	m_baudRate=baudRate;
	return connect(serialPortPath);
}

//--------------------------------------------------------------------

void SerialPort::closeSerial()
{
	if(m_serialPort>0){
		close(m_serialPort);
		m_serialPort=0;	
	}
	m_isActive=false;
}

//--------------------------------------------------------------------

ConnectorI* SerialPort::getConnector(const char* serialPortPath, unsigned int baudRate)
{
	static SerialPort serial;
	if(serial.isSameSerial(serialPortPath, baudRate)){
		serial.reset(serialPortPath, 1, baudRate);
	}
	return &serial;
}

//--------------------------------------------------------------------

bool SerialPort::isSameSerial(const char* serialPortPath, unsigned int baudRate)
{
	return m_portPath==serialPortPath && m_baudRate==baudRate;
}

//====================================================================
