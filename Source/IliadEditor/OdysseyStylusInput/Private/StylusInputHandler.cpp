// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "StylusInputHandler.h"

//#if ENABLE_STYLUS_SUPPORT

#include "StylusInputTabletContext.h"
#include "Framework/Application/SlateApplication.h"
#include "OdysseyStylusInputSettings.h"

using namespace UE::StylusInput;

#define LOCTEXT_NAMESPACE "StylusInput"

FOdysseyStylusInputHandler::FOdysseyStylusInputHandler()
{
    mOnStylusInputDriverChanged = UOdysseyStylusInputSettings::OnStylusInputDriverChanged.AddRaw( this, &FOdysseyStylusInputHandler::OnStylusInputDriverChanged );
}

FOdysseyStylusInputHandler::~FOdysseyStylusInputHandler()
{
    UnregisterWindow();
    UOdysseyStylusInputSettings::OnStylusInputDriverChanged.Remove(mOnStylusInputDriverChanged);
}

bool FOdysseyStylusInputHandler::RegisterWindow(const TSharedRef<SWidget>& iWidget)
{
    TSharedPtr<SWindow> window = FSlateApplication::Get().FindWidgetWindow(iWidget);
    if ( !window )
    {
        return false;
    }

    if( mStylusInputWindow.Pin().Get() == window.Get() )
    {
        return false;
    }

    UnregisterWindow();

    const UOdysseyStylusInputSettings* settings = GetDefault<UOdysseyStylusInputSettings>();
    FName selectedAPI = settings->StylusInputDriver;
    IStylusInputInstance* inputInstance = CreateInstance(*window, selectedAPI, false);

    //UE_LOG(LogTemp, Display, TEXT("PreInstance, %s"), *(selectedAPI.ToString()))

    if (!inputInstance)
    {
        return false;
    }

    inputInstance->AddEventHandler(this, EEventHandlerThread::OnGameThread);

    mStylusInputWindow = window;
    mStylusInputInstance = inputInstance;

    return true;
}

bool FOdysseyStylusInputHandler::RegisterWindow(TSharedPtr<SWindow> iWindow)
{
    UnregisterWindow();

    if (!iWindow)
    {
        return false;
    }

    const UOdysseyStylusInputSettings* settings = GetDefault<UOdysseyStylusInputSettings>();
    FName selectedAPI = settings->StylusInputDriver;
    IStylusInputInstance* inputInstance = CreateInstance(*iWindow, selectedAPI, false);

    if (!inputInstance)
    {
        return false;
    }

    inputInstance->AddEventHandler(this, EEventHandlerThread::OnGameThread);

    mStylusInputWindow = iWindow;
    mStylusInputInstance = inputInstance;

    return true;
}


bool FOdysseyStylusInputHandler::UnregisterWindow()
{
    if (mStylusInputInstance || mStylusInputWindow.IsValid())
    {
        mStylusInputInstance->RemoveEventHandler(this);
        ReleaseInstance(mStylusInputInstance);
        mStylusInputInstance = nullptr;
        mStylusInputWindow = nullptr;
        return true;
    }
    return false;
}

FString FOdysseyStylusInputHandler::GetName()
{
    return "OdysseyStylusInputHandler";
}

void FOdysseyStylusInputHandler::PrintPacket(const UE::StylusInput::FStylusInputPacket& iPacket)
{
    UE_LOG(LogTemp, Display, TEXT("-------------------"))
    UE_LOG(LogTemp, Display, TEXT("TabletContextID %d"), iPacket.TabletContextID)
    UE_LOG(LogTemp, Display, TEXT("CursorID %d"), iPacket.CursorID)
    UE_LOG(LogTemp, Display, TEXT("Type %d"), iPacket.Type)
    UE_LOG(LogTemp, Display, TEXT("PenStatus %d"), iPacket.PenStatus)
    UE_LOG(LogTemp, Display, TEXT("X %lf"), iPacket.X)
    UE_LOG(LogTemp, Display, TEXT("Y %lf"), iPacket.Y)
    UE_LOG(LogTemp, Display, TEXT("Z %lf"), iPacket.Z)
    UE_LOG(LogTemp, Display, TEXT("NormalPressure %lf"), iPacket.NormalPressure)
    UE_LOG(LogTemp, Display, TEXT("TimerTick %d"), iPacket.TimerTick)
}

const IStylusInputTabletContext* FOdysseyStylusInputHandler::GetTabletContext(IStylusInputInstance* iInstance, uint32 iTabletContextID)
{
    if (!iInstance)
    {
        return nullptr;
    }

    const TSharedPtr<IStylusInputTabletContext> tabletContext = iInstance->GetTabletContext(iTabletContextID);

    return tabletContext ? tabletContext.Get() : nullptr;
}

void FOdysseyStylusInputHandler::OnStylusInputDriverChanged(FName iStylusInputDriver)
{
    TSharedPtr<SWindow> window = mStylusInputWindow.Pin();
    RegisterWindow( window ); //Registers the previous Window with the new API
}

#if PLATFORM_WINDOWS
void FOdysseyStylusInputHandler::ConvertWintabToWindowCoordinates( float& ioX, float& ioY )
{
    ioX += GetSystemMetrics(SM_XVIRTUALSCREEN);
    ioY += GetSystemMetrics(SM_YVIRTUALSCREEN);
}
#endif

#undef LOCTEXT_NAMESPACE

//#endif // ENABLE_STYLUS_SUPPORT
