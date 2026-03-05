// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "IStylusState.h"

#include "EditorSubsystem.h"
#include "TickableEditorObject.h"
#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"
#include "Widgets/Docking/SDockTab.h"

DEFINE_LOG_CATEGORY_STATIC(LogStylusInput, Log, All);

/**
 * Module to handle Wacom-style tablet input using styluses.
 */
class ODYSSEYSTYLUSINPUT_API IOdysseyStylusInputModule
    : public IModuleInterface
{
public:
    /**
     * Retrieve the module instance.
     */
    static inline IOdysseyStylusInputModule& Get()
    {
        return FModuleManager::LoadModuleChecked<IOdysseyStylusInputModule>("OdysseyStylusInput");
    }

    /**
     * Checks to see if this module is loaded and ready.  It is only valid to call Get() if IsAvailable() returns true.
     *
     * @return True if the module is loaded and ready to use
     */
    static inline bool IsAvailable()
    {
        return FModuleManager::Get().IsModuleLoaded("OdysseyStylusInput");
    }
};
