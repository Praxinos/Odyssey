// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "WintabStylusInputInterface.h"
#include "Interfaces/IMainFrameModule.h"

#include "Framework/Application/SlateApplication.h"

#if PLATFORM_WINDOWS

#include "WintabContexts-Windows.h"

class FWintabStylusInputInterfaceImpl
{
public:
    ~FWintabStylusInputInterfaceImpl();

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
    for( const FWTTabletContextInfo& Context : Impl->mContexts->mTabletContexts )
    {
        // don't change focus if the stylus is down
        if( Context.GetCurrentState().ContainsByPredicate( []( const FStylusState& iStylusState ) { return iStylusState.IsStylusDown(); } ) )
        {
            return;
        }
    }

    FSlateApplication& Application = FSlateApplication::Get();

    FWidgetPath WidgetPath = Application.LocateWindowUnderMouse( Application.GetCursorPos(), Application.GetInteractiveTopLevelWindows() );
    if( WidgetPath.IsValid() )
    {
        TSharedPtr<SWindow> Window = WidgetPath.GetWindow();
        if( Window.IsValid() )
        {
            TSharedPtr<FGenericWindow> NativeWindow = Window->GetNativeWindow();
            HWND Hwnd = reinterpret_cast<HWND>( NativeWindow->GetOSWindowHandle() );

            if( Hwnd != Impl->mHwnd )
            {
                Impl->mContexts->CloseTabletContexts();
                Impl->mHwnd = Hwnd;
                Impl->mContexts->OpenTabletContexts( Impl->mHwnd );
            }

            Impl->Window = Window;
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

TSharedPtr<IStylusInputInterfaceInternal>
CreateStylusInputInterfaceWintab()
{
    TUniquePtr<FWintabStylusInputInterfaceImpl> WindowsImpl = MakeUnique<FWintabStylusInputInterfaceImpl>();

    if( !FWintabLibrary::Load() )
    {
        UE_LOG( LogStylusInput, Error, TEXT( "Could not load Wintab32.dll!" ) );
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

#endif // PLATFORM_WINDOWS
