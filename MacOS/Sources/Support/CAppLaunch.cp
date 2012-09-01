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


// CAppLaunch.cp

// Class that launches an app and opens a document in it

#include "CAppLaunch.h"

#include <LaunchServices.h>

// OpenSpecifiedDocument searches to see if the application which
// created the document is already running.  If so, it sends
// an OpenSpecifiedDocuments Apple event to the target application
// (remember that, because of puppet strings, this works even
// if the target application is not Apple event-aware.)

OSErr CAppLaunch::OpenDocumentWithApp(const PPx::FSObject* doc, OSType appCreator)
{
	// verify the document file exists and get its creator type

	if (!doc->Exists())
		return fnfErr;

	return LaunchApplicationWithDocument(appCreator, doc);
}

//----------------------------------------------------------------------------
// LaunchApplicationWithDocument
//
// given an application and any number of documents,
// LaunchApplicationWithDocument launches the application and passes the
// application an OpenDocuments event for the document(s)
//----------------------------------------------------------------------------
OSErr CAppLaunch::LaunchApplicationWithDocument(OSType appCreator, const PPx::FSObject* doc)
{
	// Code taken from TechNote #2017 on Launch Services
	
	// Find the application on disk
	CFURLRef outAppURL;
    const CFURLRef itemURL[1] = {CFURLCreateFromFSRef(NULL, &doc->UseRef())};
    CFArrayRef itemURLs;
	OSErr err;
	
	// Try to launch it with the document

    LSLaunchURLSpec inLaunchSpec;
    err = ::LSGetApplicationForInfo(kLSUnknownType, appCreator, NULL, kLSRolesAll, NULL, &outAppURL);
    itemURLs = CFArrayCreate(NULL, (const void **)itemURL, 1, &kCFTypeArrayCallBacks);
    CFRelease(itemURL[0]);
    inLaunchSpec.appURL = (err == noErr) ? outAppURL : NULL;
    inLaunchSpec.itemURLs = itemURLs;
    inLaunchSpec.passThruParams = NULL;
    inLaunchSpec.launchFlags = kLSLaunchDefaults;
    inLaunchSpec.asyncRefCon = NULL;
    
    err = ::LSOpenFromURLSpec(&inLaunchSpec, NULL);
    
    if (inLaunchSpec.appURL != NULL)
        CFRelease(outAppURL);
    CFRelease(itemURLs);
    
	return err;
}
