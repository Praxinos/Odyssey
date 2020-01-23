// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#include "NSEventContexts-Mac.h"

#if PLATFORM_MAC

//For coordinates conversion purposes
#include "Mac/MacApplication.h"

void
FWTTabletContextInfo::Tick()
{    
    PreviousState = CurrentState;
    CurrentState.Empty();
        
    for( int i = 0; i < mPacketsBuffer.Num(); i++ )
    {
        CurrentState.Push( mPacketsBuffer[i].ToPublicState() );
    }
    
    mPacketsBuffer.Empty();
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


NSEvent* FNSEventContexts::HandleNSEvent(NSEvent* Event)
{
    if( !Event )
        return NULL;
    
    FNSEventStylusState state;
    
    NSPoint cursorPosition = NSEvent.mouseLocation;
    state.Position = FMacApplication::ConvertCocoaPositionToSlate( cursorPosition.x, cursorPosition.y );
        
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
    mTabletContext.mPacketsBuffer.Push( state );
    
    return Event;
}


#endif // PLATFORM_MAC
