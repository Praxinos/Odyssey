// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

//#if ENABLE_STYLUS_SUPPORT

#include "CoreMinimal.h"
#include "StylusInput.h"
#include "StylusInputPacket.h"
#include "TickableEditorObject.h"

#include "Widgets/SWidget.h"

class FStylusInputHandler final : public UE::StylusInput::IStylusInputEventHandler
{
public:
    FStylusInputHandler();
    virtual ~FStylusInputHandler() override;

    /**
     * Registers the window containing the given Widget for Stylus input handling.
     * Registered windows persist for the lifetime of the StylusInputHandler.
     *
     * @param Widget The widget whose window is to be registered.
     * @return True if the window was registered, false if the window was invalid or was previously registered.
     */
    bool RegisterWindow(const TSharedRef<SWidget>& Widget);


    // IStylusInputEventHandler implementation
    virtual FString GetName() override;
    virtual void OnPacket(const UE::StylusInput::FStylusInputPacket& Packet, UE::StylusInput::IStylusInputInstance* Instance) override;

private:
    void ProcessPacket(const UE::StylusInput::FStylusInputPacket& Packet, UE::StylusInput::IStylusInputInstance* Instance);
    const UE::StylusInput::IStylusInputTabletContext* GetTabletContext(UE::StylusInput::IStylusInputInstance* Instance, uint32 TabletContextID);

    TMap<TSharedPtr<SWindow>, UE::StylusInput::IStylusInputInstance*> StylusInputInstances;
    TMap<uint32, TSharedPtr<UE::StylusInput::IStylusInputTabletContext>> TabletContexts;

    float ActivePressure = 1.0f;
};

//#endif // ENABLE_STYLUS_SUPPORT
