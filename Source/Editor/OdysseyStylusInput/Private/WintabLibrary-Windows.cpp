// Copyright 2018-2019 Praxinos, Inc. All Rights Reserved.

#if PLATFORM_WINDOWS

#include "WintabLibrary-Windows.h"

#include "Framework/Application/SlateApplication.h"

void* FWintabLibrary::DLLHandle = nullptr;

FWintabLibrary::WTINFOW FWintabLibrary::gpWTInfoW = nullptr;
FWintabLibrary::WTOPENW FWintabLibrary::gpWTOpenW = nullptr;
FWintabLibrary::WTGETA FWintabLibrary::gpWTGetA = nullptr;
FWintabLibrary::WTSETA FWintabLibrary::gpWTSetA = nullptr;
FWintabLibrary::WTCLOSE FWintabLibrary::gpWTClose = nullptr;
FWintabLibrary::WTPACKET FWintabLibrary::gpWTPacket = nullptr;
FWintabLibrary::WTENABLE FWintabLibrary::gpWTEnable = nullptr;
FWintabLibrary::WTOVERLAP FWintabLibrary::gpWTOverlap = nullptr;
FWintabLibrary::WTSAVE FWintabLibrary::gpWTSave = nullptr;
FWintabLibrary::WTCONFIG FWintabLibrary::gpWTConfig = nullptr;
FWintabLibrary::WTRESTORE FWintabLibrary::gpWTRestore = nullptr;
FWintabLibrary::WTEXTSET FWintabLibrary::gpWTExtSet = nullptr;
FWintabLibrary::WTEXTGET FWintabLibrary::gpWTExtGet = nullptr;
FWintabLibrary::WTQUEUESIZEGET FWintabLibrary::gpWTQueueSizeGet = nullptr;
FWintabLibrary::WTQUEUESIZESET FWintabLibrary::gpWTQueueSizeSet = nullptr;
FWintabLibrary::WTDATAPEEK FWintabLibrary::gpWTDataPeek = nullptr;
FWintabLibrary::WTPACKETSGET FWintabLibrary::gpWTPacketsGet = nullptr;
FWintabLibrary::WTMGROPEN FWintabLibrary::gpWTMgrOpen = nullptr;
FWintabLibrary::WTMGRCLOSE FWintabLibrary::gpWTMgrClose = nullptr;
FWintabLibrary::WTMGRDEFCONTEXT FWintabLibrary::gpWTMgrDefContext = nullptr;
FWintabLibrary::WTMGRDEFCONTEXTEX FWintabLibrary::gpWTMgrDefContextEx = nullptr;
FWintabLibrary::WTMGRCSRPRESSUREBTNMARKSEX FWintabLibrary::gpWTMgrCsrPressureBtnMarksEx = nullptr;

#pragma warning(suppress: 4191)

/*static*/
bool
FWintabLibrary::Load()
{
    if( DLLHandle )
        Unload();

    const FString Wintab32DLL = TEXT( "Wintab32.dll" );

    DLLHandle = FPlatformProcess::GetDllHandle( *Wintab32DLL );
    if( !DLLHandle )
        return false;

#define GETPROCADDRESS(type, func) \
		gp##func = reinterpret_cast<type>( reinterpret_cast<void*>( GetProcAddress(HMODULE(DLLHandle), #func) ) ); \
		if(!gp##func) { Unload(); return false; }

    GETPROCADDRESS( WTOPENW, WTOpenW );
    GETPROCADDRESS( WTINFOW, WTInfoW );
    GETPROCADDRESS( WTGETA, WTGetA );
    GETPROCADDRESS( WTSETA, WTSetA );
    GETPROCADDRESS( WTPACKET, WTPacket );
    GETPROCADDRESS( WTCLOSE, WTClose );
    GETPROCADDRESS( WTENABLE, WTEnable );
    GETPROCADDRESS( WTOVERLAP, WTOverlap );
    GETPROCADDRESS( WTSAVE, WTSave );
    GETPROCADDRESS( WTCONFIG, WTConfig );
    GETPROCADDRESS( WTRESTORE, WTRestore );
    GETPROCADDRESS( WTEXTSET, WTExtSet );
    GETPROCADDRESS( WTEXTGET, WTExtGet );
    GETPROCADDRESS( WTQUEUESIZEGET, WTQueueSizeGet );
    GETPROCADDRESS( WTQUEUESIZESET, WTQueueSizeSet );
    GETPROCADDRESS( WTDATAPEEK, WTDataPeek );
    GETPROCADDRESS( WTPACKETSGET, WTPacketsGet );
    GETPROCADDRESS( WTMGROPEN, WTMgrOpen );
    GETPROCADDRESS( WTMGRCLOSE, WTMgrClose );
    GETPROCADDRESS( WTMGRDEFCONTEXT, WTMgrDefContext );
    GETPROCADDRESS( WTMGRDEFCONTEXTEX, WTMgrDefContextEx );
    GETPROCADDRESS( WTMGRCSRPRESSUREBTNMARKSEX, WTMgrCsrPressureBtnMarksEx );

    return true;
}

/*static*/
void
FWintabLibrary::Unload()
{
    if( DLLHandle ) // It's first without return, to always reset following gp*
    {
        FPlatformProcess::FreeDllHandle( DLLHandle );
        DLLHandle = nullptr;
    }

    gpWTInfoW = nullptr;
    gpWTOpenW = nullptr;
    gpWTGetA = nullptr;
    gpWTSetA = nullptr;
    gpWTClose = nullptr;
    gpWTPacket = nullptr;
    gpWTEnable = nullptr;
    gpWTOverlap = nullptr;
    gpWTSave = nullptr;
    gpWTConfig = nullptr;
    gpWTRestore = nullptr;
    gpWTExtSet = nullptr;
    gpWTExtGet = nullptr;
    gpWTQueueSizeGet = nullptr;
    gpWTQueueSizeSet = nullptr;
    gpWTDataPeek = nullptr;
    gpWTPacketsGet = nullptr;
    gpWTMgrOpen = nullptr;
    gpWTMgrClose = nullptr;
    gpWTMgrDefContext = nullptr;
    gpWTMgrDefContextEx = nullptr;
    gpWTMgrCsrPressureBtnMarksEx = nullptr;
}

#endif // PLATFORM_WINDOWS
