#pragma once

#define MC			24 //max cols
#define MR			24 //max rows

#define MAXBOLS		12
#define MAXBOLVARS	10	//varieties of bols
#define MAXSTATUS	4	//0123

#define LOOP_NONE	0xFFFD

#define BOLS		{"Na", "Ta", "Ti", "Tin", "Tun", "Dha", "Dhin", "Dhit", "Dhun", "Ga", "Ge", "Ke"}
#define BOLSTATUS	{"", "~S~", "~T~", "~K~"}
#define BOL_NONE	0xFFFF
#define BOL_SILENCE	0xFFFE

#define BOL_NORMAL	0
#define BOL_SAM		1
#define BOL_TALI	2
#define BOL_KHALI	3

#define BOLVARS		{\
/*000*/ "Na01",		"Na02",		"Na03",		"Na04",		"Na05",		"Na06",		"Na07",		"Na08",		"Na09",		"Na10",\
/*010*/ "Ta01",		"Ta02",		"Ta03",		"Ta04",		"Ta05",		"Ta06",		"Ta07",		"Ta08",		"Ta09",		"Ta10",\
/*020*/ "Ti01",		"Ti02",		"Ti03",		"Ti04",		"Ti05",		"Ti06",		"Ti07",		"Ti08",		"Ti09",		"Ti10",\
/*030*/ "Tin01",	"Tin02",	"Tin03",	"Tin04",	"Tin05",	"Tin06",	"Tin07",	"Tin08",	"Tin09",	"Tin10",\
/*040*/ "Tun01",	"Tun02",	"Tun03",	"Tun04",	"Tun05",	"Tun06",	"Tun07",	"Tun08",	"Tun09",	"Tun10",\
/*050*/ "Dha01",	"Dha02",	"Dha03",	"Dha04",	"Dha05",	"Dha06",	"Dha07",	"Dha08",	"Dha09",	"Dha10",\
/*060*/ "Dhin01",	"Dhin02",	"Dhin03",	"Dhin04",	"Dhin05",	"Dhin06",	"Dhin07",	"Dhin08",	"Dhin09",	"Dhin10",\
/*070*/ "Dhit01",	"Dhit02",	"Dhit03",	"Dhit04",	"Dhit05",	"Dhit06",	"Dhit07",	"Dhit08",	"Dhit09",	"Dhit10",\
/*080*/ "Dhun01",	"Dhun02",	"Dhun03",	"Dhun04",	"Dhun05",	"Dhun06",	"Dhun07",	"Dhun08",	"Dhun09",	"Dhun10",\
/*090*/ "Ga01",		"Ga02",		"Ga03",		"Ga04",		"Ga05",		"Ga06",		"Ga07",		"Ga08",		"Ga09",		"Ga10",\
/*100*/ "Ge01",		"Ge02",		"Ge03",		"Ge04",		"Ge05",		"Ge06",		"Ge07",		"Ge08",		"Ge09",		"Ge10",\
/*110*/ "Ke01",		"Ke02",		"Ke03",		"Ke04",		"Ke05",		"Ke06",		"Ke07",		"Ke08",		"Ke09",		"Ke10" }
//		0			1			2			3			4			5			6			7			8			9


class CBol
{
public:
	CBol(void);
	~CBol(void);
//------------------------------------------------------------------------------------------------------------------------------------
	CString m_BolName, m_BolName2;
	CString m_BolStatusName;
	UINT	m_BolId;
	UINT	m_BolId2;
	UINT	m_Status;
	UINT	m_Species;
	UINT	m_Row, m_Col;
	BOOL	m_Selected;
//------------------------------------------------------------------------------------------------------------------------------------

	BOOL SetBolType(UINT id, UINT col, UINT row);
	BOOL SetBolAdditional(UINT id, UINT col, UINT row);
	BOOL SetBolType(UINT id);
	BOOL SetBolStatus(UINT status);
	void Clone(CBol srcbol);

};
