// Copyright 2018-2019 Praxinos, Inc. All Rights Reserved.

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
    static WTINFOW gpWTInfoW;
    static WTOPENW gpWTOpenW;
    static WTGETA gpWTGetA;
    static WTSETA gpWTSetA;
    static WTCLOSE gpWTClose;
    static WTPACKET gpWTPacket;
    static WTENABLE gpWTEnable;
    static WTOVERLAP gpWTOverlap;
    static WTSAVE gpWTSave;
    static WTCONFIG gpWTConfig;
    static WTRESTORE gpWTRestore;
    static WTEXTSET gpWTExtSet;
    static WTEXTGET gpWTExtGet;
    static WTQUEUESIZEGET gpWTQueueSizeGet;
    static WTQUEUESIZESET gpWTQueueSizeSet;
    static WTDATAPEEK gpWTDataPeek;
    static WTPACKETSGET gpWTPacketsGet;
    static WTMGROPEN gpWTMgrOpen;
    static WTMGRCLOSE gpWTMgrClose;
    static WTMGRDEFCONTEXT gpWTMgrDefContext;
    static WTMGRDEFCONTEXTEX gpWTMgrDefContextEx;
    static WTMGRCSRPRESSUREBTNMARKSEX gpWTMgrCsrPressureBtnMarksEx;

private:
    static void* DLLHandle;

private:
    FWintabLibrary(); // not implemented
    ~FWintabLibrary(); // not implemented
};

#endif // PLATFORM_WINDOWS
