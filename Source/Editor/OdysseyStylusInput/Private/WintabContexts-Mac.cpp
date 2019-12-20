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
    //float NormalPressure = mContext->Pressure();
    //float NormalPressure = GetPressure( mContext );
}






FWintabContexts::FWintabContexts()
{
    //NSWindow* Window = [NSApp windowWithWindowNumber:Info.WindowNumber];
    mWindow = [[NSApp keyWindow] contentView];
    
    
    UE_LOG(LogTemp, Display, TEXT("%s"), mWindow.window.title)
    
   [[NSNotificationCenter defaultCenter] addObserver:mWindow
               selector:@selector(handleProximity:)
               name:kProximityNotification
               object:nil];
    
}

FWintabContexts::~FWintabContexts()
{
    //CloseTabletContexts();
}

bool
FWintabContexts::OpenTabletContexts()
{
    UE_LOG(LogTemp, Display, TEXT("Opened"));
    return true;
}

void
FWintabContexts::CloseTabletContexts()
{
}

#endif // PLATFORM_MAC
