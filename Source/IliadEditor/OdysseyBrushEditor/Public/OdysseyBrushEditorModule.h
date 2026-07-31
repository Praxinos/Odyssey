// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"
#include "Framework/Commands/UICommandList.h"
#include "Toolkits/AssetEditorToolkit.h"

class  FAssetTypeActions_OdysseyBrush;

/**
 * The Brush editor module provides the brush editor application via classical bp editor.
 */
class FOdysseyBrushEditorModule : public IModuleInterface
{
public:
    // IModuleInterface interface
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
    // End of IModuleInterface interface

    TSharedPtr< FAssetTypeActions_OdysseyBrush >  OdysseyBrushAssetTypeActions;

private:
    void RegisterTelemetry();
    void UnregisterTelemetry();

    void OnAssetEditorOpened( UObject* iObject );
    void OnAssetEditorClosed( UObject* iObject, IAssetEditorInstance* iAssetEditorInstance );

private:
    struct FSessionInfo
    {
        FDateTime SessionStartTime;
        FName EditorName; // During close, iAssetEditorInstance is (may be) partially destroyed, that's why the editor name is stored inside open
    };
    TMap<UObject*, FSessionInfo> SessionInfoByObject;
};
