// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#pragma once

#include "CoreMinimal.h"
#include "Toolkits/IToolkitHost.h"
#include "OdysseyBrushEditorModule.h"
#include "IDetailsView.h"

class FStructureDefaultValueView;

class FUserDefinedStructureEditor : public IOdysseyUserDefinedStructureEditor
{
    /** App Identifier.*/
    static const FName UserDefinedStructureEditorAppIdentifier;

    /**    The tab ids for all the tabs used */
    static const FName MemberVariablesTabId;

    /** Property viewing widget */
    TSharedPtr<class IDetailsView> PropertyView;
    TSharedPtr<class FStructureDefaultValueView> DefaultValueView;
public:
    /**
     * Edits the specified enum
     *
     * @param    Mode                    Asset editing mode for this editor (standalone or world-centric)
     * @param    InitToolkitHost            When Mode is WorldCentric, this is the level editor instance to spawn this editor within
     * @param    EnumToEdit                The user defined enum to edit
     */
    void InitEditor(const EToolkitMode::Type Mode, const TSharedPtr< class IToolkitHost >& InitToolkitHost, class UUserDefinedStruct* EnumToEdit);

    /** Destructor */
    virtual ~FUserDefinedStructureEditor();

    /** IToolkit interface */
    virtual FName GetToolkitFName() const override;
    virtual FText GetBaseToolkitName() const override;
    virtual FText GetToolkitName() const override;
    virtual FText GetToolkitToolTipText() const override;
    virtual FString GetWorldCentricTabPrefix() const override;
    virtual FLinearColor GetWorldCentricTabColorScale() const override;

    virtual void RegisterTabSpawners(const TSharedRef<class FTabManager>& TabManager) override;
    virtual void UnregisterTabSpawners(const TSharedRef<class FTabManager>& TabManager) override;

protected:
    TSharedRef<SDockTab> SpawnStructureTab(const FSpawnTabArgs& Args);
};

