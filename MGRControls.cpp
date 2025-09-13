#include "dllmain.h"

#include "IniReader.h"
#include <string>
#include <XInput.h>

enum InputBitflags {
	WeaponMenuBit = 0x1,
	WeaponMenu2Bit = 0x2,
	HealBit = 0x4,
	TauntBit = 0x8,
	JumpBit = 0x10,
	InteractBit = 0x20,
	LightAttackBit = 0x40,
	HeavyAttackBit = 0x80,
	PauseBit = 0x100,
	CodecBit = 0x200,
	SubWeaponBit = 0x400,
	BladeModeBit = 0x800,
	AbilityBit = 0x1000,
	LockOnBit = 0x2000,
	RunBit = 0x4000,
	CamResetBit = 0x8000,
	LeftBit = 0x100000,
	RightBit = 0x200000,
	ForwardBit = 0x400000,
	BackwardBit = 0x800000,
	CamLeftBit = 0x1000000,
	CamRightBit = 0x2000000,
	CamUpBit = 0x4000000,
	CamDownBit = 0x8000000,
};



enum GamepadAnalogValues {
	LeftX,
	LeftY,
	RightX,
	RightY
};

// Returns a scale factor (expected 0.0 to 1.0, technically can give -1.0 to 1.0) for analog movement
float GetGamepadAnalog(int controllerIndex, const std::string& button)
{
	XINPUT_STATE state;
	DWORD dwResult = XInputGetState(controllerIndex, &state);



	if (dwResult == ERROR_SUCCESS)
	{
		GamepadAnalogValues mode;
		float invertFactor = 1.0;
		if (button == "XINPUT_GAMEPAD_LEFT_THUMB_UP") mode = LeftY;
		else if (button == "XINPUT_GAMEPAD_LEFT_THUMB_DOWN") { mode = LeftY; invertFactor = -1.0; }
		else if (button == "XINPUT_GAMEPAD_LEFT_THUMB_RIGHT") mode = LeftX;
		else if (button == "XINPUT_GAMEPAD_LEFT_THUMB_LEFT") { mode = LeftX; invertFactor = -1.0; }
		else if (button == "XINPUT_GAMEPAD_RIGHT_THUMB_UP") mode = RightY;
		else if (button == "XINPUT_GAMEPAD_RIGHT_THUMB_DOWN") { mode = RightY; invertFactor = -1.0; }
		else if (button == "XINPUT_GAMEPAD_RIGHT_THUMB_RIGHT") mode = RightX;
		else if (button == "XINPUT_GAMEPAD_RIGHT_THUMB_LEFT") { mode = RightX; invertFactor = -1.0; }
		else { return 1.0; } // Default behavior shouldn't slow you down

		switch (mode) {
		case LeftX: return invertFactor * state.Gamepad.sThumbLX / SHRT_MAX;
		case RightX: return invertFactor * state.Gamepad.sThumbRX / SHRT_MAX;
		case LeftY: return invertFactor * state.Gamepad.sThumbLY / SHRT_MAX;
		case RightY: return invertFactor * state.Gamepad.sThumbRY / SHRT_MAX;
		}
	}
	return 0.0;
}




bool IsGamepadButtonPressed(int controllerIndex, const std::string& button)
{
	if (controllerIndex < 0)
		return false;

	XINPUT_STATE state;
	DWORD dwResult = XInputGetState(controllerIndex, &state);


	if (dwResult == ERROR_SUCCESS)
	{
		if (button == "XINPUT_GAMEPAD_A")
			return (state.Gamepad.wButtons & XINPUT_GAMEPAD_A) != 0;
		else if (button == "XINPUT_GAMEPAD_B")
			return (state.Gamepad.wButtons & XINPUT_GAMEPAD_B) != 0;
		else if (button == "XINPUT_GAMEPAD_X")
			return (state.Gamepad.wButtons & XINPUT_GAMEPAD_X) != 0;
		else if (button == "XINPUT_GAMEPAD_Y")
			return (state.Gamepad.wButtons & XINPUT_GAMEPAD_Y) != 0;
		else if (button == "XINPUT_GAMEPAD_LEFT_SHOULDER")
			return (state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) != 0;
		else if (button == "XINPUT_GAMEPAD_RIGHT_SHOULDER")
			return (state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) != 0;
		else if (button == "XINPUT_GAMEPAD_BACK")
			return (state.Gamepad.wButtons & XINPUT_GAMEPAD_BACK) != 0;
		else if (button == "XINPUT_GAMEPAD_START")
			return (state.Gamepad.wButtons & XINPUT_GAMEPAD_START) != 0;
		else if (button == "XINPUT_GAMEPAD_LEFT_THUMB")
			return (state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB) != 0;
		else if (button == "XINPUT_GAMEPAD_RIGHT_THUMB")
			return (state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB) != 0;
		else if (button == "XINPUT_GAMEPAD_DPAD_UP")
			return (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP) != 0;
		else if (button == "XINPUT_GAMEPAD_DPAD_DOWN")
			return (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN) != 0;
		else if (button == "XINPUT_GAMEPAD_DPAD_LEFT")
			return (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT) != 0;
		else if (button == "XINPUT_GAMEPAD_DPAD_RIGHT")
			return (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) != 0;
		else if (button == "XINPUT_GAMEPAD_LEFT_THUMB_UP")
			return state.Gamepad.sThumbLY > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE;
		else if (button == "XINPUT_GAMEPAD_LEFT_THUMB_DOWN")
			return state.Gamepad.sThumbLY < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE;
		else if (button == "XINPUT_GAMEPAD_LEFT_THUMB_LEFT")
			return state.Gamepad.sThumbLX < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE;
		else if (button == "XINPUT_GAMEPAD_LEFT_THUMB_RIGHT")
			return state.Gamepad.sThumbLX > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE;
		else if (button == "XINPUT_GAMEPAD_RIGHT_THUMB_UP")
			return state.Gamepad.sThumbRY > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE;
		else if (button == "XINPUT_GAMEPAD_RIGHT_THUMB_DOWN")
			return state.Gamepad.sThumbRY < -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE;
		else if (button == "XINPUT_GAMEPAD_RIGHT_THUMB_LEFT")
			return state.Gamepad.sThumbRX < -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE;
		else if (button == "XINPUT_GAMEPAD_RIGHT_THUMB_RIGHT")
			return state.Gamepad.sThumbRX > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE;
		else if (button == "XINPUT_GAMEPAD_LEFT_TRIGGER")
			return state.Gamepad.bLeftTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD;
		else if (button == "XINPUT_GAMEPAD_RIGHT_TRIGGER")
			return state.Gamepad.bRightTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD;
	}

	return false;
}

std::string TryParseVKToHex(std::string in) {
	// Ruslan found the docs, I just copied them into a massive if-chain --Jacky720
	// http://www.kbdedit.com/manual/low_level_vk_list.html
	if (in == "LMB" || in == "Mouse1" || in == "VK_LBUTTON")
		return "01";
	if (in == "RMB" || in == "Mouse2" || in == "VK_RBUTTON")
		return "02";
	if (in == "VK_CANCEL" || in == "Break")
		return "03";
	if (in == "MMB" || in == "Mouse3" || in == "VK_MBUTTON")
		return "04";
	if (in == "Mouse4" || in == "VK_XBUTTON1")
		return "05";
	if (in == "Mouse5" || in == "VK_XBUTTON2")
		return "06";
	if (in == "VK_BACK" || in == "Backspace")
		return "08";
	if (in == "VK_TAB")
		return "09";
	if (in == "VK_CLEAR" || in == "Clear")
		return "0C";
	if (in == "VK_RETURN" || in == "Return" || in == "Enter")
		return "0D";
	if (in == "VK_PAUSE" || in == "Pause")
		return "13";
	if (in == "VK_CAPITAL" || in == "CapsLock" || in == "Caps" || in == "Caps Lock")
		return "14";
	if (in == "VK_KANA" || in == "Kana")
		return "15";
	if (in == "VK_JUNJA" || in == "Junja")
		return "17";
	if (in == "VK_FINAL" || in == "Final")
		return "18";
	if (in == "VK_KANJI" || in == "Kanji")
		return "19";
	if (in == "VK_ESCAPE" || in == "Esc" || in == "Escape")
		return "1B";
	if (in == "VK_CONVERT" || in == "Convert")
		return "1C";
	if (in == "VK_NONCONVERT" || in == "NonConvert" || in == "Non Convert")
		return "1D";
	if (in == "VK_ACCEPT" || in == "Accept")
		return "1E";
	if (in == "VK_MODECHANGE" || in == "Mode Change" || in == "ModeChange")
		return "1F";
	if (in == "VK_SPACE" || in == "Space")
		return "20";
	if (in == "VK_PRIOR" || in == "PageUp" || in == "Page Up" || in == "Prior")
		return "21";
	if (in == "VK_NEXT" || in == "PageDown" || in == "Page Down" || in == "Next")
		return "22";
	if (in == "VK_END" || in == "End")
		return "23";
	if (in == "VK_HOME" || in == "Home")
		return "24";
	if (in == "VK_LEFT" || in == "Left" || in == "Left Arrow" || in == "LeftArrow")
		return "25";
	if (in == "VK_UP" || in == "Up" || in == "Up Arrow" || in == "UpArrow")
		return "26";
	if (in == "VK_RIGHT" || in == "Right" || in == "Right Arrow" || in == "RightArrow")
		return "27";
	if (in == "VK_DOWN" || in == "Down" || in == "Down Arrow" || in == "DownArrow")
		return "28";
	if (in == "VK_SELECT" || in == "Select")
		return "29";
	if (in == "VK_PRINT" || in == "Print")
		return "2A";
	if (in == "VK_EXECUTE" || in == "Execute")
		return "2B";
	if (in == "VK_SNAPSHOT" || in == "Snapshot" || in == "PrintScreen" || in == "Print Screen")
		return "2C";
	if (in == "VK_INSERT" || in == "Insert")
		return "2D";
	if (in == "VK_DELETE" || in == "Delete")
		return "2E";
	if (in == "VK_HELP" || in == "Help")
		return "2F";
	if (in == "VK_LWIN" || in == "LeftWin" || in == "Left Win" || in == "Windows" || in == "Win" || in == "LWin")
		return "5B";
	if (in == "VK_RWIN" || in == "RightWin" || in == "Right Win" || in == "RWin")
		return "5C";
	if (in == "VK_APPS" || in == "Apps" || in == "Context")
		return "5D";
	if (in == "VK_SLEEP" || in == "Sleep")
		return "5F";
	if (in == "VK_MULTIPLY" || in == "Multiply" || in == "Numpad*" || in == "Numpad *" || in == "Mul")
		return "6A";
	if (in == "VK_ADD" || in == "Add" || in == "Numpad+" || in == "Numpad +")
		return "6B";
	if (in == "VK_SEPARATOR" || in == "Separator" || in == "Seperator")
		return "6C";
	if (in == "VK_SUBTRACT" || in == "Subtract" || in == "Numpad-" || in == "Numpad -" || in == "Sub")
		return "6D";
	if (in == "VK_DECIMAL" || in == "Decimal" || in == "Numpad." || in == "Numpad .")
		return "6E";
	if (in == "VK_DIVIDE" || in == "Divide" || in == "Numpad/" || in == "Numpad /" || in == "Div")
		return "6F";
	if (in == "VK_NUMLOCK" || in == "Num Lock" || in == "NumLock")
		return "90";
	if (in == "VK_SCROLL" || in == "Scroll Lock" || in == "ScrollLock" || in == "Scroll")
		return "91";
	if (in == "VK_OEM_FJ_JISHO" || in == "Jisho")
		return "92";
	if (in == "VK_OEM_FJ_MASSHOU" || in == "Masshou" || in == "Mashu")
		return "93";
	if (in == "VK_OEM_FJ_TOUROKU" || in == "Touroku")
		return "94";
	if (in == "VK_OEM_FJ_LOYA" || in == "Loya")
		return "95";
	if (in == "VK_OEM_FJ_ROYA" || in == "Roya")
		return "96";
	if (in == "VK_LSHIFT" || in == "Left Shift" || in == "LeftShift" || in == "Shift")
		return "A0";
	if (in == "VK_RSHIFT" || in == "Right Shift" || in == "RightShift")
		return "A1";
	if (in == "VK_LCONTROL" || in == "Left Control" || in == "LeftControl" || in == "Control")
		return "A2";
	if (in == "VK_RCONTROL" || in == "Right Control" || in == "RightControl")
		return "A3";
	if (in == "VK_LMENU" || in == "Left Alt" || in == "LeftAlt" || in == "Alt")
		return "A4";
	if (in == "VK_RMENU" || in == "Right Alt" || in == "RightAlt")
		return "A5";
	if (in == "VK_BROWSER_BACK" || in == "Browser Back" || in == "BrowserBack")
		return "A6";
	if (in == "VK_BROWSER_FORWARD" || in == "Browser Forward" || in == "BrowserForward")
		return "A7";
	if (in == "VK_BROWSER_REFRESH" || in == "Browser Refresh" || in == "BrowserRefresh")
		return "A8";
	if (in == "VK_BROWSER_STOP" || in == "Browser Stop" || in == "BrowserStop")
		return "A9";
	if (in == "VK_BROWSER_SEARCH" || in == "Browser Search" || in == "BrowserSearch")
		return "AA";
	if (in == "VK_BROWSER_FAVORITES" || in == "Browser Favorites" || in == "BrowserFavorites")
		return "AB";
	if (in == "VK_BROWSER_HOME" || in == "Browser Home" || in == "BrowserHome")
		return "AC";
	if (in == "VK_VOLUME_MUTE" || in == "Mute" || in == "Volume Mute" || in == "VolumeMute")
		return "AD";
	if (in == "VK_VOLUME_DOWN" || in == "Volume Down" || in == "VolumeDown")
		return "AE";
	if (in == "VK_VOLUME_UP" || in == "Volume Up" || in == "VolumeUp")
		return "AF";
	if (in == "VK_MEDIA_NEXT_TRACK" || in == "Next Track" || in == "NextTrack")
		return "B0";
	if (in == "VK_MEDIA_PREV_TRACK" || in == "Previous Track" || in == "PreviousTrack" || in == "PrevTrack")
		return "B1";
	if (in == "VK_MEDIA_STOP" || in == "Stop" || in == "Media Stop" || in == "MediaStop")
		return "B2";
	if (in == "VK_MEDIA_PLAY_PAUSE" || in == "Play/Pause" || in == "Media Play" || in == "MediaPlay")
		return "B3";
	if (in == "VK_LAUNCH_MAIL" || in == "Mail" || in == "Launch Mail" || in == "LaunchMail")
		return "B4";
	if (in == "VK_LAUNCH_MEDIA_SELECT" || in == "Media" || in == "Media Select" || in == "MediaSelect")
		return "B5";
	if (in == "VK_LAUNCH_APP1" || in == "App1" || in == "App 1" || in == "Application 1")
		return "B6";
	if (in == "VK_LAUNCH_APP2" || in == "App2" || in == "App 2" || in == "Application 2")
		return "B7";
	if (in == "VK_OEM_1" || in == ";" || in == "';'")
		return "BA";
	if (in == "VK_OEM_PLUS" || in == "=" || in == "'='" || in == "Plus")
		return "BB";
	if (in == "VK_OEM_COMMA" || in == "," || in == "','")
		return "BC";
	if (in == "VK_OEM_MINUS" || in == "-" || in == "'-'")
		return "BD";
	if (in == "VK_OEM_PERIOD" || in == "." || in == "'.'")
		return "BE";
	if (in == "VK_OEM_2" || in == "/" || in == "'/'")
		return "BF";
	if (in == "VK_OEM_3" || in == "`" || in == "'`'")
		return "C0";
	if (in == "VK_ABNT_C1" || in == "Abnt C1" || in == "AbntC1")
		return "C1";
	if (in == "VK_ABNT_C2" || in == "Abnt C2" || in == "AbntC2")
		return "C2";
	if (in == "VK_OEM_4" || in == "[" || in == "'['")
		return "DB";
	if (in == "VK_OEM_5" || in == "\\" || in == "'\\'")
		return "DC";
	if (in == "VK_OEM_6" || in == "]" || in == "']'")
		return "DD";
	if (in == "VK_OEM_7" || in == "'" || in == "'\\''")
		return "DE";
	if (in == "VK_OEM_8" || in == "!" || in == "'!'")
		return "DF";
	if (in == "VK_OEM_AX" || in == "Ax")
		return "E1";
	if (in == "VK_OEM_102" || in == "<" || in == "'<'")
		return "E2";
	if (in == "VK_ICO_HELP" || in == "Ico Help" || in == "IcoHelp")
		return "E3";
	if (in == "VK_ICO_00")
		return "E4";
	if (in == "VK_PROCESSKEY" || in == "Process")
		return "E5";
	if (in == "VK_ICO_CLEAR" || in == "Ico Clear" || in == "IcoClear")
		return "E6";
	if (in == "VK_PACKET" || in == "Packet")
		return "E7";
	if (in == "VK_OEM_RESET" || in == "Reset")
		return "E9";
	if (in == "VK_OEM_JUMP" || in == "Jump")
		return "EA";
	if (in == "VK_OEM_PA1" || in == "OemPa1" || in == "Oem Pa1")
		return "EB";
	if (in == "VK_OEM_PA2" || in == "OemPa2" || in == "Oem Pa2")
		return "EC";
	if (in == "VK_OEM_PA3" || in == "OemPa3" || in == "Oem Pa3")
		return "ED";
	if (in == "VK_OEM_WSCTRL" || in == "WsCtrl")
		return "EE";
	if (in == "VK_OEM_CUSEL" || in == "CuSel" || in == "Cu Sel")
		return "EF";
	if (in == "VK_OEM_ATTN" || in == "OemAttn" || in == "Oem Attn")
		return "F0";
	if (in == "VK_OEM_FINISH" || in == "Finish")
		return "F1";
	if (in == "VK_OEM_COPY" || in == "Copy")
		return "F2";
	if (in == "VK_OEM_AUTO" || in == "Auto")
		return "F3";
	if (in == "VK_OEM_ENLW" || in == "Enlw")
		return "F4";
	if (in == "VK_OEM_BACKTAB" || in == "Back Tab" || in == "BackTab")
		return "F5";
	if (in == "VK_ATTN" || in == "Attn")
		return "F6";
	if (in == "VK_CRSEL" || in == "CrSel" || in == "Cr Sel")
		return "F7";
	if (in == "VK_EXSEL" || in == "ExSel" || in == "Ex Sel")
		return "F8";
	if (in == "VK_EREOF" || in == "ErEof" || in == "Er Eof")
		return "F9";
	if (in == "VK_PLAY" || in == "Play")
		return "FA";
	if (in == "VK_ZOOM" || in == "Zoom")
		return "FB";
	if (in == "VK_NONAME" || in == "NoName" || in == "No Name")
		return "FC";
	if (in == "VK_PA1" || in == "Pa1" || in == "PA1")
		return "FD";
	if (in == "VK_OEM_CLEAR" || in == "OemClear" || in == "Oem Clear")
		return "FE";
	if (in == "VK__none_" || in == "None")
		return "FF";

	if (in.starts_with("VK_F") || (in[0] == 'F' && in.length() > 1)) {
		std::string num = in.substr(1);
		if (in[0] == 'V') num = in.substr(4);
		switch (std::stoi(num)) {
		case 1: return "70";
		case 2: return "71";
		case 3: return "72";
		case 4: return "73";
		case 5: return "74";
		case 6: return "75";
		case 7: return "76";
		case 8: return "77";
		case 9: return "78";
		case 10: return "79";
		case 11: return "7A";
		case 12: return "7B";
		case 13: return "7C";
		case 14: return "7D";
		case 15: return "7E";
		case 16: return "7F";
		case 17: return "80";
		case 18: return "81";
		case 19: return "82";
		case 20: return "83";
		case 21: return "84";
		case 22: return "85";
		case 23: return "86";
		case 24: return "87";
		}
	}
	if (in.starts_with("VK_NUMPAD") || in.starts_with("Numpad")) {
		char usednum = in[6];
		if (in[0] == 'V')
			usednum = in[9];
		else if (usednum == ' ')
			usednum = in[7];
		return "6" + usednum;
	}
	if (in.starts_with("VK_KEY_") || in.length() == 1 || (in.length() == 3 && in[0] == '\'' && in[2] == '\'')) {
		char usedchar = in[0];
		if (in.length() == 3)
			usedchar = in[1];
		if (in.length() > 3)
			usedchar = in[7];
		switch (usedchar) {
		case ' ': return "20";
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9': return "3" + usedchar;
		case 'A': return "41";
		case 'B': return "42";
		case 'C': return "43";
		case 'D': return "44";
		case 'E': return "45";
		case 'F': return "46";
		case 'G': return "47";
		case 'H': return "48";
		case 'I': return "49";
		case 'J': return "4A";
		case 'K': return "4B";
		case 'L': return "4C";
		case 'M': return "4D";
		case 'N': return "4E";
		case 'O': return "4F";
		case 'P': return "50";
		case 'Q': return "51";
		case 'R': return "52";
		case 'S': return "53";
		case 'T': return "54";
		case 'U': return "55";
		case 'V': return "56";
		case 'W': return "57";
		case 'X': return "58";
		case 'Y': return "59";
		case 'Z': return "5A";
		}
	}

	return in;
}




std::string Forward = "26";
std::string Back = "28";
std::string Left = "25";
std::string Right = "27";
std::string NormalAttack = "49"; // I
std::string StrongAttack = "4F"; // O
std::string Jump = "50"; // P
std::string Interact = "74";
std::string CamUp = "70";
std::string CamDown = "71";
std::string CamLeft = "72";
std::string CamRight = "73";
std::string Heal = "76";
std::string Taunt = "59"; // Y
std::string WeaponMenu = "77";
std::string WeaponMenu2 = "78";
std::string Run = "55"; // U
std::string BladeMode = "4C"; // L
std::string Subweapon = "79";
std::string Lockon = "80";
std::string Pause = "81";
std::string Pause2 = "75";
std::string Ability = "82";
std::string CamReset = "83";

std::string GamepadForward = "XINPUT_GAMEPAD_LEFT_THUMB_UP";
std::string GamepadBack = "XINPUT_GAMEPAD_LEFT_THUMB_DOWN";
std::string GamepadLeft = "XINPUT_GAMEPAD_LEFT_THUMB_LEFT";
std::string GamepadRight = "XINPUT_GAMEPAD_LEFT_THUMB_RIGHT";
std::string GamepadNormalAttack = "XINPUT_GAMEPAD_X";
std::string GamepadStrongAttack = "XINPUT_GAMEPAD_Y";
std::string GamepadJump = "XINPUT_GAMEPAD_A";
std::string GamepadInteract = "XINPUT_GAMEPAD_B";
std::string GamepadCamUp = "XINPUT_GAMEPAD_RIGHT_THUMB_UP";
std::string GamepadCamDown = "XINPUT_GAMEPAD_RIGHT_THUMB_DOWN";
std::string GamepadCamLeft = "XINPUT_GAMEPAD_RIGHT_THUMB_LEFT";
std::string GamepadCamRight = "XINPUT_GAMEPAD_RIGHT_THUMB_RIGHT";
std::string GamepadHeal = "XINPUT_GAMEPAD_DPAD_DOWN";
std::string GamepadTaunt = "XINPUT_GAMEPAD_DPAD_UP";
std::string GamepadWeaponMenu = "XINPUT_GAMEPAD_DPAD_LEFT";
std::string GamepadWeaponMenu2 = "XINPUT_GAMEPAD_DPAD_RIGHT";
std::string GamepadRun = "XINPUT_GAMEPAD_RIGHT_TRIGGER";
std::string GamepadBladeMode = "XINPUT_GAMEPAD_LEFT_TRIGGER";
std::string GamepadSubweapon = "XINPUT_GAMEPAD_LEFT_SHOULDER";
std::string GamepadLockon = "XINPUT_GAMEPAD_RIGHT_SHOULDER";
std::string GamepadPause = "XINPUT_GAMEPAD_START";
std::string GamepadPause2 = "XINPUT_GAMEPAD_BACK";
std::string GamepadAbility = "XINPUT_GAMEPAD_LEFT_THUMB";
std::string GamepadCamReset = "XINPUT_GAMEPAD_RIGHT_THUMB";

std::string GamepadSpawn = "XINPUT_GAMEPAD_START";


void LoadControl(CIniReader iniReader, std::string* Control, std::string* GamepadControl, std::string name) {
	*Control = TryParseVKToHex(iniReader.ReadString("MGRRMultiplayerControls", name, *Control));
	*GamepadControl = iniReader.ReadString("MGRRMultiplayerControls", "Gamepad" + name, *GamepadControl);
}

void LoadConfig() noexcept
{
	// Load configuration data
	CIniReader iniReader("MGRRMultiplayerControls.ini");

	LoadControl(iniReader, &Forward, &GamepadForward, "Forward");
	LoadControl(iniReader, &Back, &GamepadBack, "Back");
	LoadControl(iniReader, &Left, &GamepadLeft, "Left");
	LoadControl(iniReader, &Right, &GamepadRight, "Right");
	LoadControl(iniReader, &NormalAttack, &GamepadNormalAttack, "NormalAttack");
	LoadControl(iniReader, &StrongAttack, &GamepadStrongAttack, "StrongAttack");
	LoadControl(iniReader, &Jump, &GamepadJump, "Jump");
	LoadControl(iniReader, &Interact, &GamepadInteract, "Interact");
	LoadControl(iniReader, &Run, &GamepadRun, "Run");
	LoadControl(iniReader, &BladeMode, &GamepadBladeMode, "BladeMode");
	LoadControl(iniReader, &Lockon, &GamepadLockon, "Lockon");
	LoadControl(iniReader, &Subweapon, &GamepadSubweapon, "Subweapon");
	LoadControl(iniReader, &CamUp, &GamepadCamUp, "CamUp");
	LoadControl(iniReader, &CamDown, &GamepadCamDown, "CamDown");
	LoadControl(iniReader, &CamLeft, &GamepadCamLeft, "CamLeft");
	LoadControl(iniReader, &CamRight, &GamepadCamRight, "CamRight");
	LoadControl(iniReader, &Taunt, &GamepadTaunt, "Taunt");
	LoadControl(iniReader, &WeaponMenu, &GamepadWeaponMenu, "WeaponMenu");
	LoadControl(iniReader, &WeaponMenu2, &GamepadWeaponMenu2, "WeaponMenu2");
	LoadControl(iniReader, &Heal, &GamepadHeal, "Heal");
	LoadControl(iniReader, &Pause, &GamepadPause, "Pause");
	LoadControl(iniReader, &Pause2, &GamepadPause2, "Codec"); // Note non-matching, Codec is apparently an enum somewhere
	LoadControl(iniReader, &Ability, &GamepadAbility, "Ability");
	LoadControl(iniReader, &CamReset, &GamepadCamReset, "CamReset");

	GamepadSpawn = iniReader.ReadString("MGRRMultiplayerControls", "GamepadSpawn", GamepadSpawn);

	p1IsKeyboard = iniReader.ReadBoolean("MGRRMultiplayerControls", "PlayerOneKeyboard", true);

}



std::string GetVanillaKeybind(InputBitflags bit) {
	if (bit == WeaponMenuBit || bit == WeaponMenu2Bit)
		return "32"; // "2"
	if (bit == HealBit)
		return "51"; // "Q"
	if (bit == TauntBit)
		return "31"; // "1"
	if (bit == JumpBit)
		return "20"; // " "
	if (bit == InteractBit)
		return "46"; // "F"
	if (bit == LightAttackBit)
		return "01"; // LMB
	if (bit == HeavyAttackBit)
		return "02"; // RMB
	if (bit == PauseBit)
		return "1B"; // Escape
	if (bit == CodecBit)
		return "33"; // "3"
	if (bit == SubWeaponBit)
		return "43"; // "C"
	if (bit == BladeModeBit)
		return "A0"; // LShift
	if (bit == AbilityBit)
		return "52"; // "R", technically not a 1:1 match because platinum
	if (bit == LockOnBit)
		return "45"; // "E"
	if (bit == RunBit)
		return "A2"; // LCtrl
	if (bit == CamResetBit)
		return "03"; // MMB
	if (bit == LeftBit)
		return "41"; // "A"
	if (bit == RightBit)
		return "44"; // "D"
	if (bit == ForwardBit)
		return "57"; // "W"
	if (bit == BackwardBit)
		return "53"; // "S"
	return "None"; // Camera is analog and won't really work here anyway
}

std::string GetVanillaKeybind(std::string Keybind) {
	if (Keybind == WeaponMenu || Keybind == WeaponMenu2)
		return "32"; // "2"
	if (Keybind == Heal)
		return "51"; // "Q"
	if (Keybind == Taunt)
		return "31"; // "1"
	if (Keybind == Jump)
		return "20"; // " "
	if (Keybind == Interact)
		return "46"; // "F"
	if (Keybind == NormalAttack)
		return "01"; // LMB
	if (Keybind == StrongAttack)
		return "02"; // RMB
	if (Keybind == Pause)
		return "1B"; // Escape
	if (Keybind == Pause2)
		return "33"; // "3"
	if (Keybind == Subweapon)
		return "43"; // "C"
	if (Keybind == BladeMode)
		return "A0"; // LShift
	if (Keybind == Ability)
		return "52"; // "R", technically not a 1:1 match because platinum
	if (Keybind == Lockon)
		return "45"; // "E"
	if (Keybind == Run)
		return "A2"; // LCtrl
	if (Keybind == CamReset)
		return "03"; // MMB
	if (Keybind == Left)
		return "41"; // "A"
	if (Keybind == Right)
		return "44"; // "D"
	if (Keybind == Forward)
		return "57"; // "W"
	if (Keybind == Back)
		return "53"; // "S"
	return "None"; // Camera is analog and won't really work here anyway
}

bool CheckControlPressed(int controllerNumber, std::string Keybind, std::string GamepadBind) {
	if (controllerNumber == -1) {
		Keybind = GetVanillaKeybind(Keybind);
		if (Keybind == "None")
			return false;
	}
	return ((controllerNumber <= 0 && (GetKeyState(std::stoi(Keybind, nullptr, 16)) & 0x8000))
		|| IsGamepadButtonPressed(controllerNumber, GamepadBind));
}
