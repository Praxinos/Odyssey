// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "NSEventStylusInputInterface.h"
#include "Interfaces/IMainFrameModule.h"

#include "Framework/Application/SlateApplication.h"

#if PLATFORM_MAC

#include "NSEventContexts-Mac.h"


class FNSEventStylusInputInterfaceImpl
{
public:
    ~FNSEventStylusInputInterfaceImpl();

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
    if( Impl->mContexts->mTabletContext.IsDirty() )
    {
        // don't change focus if the stylus is down
        if( Impl->mContexts->mTabletContext.GetCurrentState().ContainsByPredicate( []( const FStylusState& iStylusState ) { return iStylusState.IsStylusDown(); } ) )
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
            FCocoaWindow* Hwnd = reinterpret_cast<FCocoaWindow*>( NativeWindow->GetOSWindowHandle() );

            if( Hwnd != Impl->mHwnd )
            {
                Impl->mContexts->CloseContext();
                Impl->mHwnd = Hwnd;
                Impl->mContexts->OpenContext( Impl->mHwnd );
            }

            Impl->Window = Window;
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

TSharedPtr<IStylusInputInterfaceInternal>
CreateStylusInputInterfaceNSEvent()
{
    TUniquePtr<FNSEventStylusInputInterfaceImpl> impl = MakeUnique<FNSEventStylusInputInterfaceImpl>();

    impl->mContexts = MakeShareable( new FNSEventContexts() );

    return MakeShared<FNSEventStylusInputInterface>( MoveTemp( impl ) );
}

#endif // PLATFORM_MAC
