#include "stdafx.h"


HM_CALLBACK* volatile HM_CALLBACK::_instance = nullptr;
std::mutex HM_CALLBACK::_mutex;

HM_CALLBACK::HM_CALLBACK() :_hook(nullptr)
{
	m_hDC = ::GetWindowDC(NULL);

	if (m_hDC != NULL)
	{
		m_nWidth = ::GetSystemMetrics(SM_CXSCREEN);
		m_nHeight = ::GetSystemMetrics(SM_CYSCREEN);
		m_nDepth = ::GetDeviceCaps(m_hDC, BITSPIXEL);

		m_SnapShot.Create(m_nWidth, m_nHeight, m_nDepth, 0);
	}

	SetCursor(LoadCursor(0, IDC_ARROW));
	m_hCursor = GetCursor();

	m_nCnt = 0;
	m_MousePos.x = 0;
	m_MousePos.y = 0;
}

HM_CALLBACK::~HM_CALLBACK()
{
	if (!m_SnapShot.IsNull())
		m_SnapShot.Destroy();
	if (m_hDC != NULL)
		::ReleaseDC(NULL, m_hDC);
}

LRESULT HM_CALLBACK::DoHookMouse(__in int code, __in WPARAM wparam, __in LPARAM lparam)
{
	// 선언
	MOUSEHOOKSTRUCT* HookData;


	do {
		if (code < HC_ACTION)
			break;

		HookData = (MOUSEHOOKSTRUCT*)lparam;
		m_MousePos = HookData->pt;


		switch (wparam)
		{
		case WM_LBUTTONDOWN:
			OutputDebugString(L"mouse left button down\n");
			if (m_MousePos.x < 0 || m_MousePos.y < 0)
				break;

			if (m_hDC != NULL)
			{
				SaveLog();
				m_nCnt++;
			}
			break;

		case WM_LBUTTONUP:
			OutputDebugString(L"mouse left button up\n");
			break;

		case WM_RBUTTONDOWN:
			OutputDebugString(L"mouse right button down\n");
			break;

		case WM_RBUTTONUP:
			OutputDebugString(L"mouse right button up\n");
			break;

		case WM_MOUSEMOVE:
			//OutputDebugString(L"mouse move");
			break;

		default:
			break;
		}

	} while (false);

	return ::CallNextHookEx(_hook, code, wparam, lparam);
}

void HM_CALLBACK::SaveLog()
{
	// 예외처리
	if (m_hDC == NULL)		return;
	if (m_SnapShot == NULL)	return;
	if (m_nWidth < 0)		return;
	if (m_nHeight < 0)		return;


	// 선언
	CRect CaptureROI;
	CString dir;

	// 동작
	CaptureROI.SetRect(0, 0, m_nWidth, m_nHeight);


	// 확인되면 지울것 rys
	// CCM만 캡처
	// CDisp 제외되고 복사 됨.
	/*HWND hwndCCM = ::FindWindow(NULL, _T("CCM"));
	HDC h_dc = ::GetDC(hwndCCM);
	if (h_dc == NULL)
		return;*/

	CaptureDC(m_SnapShot, CaptureROI, m_hDC);

	dir = CreateDir();
	m_SnapShot.Save(dir, Gdiplus::ImageFormatJPEG);
}

void HM_CALLBACK::CaptureDC(CImage &img, CRect ROI, HDC hDC)
{
	// img = 그려질 도화지
	// roi = 그리는 위치
	// hdc = 그리는 펜

	// 예외처리
	if (img == NULL)			return;
	if (hDC == NULL)			return;
	if (!RANGEIN(ROI.Width(), 0, m_nWidth))		return;
	if (!RANGEIN(ROI.Height(), 0, m_nHeight))	return;


	// 동작
	ClearHDC();
	DrawMouseCursor(hDC, m_MousePos);
	::BitBlt(img.GetDC(), ROI.TopLeft().x, ROI.TopLeft().y, ROI.BottomRight().x, ROI.BottomRight().y, hDC, 0, 0, SRCCOPY);
}

void HM_CALLBACK::ClearHDC()
{
	// 도화지 초기화
	// 하지 않으면 마우스 커서와 같은 것이 잔상 남음
	
	// 예외처리
	if (m_nWidth < 0)		return;
	if (m_nHeight < 0)		return;


	// 선언
	CRect rect;

	// 동작
	rect.SetRect(0, 0, m_nWidth, m_nHeight);
	InvalidateRect(NULL, rect, TRUE);
}

void HM_CALLBACK::DrawMouseCursor(HDC hDC, POINT pos)
{
	// 도화지에 마우스 커서를 그림
	// 하지 않으면 마우스 커서 없는 화면 캡처

	// 예외처리
	if (hDC == NULL)		return;
	if (m_hCursor == NULL)	return;


	// 동작
	DrawIcon(hDC, pos.x, pos.y, (HICON)m_hCursor);
}

CString HM_CALLBACK::CreateDir()
{
	// 저장 경로 생성
	
	// 예외처리
	if (m_nCnt < 0)		return _T("cntLessthan0");


	// 동작
	CString res;
	res.Format(_T("rystest%d.jpg"), m_nCnt);
	return res;
}

BOOL HM_CALLBACK::RANGEIN(double target, double min, double max)
{
	BOOL res = TRUE;

	if (target < min)
		res = FALSE;
	if (target > max)
		res = FALSE;

	return res;
}
