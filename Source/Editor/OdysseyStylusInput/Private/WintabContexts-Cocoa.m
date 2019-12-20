// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#if PLATFORM_MAC

#import "WintabContexts-Cocoa.h"
 
NSString *WTViewUpdatedNotification = @"WTViewStatsUpdatedNotification";

@implementation WintabContextCocoa
///////////////////////////////////////////////////////////////////////////
- (id)initWithFrame:(NSRect)frame
{
    self = [super initWithFrame:frame];
    if (self)
    {
        // Initialization code here.
        mCaptureMouseMoves = YES;
        mUpdateStatsDuringDrag = YES;
    }
    return self;
}



///////////////////////////////////////////////////////////////////////////
- (void)awakeFromNib
{
   // Must inform the window that we want mouse moves after all object
   // are created and linked.
   // Let our internal routine make the API call so that everything
   // stays in sych. Change the calue in the init routine to change
   // the default behavior
   [self setCaptureMouseMoves:[self mCaptureMouseMoves]];
}



///////////////////////////////////////////////////////////////////////////
- (void)mouseDown:(NSEvent *)theEvent
{
   [self handleMouseEvent:theEvent];
   
   // Save the loc the mouse down occurred at. This will be used by the
   // Drawing code during a Drag event to follow.
   mLastLoc = [self convertPoint:[theEvent locationInWindow]
                  fromView:nil];
}



///////////////////////////////////////////////////////////////////////////
- (void)mouseDragged:(NSEvent *)theEvent
{
     switch ([theEvent type])
     {
        case NSEventTypeLeftMouseDragged:
             mCaptureMouseMoves = YES;
        break;
        
        case NSEventTypeLeftMouseUp:
             mCaptureMouseMoves = YES;
        break;
        
        default:
                 /* Ignore any other kind of event. */
        break;
     }
}



///////////////////////////////////////////////////////////////////////////
- (void)mouseMoved:(NSEvent *)theEvent
{
    [self handleMouseEvent:theEvent];
}



///////////////////////////////////////////////////////////////////////////
- (void)mouseUp:(NSEvent *)theEvent
{
    [self handleMouseEvent:theEvent];
}



///////////////////////////////////////////////////////////////////////////

// -(void)handleMouseEvent:(NSEvent *)theEvent

//

// All of the Mouse Events are funneled through this function so that we
// do not have to duplicate this code. If you do something like this,
// you must be careful because certain fields are only valid for particular
// events. For example, [NSEvent pressure] is not valid for Mouse Moves!

//

-(void)handleMouseEvent:(NSEvent *)theEvent
{
   NSPoint	loc;
   
   mEventType	= [theEvent type];
   
   loc = [theEvent locationInWindow];
   mMouseX	= loc.x;
   mMouseY	= loc.y;
   
   mSubX	= 0.0;//loc.x;
   mSubY	= 0.0;//loc.y;
   
   // pressure: is not valid for MouseMove events
   if(mEventType != NSEventTypeMouseMoved)
   {
      mPressure	= [theEvent pressure];
   }
   else
   {
      mPressure = 0.0;
   }
   
   [[NSNotificationCenter defaultCenter]
         postNotificationName:WTViewUpdatedNotification
         object: self];
}


- (void) handleProximity:(NSNotification *)proxNotice
{
    mPressure = 0.0;
}


///////////////////////////////////////////////////////////////////////////
- (BOOL)acceptsFirstResponder
{
    // The view only gets MouseMoved events when the view is the First
    // Responder in the Responder event chain
    return YES;
}



///////////////////////////////////////////////////////////////////////////
-(int) mEventType
{
    return mEventType;
}



///////////////////////////////////////////////////////////////////////////
-(float) mMouseX
{
    return mMouseX;
}



///////////////////////////////////////////////////////////////////////////
-(float) mMouseY
{
    return mMouseY;
}



///////////////////////////////////////////////////////////////////////////
-(float) mSubX
{
    return mSubX;
}



///////////////////////////////////////////////////////////////////////////
-(float) mSubY
{
    return mSubY;
}



///////////////////////////////////////////////////////////////////////////
-(float) mPressure
{
    return mPressure;
}

float GetPressureFromInstance (void *self)
{
    return [(id) self mPressure];
}


///////////////////////////////////////////////////////////////////////////
-(BOOL) mCaptureMouseMoves
{
   return mCaptureMouseMoves;
}



///////////////////////////////////////////////////////////////////////////
-(void) setCaptureMouseMoves:(BOOL)value
{
   mCaptureMouseMoves = value;
   [[self window] setAcceptsMouseMovedEvents:mCaptureMouseMoves];
}



///////////////////////////////////////////////////////////////////////////
-(BOOL) mUpdateStatsDuringDrag
{
   return mUpdateStatsDuringDrag;
}



///////////////////////////////////////////////////////////////////////////
-(void) setUpdateStatsDuringDrag:(BOOL)value
{
   mUpdateStatsDuringDrag = value;
}
@end

#endif // PLATFORM_MAC
