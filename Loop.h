#pragma once
#include "bol.h"
#include <vector>


class CLoop
{
public:
	CLoop(void);
	~CLoop(void);
//------------------------------------------------------------------------------------------------------------
	CString m_LoopName, m_Note;
	UINT	m_MaatraBol, m_RepeatCount;//, m_Delay; //delay is in millisec
	UINT	m_BolCount, m_LoopBpm;
	UINT	m_Row, m_Col;
	UINT	m_Accompaniment;
	BOOL	m_Selected;
	//BOOL	m_B2B;								 //back to back play
	std::vector< CBol > m_Bol;

private:
	UINT	m_MatraCount;

//------------------------------------------------------------------------------------------------------------

public:
	BOOL AddBol(void);
	BOOL DeleteBol(UINT nbol);
	UINT GetBol(UINT col, UINT row);
	CString GetMatraCount(void);
	void DeSelectAll(void);
	void Clone(CLoop srcloop);

//------------------------------------------------------------------------------------------------------------

};
