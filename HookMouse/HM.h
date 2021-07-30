#pragma once
#include "atlimage.h"

class HM_CALLBACK
{
private:
	static HM_CALLBACK* volatile _instance;
	static std::mutex _mutex;

	HDC m_hDC;					// ���� �ڵ�
	HCURSOR m_hCursor;			// Ŀ�� �̹��� ���

	LONG m_nWidth;				// ���� ȭ�� ����
	LONG m_nHeight;				// ���� ȭ�� ����
	LONG m_nDepth;				// ���� ȭ�� ��
	LONG m_nCnt;				// ĸ�� Ƚ�� ���
	CImage m_SnapShot;			// ĸ���� ����
	POINT m_MousePos;			// Ŭ���� ���콺�� ��ġ

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

	// ���
	void SaveLog();
	void CaptureDC(CImage &img, CRect ROI, HDC hDC);
	void ClearHDC();
	void DrawMouseCursor(HDC hDC, POINT pos);
	CString CreateDir();

	// ����
	BOOL RANGEIN(double target, double min, double max);

private:
	HHOOK _hook;

};

