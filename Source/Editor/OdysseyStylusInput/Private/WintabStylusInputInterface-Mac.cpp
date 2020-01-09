// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#include "WintabStylusInputInterface.h"
#include "Interfaces/IMainFrameModule.h"

#include "Framework/Application/SlateApplication.h"

#if PLATFORM_MAC

#include "WintabContexts-Mac.h"


class FWintabStylusInputInterfaceImpl
{
public:
    ~FWintabStylusInputInterfaceImpl();

    TSharedPtr<FWintabContexts> mContext;

    FCocoaWindow* mHwnd{ 0 };
    TWeakPtr<SWindow> Window;
    TWeakPtr<SWidget> Widget;
};

FWintabStylusInputInterfaceImpl::~FWintabStylusInputInterfaceImpl()
{
    mContext.Reset();
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
    if( Impl->mContext->mTabletContext.IsDirty() )
    {
        // don't change focus if the stylus is down
        if( Impl->mContext->mTabletContext.GetCurrentState().ContainsByPredicate( []( const FStylusState& iStylusState ) { return iStylusState.IsStylusDown(); } ) )
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
                Impl->mContext->CloseTabletContexts();
                Impl->mHwnd = Hwnd;
                Impl->mContext->OpenTabletContexts( Impl->mHwnd );
            }

            Impl->Window = Window;
            Impl->Widget = WidgetPath.GetLastWidget();
        }
    }
}

int32
FWintabStylusInputInterface::NumInputDevices() const
{
    return 1;
}

IStylusInputDevice*
FWintabStylusInputInterface::GetInputDevice( int32 Index ) const
{
    return &Impl->mContext->mTabletContext;
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

    WindowsImpl->mContext = MakeShareable( new FWintabContexts() );

    return MakeShared<FWintabStylusInputInterface>( MoveTemp( WindowsImpl ) );
}

#endif // PLATFORM_MAC
