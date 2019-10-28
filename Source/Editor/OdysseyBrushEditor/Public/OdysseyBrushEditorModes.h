// Copyright 2018-2019 Praxinos, Inc. All Rights Reserved.
// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "WorkflowOrientedApp/WorkflowTabManager.h"
#include "OdysseyBrushEditor.h"
#include "WorkflowOrientedApp/ApplicationMode.h"

struct ODYSSEYBRUSHEDITOR_API FOdysseyBrushEditorApplicationModes
{
    // Mode identifiers
    static const FName StandardOdysseyBrushEditorMode;
    static FText GetLocalizedMode( const FName InMode )
    {
        static TMap< FName, FText > LocModes;

        if (LocModes.Num() == 0)
        {
            LocModes.Add( StandardOdysseyBrushEditorMode, NSLOCTEXT("OdysseyBrushEditor", "StandardOdysseyBrushEditorMode", "OdysseyBrushGraph") );
        }

        check( InMode != NAME_None );
        const FText* OutDesc = LocModes.Find( InMode );
        check( OutDesc );
        return *OutDesc;
    }
private:
    FOdysseyBrushEditorApplicationModes() {}
};

class ODYSSEYBRUSHEDITOR_API FOdysseyBrushEditorUnifiedMode : public FApplicationMode
{
public:
    FOdysseyBrushEditorUnifiedMode(TSharedPtr<class FOdysseyBrushEditor> InOdysseyBrushEditor, FName InModeName, FText(*GetLocalizedMode)( const FName ), const bool bRegisterViewport = true);

    virtual void RegisterTabFactories(TSharedPtr<FTabManager> InTabManager) override;
    virtual void PreDeactivateMode() override;
    virtual void PostActivateMode() override;
public:

protected:
    TWeakPtr<FOdysseyBrushEditor> MyOdysseyBrushEditor;

    // Set of spawnable tabs in blueprint editing mode
    FWorkflowAllowedTabSet OdysseyBrushEditorTabFactories;

    // Set of spawnable tabs useful in derived classes, even without a blueprint
    FWorkflowAllowedTabSet CoreTabFactories;

    // Set of spawnable tabs only usable in blueprint editing mode (not useful in Persona, etc...)
    FWorkflowAllowedTabSet OdysseyBrushEditorOnlyTabFactories;
};
