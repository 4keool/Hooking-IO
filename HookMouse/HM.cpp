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
	// ����
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
	// ����ó��
	if (m_hDC == NULL)		return;
	if (m_SnapShot == NULL)	return;
	if (m_nWidth < 0)		return;
	if (m_nHeight < 0)		return;


	// ����
	CRect CaptureROI;
	CString dir;

	// ����
	CaptureROI.SetRect(0, 0, m_nWidth, m_nHeight);


	// Ȯ�εǸ� ����� rys
	// CCM�� ĸó
	// CDisp ���ܵǰ� ���� ��.
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
	// img = �׷��� ��ȭ��
	// roi = �׸��� ��ġ
	// hdc = �׸��� ��

	// ����ó��
	if (img == NULL)			return;
	if (hDC == NULL)			return;
	if (!RANGEIN(ROI.Width(), 0, m_nWidth))		return;
	if (!RANGEIN(ROI.Height(), 0, m_nHeight))	return;


	// ����
	ClearHDC();
	DrawMouseCursor(hDC, m_MousePos);
	::BitBlt(img.GetDC(), ROI.TopLeft().x, ROI.TopLeft().y, ROI.BottomRight().x, ROI.BottomRight().y, hDC, 0, 0, SRCCOPY);
}

void HM_CALLBACK::ClearHDC()
{
	// ��ȭ�� �ʱ�ȭ
	// ���� ������ ���콺 Ŀ���� ���� ���� �ܻ� ����
	
	// ����ó��
	if (m_nWidth < 0)		return;
	if (m_nHeight < 0)		return;


	// ����
	CRect rect;

	// ����
	rect.SetRect(0, 0, m_nWidth, m_nHeight);
	InvalidateRect(NULL, rect, TRUE);
}

void HM_CALLBACK::DrawMouseCursor(HDC hDC, POINT pos)
{
	// ��ȭ���� ���콺 Ŀ���� �׸�
	// ���� ������ ���콺 Ŀ�� ���� ȭ�� ĸó

	// ����ó��
	if (hDC == NULL)		return;
	if (m_hCursor == NULL)	return;


	// ����
	DrawIcon(hDC, pos.x, pos.y, (HICON)m_hCursor);
}

CString HM_CALLBACK::CreateDir()
{
	// ���� ��� ����
	
	// ����ó��
	if (m_nCnt < 0)		return _T("cntLessthan0");


	// ����
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
