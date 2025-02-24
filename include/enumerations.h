/*********************************************************************
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE. 
* 
*         	                                                         *
* Version: 1.0                                                       *
* Date:    09-02-2025                                                *
* Author:  Dan Machado                                               *
**********************************************************************/
#ifndef _ENUMERATIONS_H
#define _ENUMERATIONS_H

//====================================================================

namespace WX
{
	enum 
	{
		_FIRST=102,
		START_RECORDING,
		ADD_COMMAND,
		CLOSE,
		CLEAR,
		SAVE,
		SAVE_TO_FILE,
		CANCEL_SAVE_FILE,
		SAVED_FILES,
		LOAD_FILE,
		COMMAND_INPUT_MODE,
		PLAY,
		STOP,
		CMD_LIST,
		SUBMIT_INPUT,
		SET_CHOICE,
		TIMER,
		ROI_RADIO,
		DISPLAY_KBOARD,
		LOOP_BUTTON,
		CANCEL_LOOP,
		SELECT_ALL,
		INVERT,
		CHECK_ENABLE_COMMAND,
		UPDATE_CMD,		
		ADD_CTRL_CMD,
		SWAP_SCREENSHOT,
		DELETE_CMD,
		SETTINGS,
		DEMO,
		DELETE_FILE,
		PROGRESS_TIMER,
		_LAST,
	};

	namespace KBD
	{
		enum
		{
			_FIRST=WX::_LAST,// not use
			Esc_BTN,
			ScrLk_BTN,
			Pause_BTN,
			F1_BTN,
			F2_BTN,
			F3_BTN,
			F4_BTN,
			F5_BTN,
			F6_BTN,
			F7_BTN,
			F8_BTN,
			F9_BTN,
			F10_BTN,
			F11_BTN,
			F12_BTN,
			PrtSc_BTN,
			Backspace_BTN,
			Enter_BTN,
			Insert_BTN,
			Home_BTN,
			PgUp_BTN,
			Delete_BTN,
			End_BTN,
			PgDn_BTN,
			CAPS_LOCK,
			ARROW_UP_BTN,
			ARROW_LEFT_BTN,
			ARROW_DOWN_BTN,
			ARROW_RIGHT_BTN,
			LEFT_CTRL,
			RIGHT_CTRL,
			LEFT_SHIFT,
			RIGHT_SHIFT,
			LEFT_ALT,
			RIGHT_ALT,
			SUPER,
			_LAST,
		};
	};

	namespace MENU
	{
		enum{
			_FIRST=WX::KBD::_LAST,// not use
			WINDOW_INPUT,
			OPEN_LOOP,
			CLOSE_LOOP,
			MOVE_HERE,
			DO_LEFT_CLICK,
			DO_RIGHT_CLICK,
			SCREENSHOT_CMD,
			START_ROI,
			START_DRAGGING,
			DRAG_HERE,
			CANCEL_NEW_ROI,
			DISPLAY_KBOARD,
			CLOSE_MENU,
			SUBMENU_REPEAT_ALL,
			SUBMENU_REPEAT_LAST,
			SUBMENU_AFTER_ME,
			SUBMENU_QUIET,
			_LAST,// not use
		};
	}

	namespace ROI_MENU
	{
		enum{
			_FIRST=WX::MENU::_LAST,// not use
			OK,
			CANCEL,
			_LAST,
		};
	}
};

//====================================================================

enum class InterfaceLink
{
	NONE,
	UINPUT,
	SERIAL,
	UDP,
	_LAST,
};

enum HID_TARGET
{
	NONE,
	UINPUT,
	TINYUSB,
};

//====================================================================

#endif
