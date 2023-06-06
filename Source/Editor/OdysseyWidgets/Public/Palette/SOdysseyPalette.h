// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Layout/Visibility.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "PropertyCustomizationHelpers.h"
#include "AssetThumbnail.h"
#include "Engine/StaticMesh.h"
#include "FOdysseyPalette.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"

/** Delegate used to set a generic object */
DECLARE_DELEGATE_OneParam( FOnMeshChanged, UStaticMesh* );

/**
 * Implements the mesh selector
 */
class ODYSSEYWIDGETS_API SOdysseyPalette : public STreeView<UOdysseyPalette*>
{
public:
    SLATE_BEGIN_ARGS(SOdysseyPalette)
        {}
    SLATE_END_ARGS()

public:
    // Construction / Destruction
    ~SOdysseyPalette();
    void Construct(const FArguments& InArgs);

public:
    // Getter / Setter
    FOdysseyPalette* GetColorPalette();

private:
    // Private internal callbacks
    void OnObjectChanged(const FAssetData& AssetData);
    FString ObjectPath() const;
    FReply HandleMeshColorBlockMouseButtonDown();

private:
    //Internal Widget Creation
    TSharedRef<SWidget> CreateColorPaletteWidget();

private:
    // Private data
    TSharedPtr< FOdysseyPalette > mColorPalette;
    TSharedPtr< FAssetThumbnailPool > mAssetThumbnailPool;
    SScrollBox::FSlot*  mColorPaletteSlot;
};
