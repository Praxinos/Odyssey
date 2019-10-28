// Copyright 2018-2019 Praxinos, Inc. All Rights Reserved.
// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SWidget.h"
#include "Toolkits/AssetEditorToolkit.h"
#include "WorkflowOrientedApp/WorkflowTabFactory.h"

#define LOCTEXT_NAMESPACE "OdysseyBrushEditorSharedTabFactories"

//////////////////////////////////////////////////////////////////////////
// FCompilerResultsSummoner

struct ODYSSEYBRUSHEDITOR_API FCompilerResultsSummoner : public FWorkflowTabFactory
{
public:
    FCompilerResultsSummoner(TSharedPtr<class FAssetEditorToolkit> InHostingApp);

    virtual TSharedRef<SWidget> CreateTabBody(const FWorkflowTabSpawnInfo& Info) const override;

    virtual FText GetTabToolTipText(const FWorkflowTabSpawnInfo& Info) const override
    {
        return LOCTEXT("CompilerResultsTooltip", "The compiler results tab shows any errors or warnings generated when compiling this OdysseyBrush.");
    }
};

#undef LOCTEXT_NAMESPACE
