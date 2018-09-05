#include "taskdialog.hpp"
#include "common.hpp"
#include "resource.h"
#include "ttberror.hpp"
#include "win32.hpp"

HRESULT TTBTaskDialog::CallbackProc(HWND hwnd, UINT uNotification, WPARAM wParam, LPARAM lParam, LONG_PTR dwRefData)
{
	if (uNotification == TDN_HYPERLINK_CLICKED)
	{
		win32::OpenLink(reinterpret_cast<const wchar_t *>(lParam));
		return S_OK;
	}
	else
	{
		auto pThis = reinterpret_cast<const TTBTaskDialog *>(dwRefData);
		return pThis->m_Callback(hwnd, uNotification, wParam, lParam);
	}
}

TTBTaskDialog::TTBTaskDialog(const std::wstring &title, const std::wstring &content, const callback_t &callback, const Window &parent) :
	m_Title(title),
	m_Content(content),
	m_Callback(callback),
	m_Cfg { sizeof(m_Cfg) }
{
	m_Cfg.hwndParent = parent;
	m_Cfg.hInstance = GetModuleHandle(NULL);
	m_Cfg.dwFlags = TDF_ENABLE_HYPERLINKS;
	m_Cfg.pszWindowTitle = NAME;
	m_Cfg.pszMainIcon = MAKEINTRESOURCE(MAINICON);
	m_Cfg.pszMainInstruction = m_Title.c_str();
	m_Cfg.pszContent = m_Content.c_str();
	m_Cfg.pfCallback = CallbackProc;
	m_Cfg.lpCallbackData = reinterpret_cast<LONG_PTR>(this);
}

bool TTBTaskDialog::Run(bool &checked)
{
	BOOL checkedB = checked;
	HRESULT hr = TaskDialogIndirect(&m_Cfg, nullptr, nullptr, &checkedB);
	if (FAILED(hr))
	{
		MessageBox(Window::NullWindow, (L"Failed to open task dialog.\n\n" + Error::ExceptionFromHRESULT(hr)).c_str(), NAME L" - Error", MB_ICONWARNING | MB_OK | MB_SETFOREGROUND);
		return false;
	}
	else
	{
		checked = checkedB;
		return true;
	}
}