// WaveBox.cpp: implementation of the CWave class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "WaveBox.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWaveBox::CWaveBox()
{ 
    // init wave(s) counter
    wload   = 0;

    // thread suspended < used for resuming thread at first play >
    run     = 0;

    // create suspended player thread
    thread  = CreateThread(  NULL,
                             0,
                             (LPTHREAD_START_ROUTINE)PlayThread,
                             (LPVOID)this,
                             CREATE_SUSPENDED,
                             NULL                                   );
    

    // alloc mem for interface(s)
    for( unsigned int i = 0; i < SUPPORT_INTERFACES; i++ )
    {
        I[i].wblock     = allocateBlocks( BLOCK_SIZE, BLOCK_COUNT );
        I[i].wfreeblock = BLOCK_COUNT;
        I[i].wcurrblock = 0;
        I[i].state      = INT_FREE;
        I[i].wpos       = 0;
    }
    
    // init msg
    for(int i = 0; i < SUPPORT_WAVES; i++ )    W[i].WMSG = WMSG_WAIT;

    // init cs
    InitializeCriticalSection( &cs );
}


CWaveBox::~CWaveBox()
{
	unsigned long exit = 0;

	if( run ) // thread resumed
	{
		// set thread close message
		EnterCriticalSection( &cs );
		TMSG = TMSG_CLOSE;
		LeaveCriticalSection( &cs );
		
		do // wait for soft close
		{ 
			GetExitCodeThread( thread, &exit ); 
			Sleep( 10 );

		}while( exit != THREAD_EXIT );
	
	}else // thread suspended	
	{
		// hard close 
		GetExitCodeThread( thread, &exit ); 
		TerminateThread( thread, exit );
	}
	
	
	// release wave(s)
	for( unsigned int i = 0; i < wload; i++ )
		free( W[i].data );

	// release interface(s)
	for(int i = 0; i < SUPPORT_INTERFACES; i++ )
		freeBlocks( I[i].wblock ); 

	// del cs
	DeleteCriticalSection( &cs );
}



WAVEHDR* CWaveBox::allocateBlocks(int size, int count)
{
    unsigned char* buffer;
    int i;
    WAVEHDR* blocks;
    DWORD totalBufferSize = (size + sizeof(WAVEHDR)) * count;
    
    //  allocate memory for the entire set in one go
    if((buffer = ( UCHAR*) HeapAlloc( GetProcessHeap(), 
									  HEAP_ZERO_MEMORY, 
									  totalBufferSize )) == NULL)	return NULL;
    
    // and set up the pointers to each bit
    blocks = (WAVEHDR*)buffer;
    buffer += sizeof(WAVEHDR) * count;
    for(i = 0; i < count; i++) 
	{
        blocks[i].dwBufferLength = size;
        blocks[i].lpData = (CHAR *)buffer;
        buffer += size;
    }
    
    return blocks;
}


void CWaveBox::freeBlocks(WAVEHDR* blockArray)
{
    // and this is why allocateBlocks works the way it does
    HeapFree(GetProcessHeap(), 0, blockArray);
}


int CWaveBox::Load( TCHAR *file )
{
    if( wload == SUPPORT_WAVES )
        return -1;

    HANDLE hFile;

    // open file
    if((hFile = CreateFile( file,
                            GENERIC_READ,
                            FILE_SHARE_READ,
                            NULL,
                            OPEN_EXISTING,
                            0,
                            NULL )) == INVALID_HANDLE_VALUE)    return -1;

    
    // read wave header
    char            header[HEADER_SIZE];
    unsigned long   rbytes  = 0;
    
    if( !ReadFile(hFile, header, sizeof(header), &rbytes, NULL) )
    { CloseHandle(hFile);   return -1; }

    if( !rbytes || rbytes < sizeof(header) )
    { CloseHandle(hFile);   return -1; }

    /// check if this is a wave file
    if( strncmp( header, WAVE_FILE_MARK, strlen( WAVE_FILE_MARK )) )
    { CloseHandle(hFile);   return -1; }

    if( strncmp( header + OFFSET_HEAD_MARK, WAVE_HEAD_MARK, strlen( WAVE_HEAD_MARK )) )
    { CloseHandle(hFile);   return -1; }

    /// check if wave is uncompressed PCM format
    if (    ((*(DWORD*)(header + OFFSET_WAVE_PCM1)) != WAVE_PCM_16 )
         || ((*(WORD *)(header + OFFSET_WAVE_PCM2)) != WAVE_PCM_1  ))
    {CloseHandle(hFile);     return -1; }

    /// check for 'data' mark
    if( !strncmp( header + OFFSET_DATA_MARK, WAVE_DATA_MARK, strlen( WAVE_DATA_MARK )) )
        W[wload].size = *((DWORD*)(header + OFFSET_DATA_SIZE ));                /* size of data */  
    else
    {   /// if data block size cant be read
        /// try to predict data block without extra info
        /// this is unusualy case
        W[wload].size  = *((DWORD*)(header + OFFSET_FILE_LEFT ));  
        W[wload].size -=  ( HEADER_SIZE - EOF_EXTRA_INFO );             /* size of data */ 
    }

    // fill WAVEFORMATEX from wave header
    W[wload].wfx.nSamplesPerSec  = *((DWORD*)(header + OFFSET_SAMPLESPERSEC )); /* sample rate */
    W[wload].wfx.wBitsPerSample  = *((WORD *)(header + OFFSET_BITSPERSAMPLE )); /* sample size */
    W[wload].wfx.nChannels       = *((WORD *)(header + OFFSET_CHANNELS      )); /* channels    */
    W[wload].wfx.cbSize          = 0;                                           /* size of _extra_ info */
    W[wload].wfx.wFormatTag      = WAVE_FORMAT_PCM;
    W[wload].wfx.nBlockAlign     = *((WORD *)(header + OFFSET_BLOCKALIGN    ));
    W[wload].wfx.nAvgBytesPerSec = *((DWORD*)(header + OFFSET_AVGBYTESPERSEC));

    // get mem for wave data block
    if((W[wload].data = ( char *) calloc( W[wload].size, sizeof( char ))) == NULL)
    { CloseHandle(hFile); return -1; }
 
    char            buffer[READ_BLOCK];
    
    unsigned long   size         = W[wload].size; 
    unsigned long   read_block   = 0;
                    rbytes       = 0;
                      
    do  /// copy uncompressed PCM wave data block
    {
        if( ( size -= rbytes ) >= READ_BLOCK )  read_block = READ_BLOCK;
        else
        if( size && size < READ_BLOCK )         read_block = size;
        else                                    break;

        if( !ReadFile(hFile, buffer, read_block, &rbytes, NULL) )
            break;
        if( rbytes == 0 )
            break;
        if( rbytes < sizeof(buffer) ) 
            memset(buffer + rbytes, 0, sizeof(buffer) - rbytes);
        
        memcpy( &W[wload].data[W[wload].size - size], buffer, rbytes );         

    }while( 1 );    

    // close file handle
    CloseHandle(hFile);
    
    // return current wave count
    return ++wload;
}


int CWaveBox::Play( unsigned int wave )
{
    // check wave id
    if( wave < 0 || wave >= wload )
        return -1;
    
    // set play message
    EnterCriticalSection(&cs);
    W[wave].WMSG = WMSG_START;
    LeaveCriticalSection(&cs);

    // resume thread < at first play >
    if( !run ){ run = 1; TMSG = TMSG_ALIVE; ResumeThread( thread ); }
    
    return 1;
}


int CWaveBox::AddInterface( HWAVEOUT	*dev, 
					     WAVEFORMATEX	*wfx, 
						 volatile int	*wfreeblock )
{
	// check for free device
	if( !waveOutGetNumDevs() )
		return -1;
    
    // try to open the default wave device. WAVE_MAPPER is
    // a constant defined in mmsystem.h, it always points to the
    // default wave device on the system (some people have 2 or
    // more sound cards).
    
	if(waveOutOpen(	dev,
					WAVE_MAPPER, 
					wfx, 
					(DWORD)waveOutProc, 
					(DWORD)wfreeblock, 
					CALLBACK_FUNCTION	) != MMSYSERR_NOERROR )	return -1;
    
	return 1;
}


int	CWaveBox::RemoveInterface( HWAVEOUT dev )
{
	// try to close given wave device / interface.
	
	if( waveOutClose( dev ) != MMSYSERR_NOERROR ) return -1;
	
	return 1;
}


static void CALLBACK waveOutProc( HWAVEOUT	hWaveOut, 
								  UINT		uMsg, 
								  DWORD		dwInstance,  
								  DWORD		dwParam1,    
								  DWORD		dwParam2     )
{
    // pointer to free block counter
    int* freeBlockCounter = (int*)dwInstance;

	// ignore calls that occur due to openining and closing the device.
    if(uMsg != WOM_DONE)
        return;

    // increase free block counter
	EnterCriticalSection(&cs);
    (*freeBlockCounter)++;
    LeaveCriticalSection(&cs);
}


static unsigned int __stdcall PlayThread( LPVOID lp )
{
    /// get the class instance
    CWaveBox *wb = ( CWaveBox *)lp;
    
    /// pooling variables < most frequently used / checked >
    register    WMsg            wmsg  = WMSG_WAIT;
    register    TMsg            tmsg  = TMSG_ALIVE;         
    register    unsigned int    i     = 0;
    

    /// thread life cycle
    while( tmsg )
    {

        /// check for 'play' msg
        for( i = 0; i < wb->wload; i++ )
        {
            /// read msg
            EnterCriticalSection( &cs );
            wmsg = wb->W[i].WMSG;
            LeaveCriticalSection( &cs );
            
            /// wave to play?
            if( wmsg == WMSG_START )    break;
        }
        
        
        /// playable wave
        if( wmsg == WMSG_START )
        
            /// link with first free interface
            for( unsigned int j = 0; j < SUPPORT_INTERFACES; j++ )
            
                /// check for free interface
                if( wb->I[j].state == INT_FREE )
    
                    /// attach wave to interface
                    if( wb->AddInterface( &wb->I[j].dev, 
                                          &wb->W[i].wfx, 
                                          &wb->I[j].wfreeblock ) )
                    {
                        /// get wave pointer
                        wb->I[j].wave = &wb->W[i];
                
                        /// mark interface as used
                        wb->I[j].state = INT_USED;

                        /// free wave 
                        EnterCriticalSection( &cs );
                        wb->W[i].WMSG = WMSG_WAIT;  
                        LeaveCriticalSection( &cs );                        

                        /// leave loop
                        break;
                    }
    


            
        /////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////
        ///
        ///                       < main playing loop >
        ///
        ///    search for the first marked interface and play attached wave
        ///

        for( unsigned int k = 0; k < SUPPORT_INTERFACES; k++ )
        {
            /// nothing to do with free interface
            if( wb->I[k].state == INT_FREE ) continue;

            EnterCriticalSection( &cs );
            int free = wb->I[k].wfreeblock;
            LeaveCriticalSection( &cs );
            
            /// nothing to do with full queued interface
            if( free < BP_TURN )    continue; 

            WAVEHDR     *current = NULL;
            
            /// how much blocks per turn will be queued
            for( unsigned int m = 0; m < BP_TURN; m++ )
            {   
                /// set current block pointer
                current = &wb->I[k].wblock[wb->I[k].wcurrblock]; 
                
                // first make sure the header we're going to use is unprepared
                if( current->dwFlags & WHDR_PREPARED ) 

                    waveOutUnprepareHeader( wb->I[k].dev, 
                                            current, 
                                            sizeof(WAVEHDR)  );     

                /// how much data is left at this interface to play
                unsigned long left  = wb->I[k].wave->size - wb->I[k].wpos;
                unsigned long chunk = 0;

                if( left  >= BLOCK_SIZE )
                    chunk  = BLOCK_SIZE;
                else 
                if( left && left < BLOCK_SIZE )
                    chunk  = left;
                else
                {   
                    ////////////////////
                    /// nothing left ///
                    ////////////////////////////////////////////////////////////////////////
                    ///
                    ///                   < clean job, close waveOutProc threads >
                    /// 
                    ///                    all buffers are queued to the interface
                    ///
                    
                    /// get free block count
                    EnterCriticalSection( &cs );
                    int free = wb->I[k].wfreeblock;
                    LeaveCriticalSection( &cs );


                    if( free == BLOCK_COUNT )   /// are all blocks played!?
                    {
                
                        /// unprepare any blocks that are still prepared
                        for( int i = 0; i < wb->I[k].wfreeblock; i++) 
                            
                            if( wb->I[k].wblock[i].dwFlags & WHDR_PREPARED )
                
                                waveOutUnprepareHeader(  wb->I[k].dev, 
                                                        &wb->I[k].wblock[i], 
                                                         sizeof(WAVEHDR)      );
                    
                        /// close interface
                        if( wb->RemoveInterface( wb->I[k].dev ) )
                        {
                            /// free interface
                            wb->I[k].wcurrblock = 0;
                            wb->I[k].state      = INT_FREE;
                            wb->I[k].wpos       = 0;
                            wb->I[k].wave       = NULL;
                        }
                    }

                    /// step out
                    break;
                }
    
                /// prepare current wave data block header
                memcpy( current->lpData, &wb->I[k].wave->data[wb->I[k].wpos], chunk );

                current->dwBufferLength  = chunk;   // sizeof block
                wb->I[k].wpos           += chunk;   // update position
                
                /// prepare for playback
                waveOutPrepareHeader( wb->I[k].dev, current, sizeof(WAVEHDR) );
                
                /// push to the queue
                waveOutWrite(wb->I[k].dev, current, sizeof(WAVEHDR));

                /// decrease free block counter
                EnterCriticalSection( &cs );
                wb->I[k].wfreeblock--;
                LeaveCriticalSection( &cs );
                
                /// point to the next block
                wb->I[k].wcurrblock++;
                wb->I[k].wcurrblock %= BLOCK_COUNT;
            
            }/// block(s)

        }/// interface(s)
    
        /// wait 10 ms < save CPU time >
        Sleep( 10 );
        
        /// check for thread message
        EnterCriticalSection( &cs );
        tmsg = wb->TMSG;
        LeaveCriticalSection( &cs );

    }/// thread



    ///////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////
    ///
    ///            force to close interfaces which are still playing 
    ///

    for( i = 0; i < SUPPORT_INTERFACES; i++ )
            if( wb->I[i].state == INT_USED )    
                if( waveOutReset( wb->I[i].dev ) == MMSYSERR_NOERROR )
                    wb->RemoveInterface( wb->I[i].dev );
    
                
    return  THREAD_EXIT; /// return exit code < destructor >
}

