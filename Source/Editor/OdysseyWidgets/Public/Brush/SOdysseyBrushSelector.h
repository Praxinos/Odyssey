// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#pragma once

#include "CoreMinimal.h"
#include "Layout/Visibility.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "PropertyCustomizationHelpers.h"
#include "AssetThumbnail.h"
#include "OdysseyBrushBlueprint.h"

/** Delegate used to set a generic object */
DECLARE_DELEGATE_OneParam( FOnBrushChanged, UOdysseyBrush* );

/**
 * Implements the brush selector
 */
class ODYSSEYWIDGETS_API SOdysseyBrushSelector : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SOdysseyBrushSelector)
        {}
        /** Called when the object value changes */
        SLATE_EVENT( FOnBrushChanged, OnBrushChanged )
    SLATE_END_ARGS()

public:
    // Construction / Destruction
    ~SOdysseyBrushSelector();
    void Construct(const FArguments& InArgs);

private:
    // Private internal callbacks
    FString ObjectPath() const;
    void OnObjectChanged(const FAssetData& AssetData);

private:
    // Private data
    FOnBrushChanged OnBrushChanged;
    UOdysseyBrush* current_brush;
    TSharedPtr< FAssetThumbnailPool > asset_thumbnail_pool;
};
