/*
    Copyright (c) 2007-2009 Cyrus Daboo. All rights reserved.
    
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

#ifndef H_CNewComponentDialog
#define H_CNewComponentDialog
#pragma once

#include "CModelessDialog.h"
#include "CListener.h"

#include "CICalendarComponentRecur.h"
#include "CICalendarComponentExpanded.h"

#include "CCalendarPopup.h"
#include "CCmdEdit.h"
#include "CPopupButton.h"
#include "CTabController.h"

class CNewTimingPanel;

class CNewComponentPanel;
typedef std::vector<CNewComponentPanel*> CNewComponentPanelList;

// ===========================================================================
//	CNewComponentDialog

class CNewComponentDialog : public CModelessDialog, public CListener
{
public:
	enum EModelessAction
	{
		eNew,
		eEdit,
		eDuplicate
	};

	CNewComponentDialog();
	virtual ~CNewComponentDialog();

	virtual void		ListenTo_Message(long msg, void* param);

	iCal::CICalendarRef GetCurrentCalendar() const;
	cdstring		GetCurrentSummary() const;
	void			GetCurrentPeriod(iCal::CICalendarPeriod& period) const;
	const CNewTimingPanel* GetTimingPanel() const;

	bool ContainsComponent(const iCal::CICalendarComponentRecur& vcomponent) const;

protected:
	static uint32_t					sTitleCounter;
	static std::set<CNewComponentDialog*>	sDialogs;

// Dialog Data
	//{{AFX_DATA(CNewEventDialog)
	enum { IDD = IDD_CALENDAR_NEWEVENT };
	CCmdEdit				mSummary;
	CCalendarPopup			mCalendar;

	CTabController			mTabs;
	CButton					mOrganiserEdit;
	//}}AFX_DATA

	EModelessAction			mAction;
	CNewComponentPanelList	mPanels;
	iCal::CICalendarComponentRecur*	mComponent;
	const iCal::CICalendarComponentExpanded* mExpanded;
	bool					mRecurring;
	bool					mIsOverride;
	bool					mReadOnly;
	bool					mAttended;

	static void StartModeless(iCal::CICalendarVEvent& vevent, EModelessAction action);

	virtual void		InitPanels() = 0;
	
			void	SetAction(EModelessAction action)
			{
				mAction = action;
			}
	virtual void	SetComponent(iCal::CICalendarComponentRecur& vcomponent, const iCal::CICalendarComponentExpanded* expanded = NULL);
	virtual void	GetComponent(iCal::CICalendarComponentRecur& vcomponent);
			void	ChangedSummary();
			void	ChangedCalendar();
	virtual void	ChangedMyStatus(const iCal::CICalendarProperty& attendee, const cdstring& new_status);

			void	SetReadOnly(bool read_only);

	virtual uint32_t&	TitleCounter()
	{
		return sTitleCounter;
	}

	virtual bool	DoNewOK() = 0;
	virtual bool	DoEditOK() = 0;
	virtual void	DoCancel()  = 0;
			
			bool	GetCalendar(iCal::CICalendarRef oldcal, iCal::CICalendarRef& newcal, iCal::CICalendar*& new_cal);

	// Generated message map functions
	//{{AFX_MSG(CNewEventDialog)
	virtual BOOL	OnInitDialog();
	afx_msg void	OnChangeSummary();
	afx_msg void	OnChangeCalendar();
	afx_msg void	OnSelChangeTabs(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void	OnOrganiserEdit();
	afx_msg void	OnOK();
	afx_msg void	OnCancel();
	//}}AFX_MSG

public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedCalendarNeweventAvailability();
};

#endif
