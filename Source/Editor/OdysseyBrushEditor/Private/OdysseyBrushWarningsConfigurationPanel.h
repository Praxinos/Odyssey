// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#pragma once

#include "CoreMinimal.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SCompoundWidget.h"

class SSettingsEditorCheckoutNotice;
struct FBlueprintWarningDeclaration;
enum class EBlueprintWarningBehavior : uint8;
template <typename ItemType> class SListView;

typedef TSharedPtr<FBlueprintWarningDeclaration> FBlueprintWarningListEntry;
typedef SListView<FBlueprintWarningListEntry> FOdysseyBrushWarningListView;
class SSettingsEditorCheckoutNotice;

class SOdysseyBrushWarningsConfigurationPanel : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SOdysseyBrushWarningsConfigurationPanel){}
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);
private:
    friend class SOdysseyBrushWarningRow;

    void UpdateSelectedWarningBehaviors(EBlueprintWarningBehavior NewBehavior, const FBlueprintWarningDeclaration& AlteredWarning );

    // SListView requires TArray<TSharedRef<FFoo>> so we cache off a list from core:
    TArray<TSharedPtr<FBlueprintWarningDeclaration>> CachedBlueprintWarningData;
    // Again, SListView boilerplate:
    TArray<TSharedPtr<EBlueprintWarningBehavior>> CachedOdysseyBrushWarningBehaviors;
    // Storing the listview, so we can apply updates to all selected entries:
    TSharedPtr< FOdysseyBrushWarningListView > ListView;
    // Stored so that we can only enable controls when the settings files is writable:
    TSharedPtr<SSettingsEditorCheckoutNotice> SettingsEditorCheckoutNotice;
};

