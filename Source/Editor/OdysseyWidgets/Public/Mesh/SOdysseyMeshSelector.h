// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"
#include "Layout/Visibility.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "PropertyCustomizationHelpers.h"
#include "AssetThumbnail.h"
#include "Engine/StaticMesh.h"
#include "Mesh/FOdysseyMeshSelector.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"

/** Delegate used to set a generic object */
DECLARE_DELEGATE_OneParam( FOnMeshChanged, UStaticMesh* );

/**
 * Implements the mesh selector
 */
class ODYSSEYWIDGETS_API SOdysseyMeshSelector : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SOdysseyMeshSelector)
        {}
        /** Called when the object value changes */
        SLATE_EVENT( FOnMeshChanged, OnMeshChanged )
    SLATE_END_ARGS()

public:
    // Construction / Destruction
    ~SOdysseyMeshSelector();
    void Construct(const FArguments& InArgs);

public:
    // Getter / Setter
    FOdysseyMeshSelector* GetMeshSelectorPtr();

private:
    // Private internal callbacks
    FString ObjectPath() const;
    void OnObjectChanged(const FAssetData& AssetData);
    void OnLODChanged( int inNewLOD);
    FReply HandleMeshColorBlockMouseButtonDown( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent );
    void OnSetMeshColorFromColorPicker(FLinearColor NewColor);
    FLinearColor GetMeshColor() const;

private:
    //Internal Widget Creation
    TSharedRef<SWidget> CreateLODMenuWidget();
    TSharedRef<SWidget> CreateUVMenuWidget();


private:
    // Private data
    TSharedPtr< FOdysseyMeshSelector > MeshSelector;
    FOnMeshChanged OnMeshChanged;
    TSharedPtr< FAssetThumbnailPool > AssetThumbnailPool;
    TSharedPtr<SWidget> ColorBlockWidget;
    /** Commands handled by this widget */
    TSharedPtr< FUICommandList > CommandList;
    SVerticalBox::FSlot*  LODSelectionMenu;
    SVerticalBox::FSlot*  UVSelectionMenu;
};
