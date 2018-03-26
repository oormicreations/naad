#include "StdAfx.h"
#include ".\bol.h"

CBol::CBol(void)
{
	m_BolId		= BOL_NONE;
	m_BolId2	= BOL_NONE;
	m_Status	= BOL_NORMAL;
	m_Species	= 0;
	m_Row		= 0;
	m_Col		= 0;
	m_Selected	= FALSE;
}

CBol::~CBol(void)
{
}

BOOL CBol::SetBolType(UINT id, UINT col, UINT row)
{
	if(id > BOL_NONE) return FALSE;

	CString bols[MAXBOLVARS*MAXBOLS] = BOLVARS;

	m_BolId		= id;
	m_Row		= row;
	m_Col		= col;
	m_Species	= id/MAXBOLVARS;
	
	if(id == BOL_NONE) 			m_BolName = "";
	else if(id == BOL_SILENCE) 	m_BolName = "~ o ~";
	else						m_BolName = bols[id];

	return TRUE;
}

BOOL CBol::SetBolAdditional(UINT id, UINT col, UINT row)
{
	if (id > BOL_NONE) return FALSE;

	CString bols[MAXBOLVARS*MAXBOLS] = BOLVARS;

	m_BolId2 = id;
	//m_Row = row;
	//m_Col = col;
	m_Species = id / MAXBOLVARS;

	if (id == BOL_NONE) 			m_BolName2 = "";
	else if (id == BOL_SILENCE) 	m_BolName2 = "~ o ~";
	else						m_BolName2 = bols[id];

	return TRUE;
}

BOOL CBol::SetBolType(UINT id)
{
	if(id > BOL_NONE) return FALSE;

	CString bols[MAXBOLVARS*MAXBOLS] = BOLVARS;

	m_BolId		= id;
	m_Species	= id/MAXBOLVARS;

	//TRACE("%d\r\n",id);
	if(id == BOL_NONE) 			m_BolName = "";
	else if(id == BOL_SILENCE) 	m_BolName = "~ o ~";
	else						m_BolName = bols[id];

	return TRUE;
}


BOOL CBol::SetBolStatus(UINT status)
{
	m_Status = status;
	if(status<MAXSTATUS)
	{
		CString stat[MAXSTATUS] = BOLSTATUS;
		m_BolStatusName = stat[status];
		return TRUE;
	}
	return FALSE;
}

void CBol::Clone(CBol srcbol)
{
	SetBolType(srcbol.m_BolId, srcbol.m_Col, srcbol.m_Row);
	SetBolStatus(srcbol.m_Status);
}

