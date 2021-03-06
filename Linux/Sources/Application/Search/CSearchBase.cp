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


// CSearchBase.cp : implementation of the CSearchBase class
//

#include "CSearchBase.h"

#include "CMulberryApp.h"
#include "CSearchCriteriaContainer.h"

#include <JXTextButton.h>

/////////////////////////////////////////////////////////////////////////////
// CSearchBase

/////////////////////////////////////////////////////////////////////////////
// CSearchBase construction/destruction

CSearchBase::CSearchBase(bool rules)
{
	mRules = rules;
	mGroupItems = NULL;
	mFilterType = CFilterItem::eLocal;
}

CSearchBase::~CSearchBase()
{
	// Always delete criteria items
	delete mGroupItems;
}

// Respond to list changes
void CSearchBase::ListenTo_Message(long msg, void* param)
{
	// For time being reset entire menu
	switch(msg)
	{
	case CSearchCriteriaContainer::eBroadcast_SearchCriteriaContainerResized:
		// Change the size of this one
		Resized(*reinterpret_cast<unsigned long*>(param));
		break;
	default:;
	}
}

void CSearchBase::OnMore()
{
	AddCriteria();
}

void CSearchBase::OnFewer()
{
	RemoveCriteria();
}

void CSearchBase::OnClear()
{
	// Remove all
	while(mGroupItems->GetCount() > 0)
		RemoveCriteria();

	// Reset the first one
	AddCriteria();
}

#pragma mark ____________________________Criteria Panels

const int cCriteriaPanelHeight = 46;
const int cCriteriaPanelWidth = 496;
const int cCriteriaHOffset = 4;
const int cCriteriaVInitOffset = 6;

void CSearchBase::MakeGroup()
{
	if (mGroupItems != NULL)
		return;

	JRect gsize;
	gsize.left = cCriteriaHOffset;
	gsize.right = gsize.left + cCriteriaPanelWidth;
	gsize.top = cCriteriaVInitOffset;
	gsize.bottom = gsize.top + cCriteriaPanelHeight;
	mGroupItems = new CSearchCriteriaContainer(GetContainerWnd(), JXWidget::kHElastic, JXWidget::kFixedTop, gsize.left, gsize.top, gsize.width(), gsize.height());
	mGroupItems->SetTopLevel();
	mGroupItems->SetRules(mRules);
	mGroupItems->OnCreate();
	mGroupItems->InitGroup(mFilterType, NULL);

	// Get size after init'ing the group
	gsize = mGroupItems->GetFrame();

	// Set group width based on parent
	JRect size = GetContainerWnd()->GetAperture();
	mGroupItems->AdjustSize(size.width() - 2 * cCriteriaHOffset - gsize.width(), 0);

	// Increase the size of this one
	Resized(gsize.height());

	// Position new sub-panel
	mGroupItems->Show();
	mGroupItems->Add_Listener(this);
}

void CSearchBase::InitCriteria(const CSearchItem* spec)
{
	// Always make the group - it will only be done if not done already
	MakeGroup();

	// Pass to the group
	RemoveAllCriteria();
	mGroupItems->InitCriteria(spec);

	// Do button state
	if (mGroupItems->GetCount() > 1)
		GetFewerBtn()->Show();
}

void CSearchBase::AddCriteria(const CSearchItem* spec, bool use_or)
{
	// Pass to the group
	if (mGroupItems)
		mGroupItems->AddCriteria(spec, use_or);

	// Do button state
	if (mGroupItems->GetCount() > 1)
		GetFewerBtn()->Show();
}

void CSearchBase::RemoveCriteria(unsigned long num)
{
	// Pass to the group
	if (mGroupItems)
		mGroupItems->RemoveCriteria(num);

	// Do button state
	if (mGroupItems->GetCount() < 2)
		GetFewerBtn()->Hide();
}

void CSearchBase::RemoveAllCriteria()
{
	// Remove all
	if (mGroupItems)
		mGroupItems->RemoveAllCriteria();

	// Do button state
	GetFewerBtn()->Hide();
}

void CSearchBase::SelectNextCriteria(CSearchCriteria* previous)
{
	// Pass to the group
	if (mGroupItems)
		mGroupItems->SelectNextCriteria(previous);
}

#pragma mark ____________________________Build Search

CSearchItem* CSearchBase::ConstructSearch() const
{
	// Pass to the group
	return mGroupItems->ConstructSearch();
}
