// WaveBox.h: interface for the CWave class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WAVEBOX_H__DE24CFE1_7501_4DA3_AF18_667A845AAE49__INCLUDED_)
#define AFX_WAVEBOX_H__DE24CFE1_7501_4DA3_AF18_667A845AAE49__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
////                                                                        
////                                                                                  by Zen '05
////
////                                    WaveBox class v0.95
////                                  ~~~~~~~~~~~~~~~~~~~~~~
////                                  ( PCM multiwave player )
////                                        play & joy
////
////
////    
////

/// precompiler
#include <windows.h>
#include <mmsystem.h>

/// wave & PCM marks
#define WAVE_FILE_MARK          "RIFF"
#define WAVE_HEAD_MARK          "WAVEfmt "
#define WAVE_DATA_MARK          "data"
#define WAVE_PCM_16             16
#define WAVE_PCM_1               1

/// wfx header offsets
#define OFFSET_FILE_LEFT         4
#define OFFSET_HEAD_MARK         8
#define OFFSET_WAVE_PCM1        16
#define OFFSET_WAVE_PCM2        20
#define OFFSET_CHANNELS         22
#define OFFSET_SAMPLESPERSEC    24
#define OFFSET_AVGBYTESPERSEC   28
#define OFFSET_BLOCKALIGN       32
#define OFFSET_BITSPERSAMPLE    34
#define OFFSET_DATA_MARK        36
#define OFFSET_DATA_SIZE        40
#define OFFSET_WAVEDATA         44
#define HEADER_SIZE             OFFSET_WAVEDATA
#define EOF_EXTRA_INFO          60


/// messages
typedef unsigned int            WMsg;   // wave messages
typedef unsigned int            TMsg;   // thread messages

#define WMSG_WAIT               0       // wave wait
#define WMSG_START              1       // wave play
#define TMSG_ALIVE              1       // thread alive
#define TMSG_CLOSE              0       // thread close
#define INT_FREE                0       // interface free
#define INT_USED                1       // interface used
#define THREAD_EXIT     0xABECEDA       // thread exit code

/// performance predefines
#define SUPPORT_WAVES          130       // predefined wave count
#define SUPPORT_INTERFACES     130       // predefined interface count

/// buffering
#define READ_BLOCK           8192       // read wave ( file ) block size
#define BLOCK_SIZE           8192       // queue block size
#define BLOCK_COUNT            20       // queue block count
#define BP_TURN                 1       // blocks per turn


static  CRITICAL_SECTION        cs;     // critical section

static unsigned int __stdcall   PlayThread( LPVOID lp );                // main play thread
static void CALLBACK            waveOutProc(  HWAVEOUT  hWaveOut,       // waveOut prototype
                                              UINT      uMsg, 
                                              DWORD     dwInstance,  
                                              DWORD     dwParam1,    
                                              DWORD     dwParam2     );
class CWaveBox 
{
    struct WAVE
    {
        char            *data;      /// wave    
        unsigned long    size;      /// size
        WAVEFORMATEX     wfx;       /// wfx
        WMsg             WMSG;      /// { 0,1 } wait / play
    };
    
    struct INTERFACE
    {
        /// interface
        HWAVEOUT        dev;        /// device handle
        unsigned int    state;      /// { 0,1 } free / used

        /// wave
        WAVE           *wave;       /// current wave

        /// wave interface
        unsigned long   wpos;       /// current play position
        WAVEHDR*        wblock;     /// wave block
        volatile int    wfreeblock; /// free blocks left
        int             wcurrblock; /// current block
    };

public:
    
    /// members
    INTERFACE       I[SUPPORT_INTERFACES];                      // interface(s)
    WAVE            W[SUPPORT_WAVES];                           // wave(s)
    unsigned int    wload;                                      // current wave(s) loaded
    TMsg            TMSG;                                       // thread msg { 1,0 } alive / close

    /// prototypes 
    int Load( TCHAR       *file );                              // load wave into WaveBox
    int Play( unsigned int wave );                              // play n wave from WaveBox ( starts play thread )

             CWaveBox();                                        // play thread created suspended
    virtual ~CWaveBox();                                        // play thread terminated

    /// waveOut open & close
    int         AddInterface( HWAVEOUT      *dev,               /// push wave wfx to interface
                              WAVEFORMATEX  *wfx,       
                              volatile int  *wfreeblock );      
    int         RemoveInterface( HWAVEOUT dev );                /// pull wave wfx from interface

protected:
    
    /// thread
    HANDLE          thread;                                     /// play thread handle
    unsigned int    run;                                        /// suspended / resumed

    /// prototypes
    WAVEHDR*    allocateBlocks( int size, int count );          /// alloc heap for wave header blocks
    void        freeBlocks( WAVEHDR* blockArray );              /// free heap
};

#endif // !defined(AFX_WAVEBOX_H__DE24CFE1_7501_4DA3_AF18_667A845AAE49__INCLUDED_)