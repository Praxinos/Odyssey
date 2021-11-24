// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "NSEventStylusInputInterface.h"
#include "Interfaces/IMainFrameModule.h"

#include "Framework/Application/SlateApplication.h"

#if PLATFORM_MAC

#include "NSEventContexts-Mac.h"

// An implementation which represents the NSEvent 'driver'
class FNSEventStylusInputInterfaceImpl
{
public:
    ~FNSEventStylusInputInterfaceImpl();

    /** All the contexts (tablets) detected (harcoded to 1) */
    TSharedPtr<FNSEventContexts> mContexts;

    FCocoaWindow* mHwnd{ 0 };
    TWeakPtr<SWindow> Window;
    TWeakPtr<SWidget> Widget;
};

FNSEventStylusInputInterfaceImpl::~FNSEventStylusInputInterfaceImpl()
{
    mContexts.Reset();
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
    if( Impl->mContexts->mTabletContext.IsDirty() )
    {
        if( Impl->mContexts->mTabletContext.GetCurrentState().ContainsByPredicate( []( const FStylusState& iStylusState ) { return iStylusState.IsStylusDown(); } ) )
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
                // Remove all contexts (tablets) detected
                Impl->mContexts->CloseContext();
                // Set the new referenced window in the plugin
                Impl->mHwnd = Hwnd;
                // Re-detect all tablets
                Impl->mContexts->OpenContext( Impl->mHwnd );
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
    return 1;
}

IStylusInputDevice*
FNSEventStylusInputInterface::GetInputDevice( int32 Index ) const
{
    return &Impl->mContexts->mTabletContext;
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

    impl->mContexts = MakeShareable( new FNSEventContexts() );

    return MakeShared<FNSEventStylusInputInterface>( MoveTemp( impl ) );
}

#endif // PLATFORM_MAC
