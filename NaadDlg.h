// NaadhDlg.h : header file
//

#pragma once
#include "UpdateCheck.h"
#include "multilinelist.h"
#include "afxcmn.h"
#include "afxwin.h"
#include "wavebox.h"
#include "composition.h"
#include "Sequence.h"
#include "WaveRender.h"

#define NAAD_FOLDER		 1
#define NAAD_FOLDER_COMP 2
#define SLASH			"//"

// CNaadDlg dialog
class CNaadDlg : public CDialog
{
// Construction
public:
	CNaadDlg(CWnd* pParent = NULL);	// standard constructor
	~CNaadDlg(void);

// Dialog Data
	enum { IDD = IDD_NAAD_DIALOG };

//------------------------------------------------------------------------------------------------------------
	CMenu			m_Menu, m_BolMenu, m_LoopMenu;
	BOOL			m_Quiet, m_Saved, m_SamBeat, m_IsInstalled, m_IsShellOpen, m_IsCompView, m_IsPlaying, m_IsBolLoaded;
	BOOL			m_VarietySameSpecies, m_VarietyAnySpecies, m_VarietyComp, m_VarietyLoop, m_PlayLoop, m_EditMode, m_AddBol;
	BOOL			m_LoopDone;
	CString			m_Mesg, m_AppPath, m_NaadFileName, m_BolFileSeq[MC*MR], m_BolFolder;
	UINT			m_Version, /*m_Cols, m_Rows,*/ m_Tempo, m_NextBol, m_NextLoop, m_MaatraBol, m_BolVar;
	//UINT			m_MaxBol, m_Echo;
	UINT			m_ActiveLoop, m_Bpm, *m_LoopSequence, m_TotalLoops, m_SeqNum;
	int				m_BpmDeviation, m_Gun, m_Rand, m_BpmRand;
	UINT			m_Ghungru[MAXGHUNGRU], m_Tanpura[MAXTANPURA], m_GhungruSel, m_TanpuraSel;
	UINT_PTR		m_Timer, m_TimerAcc;

	CMultilineList	m_List;
	CSliderCtrl		m_TempoSliderCtrl, m_RandSliderCtrl, m_BolVarSliderCtrl;
    CWaveBox		m_WaveBox;
	CComposition	m_Composition, m_CompositionCopy;
	CSequence*		m_Sequence;

	CWaveRender		m_Render;

//------------------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------------------
	void DisplayComposition(void);
	void DisplayLoop(UINT nloop);
	void DisplayBol(UINT nloop, UINT nbol);
	void ClearDisplay(void);
	void ClearSelection(void);
	void LoadBolWaves(void);
	void FormatCells(void);
	void SaveSettings(void);
	void LoadSettings(void);
	void AssociateFileType(CString ext, CString app, CString id, CString desc, CString icon);
	//void DoLoopPropDlg(UINT nloop);
	bool CreateLoopSequence(void);
	//bool GetNextLoop(void);
	void DeleteLoopSequence(void);
	//void PlaySequence(void);
	void PlayLoopSequence(void);
	void PlayBolSequence(void);
	UINT GetBolVarient(UINT nbol);
	void CreateVarient(void);
	void ResetVarient(void);
	void UpdateTempo(void);
	void InitPlay(void);
	void TempoVariation(void);
	void SetLoopBpm(void);
	void Reset(void);
	void OpenOldVer(void);
	void OpenLoopOldVer(CString loopfilename);
	void Paste(UINT col, UINT row);
	void NormalizeLocations();
	CString GetUserDocumentPath(UINT type);
	void ReportUsage();

//------------------------------------------------------------------------------------------------------------

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	afx_msg void OnFileExit();
	afx_msg void OnHelpAboutnaadh();
	afx_msg void OnHelpCheckforupdates();
	afx_msg void OnFileOpen32773();
	afx_msg void OnFileSave32774();
	afx_msg void OnFileSaveas();
	afx_msg void OnBolsClear32792();
	afx_msg void OnFileNewtaal();
	afx_msg void OnBnClickedButtonPlay();
	afx_msg void OnBnClickedButtonStop();
	afx_msg void OnBolsSilence();
	afx_msg void OnBnClickedCheckAcc();
	afx_msg void OnBnClickedCheckRndbol();
	afx_msg void OnMenuSelect(UINT nItemID, UINT nFlags, HMENU hSysMenu);
	afx_msg void OnOptionsPlayallsamples();
	afx_msg void OnSelectBol(UINT nID);
	afx_msg void OnBolsSam();
	afx_msg void OnBolsTali();
	afx_msg void OnBolsKhali();
	afx_msg void OnBnClickedCheckSam();
	afx_msg void OnOptionsBolfolder();
	afx_msg void OnHelpContact();
	afx_msg void OnHelpNaadhhelp();
	afx_msg void OnFileNewcomposition();
	afx_msg void OnLoopOpen();
	afx_msg void OnViewComposition();
	afx_msg void OnLoopClear();
	afx_msg void OnBolsViewcomp();
	afx_msg void OnLoopProperties();
	afx_msg void OnViewLoop();
	afx_msg void OnBnClickedCheckEcho();
	afx_msg void OnBnClickedOk();
	afx_msg void OnGhungruGhungru01();
	afx_msg void OnGhungruGhungru02();
	afx_msg void OnGhungruGhungru03();
	afx_msg void OnGhungruGhungru04();
	afx_msg void OnGhungruGhungru05();
	afx_msg void OnGhungruGhungru06();
	afx_msg void OnAccompanimentTanpura();
	afx_msg void OnAccompaniment2();
	afx_msg void OnAccompaniment3();
	afx_msg void OnAccompaniment4();
	afx_msg void OnBnClickedCheckVarsame();
	afx_msg void OnBnClickedCheckVarany();
	afx_msg void OnBnClickedButtonSavevar();
	afx_msg void OnBnClickedCheckVarcomp();
	afx_msg void OnBnClickedCheckVarloop();
	afx_msg void OnBnClickedButtonAutocompose();
	afx_msg void OnBnClickedButtonPlayloop();
	afx_msg void OnBnClickedButtonGun1();
	afx_msg void OnLoopLoadloop();
	afx_msg void OnLoopSaveloop();
	afx_msg void OnEditSelect();
	afx_msg void OnEditClear32974();
	afx_msg void OnEditCopy32956();
	afx_msg void OnEditCut32957();
	afx_msg void OnEditPaste32958();
	afx_msg void OnEditAdd();
	afx_msg void OnBnClickedButtonRender();
};

