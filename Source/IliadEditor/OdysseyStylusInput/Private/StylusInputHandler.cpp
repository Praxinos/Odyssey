// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "StylusInputHandler.h"

//#if ENABLE_STYLUS_SUPPORT

#include "StylusInputTabletContext.h"
#include "Framework/Application/SlateApplication.h"

using namespace UE::StylusInput;

#define LOCTEXT_NAMESPACE "StylusInput"

FOdysseyStylusInputHandler::FOdysseyStylusInputHandler()
{
}

FOdysseyStylusInputHandler::~FOdysseyStylusInputHandler()
{
    if( StylusInputInstance )
    {
        StylusInputInstance->RemoveEventHandler(this);
        ReleaseInstance(StylusInputInstance);
        StylusInputInstance = nullptr;
        StylusInputWindow = nullptr;
    }
}

bool FOdysseyStylusInputHandler::RegisterWindow(const TSharedRef<SWidget>& Widget)
{
    TSharedPtr<SWindow> Window = FSlateApplication::Get().FindWidgetWindow(Widget);
    if (!Window)
    {
        UE_LOG(LogTemp, Display, TEXT("NoWindow"));
        return false;
    }

    if( StylusInputWindow.Pin().Get() == Window.Get() )
    {
        return false;
    }

    UnregisterWindow();

    UE_LOG(LogTemp, Display, TEXT("Preregister"));

    IStylusInputInstance* InputInstance = CreateInstance(*Window, "Wintab", false);
    //IStylusInputInstance* InputInstance = CreateInstance(*Window);

    UE_LOG(LogTemp, Display, TEXT("PreInstance"))

    if (!InputInstance)
    {
        return false;
    }
    UE_LOG(LogTemp, Display, TEXT("PostInstance"))


    InputInstance->AddEventHandler(this, EEventHandlerThread::OnGameThread);

    StylusInputWindow = Window;
    StylusInputInstance = InputInstance;
    UE_LOG(LogTemp, Display, TEXT("Postregister"))

    return true;
}


bool FOdysseyStylusInputHandler::UnregisterWindow()
{
    if (StylusInputInstance || StylusInputWindow.IsValid())
    {
        StylusInputInstance->RemoveEventHandler(this);
        ReleaseInstance(StylusInputInstance);
        StylusInputInstance = nullptr;
        StylusInputWindow = nullptr;
        UE_LOG(LogTemp, Display, TEXT("Unregister"))
        return true;
    }
    return false;
}

FString FOdysseyStylusInputHandler::GetName()
{
    return "OdysseyStylusInputHandler";
}

/*
void FOdysseyStylusInputHandler::OnPacket(const FStylusInputPacket& Packet, IStylusInputInstance* Instance)
{
    PacketQueue.Enqueue(Packet);
    PrintPacket(Packet);
    if (Packet.Type != EPacketType::Invalid &&
        Packet.Type != EPacketType::AboveDigitizer)
    {
        ProcessPacket(Packet, Instance);
    }
}*/

void FOdysseyStylusInputHandler::PrintPacket(const UE::StylusInput::FStylusInputPacket& Packet)
{
    UE_LOG(LogTemp, Display, TEXT("-------------------"))
    UE_LOG(LogTemp, Display, TEXT("TabletContextID %d"), Packet.TabletContextID)
    UE_LOG(LogTemp, Display, TEXT("CursorID %d"), Packet.CursorID)
    UE_LOG(LogTemp, Display, TEXT("Type %d"), Packet.Type)
    UE_LOG(LogTemp, Display, TEXT("PenStatus %d"), Packet.PenStatus)
    UE_LOG(LogTemp, Display, TEXT("X %lf"), Packet.X)
    UE_LOG(LogTemp, Display, TEXT("Y %lf"), Packet.Y)
    UE_LOG(LogTemp, Display, TEXT("Z %lf"), Packet.Z)
    UE_LOG(LogTemp, Display, TEXT("NormalPressure %lf"), Packet.NormalPressure)
}

void FOdysseyStylusInputHandler::ProcessPacket(const FStylusInputPacket& Packet, IStylusInputInstance* Instance)
{
}

const IStylusInputTabletContext* FOdysseyStylusInputHandler::GetTabletContext(IStylusInputInstance* Instance, uint32 TabletContextID)
{
    if (!Instance)
    {
        return nullptr;
    }

    const TSharedPtr<IStylusInputTabletContext>* TabletContext = TabletContexts.Find(TabletContextID);
    if (!TabletContext)
    {
        if (const TSharedPtr<IStylusInputTabletContext>& NewTabletContext = Instance->GetTabletContext(TabletContextID))
        {
            // We currently assume that TabletContextIDs are unique across all instances.
            TabletContext = &TabletContexts.Emplace(TabletContextID, NewTabletContext);
        }
    }

    return TabletContext ? TabletContext->Get() : nullptr;
}

#undef LOCTEXT_NAMESPACE

//#endif // ENABLE_STYLUS_SUPPORT
