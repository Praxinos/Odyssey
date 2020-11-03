// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#if PLATFORM_WINDOWS

#include "WintabLibrary-Windows.h"

#include "Framework/Application/SlateApplication.h"

void* FWintabLibrary::DLLHandle = nullptr;
int FWintabLibrary::mRefCount = 0;

FWintabLibrary::WTINFOW FWintabLibrary::WTInfoW = nullptr;
FWintabLibrary::WTOPENW FWintabLibrary::WTOpenW = nullptr;
FWintabLibrary::WTGETA FWintabLibrary::WTGetA = nullptr;
FWintabLibrary::WTSETA FWintabLibrary::WTSetA = nullptr;
FWintabLibrary::WTCLOSE FWintabLibrary::WTClose = nullptr;
FWintabLibrary::WTPACKET FWintabLibrary::WTPacket = nullptr;
FWintabLibrary::WTENABLE FWintabLibrary::WTEnable = nullptr;
FWintabLibrary::WTOVERLAP FWintabLibrary::WTOverlap = nullptr;
FWintabLibrary::WTSAVE FWintabLibrary::WTSave = nullptr;
FWintabLibrary::WTCONFIG FWintabLibrary::WTConfig = nullptr;
FWintabLibrary::WTRESTORE FWintabLibrary::WTRestore = nullptr;
FWintabLibrary::WTEXTSET FWintabLibrary::WTExtSet = nullptr;
FWintabLibrary::WTEXTGET FWintabLibrary::WTExtGet = nullptr;
FWintabLibrary::WTQUEUESIZEGET FWintabLibrary::WTQueueSizeGet = nullptr;
FWintabLibrary::WTQUEUESIZESET FWintabLibrary::WTQueueSizeSet = nullptr;
FWintabLibrary::WTDATAPEEK FWintabLibrary::WTDataPeek = nullptr;
FWintabLibrary::WTPACKETSGET FWintabLibrary::WTPacketsGet = nullptr;
FWintabLibrary::WTMGROPEN FWintabLibrary::WTMgrOpen = nullptr;
FWintabLibrary::WTMGRCLOSE FWintabLibrary::WTMgrClose = nullptr;
FWintabLibrary::WTMGRDEFCONTEXT FWintabLibrary::WTMgrDefContext = nullptr;
FWintabLibrary::WTMGRDEFCONTEXTEX FWintabLibrary::WTMgrDefContextEx = nullptr;
FWintabLibrary::WTMGRCSRPRESSUREBTNMARKSEX FWintabLibrary::WTMgrCsrPressureBtnMarksEx = nullptr;

#pragma warning(suppress: 4191)

/*static*/
bool
FWintabLibrary::Load()
{
    // should never be above 2, it happens when:
    // InitSubsystem()
    //      current = CreateStylusInputInterfaceWintab() [ref=1]
    // ...
    // new = CreateStylusInputInterfaceWintab() [ref=2]
    // SetStylusInputInterface( new )
    //      current.Reset() [ref=1]
    //      current = new [ref=1]
    check( mRefCount <= 1 );

    if( DLLHandle )
    {
        mRefCount++;
        return true;
    }

    const FString Wintab32DLL = TEXT( "Wintab32.dll" );

    DLLHandle = FPlatformProcess::GetDllHandle( *Wintab32DLL );
    if( !DLLHandle )
        return false;

    mRefCount++;

#define GETPROCADDRESS(type, func) \
		func = reinterpret_cast<type>( reinterpret_cast<void*>( GetProcAddress(HMODULE(DLLHandle), #func) ) ); \
		if( !func ) { Unload(); return false; }

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
    mRefCount--;
    if( mRefCount > 0 )
        return;

    if( DLLHandle ) // It's first without return, to always reset following WT*
    {
        FPlatformProcess::FreeDllHandle( DLLHandle );
        DLLHandle = nullptr;
    }

    WTInfoW = nullptr;
    WTOpenW = nullptr;
    WTGetA = nullptr;
    WTSetA = nullptr;
    WTClose = nullptr;
    WTPacket = nullptr;
    WTEnable = nullptr;
    WTOverlap = nullptr;
    WTSave = nullptr;
    WTConfig = nullptr;
    WTRestore = nullptr;
    WTExtSet = nullptr;
    WTExtGet = nullptr;
    WTQueueSizeGet = nullptr;
    WTQueueSizeSet = nullptr;
    WTDataPeek = nullptr;
    WTPacketsGet = nullptr;
    WTMgrOpen = nullptr;
    WTMgrClose = nullptr;
    WTMgrDefContext = nullptr;
    WTMgrDefContextEx = nullptr;
    WTMgrCsrPressureBtnMarksEx = nullptr;
}

#endif // PLATFORM_WINDOWS
