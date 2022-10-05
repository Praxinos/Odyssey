// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "NSEventContexts.h"

//For coordinates conversion purposes
#include "Mac/MacApplication.h"
#include "Mac/MacPlatformApplicationMisc.h"

//---

FCriticalSection sgMutex;

//---

void
FNSEventTabletContextInfo::Tick()
{
    PreviousState = CurrentState;
    CurrentState.Empty();
    
    sgMutex.Lock();
    TArray<FNSEventStylusState> tmp( mPacketsBuffer );
    mPacketsBuffer.Empty();
    sgMutex.Unlock();
    
    for( int i = 0; i < tmp.Num(); i++ )
    {
        CurrentState.Push( tmp[i].ToPublicState() );
    }

    Dirty = false;
}


FNSEventContext::FNSEventContext()
{
    mEventMonitor = 0;
    mTabletContext = FNSEventTabletContextInfo();
}

FNSEventContext::~FNSEventContext()
{
    CloseContext();
}

bool
FNSEventContext::OpenContext( FCocoaWindow* iHwnd )
{
    mTabletContext.mIsInverted = false;
        
    mTabletContext.SetDirty(); // Mandatory! Sometimes may be 0 -_- ?!
    
    //We listen to NSEvents
    if( !mEventMonitor )
    {
        mEventMonitor = [NSEvent addLocalMonitorForEventsMatchingMask:NSEventMaskAny handler:^(NSEvent* Event) {
            return HandleNSEvent(Event); }];
    }

    return true;
}

void
FNSEventContext::CloseContext()
{
    if( mEventMonitor )
    {
        [NSEvent removeMonitor:mEventMonitor];
        mEventMonitor = 0;
        mTabletContext.Clear();
    }
}

//Workaround method from https://udn.unrealengine.com/s/question/0D54z00006tN3iECAS/crash-on-macslatewacomissues--
//Fixed here: https://github.com/EpicGames/UnrealEngine/commit/f5d99e2c68673d9b675d212cf2816840e0c99cf5
//TODO: check if the slate crash is fixed by removing this workaround and draw during long sessions on mac
FVector2D Internal_ConvertCocoaPositionToSlate(NSPoint const& CursorPosition)
{
#if UE_BUILD_DEBUG
    // This function has to be on the main thread
    check([NSThread isMainThread]);
#endif

    NSScreen* TargetScreen = [NSScreen mainScreen];
    {
        NSArray<NSScreen*>* Screens = [NSScreen screens];
        for (NSScreen* Screen in Screens)
        {
            if (NSPointInRect(CursorPosition, Screen.frame))
            {
                TargetScreen = Screen;
                break;
            }
        }
    }

    const bool bUseHighDPIMode = FPlatformApplicationMisc::IsHighDPIModeEnabled();
    const float DPIScaleFactor = bUseHighDPIMode ? TargetScreen.backingScaleFactor : 1.f;
    const FVector2D OffsetOnScreen = FVector2D(CursorPosition.x - TargetScreen.frame.origin.x, TargetScreen.frame.origin.y + TargetScreen.frame.size.height - CursorPosition.y) * DPIScaleFactor;
    return FVector2D(TargetScreen.frame.origin.x * DPIScaleFactor + OffsetOnScreen.X, -TargetScreen.frame.origin.y * DPIScaleFactor + OffsetOnScreen.Y);
}
//Workaround end--

NSEvent* FNSEventContext::HandleNSEvent(NSEvent* Event)
{
    if( !Event )
        return NULL;
    
    FNSEventStylusState state;
    
    NSPoint cursorPosition = NSEvent.mouseLocation;
    
    //Sometimes, NSEvent.mouseLocation isn't initialized, so we have to make sure it exists before we convert it to cocoaPosition
    if( &cursorPosition != 0 )
    {
        state.Position = Internal_ConvertCocoaPositionToSlate(cursorPosition);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Warning: NSEvent cursor position not initialized"));
        return Event;
    }
    
    //Todo: Check if useful
    /*if (state.Position == FVector2D(0, 0))
        UE_LOG(LogTemp, Display, TEXT("Warning: NSEvent cursor state not initialized"));*/

    state.Timer = Event.timestamp * 1000;
        
    //Tablet and mouse events are under the same main type of event. To distinguish between them, we can check the subtype of the event received
    if( [Event type] == NSEventTypeLeftMouseDown || [Event type] == NSEventTypeLeftMouseDragged )
    {
        state.NormalPressure = Event.pressure;
        state.IsTouching = true;
        
        if( [Event subtype] == NSEventSubtype::NSEventSubtypeTabletPoint )
        {
            state.TangentPressure = Event.tangentialPressure;

            NSPoint tilt = Event.tilt;
            state.Tilt = FVector2D( tilt.x, tilt.y );
            //Set Azimuth and Altitude with the content of Tilt
            state.TiltToOrientation();
            
            state.Z = Event.absoluteZ;
            state.Twist = Event.rotation;
        }
    }
    else if( [Event type ] == NSEventTypeTabletProximity )
    {
        if( Event.pointingDeviceType == NSPointingDeviceType::NSPointingDeviceTypeEraser && Event.isEnteringProximity )
            mTabletContext.mIsInverted = true;
        else
            mTabletContext.mIsInverted = false;
    }
            
    state.IsInverted = mTabletContext.mIsInverted;
    
    sgMutex.Lock();
    mTabletContext.SetDirty();
    mTabletContext.mPacketsBuffer.Push( state );
    sgMutex.Unlock();

    return Event;
}
