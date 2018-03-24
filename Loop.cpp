#include "StdAfx.h"
#include "Naadh.h"
#include ".\loop.h"

CLoop::CLoop(void)
{
	m_MaatraBol			= 1;
	m_RepeatCount		= 1;	//play once by default
	m_BolCount			= 0;
	m_MatraCount		= 0;
	m_Row				= 0;
	m_Col				= 0;
	m_Accompaniment		= 0;
	m_Selected			= FALSE;
	//m_Delay				= 0;
	m_LoopBpm			= DEFAULT_BPM;
	//m_B2B				= TRUE;
	m_LoopName			= "New Loop";
}

CLoop::~CLoop(void)
{
}

BOOL CLoop::AddBol(void)
{
	CBol bol;
	m_Bol.push_back(bol);
	m_BolCount ++;

	m_MatraCount = m_BolCount/m_MaatraBol;

	return TRUE;
}

UINT CLoop::GetBol(UINT col, UINT row)
{
	for(UINT x=0; x<m_BolCount; x++)
	{
		if(m_Bol[x].m_Col == col)
		{
			if(m_Bol[x].m_Row == row) return x;
		}
	}
	return BOL_NONE;
}

BOOL CLoop::DeleteBol(UINT nbol)
{
	m_Bol.erase(m_Bol.begin()+nbol);
	m_BolCount --;

	m_MatraCount = m_BolCount/m_MaatraBol;

	return TRUE;
}

CString CLoop::GetMatraCount()
{
	CString t;
	UINT frac = m_BolCount % m_MaatraBol;
	m_MatraCount = m_BolCount/m_MaatraBol;
	if(frac) t.Format("%d-%d/%d Maatras", m_MatraCount, frac, m_MaatraBol);
	else t.Format("%d Maatras", m_MatraCount);
	return t;
}

void CLoop::DeSelectAll(void)
{
	for(UINT x=0; x<m_BolCount ; x++)
	{
		m_Bol[x].m_Selected = FALSE;
	}
}

void CLoop::Clone(CLoop srcloop)
{
	m_MaatraBol			= srcloop.m_MaatraBol;
	m_RepeatCount		= srcloop.m_RepeatCount;	//play once by default
	//m_BolCount			= srcloop.m_BolCount;
	//m_MatraCount		= srcloop.m_MatraCount;
	//m_Row				= 0;
	//m_Col				= 0;
	m_Accompaniment		= srcloop.m_Accompaniment;
	//m_Selected			= FALSE;
	//m_Delay				= 0;
	m_LoopBpm			= srcloop.m_LoopBpm;
	//m_B2B				= TRUE;
	m_LoopName			= srcloop.m_LoopName;
	for(UINT x=0; x<srcloop.m_BolCount ; x++)
	{
		AddBol();
		m_Bol[x].Clone(srcloop.m_Bol[x]);
	}

}
