// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#pragma once

#include "CoreMinimal.h"
#include "SlateFwd.h"
#include "Widgets/Input/SComboButton.h"

class SGraphTitleBarAddNewBookmark : public SComboButton
{
public:
    SLATE_BEGIN_ARGS(SGraphTitleBarAddNewBookmark)
        : _EditorPtr()
    {}

    SLATE_ARGUMENT(TWeakPtr<class FOdysseyBrushEditor>, EditorPtr)
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);

protected:
    FText GetAddButtonGlyph() const;
    FText GetPopupTitleText() const;
    FText GetDefaultNameText() const;
    FText GetAddButtonLabel() const;

    void OnComboBoxOpened();
    FReply OnAddButtonClicked();
    bool IsAddButtonEnabled() const;
    FReply OnRemoveButtonClicked();
    void OnNameTextCommitted(const FText& InText, ETextCommit::Type CommitType);

private:
    TWeakPtr<class FOdysseyBrushEditor> EditorContextPtr;
    TSharedPtr<class SEditableTextBox> NameEntryWidget;

    FText CurrentNameText;
    FText OriginalNameText;
    FGuid CurrentViewBookmarkId;
};
