// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#pragma once

#if PLATFORM_MAC

#import <AppKit/AppKit.h>
#import "WintabContexts-CocoaMacInterface.h"


@interface WintabContextCocoa : NSView {
    int		mEventType;
    float	mMouseX;
    float	mMouseY;
    float	mSubX;
    float	mSubY;
    float	mPressure;

    BOOL		mCaptureMouseMoves;
    BOOL		mUpdateStatsDuringDrag;
    
    
    //Private
    NSPoint mLastLoc;
}

-(int) mEventType;
-(float) mMouseX;
-(float) mMouseY;
-(float) mSubX;
-(float) mSubY;
-(float) mPressure;


-(BOOL) mCaptureMouseMoves;
-(void) setCaptureMouseMoves:(BOOL)value;
-(BOOL) mUpdateStatsDuringDrag;
-(void) setUpdateStatsDuringDrag:(BOOL)value;

-(void) handleMouseEvent:(NSEvent *)theEvent;
@end

extern NSString *WTViewUpdatedNotification;

#endif // PLATFORM_MAC
