// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "WintabStylusInputInterface.h"

#include "Interfaces/IMainFrameModule.h"
#include "Framework/Application/SlateApplication.h"

#include "WintabContexts.h"

//---

// An implementation which represents the Wintab driver
class FWintabStylusInputInterfaceImpl
{
public:
    ~FWintabStylusInputInterfaceImpl();

    /** All the contexts (tablets) detected */
    TSharedPtr<FWintabContexts> mContexts;

    HWND mHwnd{ 0 };
    TWeakPtr<SWindow> Window;
    TWeakPtr<SWidget> Widget;
};

FWintabStylusInputInterfaceImpl::~FWintabStylusInputInterfaceImpl()
{
    mContexts.Reset();

    FWintabLibrary::Unload();
}

//---
//---
//---

FWintabStylusInputInterface::FWintabStylusInputInterface( TUniquePtr<FWintabStylusInputInterfaceImpl> InImpl )
{
    check( InImpl.IsValid() );

    Impl = MoveTemp( InImpl );
}

FWintabStylusInputInterface::~FWintabStylusInputInterface() = default;

//---

void
FWintabStylusInputInterface::Tick()
{
    // If the stylus is down (= drawing), don't change the focused window (and current widget) of the plugin
    // When we draw on a zoomed viewport and the mouse go over the limits of the viewport, 
    // we want to continue drawing on the right window and widget and not start "drawing" on the new hovered window and widget
    for( const FWintabTabletContextInfo& Context : Impl->mContexts->mTabletContexts )
    {
        if( Context.GetCurrentState().ContainsByPredicate( []( const FStylusState& iStylusState ) { return iStylusState.IsStylusDown(); } ) )
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
            HWND Hwnd = reinterpret_cast<HWND>( NativeWindow->GetOSWindowHandle() );

            // If the current hovered window is different than the referenced window in the plugin, change it
            if( Hwnd != Impl->mHwnd )
            {
                // Remove all contexts (tablets) detected
                Impl->mContexts->CloseTabletContexts();
                // Set the new referenced window in the plugin
                Impl->mHwnd = Hwnd;
                // Re-detect all tablets
                Impl->mContexts->OpenTabletContexts( Impl->mHwnd );
            }

            // Store the new referenced plugin window
            Impl->Window = Window;
            // Also store the widget
            Impl->Widget = WidgetPath.GetLastWidget();
        }
    }
}

int32
FWintabStylusInputInterface::NumInputDevices() const
{
    return Impl->mContexts->mTabletContexts.Num();
}

IStylusInputDevice*
FWintabStylusInputInterface::GetInputDevice( int32 Index ) const
{
    if( Index < 0 || Index >= Impl->mContexts->mTabletContexts.Num() )
    {
        return nullptr;
    }

    return &Impl->mContexts->mTabletContexts[Index];
}

TWeakPtr<SWindow>
FWintabStylusInputInterface::Window() const
{
    return Impl->Window;
}

TWeakPtr<SWidget>
FWintabStylusInputInterface::Widget() const
{
    return Impl->Widget;
}

//---
//---
//---

// Create the StylusInputInterface corresponding to the Wintab driver
TSharedPtr<IStylusInputInterfaceInternal> CreateStylusInputInterfaceWintab()
{
    TUniquePtr<FWintabStylusInputInterfaceImpl> WindowsImpl = MakeUnique<FWintabStylusInputInterfaceImpl>();

    // Load the wintab dll
    if( !FWintabLibrary::Load() )
    {
        UE_LOG( LogStylusInput, Warning, TEXT( "Could not load Wintab32.dll!" ) );
        return nullptr;
    }

    if( !FWintabLibrary::WTInfoW( 0, 0, NULL ) )
    {
        UE_LOG( LogStylusInput, Warning, TEXT( "WinTab Services are unavailable" ) );
        return nullptr;
    }

    WindowsImpl->mContexts = MakeShareable( new FWintabContexts() );

    return MakeShared<FWintabStylusInputInterface>( MoveTemp( WindowsImpl ) );
}
