// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#if PLATFORM_MAC

#import "WintabContexts-Mac.h"
#include "WintabContexts-CocoaMacInterface.h"


NSString *kProximityNotification = @"kProximityNotification";
NSString *kProximityEventKey = @"kProximityEventKey";

void
FWTTabletContextInfo::Tick()
{
    //if( mContext )
      //  UE_LOG(LogTemp, Display, TEXT("%lf"), mContext.mMouseX );
    //float NormalPressure = mContext->Pressure();
    //float NormalPressure = GetPressure( mContext );
}






FWintabContexts::FWintabContexts()
{
}

FWintabContexts::~FWintabContexts()
{
    CloseTabletContexts();
}

bool
FWintabContexts::OpenTabletContexts( FCocoaWindow* iHwnd )
{
    check( !mTabletContexts.Num() );
    
    [iHwnd setAcceptsInput:YES];
    
    mEventMonitor = [NSEvent addLocalMonitorForEventsMatchingMask:NSEventMaskAny handler:^(NSEvent* Event) { return HandleNSEvent(Event); }];

    FWTTabletContextInfo tablet_context_info;
    tablet_context_info.SetDirty(); // Mandatory! Sometimes may be 0 -_- ?!
    
    mTabletContexts.Add( tablet_context_info );
    
    mTabletContexts[0].mContext = iHwnd.contentView;

    return true;
}


void
FWintabContexts::CloseTabletContexts()
{
    mTabletContexts.Empty();

    if ( mEventMonitor ) {

        [NSEvent removeMonitor:mEventMonitor];

        mEventMonitor = nil;

    }
}


NSEvent* FWintabContexts::HandleNSEvent(NSEvent* Event)
{
   if([Event type] == NSEventTypeTabletProximity)
       UE_LOG(LogTemp, Display, TEXT("Proximity tablet"));

    return Event;
}


#endif // PLATFORM_MAC
