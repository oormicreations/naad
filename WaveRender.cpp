#include "stdafx.h"
#include "WaveRender.h"


CWaveRender::CWaveRender()
{
}


CWaveRender::~CWaveRender()
{
}

BOOL CWaveRender::CreateDesWav()
{
	BOOL bResult = false;
	//m_Dest.

	return bResult;
}

BOOL CWaveRender::Render(CString bolpath)
{
	BOOL bResult = false;
	CString str;
	str = bolpath + "\\Acc\\Render.wav";

	m_Dest.Load((LPTSTR)str.GetBuffer());
	//m_Src.Play();

	str = bolpath + "\\Na\\Na01.wav";
	m_Src.Load((LPTSTR)str.GetBuffer());

	m_Dest.MixWithDelay(m_Src, 2000);

	return bResult;
}
