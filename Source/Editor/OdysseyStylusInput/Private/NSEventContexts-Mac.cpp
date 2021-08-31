// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "NSEventContexts-Mac.h"

#if PLATFORM_MAC

//For coordinates conversion purposes
#include "Mac/MacApplication.h"
#include "Mac/MacPlatformApplicationMisc.h"

FCriticalSection sgMutex;

void
FWTTabletContextInfo::Tick()
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
}


FNSEventContexts::FNSEventContexts()
{
    mEventMonitor = 0;
    mTabletContext = FWTTabletContextInfo();
}

FNSEventContexts::~FNSEventContexts()
{
    CloseContext();
}


//Register the window to get NSEvents, enabling the window to get tablet events
bool
FNSEventContexts::OpenContext( FCocoaWindow* iHwnd )
{
    mTabletContext.mIsInverted = false;
        
    mTabletContext.SetDirty(); // Mandatory! Sometimes may be 0 -_- ?!
    
    //We listen to the NSEvents
    if( !mEventMonitor )
    {
        mEventMonitor = [NSEvent addLocalMonitorForEventsMatchingMask:NSEventMaskAny handler:^(NSEvent* Event) {
            return HandleNSEvent(Event); }];
    }

    return true;
}


void
FNSEventContexts::CloseContext()
{
    if( mEventMonitor )
    {
        [NSEvent removeMonitor:mEventMonitor];
        mEventMonitor = 0;
        mTabletContext.Clear();
    }
}

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


NSEvent* FNSEventContexts::HandleNSEvent(NSEvent* Event)
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
        UE_LOG(LogTemp, Display, TEXT("WARNING, ERROR POSITION"));
        return Event;
    }
    
    if( state.Position == FVector2D( 0, 0 ))
        UE_LOG(LogTemp, Display, TEXT("WARNING, ERROR IN STATE"));

    state.Timer = Event.timestamp * 1000;
        
    //Tablet and mouse events are under the same main type of event. To distinguish them, we can check the subtype of the event received
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
    mTabletContext.mPacketsBuffer.Push( state );
    sgMutex.Unlock();

    return Event;
}


#endif // PLATFORM_MAC
