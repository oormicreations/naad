// Naadh.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

#define VERSION		11
#define VERMONTH	"March 2018"

#define MAXRESRV	16
#define MAXGHUNGRU	5
#define MAXTANPURA	1
#define MAXBPM		800
#define DEFAULT_BPM	180

#define FILETYPE_COMP 1
#define FILETYPE_LOOP 2
#define FILETYPE_SETT 3


//#define BOLFILE		{"Dha0^.wav", "Dhin0^.wav", "Dhit0^.wav", "Dhun0^.wav", "Ga0^.wav", "Ge0^.wav", "Ke0^.wav", "Na0^.wav", "Tat0^.wav", "Ti0^.wav", "Tin0^.wav", "Tun0^.wav"}

// CNaadApp:
// See Naadh.cpp for the implementation of this class
//

class CNaadApp : public CWinApp
{
public:
	CNaadApp();

// Overrides
	public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CNaadApp theApp;

