#include "StdAfx.h"
#include ".\composition.h"
#include "LoopPropDlg.h"
#include "CompPropDlg.h"

CComposition::CComposition(void)
{
	m_LoopCount			= 0;
	m_CompositionName	= "New Composition";
}

CComposition::~CComposition(void)
{
}

BOOL CComposition::AddLoop(UINT col, UINT row)
{
	CLoop loop;
	
	m_Loop.push_back(loop);
	m_Loop[m_LoopCount].m_LoopName.Format("New Loop %d", m_LoopCount+1);
	m_Loop[m_LoopCount].m_Col = col;
	m_Loop[m_LoopCount].m_Row = row;

	m_LoopCount ++;
	return TRUE;
}

BOOL CComposition::DeleteLoop(UINT nloop)
{
	if(nloop == LOOP_NONE) return FALSE;

	m_Loop.erase(m_Loop.begin()+nloop);
	m_LoopCount --;

	return TRUE;
}

UINT CComposition::GetLoop(UINT col, UINT row)
{
	for(UINT x=0; x<m_LoopCount; x++)
	{
		if(m_Loop[x].m_Col == col)
		{
			if(m_Loop[x].m_Row == row) return x;
		}
	}
	return LOOP_NONE;
}

void CComposition::DoLoopPropDlg(UINT nloop)
{
	CLoopPropDlg loopdlg;
	loopdlg.m_Name		= m_Loop[nloop].m_LoopName;
	loopdlg.m_Repeat	= m_Loop[nloop].m_RepeatCount;
	loopdlg.m_Acc		= m_Loop[nloop].m_Accompaniment;
	loopdlg.m_Matra		= m_Loop[nloop].m_MaatraBol;
	loopdlg.m_Note		= m_Loop[nloop].m_Note;
	loopdlg.m_Bpm		= m_Loop[nloop].m_LoopBpm;//(double)m_Loop[nloop].m_Delay/1000.0;
	//loopdlg.m_B2B		= m_Loop[nloop].m_B2B;
	loopdlg.m_MatraCountStr = m_Loop[nloop].GetMatraCount();

	if(IDCANCEL==loopdlg.DoModal()) return;

	if(loopdlg.m_Repeat < 1) loopdlg.m_Repeat = 1;

	m_Loop[nloop].m_LoopName		= loopdlg.m_Name;
	m_Loop[nloop].m_RepeatCount		= loopdlg.m_Repeat;
	m_Loop[nloop].m_Accompaniment	= loopdlg.m_Acc;
	m_Loop[nloop].m_MaatraBol		= loopdlg.m_Matra;
	m_Loop[nloop].m_Note			= loopdlg.m_Note;
	m_Loop[nloop].m_LoopBpm			= loopdlg.m_Bpm;//(UINT)(loopdlg.m_Delay*1000.0);
	//m_Loop[nloop].m_B2B				= loopdlg.m_B2B;
}

void CComposition::DoCompositionPropDlg()
{
	CCompPropDlg compdlg;

	compdlg.m_Name		= m_CompositionName;
	compdlg.m_Composer	= m_ComposerName;
	compdlg.m_Notes		= m_Notes;

	if(IDCANCEL==compdlg.DoModal()) return;

	m_CompositionName	= compdlg.m_Name;
	m_ComposerName		= compdlg.m_Composer;
	m_Notes				= compdlg.m_Notes;
}

UINT CComposition::GetRandValue(UINT min, UINT max)
{
	UINT r = 0;
	if(min==0) return rand() % max;
	while(r<min) r = rand() % max;
	return r;
}

void CComposition::SetBolSelection(UINT nloop, UINT col, UINT row, BOOL sel)
{
	if(nloop == LOOP_NONE) return;
	UINT nbol = m_Loop[nloop].GetBol(col, row);
	if(nbol == BOL_NONE) return;
	
	m_Loop[nloop].m_Bol[nbol].m_Selected = sel;
}

void CComposition::SetLoopSelection(UINT col, UINT row, BOOL sel)
{
	UINT nloop = GetLoop(col, row);
	if(nloop == LOOP_NONE) return;
	
	m_Loop[nloop].m_Selected = sel;
}

void CComposition::DeSelectAll()
{
	for(UINT x=0; x<m_LoopCount; x++)
	{
		m_Loop[x].m_Selected = FALSE;
		m_Loop[x].DeSelectAll();
	}
}

void CComposition::Clear()
{
	UINT count = m_LoopCount;
	for(UINT x=0; x<count; x++)
	{
		DeleteLoop(0);
	}
}

void CComposition::AutoCompose()
{	
	//seed rand
	CTime time = CTime::GetCurrentTime();
	srand(time.GetSecond());

	//info
	m_ComposerName	 = "Naad AI";
	m_CompositionName  = "AutoComp";
	m_Notes			+= "\r\n\r\nAutomatically composed by Naad AI on " + time.Format("%H:%M:%S %d/%m/%Y");

	//params
	UINT maxmatra		= 16;
	UINT minmatra		= 3;

	UINT maxloops		= 10;
	UINT minloops		= 7;

	UINT maxbols		= 16;
	UINT minbols		= 4;

	UINT maxrepeats		= 3;
	UINT minrepeats		= 1;

	UINT maxmatrabols	= 4;
	UINT minmatrabols	= 1;

	UINT maxboltypes	= MAXBOLVARS*MAXBOLS - 1;

	//choose a maatra
	UINT rmatra = 6;//GetRandValue(minmatra, maxmatra);

	//loops
	UINT rloop = GetRandValue(minloops, maxloops);
	for(UINT x=0; x<rloop; x++)
	{
		AddLoop(0,x);
	}

	//peshkar
	m_Loop[0].m_LoopName = "Peshkar";
	m_Loop[0].m_RepeatCount = 1;
	m_Loop[0].m_MaatraBol = 4;
	m_Loop[0].m_LoopBpm = 90;

	//kaida
	m_Loop[1].m_LoopName = "Kaida";
	m_Loop[1].m_RepeatCount = 4;

	//rela
	m_Loop[2].m_LoopName = "Rela";
	m_Loop[2].m_RepeatCount = 4;

	//prakar
	m_Loop[3].m_LoopName = "Prakar";
	m_Loop[3].m_RepeatCount = 4;

	//gat
	m_Loop[4].m_LoopName = "Gat";
	m_Loop[4].m_RepeatCount = 4;

	//chakradhar
	m_Loop[5].m_LoopName = "Chakradhar";
	m_Loop[5].m_RepeatCount = 3;

	//tod
	m_Loop[6].m_LoopName = "Tod";
	m_Loop[6].m_RepeatCount = 1;

	//frequent tukdas for peshkar
	UINT tpeshkar[6][4] = 
		{20,118,110,11,					//ti ra ke ta
		105,100,80,50,					//ge ge dhun dha
		BOL_SILENCE,60,BOL_SILENCE,0,	//dhin na
		110,20,11,BOL_SILENCE,			//ke ti ta
		60,BOL_SILENCE,0,0,				//dhin na na
		60,40,0,0};						//dhin tun na na


	//insert tukdas
	//UINT nbol = 0;
	//UINT row = 0, col = 0;	
	//for(UINT y=0; y<rmatra; y++)
	//{
	//	for(UINT x=0; x<4; x++)
	//	{
	//		m_Loop[0].AddBol();
	//		m_Loop[0].m_Bol[nbol].SetBolType(tpeshkar[y][x], col, row);
	//		nbol++;
	//		col++;
	//		if(col>7)
	//		{
	//			col = 0;
	//			row++;
	//		}
	//	}
	//}


	//for(UINT y=4; y<rmatra*m_Loop[0].m_MaatraBol; y++)
	//{
	//	m_Loop[0].AddBol();
	//	m_Loop[0].m_Bol[y].SetBolType(BOL_SILENCE, col, row);
	//	col++;
	//}





		////rand bols
		//UINT rbol = 0;
		//while(rbol<1) rbol = rand() % maxbols;

		//for(UINT y=0; y<rbol; y++)
		//{
		//	m_Loop[x].AddBol();
		//	m_Loop[x].m_Bol[y].SetBolType(rand() % maxboltypes, y, 0);
		//}
	

}
