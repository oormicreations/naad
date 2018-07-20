#pragma once
#include "Wave.h"

class CWaveRender
{
public:
	CWaveRender();
	~CWaveRender();

	CWave m_Src;
	CWave m_Dest;

	BOOL CreateDesWav();
	BOOL Render(CString emptypath);
};

