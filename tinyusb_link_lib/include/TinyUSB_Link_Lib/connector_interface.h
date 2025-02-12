/*********************************************************************
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE. 
*
* ConnectorI interface                                               *
* class NullConnector                                                *
*         	                                                         *
* Version: 1.0                                                       *
* Date:    09-02-2025                                                *
* Author:  Dan Machado                                               *                                         *
**********************************************************************/
#ifndef _CONNECTOR_INTERFACE_H
#define _CONNECTOR_INTERFACE_H

#include <cstdlib>

//====================================================================

class ConnectorI
{
	public:
		virtual ssize_t receive(void* buffer, const size_t bufferSize)=0;

		virtual ssize_t send(const void* data, const size_t dataSize)=0;

		virtual bool receive(int timeout=50)=0;

		virtual bool isActive() const=0;
};

//====================================================================

class NullConnector final : public ConnectorI
{
	public:
		NullConnector()=default;
		~NullConnector()=default;

		virtual ssize_t receive(void* buffer, const size_t bufferSize) override
		{
			return 0;
		}

		virtual ssize_t send(const void* data, const size_t dataSize) override
		{
			return 0;
		}

		virtual bool receive(int timeout=50) override
		{
			return true;
		}

		virtual bool isActive() const override
		{
			return false;
		}

		static ConnectorI* getConnector()
		{
			static NullConnector nullConnector;
			return &nullConnector;
		}
};

//====================================================================

#endif
