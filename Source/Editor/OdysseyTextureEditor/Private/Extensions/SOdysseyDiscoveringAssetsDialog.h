// Copyright Epic Games, Inc. All Rights Reserved.
// This a copy of SDiscoveringAssetsDialog.h private in the source code of UE4
// see: https://udn.unrealengine.com/s/question/0D54z0000775oOGCAY/spackagereportdialog-and-sdiscoveringassetsdialog-are-private

#pragma once

#include "CoreMinimal.h"
#include "Input/Reply.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SCompoundWidget.h"

class SOdysseyDiscoveringAssetsDialog : public SCompoundWidget
{
public:
    DECLARE_DELEGATE( FOnAssetsDiscovered )

    SLATE_BEGIN_ARGS( SOdysseyDiscoveringAssetsDialog ){}

    SLATE_EVENT( FOnAssetsDiscovered, mOnAssetsDiscovered )

    SLATE_END_ARGS()

    /** Destructor */
    ~SOdysseyDiscoveringAssetsDialog();

    /** Constructs this widget with InArgs */
    void Construct( const FArguments& iInArgs );

    /** Opens the dialog in a new window */
    static void OpenDiscoveringAssetsDialog( const FOnAssetsDiscovered& iInOnAssetsDiscovered );

private:
    /** Handler for when "Cancel" is clicked */
    FReply CancelClicked();

    /** Called when the asset registry initial load has completed */
    void AssetRegistryLoadComplete();

    /** Closes the dialog. */
    void CloseDialog();

private:
    FOnAssetsDiscovered mOnAssetsDiscovered;
};
