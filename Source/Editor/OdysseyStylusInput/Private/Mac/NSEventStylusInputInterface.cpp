// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "NSEventStylusInputInterface.h"

#include "Interfaces/IMainFrameModule.h"
#include "Framework/Application/SlateApplication.h"

#include "NSEventContexts.h"

//---

// An implementation which represents the NSEvent 'driver'
class FNSEventStylusInputInterfaceImpl
{
public:
    ~FNSEventStylusInputInterfaceImpl();

    /** All the contexts (tablets) detected (harcoded to 1) */
    TSharedPtr<FNSEventContext> mContext;

    FCocoaWindow* mHwnd{ 0 };
    TWeakPtr<SWindow> Window;
    TWeakPtr<SWidget> Widget;
};

FNSEventStylusInputInterfaceImpl::~FNSEventStylusInputInterfaceImpl()
{
    mContext.Reset();
}

//---
//---
//---

FNSEventStylusInputInterface::FNSEventStylusInputInterface( TUniquePtr<FNSEventStylusInputInterfaceImpl> InImpl )
{
    check( InImpl.IsValid() );

    Impl = MoveTemp( InImpl );
}

FNSEventStylusInputInterface::~FNSEventStylusInputInterface() = default;

//---

void
FNSEventStylusInputInterface::Tick()
{
    // If the stylus is down (= drawing), don't change the focused window (and current widget) of the plugin
    // When we draw on a zoomed viewport and the mouse go over the limits of the viewport, 
    // we want to continue drawing on the right window and widget and not start "drawing" on the new hovered window and widget
    if( Impl->mContext->mTabletContext.IsDirty() )
    {
        if( Impl->mContext->mTabletContext.GetCurrentState().ContainsByPredicate( []( const FStylusState& iStylusState ) { return iStylusState.IsStylusDown(); } ) )
        {
            return;
        }
    }
    
    FSlateApplication& Application = FSlateApplication::Get();

    // Get the widget hovered by the stylus/mouse
    FWidgetPath WidgetPath = Application.LocateWindowUnderMouse( Application.GetCursorPos(), Application.GetInteractiveTopLevelWindows() );
    if( WidgetPath.IsValid() )
    {
        // Get its corresponding window
        TSharedPtr<SWindow> Window = WidgetPath.GetWindow();
        if( Window.IsValid() )
        {
            TSharedPtr<FGenericWindow> NativeWindow = Window->GetNativeWindow();
            FCocoaWindow* Hwnd = reinterpret_cast<FCocoaWindow*>( NativeWindow->GetOSWindowHandle() );

            // If the current hovered window is different than the referenced window in the plugin, change it
            if( Hwnd != Impl->mHwnd )
            {
                // Remove the context (tablet) detected
                Impl->mContext->CloseContext();
                // Set the new referenced window in the plugin
                Impl->mHwnd = Hwnd;
                // Re-detect the context
                Impl->mContext->OpenContext( Impl->mHwnd );
            }

            // Store the new referenced plugin window
            Impl->Window = Window;
            // Also store the widget
            Impl->Widget = WidgetPath.GetLastWidget();
        }
    }
}

int32
FNSEventStylusInputInterface::NumInputDevices() const
{
    //No simple way to check the number of different input devices on Mac. Instead, we retain one we can change: mTabletContext
    return 1;
}

IStylusInputDevice*
FNSEventStylusInputInterface::GetInputDevice( int32 Index ) const
{
    return &Impl->mContext->mTabletContext;
}

TWeakPtr<SWindow>
FNSEventStylusInputInterface::Window() const
{
    return Impl->Window;
}

TWeakPtr<SWidget>
FNSEventStylusInputInterface::Widget() const
{
    return Impl->Widget;
}

//---
//---
//---

// Create the StylusInputInterface corresponding to the NSEvent 'driver'
TSharedPtr<IStylusInputInterfaceInternal> CreateStylusInputInterfaceNSEvent()
{
    TUniquePtr<FNSEventStylusInputInterfaceImpl> impl = MakeUnique<FNSEventStylusInputInterfaceImpl>();

    impl->mContext = MakeShareable( new FNSEventContext() );

    return MakeShared<FNSEventStylusInputInterface>( MoveTemp( impl ) );
}
