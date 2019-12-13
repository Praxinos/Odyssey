// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#pragma once

#include "CoreMinimal.h"
#include "Layout/Visibility.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SCompoundWidget.h"
#include "OdysseyBrushEditor.h"

class ODYSSEYBRUSHEDITOR_API SOdysseyBrushEditorSelectedDebugObjectWidget : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SOdysseyBrushEditorSelectedDebugObjectWidget){}
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs, TSharedPtr<FOdysseyBrushEditor> InOdysseyBrushEditor);

    // SWidget interface
    virtual void Tick( const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime ) override;
    // End of SWidget interface

    /** Adds an object to the list of debug choices */
    void AddDebugObject(UObject* TestObject);
    void AddDebugObjectWithName(UObject* TestObject, const FString& TestObjectName);

private:
    UBlueprint* GetBlueprintObj() const { return OdysseyBrushEditor.Pin()->GetBlueprintObj(); }

    /** Creates a list of all debug objects **/
    void GenerateDebugObjectNames(bool bRestoreSelection);

    /** Generate list of active PIE worlds to debug **/
    void GenerateDebugWorldNames(bool bRestoreSelection);

    /** Refresh the widget. **/
    void OnRefresh();

    /** Returns the name of the current debug actor */
    TSharedPtr<FString> GetDebugObjectName() const;

    /** Returns the name of the current debug actor */
    TSharedPtr<FString> GetDebugWorldName() const;

    /** Handles the selection changed event for the debug actor combo box */
    void DebugObjectSelectionChanged(TSharedPtr<FString> NewSelection, ESelectInfo::Type SelectInfo);

    void DebugWorldSelectionChanged(TSharedPtr<FString> NewSelection, ESelectInfo::Type SelectInfo);

    /** Called when user clicks button to select the current object being debugged */
    void SelectedDebugObject_OnClicked();

    /** Returns true if a debug actor is currently selected */
    EVisibility IsSelectDebugObjectButtonVisible() const;

    EVisibility IsDebugWorldComboVisible() const;

private:
    /* Returns the string to indicate no debug object is selected */
    const FString& GetNoDebugString() const;

    const FString& GetDebugAllWorldsString() const;

private:
    /** Pointer back to the blueprint editor tool that owns us */
    TWeakPtr<FOdysseyBrushEditor> OdysseyBrushEditor;

    /** Lists of actors of a given blueprint type and their names */
    TArray< TWeakObjectPtr<UObject> > DebugObjects;
    TArray< TSharedPtr<FString> > DebugObjectNames;

    /** PIE worlds that we can debug */
    TArray< TWeakObjectPtr<UWorld> > DebugWorlds;
    TArray< TSharedPtr<FString> > DebugWorldNames;

    /** Widget containing the names of all possible debug actors */
    TSharedPtr<STextComboBox> DebugObjectsComboBox;

    TSharedPtr<STextComboBox> DebugWorldsComboBox;

    TWeakObjectPtr<UObject> LastObjectObserved;
};
