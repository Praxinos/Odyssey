// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#pragma once

#include "CoreMinimal.h"
#include "Layout/Visibility.h"
#include "Input/Reply.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SCompoundWidget.h"
#include "ISourceControlProvider.h"
#include "SourceControlOperations.h"

class SVerticalBox;
class UBlueprint;
struct FRevisionInfo;

class ODYSSEYBRUSHEDITOR_API SOdysseyBrushRevisionMenu : public SCompoundWidget
{
    DECLARE_DELEGATE_OneParam(FOnRevisionSelected, FRevisionInfo const&)

public:
    SLATE_BEGIN_ARGS(SOdysseyBrushRevisionMenu)
        : _bIncludeLocalRevision(false)
    {}
        SLATE_ARGUMENT(bool, bIncludeLocalRevision)
        SLATE_EVENT(FOnRevisionSelected, OnRevisionSelected)
    SLATE_END_ARGS()

    ~SOdysseyBrushRevisionMenu();

    void Construct(const FArguments& InArgs, UBlueprint const* OdysseyBrush);

private:
    /** Delegate used to determine the visibility 'in progress' widgets */
    EVisibility GetInProgressVisibility() const;
    /** Delegate used to determine the visibility of the cancel button */
    EVisibility GetCancelButtonVisibility() const;

    /** Delegate used to cancel a source control operation in progress */
    FReply OnCancelButtonClicked() const;
    /** Callback for when the source control operation is complete */
    void OnSourceControlQueryComplete(const FSourceControlOperationRef& InOperation, ECommandResult::Type InResult);

    /**  */
    bool bIncludeLocalRevision;
    /**  */
    FOnRevisionSelected OnRevisionSelected;
    /** The name of the file we want revision info for */
    FString Filename;
    /** The box we are using to display our menu */
    TSharedPtr<SVerticalBox> MenuBox;
    /** The source control operation in progress */
    TSharedPtr<FUpdateStatus, ESPMode::ThreadSafe> SourceControlQueryOp;
    /** The state of the SCC query */
    uint32 SourceControlQueryState;
};
