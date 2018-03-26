#include "StdAfx.h"
#include "Sequence.h"


CSequence::CSequence(void)
{
	 m_BolIdSeq				= NULL;
	 m_BolId2Seq			= NULL;
	 m_BolVarSeq			= NULL;
	 m_BolStatusSeq			= NULL;
	 m_BolSeqCount		= 0;
	 m_RepeatSeqCount	= 1;
	 m_RepeatCountDown	= 1;
}


CSequence::~CSequence(void)
{
	if((m_BolIdSeq)|| (m_BolId2Seq)||(m_BolStatusSeq)) DeleteBolSequence();
}

bool CSequence::CreateBolSequence(CLoop loop)
{
	if(loop.m_BolCount==0) 
	{	
		m_BolSeqCount = 0;
		return true;
	}

	//allocate bol seq memory
	if((m_BolIdSeq) || (m_BolId2Seq) || (m_BolStatusSeq) || (m_BolVarSeq)) DeleteBolSequence();
	bool allocbolid		= m_BolIdSeq	 = new UINT[loop.m_BolCount];
	bool allocbolid2	= m_BolId2Seq	 = new UINT[loop.m_BolCount];
	bool allocbolvar	= m_BolVarSeq	 = new UINT[loop.m_BolCount];
	bool allocbolstatus = m_BolStatusSeq = new UINT[loop.m_BolCount];
	if(!(allocbolid && allocbolid2 && allocbolstatus && allocbolvar)) return false;

	//prepare bol sequence
	UINT nseq = 0;

	for(int by=0;by<MR;by++)
	{
		for(int bx=0;bx<MC;bx++)
		{
			UINT nbol = BOL_NONE;
				nbol = loop.GetBol(bx, by);

				if (nbol != BOL_NONE)
				{
					m_BolIdSeq[nseq] = loop.m_Bol[nbol].m_BolId;
					m_BolId2Seq[nseq] = loop.m_Bol[nbol].m_BolId2;
					m_BolVarSeq[nseq] = loop.m_Bol[nbol].m_BolId;
					m_BolStatusSeq[nseq] = loop.m_Bol[nbol].m_Status;

					//TRACE("[%d] = %d\r\n", nseq, m_BolIdSeq[nseq]);
					//TRACE("[%d] = %d\r\n", nseq, m_BolStatusSeq[nseq]);

					nseq++;
				}
		}

	}

	m_BolSeqCount	 = nseq;
	m_RepeatCountDown = m_RepeatSeqCount = loop.m_RepeatCount;
	return true;
}

void CSequence::DeleteBolSequence()
{
	if(m_BolIdSeq)		delete [] m_BolIdSeq;
	if (m_BolId2Seq)	delete [] m_BolId2Seq;
	if(m_BolVarSeq)		delete [] m_BolVarSeq;
	if(m_BolStatusSeq)	delete [] m_BolStatusSeq;
}

