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

#include "CTimezonePopup.h"

#include "CPreferences.h"
#include "CTextListChoice.h"
#include "CStringUtils.h"
#include "CUnicodeUtils.h"

#include "CICalendar.h"
#include "CICalendarManager.h"

#include <algorithm>

BEGIN_MESSAGE_MAP(CTimezonePopup, CPopupButton)
	ON_WM_CREATE()
END_MESSAGE_MAP()

// ---------------------------------------------------------------------------
//	CTimezonePopup														  [public]
/**
	Default constructor */

CTimezonePopup::CTimezonePopup() :
	CPopupButton(true)
{
	mNoFloating = false;
}


// ---------------------------------------------------------------------------
//	~CTimezonePopup														  [public]
/**
	Destructor */

CTimezonePopup::~CTimezonePopup()
{
}

#pragma mark -

int CTimezonePopup::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CPopupButton::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// Set menu
	SetMenu(IDR_POPUP_TIMEZONE);

	// Always start with the current user default
	SetTimezone(iCal::CICalendarManager::sICalendarManager->GetDefaultTimezone());
	
	return 0;
}

BOOL CTimezonePopup::SubclassDlgItem(UINT nID, CWnd* pParent)
{
	CPopupButton::SubclassDlgItem(nID, pParent, IDI_POPUPBTN, 0, 0, 0, true, false);

	// Set menu
	SetMenu(IDR_POPUP_TIMEZONE);

	// Always start with the current user default
	SetTimezone(iCal::CICalendarManager::sICalendarManager->GetDefaultTimezone());
	
	return true;
}


void CTimezonePopup::Reset(const iCal::CICalendarTimezone& tz)
{
	// Get the set of favorite timezones and always include the default
	cdstrvect tzids;
	cdstrset menutzids(CPreferences::sPrefs->mFavouriteTimezones.GetValue());
	menutzids.insert(iCal::CICalendarManager::sICalendarManager->GetDefaultTimezoneID());
	menutzids.insert(tz.GetTimezoneID());
	for(cdstrset::const_iterator iter = menutzids.begin(); iter != menutzids.end(); iter++)
		tzids.push_back(*iter);

	// Sort list of favorite timezones
	iCal::CICalendar::sICalendar.SortTimezones(tzids);

	// Clear existing menu
	CMenu* pPopup = GetPopupMenu();
	while(pPopup->GetMenuItemCount() > eFirstTimezone - (mNoFloating ? 1 : 0))
		pPopup->RemoveMenu(eFirstTimezone - (mNoFloating ? 1 : 0), MF_BYPOSITION);

	// Add to menu
	int menu_id = IDM_TIMEZONE_FIRST;
	for(cdstrvect::const_iterator iter = tzids.begin(); iter != tzids.end(); iter++)
	{
		// Convert from UTF8 data
		CUnicodeUtils::AppendMenuUTF8(pPopup, MF_STRING, menu_id++, *iter);
	}
}

void CTimezonePopup::NoFloating()
{
	// Remove first menu item
	CMenu* pPopup = GetPopupMenu();
	pPopup->RemoveMenu(eNoTimezone, MF_BYPOSITION);
	mNoFloating = true;

	// Always start with the current user default
	SetTimezone(iCal::CICalendarManager::sICalendarManager->GetDefaultTimezone());
}

void CTimezonePopup::SetTimezone(const iCal::CICalendarTimezone& tz)
{
	Reset(tz);

	// Extract zone from dt and apply to menu
	UINT new_value;
	if (tz.GetUTC())
	{
		new_value = IDM_TIMEZONE_UTC;
	}
	else
	{
		// Always set no timezone (or UTC when floating) so that if named timezone is not found we have a default
		new_value = mNoFloating ? IDM_TIMEZONE_UTC : IDM_TIMEZONE_ANY;

		cdstring tzid(tz.GetTimezoneID());
		if (!tzid.empty())
		{
			// Scan menu looking for match
			CMenu* pPopup = GetPopupMenu();
			uint32_t mcount = pPopup->GetMenuItemCount();
			UINT menu_id = IDM_TIMEZONE_FIRST;
			for(UInt32 i = eFirstTimezone; i <= mcount; i++, menu_id++)
			{
				// Get name of menu title
				cdstring txt = CUnicodeUtils::GetMenuStringUTF8(pPopup, menu_id, MF_BYCOMMAND);
				
				// Compare with tzid
				if (::strcmpnocase(tzid, txt) == 0)
				{
					new_value = menu_id;
					break;
				}
			}
		}
	}
	
	// Set value only if different
	if (new_value != GetValue())
	{
		SetValue(new_value);
		mOldValue = new_value;
	}
}

void CTimezonePopup::GetTimezone(iCal::CICalendarTimezone& tz) const
{
	UINT value = GetValue();
	tz.SetUTC(value == IDM_TIMEZONE_UTC);
	if ((value == IDM_TIMEZONE_ANY) || (value == IDM_TIMEZONE_UTC))
	{
		cdstring empty;
		tz.SetTimezoneID(empty);
	}
	else if (value == IDM_TIMEZONE_OTHER)
	{
		cdstrvect tzids;
		iCal::CICalendar::sICalendar.GetTimezones(tzids);
		std::sort(tzids.begin(), tzids.end());

		ulvector selected;
		if (CTextListChoice::PoseDialog(
			"Alerts::Calendar::TimezoneChoice::Title",
			"Alerts::Calendar::TimezoneChoice::Description",
			NULL, false, true, false, true, tzids, cdstring::null_str, selected,
			"Alerts::Calendar::TimezoneChoice::Button"))
		{
			// Get selection from list
			cdstring tzid = tzids.at(selected.front());
			tz.SetTimezoneID(tzid);
			CPreferences::sPrefs->mFavouriteTimezones.Value().insert(tzid);
			const_cast<CTimezonePopup*>(this)->SetTimezone(iCal::CICalendar::sICalendar.GetTimezone(tzid));
		}
		else
		{
			const_cast<CTimezonePopup*>(this)->SetValue(mOldValue);
		}
	}
	else
	{
		CMenu* pPopup = const_cast<CMenu*>(GetPopupMenu());

		// Get name of menu title
		cdstring tzid = CUnicodeUtils::GetMenuStringUTF8(pPopup, value, MF_BYCOMMAND);

		tz.SetTimezoneID(tzid);
	}
}
