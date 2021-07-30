#pragma once
#include "atlimage.h"

class HM_CALLBACK
{
private:
	static HM_CALLBACK* volatile _instance;
	static std::mutex _mutex;

	HDC m_hDC;					// 메인 핸들
	HCURSOR m_hCursor;			// 커서 이미지 담기

	LONG m_nWidth;				// 메인 화면 가로
	LONG m_nHeight;				// 메인 화면 세로
	LONG m_nDepth;				// 메인 화면 댑스
	LONG m_nCnt;				// 캡쳐 횟수 기록
	CImage m_SnapShot;			// 캡쳐한 영상
	POINT m_MousePos;			// 클릭한 마우스의 위치

	HM_CALLBACK();
	HM_CALLBACK(const HM_CALLBACK& other) {};
	~HM_CALLBACK();
public:
	static HM_CALLBACK* volatile instance()
	{
		if (_instance == nullptr)
		{
			std::lock_guard<std::mutex> lock(_mutex);

			if (_instance == nullptr)
				_instance = new HM_CALLBACK();

			atexit(_finalize);
		}
		return _instance;
	}

	static void _finalize()
	{
		HM_CALLBACK::instance()->finalize();

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
		_hook = SetWindowsHookExW(WH_MOUSE_LL, proxy_function, module, 0);
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
		return HM_CALLBACK::instance()->DoHookMouse(code,	wparam, lparam);
	}

	LRESULT DoHookMouse(__in int code,	__in WPARAM wparam,	__in LPARAM lparam);

	// 기능
	void SaveLog();
	void CaptureDC(CImage &img, CRect ROI, HDC hDC);
	void ClearHDC();
	void DrawMouseCursor(HDC hDC, POINT pos);
	CString CreateDir();

	// 도구
	BOOL RANGEIN(double target, double min, double max);

private:
	HHOOK _hook;

};

