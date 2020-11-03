// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#if PLATFORM_WINDOWS

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "Windows/WINTAB.H"

class FWintabLibrary
{
public:
    typedef UINT( API *WTINFOW ) ( UINT, UINT, LPVOID );
    typedef HCTX( API *WTOPENW )( HWND, LPLOGCONTEXTW, BOOL );
    typedef BOOL( API *WTGETA ) ( HCTX, LPLOGCONTEXT );
    typedef BOOL( API *WTSETA ) ( HCTX, LPLOGCONTEXT );
    typedef BOOL( API *WTCLOSE ) ( HCTX );
    typedef BOOL( API *WTENABLE ) ( HCTX, BOOL );
    typedef BOOL( API *WTPACKET ) ( HCTX, UINT, LPVOID );
    typedef BOOL( API *WTOVERLAP ) ( HCTX, BOOL );

    typedef BOOL( API *WTSAVE ) ( HCTX, LPVOID );
    typedef BOOL( API *WTCONFIG ) ( HCTX, HWND );
    typedef HCTX( API *WTRESTORE ) ( HWND, LPVOID, BOOL );
    typedef BOOL( API *WTEXTSET ) ( HCTX, UINT, LPVOID );
    typedef BOOL( API *WTEXTGET ) ( HCTX, UINT, LPVOID );

    typedef int ( API *WTQUEUESIZEGET ) ( HCTX );
    typedef BOOL( API *WTQUEUESIZESET ) ( HCTX, int );
    typedef int ( API *WTDATAPEEK ) ( HCTX, UINT, UINT, int, LPVOID, LPINT );
    typedef int ( API *WTPACKETSGET ) ( HCTX, int, LPVOID );

    typedef HMGR( API *WTMGROPEN ) ( HWND, UINT );
    typedef BOOL( API *WTMGRCLOSE ) ( HMGR );
    typedef HCTX( API *WTMGRDEFCONTEXT ) ( HMGR, BOOL );
    typedef HCTX( API *WTMGRDEFCONTEXTEX ) ( HMGR, UINT, BOOL );
    typedef BOOL( API *WTMGRCSRPRESSUREBTNMARKSEX ) ( HMGR, UINT, UINT FAR *, UINT FAR * );

public:
    static bool Load();
    static void Unload();

public:
    static WTINFOW WTInfoW;
    static WTOPENW WTOpenW;
    static WTGETA WTGetA;
    static WTSETA WTSetA;
    static WTCLOSE WTClose;
    static WTPACKET WTPacket;
    static WTENABLE WTEnable;
    static WTOVERLAP WTOverlap;
    static WTSAVE WTSave;
    static WTCONFIG WTConfig;
    static WTRESTORE WTRestore;
    static WTEXTSET WTExtSet;
    static WTEXTGET WTExtGet;
    static WTQUEUESIZEGET WTQueueSizeGet;
    static WTQUEUESIZESET WTQueueSizeSet;
    static WTDATAPEEK WTDataPeek;
    static WTPACKETSGET WTPacketsGet;
    static WTMGROPEN WTMgrOpen;
    static WTMGRCLOSE WTMgrClose;
    static WTMGRDEFCONTEXT WTMgrDefContext;
    static WTMGRDEFCONTEXTEX WTMgrDefContextEx;
    static WTMGRCSRPRESSUREBTNMARKSEX WTMgrCsrPressureBtnMarksEx;

private:
    static void* DLLHandle;
    static int mRefCount;

private:
    FWintabLibrary(); // not implemented
    ~FWintabLibrary(); // not implemented
};

#endif // PLATFORM_WINDOWS
