// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/SOdysseyViewportDrawingEditorMasterTab.h"

#include "PropertyCustomizationHelpers.h"
#include "Widgets/Layout/SSeparator.h"
#include "UnrealEdGlobals.h"
#include "Editor/UnrealEdEngine.h"
#include "Widgets/Layout/SScrollBox.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyViewportDrawingEditorUtils.h"
#include "SAssetView.h"
#include "OdysseyAnimationActor.h"
#include "OdysseyAnimationComponent.h"

#define LOCTEXT_NAMESPACE "ViewportDrawingEditor"

//--------------------------------------------------------------------------------------
//--------------------------------------------------- FOdysseyEditorTab interface

void
SOdysseyViewportDrawingEditorMasterTab::Construct(const FArguments& InArgs, FOdysseyViewportDrawingEditorExtension* iExtension)
{
    mExtension = iExtension;

    mOptions.Empty();
    mOptions.Add(MakeShared< EOdysseyViewportDrawingPaintingAdapterMethod >(EOdysseyViewportDrawingPaintingAdapterMethod::OdysseyTextureBased));
#if !PLATFORM_MAC
    mOptions.Add(MakeShared< EOdysseyViewportDrawingPaintingAdapterMethod >(EOdysseyViewportDrawingPaintingAdapterMethod::OdysseyMeshBasedPlanar));
    //mOptions.Add(MakeShared< EOdysseyViewportDrawingPaintingAdapterMethod >(EOdysseyViewportDrawingPaintingAdapterMethod::OdysseyMeshBasedSphere));
    mOptions.Add(MakeShared< EOdysseyViewportDrawingPaintingAdapterMethod >(EOdysseyViewportDrawingPaintingAdapterMethod::OdysseyScreenBased));
#endif

    mThumbnailPool = MakeShareable( new FAssetThumbnailPool( 50 ) );

    ChildSlot
    [
        SNew( SScrollBox )
        .Orientation( Orient_Vertical )
        .ScrollBarAlwaysVisible( false )
        + SScrollBox::Slot()
        [
            SNew( SVerticalBox )
            +SVerticalBox::Slot()
            .Padding(2)
            .AutoHeight()
            [
                SNew(STextBlock)
                .Text(FText::FromString("Select Actor"))
            ]
            + SVerticalBox::Slot()
            .Padding(2)
            .AutoHeight()
            [
                SNew(SObjectPropertyEntryBox)
                .AllowedClass(AActor::StaticClass())
                .ObjectPath(this, &SOdysseyViewportDrawingEditorMasterTab::PaintActorPath)
                .OnObjectChanged(FOnSetObject::CreateRaw(this, &SOdysseyViewportDrawingEditorMasterTab::OnActorChanged))
                .EnableContentPicker(true)
                .DisplayCompactSize(true)
                .DisplayThumbnail(true)
                .ThumbnailSizeOverride(FIntPoint(30, 30))
            ]
            +SVerticalBox::Slot()
            .Padding(2)
            .AutoHeight()
            [
                CreateMeshComponentMenuWidget()
            ]
            +SVerticalBox::Slot()
            [
                SNew( SVerticalBox )
                .Visibility_Lambda(
                    [this]()
                    {
                        if ( !mExtension->Component() || !mExtension->Component()->IsA<UOdysseyAnimationComponent>() )
                            return EVisibility::Visible;

                        return EVisibility::Collapsed;
                    }
                )
                +SVerticalBox::Slot()
                .Padding(2)
                .AutoHeight()
                [
                    SNew(STextBlock)
                    .Text(FText::FromString("Select Material"))
                ]
                + SVerticalBox::Slot()
                .Padding(2)
                .AutoHeight()
                [
                    SNew(SObjectPropertyEntryBox)
                    .AllowedClass(UMaterialInterface::StaticClass())
                    .ObjectPath(this, &SOdysseyViewportDrawingEditorMasterTab::PaintMaterialPath)
                    .OnObjectChanged(FOnSetObject::CreateRaw(this, &SOdysseyViewportDrawingEditorMasterTab::OnMaterialChanged))
                    .OnShouldFilterAsset(FOnShouldFilterAsset::CreateRaw(this, &SOdysseyViewportDrawingEditorMasterTab::ShouldFilterMaterialAsset))
                    .DisplayBrowse(true)
                    .EnableContentPicker(true)
                    .DisplayCompactSize(true)
                    //.DisplayThumbnail(true)
                    //.ThumbnailSizeOverride(FIntPoint(30, 30))
                    //.ThumbnailPool( mThumbnailPool )
                ]
                + SVerticalBox::Slot()
                .Padding(2)
                .AutoHeight()
                [
                    SNew(STextBlock)
                    .Text(FText::FromString("Select Texture"))
                ]
                + SVerticalBox::Slot()
                .Padding(2)
                .AutoHeight()
                [
                    SNew(SObjectPropertyEntryBox)
                        .AllowedClass(UTexture::StaticClass())
                        .ObjectPath(this, &SOdysseyViewportDrawingEditorMasterTab::PaintTexturePath)
                        .OnObjectChanged(FOnSetObject::CreateRaw(this, &SOdysseyViewportDrawingEditorMasterTab::OnTextureChanged))
                        .OnShouldFilterAsset(FOnShouldFilterAsset::CreateRaw(this, &SOdysseyViewportDrawingEditorMasterTab::ShouldFilterTextureAsset))
                        .DisplayBrowse(true)
                        .EnableContentPicker(true)
                        .DisplayCompactSize(true)
                        .DisplayThumbnail(true)
                        .ThumbnailSizeOverride(FIntPoint(70, 70))
                        .ThumbnailPool( mThumbnailPool )
                ]

                //Here is an interesting way to show selected assets using a content browser like view
                /* + SVerticalBox::Slot()
                .Padding(2)
                .AutoHeight()
                [
                    SNew(SAssetView)
                        .OnShouldFilterAsset_Lambda(
                            [this](const FAssetData& iAssetData)
                            {
                                UObject* object = iAssetData.FastGetAsset();
                                return !mExtension->SelectableTextures().ContainsByPredicate(
                                    [object](const FPaintableTexture& iTexture)
                                    {
                                        return object == iTexture.Texture;
                                    }
                                );
                            }
                        )
                        //SLATE_EVENT( FOnShouldFilterItem, OnShouldFilterItem)
                        //SLATE_EVENT( FOnGetContentBrowserItemContextMenu, OnGetItemContextMenu )
                        //SLATE_EVENT(FOnIsAssetValidForCustomToolTip, OnIsAssetValidForCustomToolTip)
                        //SLATE_EVENT( FOnGetCustomAssetToolTip, OnGetCustomAssetToolTip )
                        //SLATE_EVENT(FOnExtendAssetViewOptionsMenuContext, OnExtendAssetViewOptionsMenuContext)
                        //SLATE_ARGUMENT( EContentBrowserItemCategoryFilter, InitialCategoryFilter )
                        //SLATE_ARGUMENT( TSharedPtr<FAssetFilterCollectionType>, FrontendFilters )
                        //SLATE_ARGUMENT( FARFilter, InitialBackendFilter )
                        //SLATE_ARGUMENT( FAssetData, InitialAssetSelection )
                        //SLATE_ARGUMENT( bool, FilterRecursivelyWithBackendFilter ) //???
                        .OnItemSelectionChanged_Lambda(
                            [](const FContentBrowserItem& SelectedItem, ESelectInfo::Type SelectInfo)
                            {
                                UE_LOG(LogTemp, Warning, TEXT("Item Selected"));
                            }
                        )
                        .OnItemsActivated_Lambda(
                            [](TArrayView<const FContentBrowserItem> ActivatedItems, EAssetTypeActivationMethod::Type ActivationMethod)
                            {
                                UE_LOG(LogTemp, Warning, TEXT("Item Activated"));
                            }
                        )
                        .AssetShowWarningText(LOCTEXT("master-tab.asset-show-warning-text", "No Editable Asset To Show"))
                        .ThumbnailLabel( EThumbnailLabel::NoLabel )
                        .AllowThumbnailHintLabel( false )
                        .bShowPathViewFilters( false )
                        .InitialSourcesData(FSourcesData())
                        .InitialViewType( EAssetViewType::Tile)
                        .InitialThumbnailSize( EThumbnailSize::Medium )
                        .ShowBottomToolbar( false)
                        .ShowViewOptions( false)
                        .AllowThumbnailEditMode( false)
                        .CanShowClasses( false)
                        .CanShowFolders( false)
                        .CanShowReadOnlyFolders( true )
                        .CanShowRealTimeThumbnails( true )
                        .CanShowDevelopersFolder( false )
                        .CanShowFavorites( false )
                        .CanDockCollections( false )
                        .SelectionMode( ESelectionMode::Single)
                        .AllowDragging( false)
                        .AllowFocusOnSync( false)
                        //SLATE_ARGUMENT( bool, FillEmptySpaceInTileView ) //???
                        .ShowPathInColumnView(false)
                        .ShowTypeInColumnView(true)
                        .SortByPathInColumnView(false) //false
                        .ShowTypeInTileView(true)
                        .ForceShowEngineContent(false)
                        .ForceShowPluginContent(false)
                        .ForceHideScrollbar(false)
                        .ShowDisallowedAssetClassAsUnsupportedItems(false)
                        .OnGetCustomSourceAssets_Lambda(
                            [this](const FARFilter& SourceFilter, TArray<FAssetData>& AddedAssets)
                            {
                                for (int i = 0; i < mExtension->SelectableTextures().Num(); i++)
                                {
                                    const FPaintableTexture& paintableTexture = mExtension->SelectableTextures()[i];
                                    UTexture* texture = paintableTexture.Texture;
                                    if (!texture)
                                        return;

                                    AddedAssets.Add(FAssetData(texture));
                                }
                            }
                        )
                ]*/


                + SVerticalBox::Slot()
                .Padding(2)
                .AutoHeight()
                [
                    SNew(SSeparator)
                ]
                //Select painting method (texture based, mesh based...) ----
                + SVerticalBox::Slot()
                .Padding(2)
                .AutoHeight()
                [
                    SNew(STextBlock)
                    .Text(FText::FromString("Stamp alignment"))
                ]
                + SVerticalBox::Slot()
                .Padding(2)
                .AutoHeight()
                [
                    SNew(SComboBox<TSharedPtr<EOdysseyViewportDrawingPaintingAdapterMethod>>)
                    .ButtonStyle(FAppStyle::Get(), "PropertyEditor.AssetComboStyle")
                    .ForegroundColor(FAppStyle::GetColor("PropertyEditor.AssetName.ColorAndOpacity"))
                    .ContentPadding(2.0f)
                    .OptionsSource(&mOptions)
                    .OnGenerateWidget(this, &SOdysseyViewportDrawingEditorMasterTab::GeneratePaintingMethodComboBoxItem)
                    .OnSelectionChanged(this, &SOdysseyViewportDrawingEditorMasterTab::ChangeSelectionPaintingMethodComboBoxItem)
                    [
                        SNew(STextBlock)
                        .TextStyle(FAppStyle::Get(), "PropertyEditor.AssetClass")
                        .Font(FAppStyle::GetFontStyle("PropertyWindow.NormalFont"))
                        .Text_Lambda([this] { return SOdysseyViewportDrawingEditorMasterTab::GetMethodAsText(mExtension->PaintingAdapterMethod());})
                    ]
                ]
            ]
        ]
    ];
}

TSharedRef<SWidget>
SOdysseyViewportDrawingEditorMasterTab::OnGetMenuContent()
{
    mMeshSelectorVerticalBox = SNew( SVerticalBox );

    for( int i = 0; i < mExtension->SelectableComponents().Num(); i++ )
    {
        mMeshSelectorVerticalBox->AddSlot()
            .Padding(2)
            .AutoHeight()
            [
                SNew( SButton )
                    .ButtonStyle( FAppStyle::Get(), "HoverHintOnly" )
                    .ForegroundColor( FAppStyle::GetColor("PropertyEditor.AssetName.ColorAndOpacity"))
                    .Text( FText::FromString( mExtension->SelectableComponents()[i]->GetName() ) )
                    .OnClicked( this, &SOdysseyViewportDrawingEditorMasterTab::OnMeshComponentChanged, mExtension->SelectableComponents()[i]->GetName() )
            ];
    }

    return mMeshSelectorVerticalBox->AsShared();
}

void
SOdysseyViewportDrawingEditorMasterTab::OnMenuClosed( bool iOpen)
{
    if ( iOpen == false )
        mMeshSelectComboButton->SetMenuContent(SNullWidget::NullWidget);
}


FText
SOdysseyViewportDrawingEditorMasterTab::CreateTextMeshSelector() const
{
    if( mExtension->Component() )
    {
        return FText::FromString( mExtension->Component()->GetName() );
    }
    return FText::FromString( "None" );
}

TSharedRef<SWidget>
SOdysseyViewportDrawingEditorMasterTab::CreateMeshComponentMenuWidget()
{
    mMeshSelectComboButton = SNew(SComboButton)
        .ButtonStyle( FAppStyle::Get(), "PropertyEditor.AssetComboStyle" )
        .ForegroundColor( FAppStyle::GetColor("PropertyEditor.AssetName.ColorAndOpacity") )
        .OnGetMenuContent( this, &SOdysseyViewportDrawingEditorMasterTab::OnGetMenuContent )
        .OnMenuOpenChanged( this, &SOdysseyViewportDrawingEditorMasterTab::OnMenuClosed )
        .ContentPadding(2.0f)
        .ButtonContent()
        [
            SNew(SHorizontalBox)
                +SHorizontalBox::Slot()
                .FillWidth(1)
                .VAlign(VAlign_Center)
                [
                    SNew( STextBlock )
                       .Text( this, &SOdysseyViewportDrawingEditorMasterTab::CreateTextMeshSelector )
                       .TextStyle( FAppStyle::Get(), "PropertyEditor.AssetClass" )
                       .Font( FAppStyle::GetFontStyle( "PropertyWindow.NormalFont" ) )
                ]
        ];

    TSharedRef< SWidget > widget =

    SNew( SVerticalBox )
        + SVerticalBox::Slot()
        .Padding( 2 )
        .AutoHeight()
        [
            SNew(STextBlock)
            .Text(LOCTEXT("master-tab.select-mesh", "Select Mesh"))
        ]
        + SVerticalBox::Slot()
        .Padding( 2 )
        .AutoHeight()
        [
            mMeshSelectComboButton->AsShared()
        ];

    return widget;
}

TSharedRef<SWidget> SOdysseyViewportDrawingEditorMasterTab::GeneratePaintingMethodComboBoxItem(TSharedPtr<EOdysseyViewportDrawingPaintingAdapterMethod> iItem)
{
    return  SNew(STextBlock)
        .Text(SOdysseyViewportDrawingEditorMasterTab::GetMethodAsText(*(iItem.Get())))
        .ToolTipText(SOdysseyViewportDrawingEditorMasterTab::GetTooltipAsText(*(iItem.Get())));
}

void SOdysseyViewportDrawingEditorMasterTab::ChangeSelectionPaintingMethodComboBoxItem(TSharedPtr<EOdysseyViewportDrawingPaintingAdapterMethod> iNewSelection, ESelectInfo::Type iSelectInfo)
{
    mExtension->SetPaintingAdapterMethod(*(iNewSelection.Get()));
}

FText SOdysseyViewportDrawingEditorMasterTab::GetMethodAsText(EOdysseyViewportDrawingPaintingAdapterMethod iMethod)
{
    switch (iMethod)
    {
        case OdysseyTextureBased:        return LOCTEXT("master-tab.painting-adapter-method.texture-based.name", "Texture UV");
        case OdysseyMeshBasedPlanar:     return LOCTEXT("master-tab.painting-adapter-method.mesh-based-planar.name", "Mesh (planar drawing)");
        case OdysseyMeshBasedSphere:     return LOCTEXT("master-tab.painting-adapter-method.mesh-based-sphere.name", "Mesh (sphere drawing)");
        case OdysseyScreenBased:         return LOCTEXT("master-tab.painting-adapter-method.screen-based.name", "Screen");
    }
    return LOCTEXT("master-tab.painting-adapter-method.invalid.name", "Invalid");
}

FText SOdysseyViewportDrawingEditorMasterTab::GetTooltipAsText(EOdysseyViewportDrawingPaintingAdapterMethod iMethod)
{
    switch (iMethod)
    {
        case OdysseyTextureBased:        return LOCTEXT("master-tab.painting-adapter-method.texture-based.tooltip", "Stamp will be based on texture (2D) UVs size and orientation");
        case OdysseyMeshBasedPlanar:     return LOCTEXT("master-tab.painting-adapter-method.mesh-based-planar.tooltip", "Stamp will be based on mesh (3D) size and orientation. The Z axis is normal to hit plane on the mesh and will follow each edge");
        case OdysseyMeshBasedSphere:     return LOCTEXT("master-tab.painting-adapter-method.mesh-based-sphere.tooltip", "Stamp will be based on mesh (3D) size and orientation. The stamp will be interpreted as a sphere and applied to the mesh");
        case OdysseyScreenBased:         return LOCTEXT("master-tab.painting-adapter-method.screen-based.tooltip", "Stamp will be based on viewport screen view. Its size and orientation depend on the position of the view.");
    }
    return LOCTEXT("master-tab.painting-adapter-method.invalid.tooltip", "Invalid");
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Widget Getters

FString
SOdysseyViewportDrawingEditorMasterTab::PaintActorPath() const
{
    if ( !mExtension->Actor() )
        return FString();

    return mExtension->Actor()->GetPathName();
}

FString
SOdysseyViewportDrawingEditorMasterTab::PaintMaterialPath() const
{
    if ( !mExtension->Material() )
        return FString();

    return mExtension->Material()->GetPathName();
}

FString
SOdysseyViewportDrawingEditorMasterTab::PaintTexturePath() const
{
    if( !mExtension->Texture() )
        return FString();

    return mExtension->Texture()->GetPathName();
}

bool
SOdysseyViewportDrawingEditorMasterTab::ShouldFilterMaterialAsset(const FAssetData& iAssetData) const
{
    TArray<UMaterialInterface*> materialsArray;
    mExtension->SelectableMaterials( materialsArray );
    return !(materialsArray.ContainsByPredicate([=](const UMaterialInterface* iMaterial) { return (iAssetData.FastGetAsset() != nullptr && iMaterial->GetFullName() == iAssetData.FastGetAsset()->GetFullName() ); }));
}

bool
SOdysseyViewportDrawingEditorMasterTab::ShouldFilterTextureAsset(const FAssetData& iAssetData) const
{
    return !(mExtension->SelectableTextures().ContainsByPredicate([=, this](const FPaintableTexture& iTexture) { return (iAssetData.FastGetAsset() != nullptr && iTexture.Texture->GetFullName() == iAssetData.FastGetAsset()->GetFullName() && DoesMaterialUseTexture( mExtension->Material(), iTexture.Texture ) ); }));
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------- Event Listeners

void
SOdysseyViewportDrawingEditorMasterTab::OnActorChanged(const FAssetData& iAssetData)
{
    AActor* actor = Cast<AActor>(iAssetData.GetAsset());

    if ( actor )
    {
        GEditor->SelectNone( false, true, false );
        GUnrealEd->SelectActor(actor, true, true, true);
    }
}


FReply
SOdysseyViewportDrawingEditorMasterTab::OnMeshComponentChanged(const FString iName)
{
    if( mExtension->Component()->GetName() == iName ) return FReply::Handled();

    for( int i = 0; i < mExtension->SelectableComponents().Num(); i++ )
    {
        if( mExtension->SelectableComponents()[i]->GetName() == iName )
        {
            mExtension->SetComponent( mExtension->SelectableComponents()[i] );
        }
    }

    //TODO: use the right max size from current adapter
    mExtension->GetEditor()->GetRasterDrawingTool()->SetBaseSize(mExtension->GetMeshComponentMaxSize());
    //mExtension->GetGUI()->GetTopTab()->SetMeshMaxSize( mExtension->GetMeshComponentMaxSize() );

    return FReply::Handled();
}

void
SOdysseyViewportDrawingEditorMasterTab::OnMaterialChanged(const FAssetData& iAssetData)
{
    UMaterialInterface* material = Cast<UMaterialInterface>(iAssetData.GetAsset());

    if ( material )
    {
        mExtension->SetMaterial( material );
    }

}

void
SOdysseyViewportDrawingEditorMasterTab::OnTextureChanged(const FAssetData& iAssetData)
{
    UTexture* texture = Cast<UTexture>(iAssetData.GetAsset());

    if ( texture )
    {
        mExtension->SetTexture( texture, true );
    }
}

#undef LOCTEXT_NAMESPACE

