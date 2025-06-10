/*********************************************************************
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE. 
* 
* class KeyMapPopup                                                  *
*         	                                                         *
* Version: 1.0                                                       *
* Date:    30-05-2025                                                *
* Author:  Dan Machado                                               *
**********************************************************************/
#ifndef _KEYBOAR_CONFIGURATION_H
#define _KEYBOAR_CONFIGURATION_H
#include "extended_popup.h"
#include "key_index.h"
#include "keycombo.h"
#include "basic_scrolled_window.h"

#include <wx/wx.h>

//====================================================================

class KeyMapPopup : public BasicPopup
{
	public:
		KeyMapPopup(wxWindow* parent);
		virtual ~KeyMapPopup()=default;

		void loadKeyMap();

	protected:
		virtual void OnPopup() override;

	private:

		class SymbolRow : public wxPanel
		{
			public:
				SymbolRow(wxWindow* parent, const wxSize& size);
				
				~SymbolRow()
				{
					delete[] m_symbols;
				}

				void addSymbol(wxPanel* panelPtr);

				bool isFull() const
				{
					return m_isFull;
				}

				unsigned char size() const
				{
					return m_idx;
				}

			private:
				wxPanel** m_symbols;
				wxBoxSizer* m_row;
				const unsigned char c_totalSymbols;
				unsigned char m_idx;
				bool m_isFull;
		};

		KeyCombo m_keyCodeGrabber1;
		KeyCombo m_keyCodeGrabber2;

		wxTimer m_configurationTimer;

		wxTextCtrl* m_keyInput;
		wxTextCtrl* m_symbolInput;
		wxTextCtrl* m_keyCheck;
		wxTextCtrl* m_keyGrabber1;
		wxTextCtrl* m_keyGrabber2;
		BasicScrolledWindow* m_symbolList;
		wxStaticText* m_progressTag;

		enum class TIMER_TASK
		{
			NONE,
			KBOARD,
			GET_FOCUS,
			NEW_SYMBOL,
		};

		TIMER_TASK m_state;
		int m_progress;
		int m_oneKeySymbols;
		bool m_skipInput;
		bool m_processComposite;

		void nextStage();

		void unlockCapsNumb();
		void addSymbol(const wxString& symbol);
		void processGrabberInput();
		void processKeyInput(wxKeyEvent& event);
		void processKeyCheck();
		void triggerKey();
		void processComposite();
		void onKeyDownGrabber(wxTextCtrl* keyGrabberCtrl, KeyCombo& keyCodeGrabber, wxKeyEvent& event);

		void OnAutoConfigure(wxTimerEvent& event);

		DECLARE_EVENT_TABLE()
};

//====================================================================

#endif
