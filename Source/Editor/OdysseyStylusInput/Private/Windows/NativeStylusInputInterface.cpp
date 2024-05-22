// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "NativeStylusInputInterface.h"

#include "Interfaces/IMainFrameModule.h"
#include "Framework/Application/SlateApplication.h"
#include "Windows/WindowsApplication.h"
#include "Windows/NativeStylusInputDevice.h"

#include <chrono>
#include <tpcshrd.h>

//---

// An implementation which represents the Native driver
class FNativeStylusInputInterfaceImpl
    : public IWindowsMessageHandler
{
public:
    virtual ~FNativeStylusInputInterfaceImpl();
    FNativeStylusInputInterfaceImpl();

public:
    // IWindowsMessageHandler interface

    /**
	 * Processes a Windows message.
	 *
	 * @param hwnd Handle to the window that received the message.
	 * @param msg The message.
	 * @param wParam Additional message information.
	 * @param lParam Additional message information.
	 * @param OutResult Will contain the result if the message was handled.
	 * @return true if the message was handled, false otherwise.
	 */
	virtual bool ProcessMessage(HWND hwnd, uint32 msg, WPARAM wParam, LPARAM lParam, int32& OutResult) override;

public:
    HWND mHwnd{ 0 };
    TWeakPtr<SWindow> Window;
    TWeakPtr<SWidget> Widget;

	FNativeStylusInputDevice mDevice;
};

FNativeStylusInputInterfaceImpl::~FNativeStylusInputInterfaceImpl()
{
	//needed because slate is not present in some phases of Odyssey DDC building process
	//while we are building a new release version through Odyssey's scripts
	if ( FSlateApplication::IsInitialized() )
	{
		FWindowsApplication* WindowsApplication = (FWindowsApplication*)FSlateApplication::Get().GetPlatformApplication().Get();
		WindowsApplication->RemoveMessageHandler(*this);
	}
}

FNativeStylusInputInterfaceImpl::FNativeStylusInputInterfaceImpl()
{
	//needed because slate is not present in some phases of Odyssey DDC building process
	//while we are building a new release version through Odyssey's scripts
	if ( FSlateApplication::IsInitialized() )
	{
		FWindowsApplication* WindowsApplication = (FWindowsApplication*)FSlateApplication::Get().GetPlatformApplication().Get();
		WindowsApplication->AddMessageHandler(*this);
	}
}

bool
FNativeStylusInputInterfaceImpl::ProcessMessage(HWND hwnd, uint32 msg, WPARAM wParam, LPARAM lParam, int32& OutResult)
{
    switch(msg)
    {
        case WM_TABLET_QUERYSYSTEMGESTURESTATUS:
        {   
            OutResult = TABLET_DISABLE_FLICKS //remove lag between stylus down and WM_LMOUSEBUTTONDOWN (was a 500ms lag)
                | TABLET_DISABLE_PENTAPFEEDBACK //remove Windows circle around the pen when right click button is down
                | TABLET_DISABLE_PENBARRELFEEDBACK;  //remove Windows waves when clicking with the stylus

            return true;
        }

        case WM_POINTERENTER:
        case WM_POINTERLEAVE:
        case WM_POINTERDOWN:
        case WM_POINTERUP:
        case WM_POINTERUPDATE:
        {
            //OutResult = 0;

            UINT32 pointerId = GET_POINTERID_WPARAM(wParam);

            POINTER_INPUT_TYPE pointerType = PT_POINTER;
            if (!GetPointerType(pointerId, &pointerType))
                return false;
            
            if (pointerType != PT_PEN)
                return false;
    
            //GetPointerInfoHistory allows us to get all subpointer messages (coalesced messages)
            uint32 entries_count = 0;
            if (!GetPointerInfoHistory(pointerId, &entries_count, nullptr))
                return false;

            if (entries_count == 0)
                return true;

            TArray<POINTER_PEN_INFO> pen_infos;
            pen_infos.AddUninitialized(entries_count);
            if (!GetPointerPenInfoHistory(pointerId, &entries_count, pen_infos.GetData()))
                return false;

            for (int i = pen_infos.Num() - 1; i >= 0; i--)
            {
                POINTER_PEN_INFO& penInfo = pen_infos[i];
                mDevice.SetPenMask(penInfo.penMask);
                mDevice.OnPointerUpdate(penInfo);
            }

            //return true;
        }
        break;
        
        default:
            return false;
    }

    return false;
}

//---
//---
//---

FNativeStylusInputInterface::FNativeStylusInputInterface( TUniquePtr<FNativeStylusInputInterfaceImpl> InImpl )
{
    check( InImpl.IsValid() );
    Impl = MoveTemp( InImpl );
}

FNativeStylusInputInterface::~FNativeStylusInputInterface() = default;

//---

void
FNativeStylusInputInterface::Tick()
{
    /*
    // If the stylus is down (= drawing), don't change the focused window (and current widget) of the plugin
    // When we draw on a zoomed viewport and the mouse go over the limits of the viewport, 
    // we want to continue drawing on the right window and widget and not start "drawing" on the new hovered window and widget
    for( const FNativeTabletContextInfo& Context : Impl->mContexts->mTabletContexts )
    {
        if( Context.GetCurrentState().ContainsByPredicate( []( const FStylusState& iStylusState ) { return iStylusState.IsStylusDown(); } ) )
        {
            return;
        }
    }
    */

    FSlateApplication& Application = FSlateApplication::Get();

	if (Application.GetMouseCaptureWindow() != nullptr)
		return;

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
                // Set the new referenced window in the plugin
                Impl->mHwnd = Hwnd;

                // register the window for touch instead of gestures
                //RegisterTouchWindow(Hwnd, 0); //TODO : See if we need TWF_PALM and TWF_FINETOUCH
            }

            // Store the new referenced plugin window
            Impl->Window = Window;
            // Also store the widget
            Impl->Widget = WidgetPath.GetLastWidget();
        }
    }
}

int32
FNativeStylusInputInterface::NumInputDevices() const
{
    return 1;
    //return Impl->mContexts->mTabletContexts.Num();
}

IStylusInputDevice*
FNativeStylusInputInterface::GetInputDevice( int32 Index ) const
{
    /*if( Index < 0 || Index >= Impl->mContexts->mTabletContexts.Num() )
    {
        return nullptr;
    }

    return &Impl->mContexts->mTabletContexts[Index]; */

    return &Impl->mDevice;
}

TWeakPtr<SWindow>
FNativeStylusInputInterface::Window() const
{
    return Impl->Window;
}

TWeakPtr<SWidget>
FNativeStylusInputInterface::Widget() const
{
    return Impl->Widget;
}

//---
//---
//---

// Create the StylusInputInterface corresponding to the Native driver
TSharedPtr<IStylusInputInterfaceInternal> CreateStylusInputInterfaceNative()
{
    TUniquePtr<FNativeStylusInputInterfaceImpl> WindowsImpl = MakeUnique<FNativeStylusInputInterfaceImpl>();

    //WindowsImpl->mContexts = MakeShareable( new FNativeContexts() );

    return MakeShared<FNativeStylusInputInterface>( MoveTemp( WindowsImpl ) );
}
