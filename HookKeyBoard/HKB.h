#pragma once

class HKB_CALLBACK
{
private:
	static HKB_CALLBACK* volatile _instance;
	static std::mutex _mutex;



	HKB_CALLBACK();
	HKB_CALLBACK(const HKB_CALLBACK& other) {};
	~HKB_CALLBACK();
public:
	static HKB_CALLBACK* volatile instance()
	{
		if (_instance == nullptr)
		{
			std::lock_guard<std::mutex> lock(_mutex);

			if (_instance == nullptr)
				_instance = new HKB_CALLBACK();

			atexit(_finalize);
		}
		return _instance;
	}

	static void _finalize()
	{
		HKB_CALLBACK::instance()->finalize();

		if (_instance)
			delete _instance;
	}

public:

	void finalize()
	{

	}

	bool attached() const
	{
		return _hook != nullptr;
	}

	bool attach(__in HINSTANCE module)
	{
		_hook = SetWindowsHookExW(WH_KEYBOARD_LL, proxy_function, module, 0);
		return _hook != nullptr;
	}

	void detach()
	{
		if (attached())
		{
			UnhookWindowsHookEx(_hook);
		}
	}

	static LRESULT __stdcall proxy_function(__in int code, __in WPARAM wparam, __in LPARAM lparam)
	{
		return HKB_CALLBACK::instance()->DoHookKeyBoard(code, wparam, lparam);
	}

	LRESULT DoHookKeyBoard(__in int code, __in WPARAM wparam, __in LPARAM lparam);

	// 기능


	// 도구

private:
	HHOOK _hook;

};

