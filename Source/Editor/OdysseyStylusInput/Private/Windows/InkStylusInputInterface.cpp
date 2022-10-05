// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "InkStylusInputInterface.h"

#include "Framework/Application/SlateApplication.h"
#include "GenericPlatform/GenericPlatformProcess.h"
#include "Interfaces/IMainFrameModule.h"

#include "InkRealTimeStylusPlugin.h"

//---

// An implementation which represents the Windows Ink driver
class FInkStylusInputInterfaceImpl
{
public:
	~FInkStylusInputInterfaceImpl();

	// The Windows Ink driver itself
	TComPtr<IRealTimeStylus> RealTimeStylus;
	// The object to connect to the driver to be able to override/manage all stylus messages (down/up/packets/...)
	TSharedPtr<FInkRealTimeStylusPlugin> StylusPlugin;
	void* DLLHandle { nullptr };

	TWeakPtr<SWindow> Window;
	TWeakPtr<SWidget> Widget;
};

FInkStylusInputInterfaceImpl::~FInkStylusInputInterfaceImpl()
{
	RealTimeStylus->RemoveAllStylusSyncPlugins();
	RealTimeStylus.Reset();

	StylusPlugin.Reset();

	if( DLLHandle != nullptr )
	{
		FPlatformProcess::FreeDllHandle( DLLHandle );
		DLLHandle = nullptr;
	}
}

//---
//---
//---

FInkStylusInputInterface::FInkStylusInputInterface(TUniquePtr<FInkStylusInputInterfaceImpl> InImpl)
{
	check(InImpl.IsValid());

	Impl = MoveTemp(InImpl);

	// We desire to receive everything, but what we actually will receive is determined in AddTabletContext
	TArray<GUID> DesiredPackets = {
		GUID_PACKETPROPERTY_GUID_X,
		GUID_PACKETPROPERTY_GUID_Y,
		GUID_PACKETPROPERTY_GUID_Z,
		GUID_PACKETPROPERTY_GUID_PACKET_STATUS,
		GUID_PACKETPROPERTY_GUID_TIMER_TICK,
		GUID_PACKETPROPERTY_GUID_NORMAL_PRESSURE,
		GUID_PACKETPROPERTY_GUID_TANGENT_PRESSURE,
		GUID_PACKETPROPERTY_GUID_AZIMUTH_ORIENTATION,
		GUID_PACKETPROPERTY_GUID_ALTITUDE_ORIENTATION,
		GUID_PACKETPROPERTY_GUID_X_TILT_ORIENTATION,
		GUID_PACKETPROPERTY_GUID_Y_TILT_ORIENTATION,
		GUID_PACKETPROPERTY_GUID_TWIST_ORIENTATION,
		GUID_PACKETPROPERTY_GUID_WIDTH,
		GUID_PACKETPROPERTY_GUID_HEIGHT,
		// Currently not needed.
		//GUID_PACKETPROPERTY_GUID_BUTTON_PRESSURE,
	};

	Impl->RealTimeStylus->SetDesiredPacketDescription(DesiredPackets.Num(), DesiredPackets.GetData());
}

FInkStylusInputInterface::~FInkStylusInputInterface() = default;

//---

void FInkStylusInputInterface::Tick()
{
	// If the stylus is down (= drawing), don't change the focused window (and current widget) of the plugin
	// When we draw on a zoomed viewport and the mouse go over the limits of the viewport, 
	// we want to continue drawing on the right window and widget and not start "drawing" on the new hovered window and widget
	for (const FInkTabletContextInfo& Context : Impl->StylusPlugin->TabletContexts)
	{
		if ( Context.GetCurrentState().ContainsByPredicate( []( const FStylusState& iStylusState ) { return iStylusState.IsStylusDown(); }) )
		{
			return;
		}
	}

	// Get the current window referenced by the plugin
	HANDLE_PTR HCurrentWnd;
	Impl->RealTimeStylus->get_HWND(&HCurrentWnd);

	FSlateApplication& Application = FSlateApplication::Get();

	// Get the widget hovered by the stylus/mouse
	FWidgetPath WidgetPath = Application.LocateWindowUnderMouse(Application.GetCursorPos(), Application.GetInteractiveTopLevelWindows());
	if (WidgetPath.IsValid())
	{
		// Get its corresponding window
		TSharedPtr<SWindow> Window = WidgetPath.GetWindow();
		if (Window.IsValid())
		{
			TSharedPtr<FGenericWindow> NativeWindow = Window->GetNativeWindow();
			HWND Hwnd = reinterpret_cast<HWND>(NativeWindow->GetOSWindowHandle());

			// If the current hovered window is different than the referenced window in the plugin, change it
			if (reinterpret_cast<HWND>(HCurrentWnd) != Hwnd)
			{
				// Changing the HWND in the plugin isn't supported when the plugin is enabled
				Impl->RealTimeStylus->put_Enabled(Windows::FALSE);
				//PATCH: add some delay to be sure that the plugin is disabled
				FPlatformProcess::Sleep( 0.03 );
				// Change the referenced window with the new one (hovered by the stylus/mouse)
				Impl->RealTimeStylus->put_HWND(reinterpret_cast<uint64>(Hwnd));
				//PATCH: to be sure...
				FPlatformProcess::Sleep( 0.03 );
				// Reactivate the plugin with the new referenced window
				Impl->RealTimeStylus->put_Enabled(Windows::TRUE);
				//PATCH: to be sure...
				FPlatformProcess::Sleep( 0.03 );
			}

			// Store the new referenced plugin window
			Impl->Window = Window;
			// Also store the widget
			Impl->Widget = WidgetPath.GetLastWidget();
		}
	}
}

int32 FInkStylusInputInterface::NumInputDevices() const
{
	return Impl->StylusPlugin->TabletContexts.Num();
}

IStylusInputDevice* FInkStylusInputInterface::GetInputDevice(int32 Index) const
{
	if (Index < 0 || Index >= Impl->StylusPlugin->TabletContexts.Num())
	{
		return nullptr;
	}

	return &Impl->StylusPlugin->TabletContexts[Index];
}

TWeakPtr<SWindow> FInkStylusInputInterface::Window() const
{
	return Impl->Window;
}

TWeakPtr<SWidget> FInkStylusInputInterface::Widget() const
{
	return Impl->Widget;
}

//---
//---
//---

// Create the StylusInputInterface corresponding to the native Windows Ink driver
TSharedPtr<IStylusInputInterfaceInternal> CreateStylusInputInterfaceInk()
{
	if (!FWindowsPlatformMisc::CoInitialize()) 
	{
		UE_LOG(LogStylusInput, Error, TEXT("Could not initialize COM library!"));
		return nullptr;
	}

	TUniquePtr<FInkStylusInputInterfaceImpl> WindowsImpl = MakeUnique<FInkStylusInputInterfaceImpl>();

	// Load RealTimeStylus DLL
	const FString InkDLLDirectory = TEXT("C:\\Program Files\\Common Files\\microsoft shared\\ink");
	const FString RTSComDLL = TEXT("RTSCom.dll");
	FPlatformProcess::PushDllDirectory(*InkDLLDirectory);

	WindowsImpl->DLLHandle = FPlatformProcess::GetDllHandle(*(InkDLLDirectory / RTSComDLL));
	if (WindowsImpl->DLLHandle == nullptr)
	{
		FWindowsPlatformMisc::CoUninitialize();
		UE_LOG(LogStylusInput, Error, TEXT("Could not load RTSCom.dll!"));
		return nullptr;
	}

	FPlatformProcess::PopDllDirectory(*InkDLLDirectory);

	// Create RealTimeStylus interface
	void* OutInstance { nullptr };
	HRESULT hr = ::CoCreateInstance(__uuidof(RealTimeStylus), nullptr, CLSCTX_INPROC, __uuidof(IRealTimeStylus), &OutInstance);
	if (FAILED(hr))
	{
		FWindowsPlatformMisc::CoUninitialize();
		UE_LOG(LogStylusInput, Error, TEXT("Could not create RealTimeStylus!"));
		return nullptr;
	}

	WindowsImpl->RealTimeStylus = static_cast<IRealTimeStylus*>(OutInstance);
	WindowsImpl->StylusPlugin = MakeShareable(new FInkRealTimeStylusPlugin());
	
	// Create free-threaded marshaller for the plugin
	hr = ::CoCreateFreeThreadedMarshaler(WindowsImpl->StylusPlugin.Get(), &WindowsImpl->StylusPlugin->FreeThreadedMarshaller);
	if (FAILED(hr))
	{
		FWindowsPlatformMisc::CoUninitialize();
		UE_LOG(LogStylusInput, Error, TEXT("Could not create FreeThreadedMarshaller!"));
		return nullptr;
	}

	// Add stylus plugin to the interface
	hr = WindowsImpl->RealTimeStylus->AddStylusSyncPlugin(0, WindowsImpl->StylusPlugin.Get());
	if (FAILED(hr))
	{
		FWindowsPlatformMisc::CoUninitialize();
		UE_LOG(LogStylusInput, Error, TEXT("Could not add stylus plugin to API!"));
		return nullptr;
	}
	
	return MakeShared<FInkStylusInputInterface>(MoveTemp(WindowsImpl));
}
