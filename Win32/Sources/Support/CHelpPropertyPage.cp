/*
    Copyright (c) 2007 Cyrus Daboo. All rights reserved.
    
    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at
    
        http://www.apache.org/licenses/LICENSE-2.0
    
    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/


// CHelpPropertyPage.cpp : implementation file
//


#include "CHelpPropertyPage.h"

#include "CSDIFrame.h"

/////////////////////////////////////////////////////////////////////////////
// CHelpPropertyPage property page

IMPLEMENT_DYNCREATE(CHelpPropertyPage, CPropertyPage)

CHelpPropertyPage::CHelpPropertyPage()
{
}

CHelpPropertyPage::CHelpPropertyPage(UINT nID) : CPropertyPage(nID)
{
}

BEGIN_MESSAGE_MAP(CHelpPropertyPage, CPropertyPage)
	//{{AFX_MSG_MAP(CHelpPropertyPage)
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHelpPropertyPage message handlers

BOOL CHelpPropertyPage::OnHelpInfo(HELPINFO* pHelpInfo)
{
	if ((pHelpInfo->iContextType == HELPINFO_WINDOW) && (pHelpInfo->iCtrlId > 0))
	{
		// finally, run the Windows Help engine
		if (!::WinHelp((HWND) pHelpInfo->hItemHandle, ::AfxGetApp()->m_pszHelpFilePath, HELP_CONTEXTPOPUP, (DWORD) HID_BASE_RESOURCE + pHelpInfo->iCtrlId))
			::AfxMessageBox(AFX_IDP_FAILED_TO_LAUNCH_HELP);
		
		return TRUE;
	}
	else
		return CPropertyPage::OnHelpInfo(pHelpInfo);
}
