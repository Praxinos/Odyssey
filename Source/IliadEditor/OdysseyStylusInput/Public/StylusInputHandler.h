// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

//#if ENABLE_STYLUS_SUPPORT

#include "CoreMinimal.h"
#include "StylusInput.h"
#include "StylusInputPacket.h"
#include "TickableEditorObject.h"

#include "Widgets/SWidget.h"
#include "Containers/SpscQueue.h"

class ODYSSEYSTYLUSINPUT_API FOdysseyStylusInputHandler : public UE::StylusInput::IStylusInputEventHandler
{
public:
    FOdysseyStylusInputHandler();
    virtual ~FOdysseyStylusInputHandler() override;

    /**
     * Registers the window containing the given Widget for Stylus input handling.
     * Registered windows persist for the lifetime of the StylusInputHandler.
     *
     * @param Widget The widget whose window is to be registered.
     * @return True if the window was registered, false if the window was invalid or was previously registered.
     */
    bool RegisterWindow(const TSharedRef<SWidget>& iWidget);
    bool RegisterWindow(TSharedPtr<SWindow> iWindow); //Same but Window version

    bool UnregisterWindow();

    // IStylusInputEventHandler implementation
    virtual FString GetName() override;

    void PrintPacket(const UE::StylusInput::FStylusInputPacket& iPacket);

public:
    TSpscQueue<UE::StylusInput::FStylusInputPacket> PacketQueue;

protected:
    const UE::StylusInput::IStylusInputTabletContext* GetTabletContext(UE::StylusInput::IStylusInputInstance* iInstance, uint32 iTabletContextID);

    void OnStylusInputDriverChanged(FName iStylusInputDriver);

    UE::StylusInput::IStylusInputInstance* mStylusInputInstance = nullptr;
    TWeakPtr<SWindow> mStylusInputWindow;

    FDelegateHandle mOnStylusInputDriverChanged;

};

//#endif // ENABLE_STYLUS_SUPPORT
