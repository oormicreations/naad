#pragma once
#include "loop.h"

class CSequence
{
public:
	CSequence(void);
	~CSequence(void);

	UINT *m_BolIdSeq, *m_BolStatusSeq, *m_BolVarSeq, *m_BolId2Seq;
	UINT m_BolSeqCount, m_RepeatSeqCount, m_RepeatCountDown;

	bool CreateBolSequence(CLoop loop);

private:
	void DeleteBolSequence(void);
};

