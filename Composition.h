#pragma once
#include "loop.h"


class CComposition
{
public:
	CComposition(void);
	~CComposition(void);

//----------------------------------------------------------------------------------------------------------------

	CString m_CompositionName, m_ComposerName, m_Notes;
	UINT	m_LoopCount;
	std::vector< CLoop > m_Loop;
//----------------------------------------------------------------------------------------------------------------

	BOOL AddLoop(UINT col, UINT row);
	BOOL DeleteLoop(UINT nloop);
	UINT GetLoop(UINT col, UINT row);
	void DoLoopPropDlg(UINT nloop);
	void DoCompositionPropDlg(void);
	UINT GetRandValue(UINT min, UINT max);
	void AutoCompose(void);
	void SetBolSelection(UINT nloop, UINT col, UINT row, BOOL sel);
	void SetLoopSelection(UINT col, UINT row, BOOL sel);
	void DeSelectAll(void);
	void Clear(void);
//----------------------------------------------------------------------------------------------------------------
};
