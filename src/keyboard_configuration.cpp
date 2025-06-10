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
#include "configurator.h"
#include "keyboard_configuration.h"
#include "keyboard_emulator.h"
#include "utilities.h"
#include "wx_utils.h"
#include "enumerations.h"

extern KeyboardEmulatorI* s_KeyboardEmulator;

//====================================================================

KeyMapPopup::KeyMapPopup(wxWindow* parent)
:BasicPopup(parent, "Key Configurating")
, m_configurationTimer(this, WX::CONFIG_TIMER)
, m_state(TIMER_TASK::NONE)
, m_oneKeySymbols(0)
, m_skipInput(false)
, m_processComposite(false)
{
	m_symbolList=new BasicScrolledWindow(this, wxID_ANY, wxDefaultPosition, FromDIP(wxSize(380, 100)));

	KeyboardConfigurator::loadSymbols([this](const char* symbol){
		addSymbol(wxString::FromUTF8(symbol));
	});

	auto waitTag=builder<wxStaticText>(wxID_ANY, wxT("Checking keyboard"));

	m_keyInput=builder<wxTextCtrl>(wxID_ANY, wxT(""),
									wxDefaultPosition, FromDIP(wxSize(30, 30)),
									wxTE_PROCESS_ENTER | wxTE_CENTRE,
									wxDefaultValidator, wxTextCtrlNameStr);
									
	m_keyInput->Bind(wxEVT_TEXT_ENTER, [this](wxCommandEvent& event){
		if(m_processComposite){
			processComposite();
		}
		else{
			event.Skip();
		}
	});

	m_keyInput->Bind(wxEVT_KEY_DOWN, [this](wxKeyEvent& event){
		if(!m_skipInput && !m_processComposite){
			/* we do the key mapping here, not in wxEVT_KEY_UP,
			 * in case the key does not trigger the respective
			 * wxEVT_KEY_UP event (like TAB)
			 * */
			KeyboardConfigurator::mapper(event.GetRawKeyCode());
		}
		event.Skip();
	});

	m_keyInput->Bind(wxEVT_KEY_UP, [this](wxKeyEvent& event){
		if(!m_processComposite){
			if(m_state==TIMER_TASK::GET_FOCUS){
				m_state=TIMER_TASK::NONE;
				m_configurationTimer.Stop();
			}
			processKeyInput(event);
		}
		else{
			event.Skip();
		}
	});

	m_progressTag=builder<wxStaticText>(wxID_ANY, wxT("100%"));

	auto symbolTag=builder<wxStaticText>(wxID_ANY, wxT("Symbol"));

	m_symbolInput=builder<wxTextCtrl>(wxID_ANY, wxT(""), wxDefaultPosition, FromDIP(wxSize(45, 30)),
									wxTE_CENTRE, wxDefaultValidator, wxTextCtrlNameStr);


	m_symbolInput->Bind(wxEVT_KEY_UP, [this](wxKeyEvent& event){
		dbg(event.GetRawKeyCode(), " : ", m_symbolInput->GetValue());
		event.Skip();
	});

	//Grabbers
	
	auto keysTag=builder<wxStaticText>(wxID_ANY, wxT("Keys"));

	m_keyGrabber1=builder<wxTextCtrl>(wxID_ANY, wxT(""), wxDefaultPosition, FromDIP(wxSize(110, 30)),
									wxTE_PROCESS_ENTER, wxDefaultValidator, wxTextCtrlNameStr);

	m_keyGrabber1->Bind(wxEVT_TEXT_ENTER, [this](wxCommandEvent& event){
		processGrabberInput();
	});

	m_keyGrabber1->Bind(wxEVT_KEY_DOWN, [this](wxKeyEvent& event){
		onKeyDownGrabber(m_keyGrabber1, m_keyCodeGrabber1, event);
	});

	m_keyGrabber2=builder<wxTextCtrl>(wxID_ANY, wxT(""), wxDefaultPosition, FromDIP(wxSize(110, 30)),
									wxTE_PROCESS_ENTER, wxDefaultValidator, wxTextCtrlNameStr);

	m_keyGrabber2->Bind(wxEVT_TEXT_ENTER, [this](wxCommandEvent& event){
		processGrabberInput();
	});

	m_keyGrabber2->Bind(wxEVT_KEY_DOWN, [this](wxKeyEvent& event){
		onKeyDownGrabber(m_keyGrabber2, m_keyCodeGrabber2, event);
	});

	auto resultTag=builder<wxStaticText>(wxID_ANY, wxT(" "));
	
	m_keyCheck=builder<wxTextCtrl>(wxID_ANY, wxT(""), wxDefaultPosition, FromDIP(wxSize(45, 30)),
									wxTE_CENTRE, wxDefaultValidator, wxTextCtrlNameStr);

	m_symbolInput->Disable();
	m_keyGrabber1->Disable();
	m_keyGrabber2->Disable();
	m_keyCheck->Disable();

	// Layout
	
	wxBoxSizer* col = new wxBoxSizer(wxVERTICAL);
	{
		wxBoxSizer* row = new wxBoxSizer(wxHORIZONTAL);
		row->Add(waitTag, 0, wxRIGHT, FromDIP(10));
		row->Add(m_keyInput, 0, wxRIGHT, FromDIP(10));
		row->Add(m_progressTag);
		col->Add(row, 0, wxBOTTOM, FromDIP(10));
	}

	wxBoxSizer* row = new wxBoxSizer(wxHORIZONTAL);
	{
		wxBoxSizer* col = new wxBoxSizer(wxVERTICAL);
		col->Add(symbolTag, 0, wxRIGHT, FromDIP(10));
		col->Add(m_symbolInput);
		row->Add(col, 0, wxBOTTOM, FromDIP(10));
	}
	{
		wxBoxSizer* col = new wxBoxSizer(wxVERTICAL);
		col->Add(keysTag, 0, wxRIGHT, FromDIP(10));
		wxBoxSizer* row1 = new wxBoxSizer(wxHORIZONTAL);
		row1->Add(m_keyGrabber1, 0, wxRIGHT, FromDIP(5));
		row1->Add(m_keyGrabber2);
		col->Add(row1);
		row->Add(col, 0, wxBOTTOM | wxRIGHT, FromDIP(10));
	}
	{
		wxBoxSizer* col = new wxBoxSizer(wxVERTICAL);
		col->Add(resultTag, 0, wxRIGHT, FromDIP(10));
		col->Add(m_keyCheck);
		row->Add(col, 0, wxBOTTOM, FromDIP(10));
	}

	col->Add(row, 0, wxBOTTOM, FromDIP(10));
	col->Add(m_symbolList, 0, wxALIGN_CENTER);

	setSizer(col);
}

BEGIN_EVENT_TABLE(KeyMapPopup, wxPanel)
	EVT_TIMER(WX::CONFIG_TIMER, KeyMapPopup::OnAutoConfigure)
END_EVENT_TABLE()

//--------------------------------------------------------------------

void KeyMapPopup::triggerKey()
{
	m_state=TIMER_TASK::GET_FOCUS;
	m_configurationTimer.StartOnce(500);
	m_progress=(100*KeyboardConfigurator::typing())/KEY_INDEX::_STOP;
	m_progressTag->SetLabel(wxString::Format("%d %s", m_progress, "%"));

	Update();
}

//--------------------------------------------------------------------

void KeyMapPopup::OnAutoConfigure(wxTimerEvent& event)
{
	if(m_state==TIMER_TASK::KBOARD){
		triggerKey();
	}
	else if(m_state==TIMER_TASK::GET_FOCUS){
		if(m_progress<100){
			m_keyInput->SetFocus();
			m_skipInput=true;
			s_KeyboardEmulator->commandKey(SPKEYS::ENTER);
		}
		else{
			nextStage();
		}
	}
	else if(m_state==TIMER_TASK::NEW_SYMBOL){
		processKeyCheck();
	}

	m_state=TIMER_TASK::NONE;
}

//--------------------------------------------------------------------

void KeyMapPopup::OnPopup()
{
	m_skipInput=false;
	if(KeyboardConfigurator::loadKeyMap()){
		m_keyInput->Disable();
		m_symbolInput->Enable();
		m_symbolInput->SetFocus();
		m_keyGrabber1->Enable();
		m_keyGrabber2->Enable();
	}
	else{		
		unlockCapsNumb();
		m_state=TIMER_TASK::KBOARD;
		m_configurationTimer.StartOnce(300);
	}
}

//--------------------------------------------------------------------

void KeyMapPopup::processKeyInput(wxKeyEvent& event)
{
	if(m_skipInput){
		m_skipInput=false;
		triggerKey();
		return;
	}

	std::u8string utf8Symbol=wxString2u8String(m_keyInput->GetValue());

	bool completed, validSymbol;
	std::tie(completed, validSymbol)=KeyboardConfigurator::listener(event.GetRawKeyCode(), utf8Symbol.data());

	if(validSymbol){
		m_oneKeySymbols++;
		addSymbol(m_keyInput->GetValue());
	}

	m_keyInput->SetValue("");
	Update();

	if(completed){
		nextStage();	
	}
	else {
		m_keyInput->SetFocus();

		if(validSymbol){
			triggerKey();
		}
		else{
			/* Some keys might have some effect of the interface,
			 * like TAB which remove the focus from the current control
			 * to the next, or APOSTROPHE in Spanish keyboard layout which
			 * set the keyboard to state for input acents: á APOSTROPHE A
			 * as far as the following character accepts acent otherwise
			 * the input is ignored: APOSTROPHE B will produce nothing.
			 * That is why we need to clear the input control before issuing
			 * the next key.
			 */

			m_skipInput=true;
			s_KeyboardEmulator->commandKey(SPKEYS::BACKSPACE);
		}
	}
}

//--------------------------------------------------------------------

void KeyMapPopup::processGrabberInput()
{
	std::u8string utf8Symbol=wxString2u8String(m_symbolInput->GetValue());

	wxString errorMsg="";
	if(utf8Symbol.length()==0 || utf8Symbol.length()>4){
		errorMsg=wxString::Format("Symbol is empty or too long \"%s\".", m_symbolInput->GetValue());
	}
	else if(KeyboardConfigurator::symbolExists(utf8Symbol.data())){
		errorMsg=wxString::Format("Symbol \"%s\" already added.", m_symbolInput->GetValue());
	}

	if(errorMsg!=""){
		wxMessageDialog msgD(this, errorMsg, "Error");
		msgD.ShowModal();

		m_symbolInput->SetValue("");
		m_keyGrabber1->SetValue("");
		m_keyGrabber2->SetValue("");
		m_symbolInput->SetFocus();
		m_keyCodeGrabber1.reset();
		m_keyCodeGrabber2.reset();
	}
	else{
		m_keyCheck->Enable();
		m_keyCheck->SetFocus();
		KeyboardConfigurator::testCombo(m_keyCodeGrabber1.m_data, m_keyCodeGrabber2.m_data);

		m_state=TIMER_TASK::NEW_SYMBOL;
		m_configurationTimer.StartOnce(100);
	}
}

//--------------------------------------------------------------------

void KeyMapPopup::processKeyCheck()
{
	m_keyCheck->Disable();

	std::u8string utf8Symbol=wxString2u8String(m_symbolInput->GetValue());
	std::u8string utf8Symbol2=wxString2u8String(m_keyCheck->GetValue());

	if(utf8Symbol==utf8Symbol2){
		KeyboardConfigurator::addCombo(utf8Symbol.data(), m_keyCodeGrabber1.m_data, m_keyCodeGrabber2.m_data);
		addSymbol(m_symbolInput->GetValue());
	}
	else{
		wxString msg=wxString::Format("Symbols %s and %s does not match.", m_symbolInput->GetValue(), m_keyCheck->GetValue());
		wxMessageDialog msgD(this, msg, "Error");
		msgD.ShowModal();
	}

	m_symbolInput->SetValue("");
	m_keyGrabber1->SetValue("");
	m_keyGrabber2->SetValue("");
	m_keyCheck->SetValue("");
	m_symbolInput->SetFocus();
		
	m_keyCodeGrabber1.reset();	
	m_keyCodeGrabber2.reset();	
}

//--------------------------------------------------------------------

void KeyMapPopup::unlockCapsNumb()
{
	int ledMask=getLedMask();

	if(ledMask & 1){
		s_KeyboardEmulator->commandKey(SPKEYS::CAPSLOCK);
	}

	if(ledMask & 2){
		s_KeyboardEmulator->commandKey(SPKEYS::NUMLOCK);
	}
}

//--------------------------------------------------------------------

void KeyMapPopup::addSymbol(const wxString& symbol)
{
	const wxSize rowSize=FromDIP(wxSize(380, 30));
	static SymbolRow* symbolRow=new SymbolRow(m_symbolList, rowSize);

	wxPanel* panelPtr=new wxPanel(symbolRow, wxID_ANY, wxDefaultPosition, FromDIP(wxSize(35, 25)));

	wxBoxSizer* col = new wxBoxSizer(wxVERTICAL);
	wxControl* symbolTag=nullptr;

	if(symbol=="&"){
		symbolTag=new wxTextCtrl(panelPtr, wxID_ANY, symbol, wxDefaultPosition, FromDIP(wxSize(30, 25)), 0, wxDefaultValidator);
		symbolTag->Disable();
	}
	else{
		symbolTag=new wxStaticText(panelPtr, wxID_ANY, symbol);	
	}

	col->Add(symbolTag, 0, wxALIGN_CENTER);
	panelPtr->SetSizer(col);

	symbolRow->addSymbol(panelPtr);

	if(symbolRow->size()==1){
		m_symbolList->addPanel(symbolRow);
	}
	if(symbolRow->isFull()){
		symbolRow=new SymbolRow(m_symbolList, rowSize);
	}
}

//--------------------------------------------------------------------

void KeyMapPopup::nextStage()
{
	unlockCapsNumb();
	m_processComposite=true;
	KeyboardConfigurator::typingComposite();
	s_KeyboardEmulator->commandKey(SPKEYS::ENTER);
}

//--------------------------------------------------------------------

void KeyMapPopup::onKeyDownGrabber(wxTextCtrl* keyGrabberCtrl, KeyCombo& keyCodeGrabber, wxKeyEvent& event)
{
	if(WXK_BACK==event.GetKeyCode()){
			keyCodeGrabber.pop();
	}
	else if(event.GetKeyCode()!=WXK_RETURN && event.GetKeyCode()!=WXK_CAPITAL){
		keyCodeGrabber.pushBack(event.GetRawKeyCode());
	}

	if(event.GetKeyCode()==WXK_RETURN){
		event.Skip();
	}
	else{
		wxString str;
		for(unsigned char i=0; i<MAX_HID_CODES; i++){
			if(keyCodeGrabber[i]==-1){					
				break;
			}
			str+="*";
		}

		keyGrabberCtrl->SetValue(str);
		keyGrabberCtrl->SetInsertionPointEnd();
	}
}

//--------------------------------------------------------------------

void KeyMapPopup::processComposite()
{
	std::u8string utf8Symbol=wxString2u8String(m_keyInput->GetValue());

	bool completed, validSymbol;
	std::tie(completed, validSymbol)=KeyboardConfigurator::listenerComposite(utf8Symbol.data());

	if(validSymbol){
		addSymbol(m_keyInput->GetValue());
	}

	m_keyInput->SetValue("");
	Update();

	if(completed){
		m_processComposite=false;

		m_keyInput->Disable();
		m_symbolInput->Enable();
		m_symbolInput->SetFocus();
		m_keyGrabber1->Enable();
		m_keyGrabber2->Enable();
	}
	else {
		m_keyInput->SetFocus();

		m_progress=(100*KeyboardConfigurator::typingComposite())/m_oneKeySymbols;
		m_progressTag->SetLabel(wxString::Format("%d %s", m_progress, "%"));
		s_KeyboardEmulator->commandKey(SPKEYS::ENTER);
	}
}

//--------------------------------------------------------------------

KeyMapPopup::SymbolRow::SymbolRow(wxWindow* parent, const wxSize& size)
:wxPanel(parent, wxID_ANY, wxDefaultPosition, size)
,m_row(new wxBoxSizer(wxHORIZONTAL))
, c_totalSymbols(9)
, m_idx(0)
, m_isFull(false)
{
	SetSizer(m_row);
	m_symbols=new wxPanel*[c_totalSymbols];	
}

//--------------------------------------------------------------------

void KeyMapPopup::SymbolRow::addSymbol(wxPanel* panelPtr)
{
	if(!m_isFull){
		m_symbols[m_idx]=panelPtr;
		m_row->Add(m_symbols[m_idx], 0, wxLEFT, FromDIP(5));
		if(m_idx%2==1){
			panelPtr->SetBackgroundColour(wxColour("#F0FAF0"));
		}
		Layout();
		m_isFull=m_idx==c_totalSymbols-1;
		m_idx++;
	}
}


//====================================================================
