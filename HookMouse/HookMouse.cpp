// PSV_IF.cpp : �ش� DLL�� �ʱ�ȭ ��ƾ�� �����մϴ�.
//

#include "stdafx.h"
#include <afxdllx.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


static AFX_EXTENSION_MODULE HOOKDLL = { NULL, NULL };

extern "C" int APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
		HM_CALLBACK::instance()->attach(hInstance);
// 		if (HM_CALLBACK::instance()->attach(hInstance)) {
// 			MessageBox(nullptr, L"hook success", L"analog_note", MB_OK);
// 		}
// 		else {
// 			MessageBox(nullptr, L"hook success", L"analog_note", MB_OK);
// 		}

		break;

	case DLL_THREAD_ATTACH:
		break;

	case DLL_THREAD_DETACH:
		if (HM_CALLBACK::instance()->attached()) {
			HM_CALLBACK::instance()->detach();
		}

		break;

	case DLL_PROCESS_DETACH:
		break;
	}

	// Remove this if you use lpReserved
	UNREFERENCED_PARAMETER(lpReserved);

	if (dwReason == DLL_PROCESS_ATTACH)
	{
		TRACE("HOOK.DLL Initializing!\n");

		// Extension DLL one-time initialization
		if (!AfxInitExtensionModule(HOOKDLL, hInstance))
			return 0;

		// Insert this DLL into the resource chain
		// NOTE: If this Extension DLL is being implicitly linked to by
		//  an MFC Regular DLL (such as an ActiveX Control)
		//  instead of an MFC application, then you will want to
		//  remove this line from DllMain and put it in a separate
		//  function exported from this Extension DLL.  The Regular DLL
		//  that uses this Extension DLL should then explicitly call that
		//  function to initialize this Extension DLL.  Otherwise,
		//  the CDynLinkLibrary object will not be attached to the
		//  Regular DLL's resource chain, and serious problems will
		//  result.

		new CDynLinkLibrary(HOOKDLL);
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		TRACE0("HOOK.DLL Terminating!\n");
		// Terminate the library before destructors are called
		AfxTermExtensionModule(HOOKDLL);
	}
	return 1;   // ok
}
