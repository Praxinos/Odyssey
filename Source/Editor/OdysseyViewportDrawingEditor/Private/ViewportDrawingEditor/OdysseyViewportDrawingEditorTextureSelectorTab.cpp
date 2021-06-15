// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyViewportDrawingEditorTextureSelectorTab.h"

#include "Color/SOdysseyColorSliders.h"
#include "OdysseyPainterEditor.h"

#define LOCTEXT_NAMESPACE "OdysseyViewportDrawingEditorTextureSelectorTab"

/////////////////////////////////////////////////////
// FOdysseyViewportDrawingEditorTextureSelectorTab
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyViewportDrawingEditorTextureSelectorTab::~FOdysseyViewportDrawingEditorTextureSelectorTab()
{
}

FOdysseyViewportDrawingEditorTextureSelectorTab::FOdysseyViewportDrawingEditorTextureSelectorTab(FOdysseyViewportDrawingEditor* iEditor)
    : FOdysseyEditorTab(TEXT("OdysseyPainterEditor_TextureSelector"),
                            LOCTEXT( "OdysseyViewportDrawingEditorTextureSelectorTab", "Texture Selector" ),
                            FSlateIcon( "OdysseyStyle", "PainterEditor.TextureSelector_16" ))
    , mEditor(iEditor)
{
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------- FOdysseyEditorTab interface

TSharedPtr<SWidget>
FOdysseyViewportDrawingEditorTextureSelectorTab::CreateWidget()
{
    return
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
                        .Text(FText::FromString("Select Mesh"))
                    ]
                    + SVerticalBox::Slot()
                    .Padding( 2 )
                    .AutoHeight()
                    .Expose( mMeshComponentSelectionMenu )
                    [
                        SNullWidget::NullWidget
                    ]
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
                            .AllowedClass(UMaterial::StaticClass())
                            .ObjectPath(this, &FOdysseyViewportDrawingEditorTextureSelectorTab::PaintMaterialPath)
                            .OnObjectChanged(FOnSetObject::CreateRaw(this, &FOdysseyViewportDrawingEditorTextureSelectorTab::OnMaterialChanged))
                            .OnShouldFilterAsset(FOnShouldFilterAsset::CreateRaw(this, &FOdysseyViewportDrawingEditorTextureSelectorTab::ShouldFilterMaterialAsset))
                            .AllowClear(true)
                            .DisplayUseSelected(true)
                            .DisplayBrowse(true)
                            .EnableContentPicker(true)
                            .DisplayCompactSize(true)
                            .DisplayThumbnail(true)
                            .ThumbnailSizeOverride(FIntPoint(30, 30))
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
                                .AllowedClass(UTexture2D::StaticClass())
                                .ObjectPath(this, &FOdysseyViewportDrawingEditorTextureSelectorTab::PaintTexturePath)
                                .OnObjectChanged(FOnSetObject::CreateRaw(this, &FOdysseyViewportDrawingEditorTextureSelectorTab::OnTextureChanged))
                                .OnShouldFilterAsset(FOnShouldFilterAsset::CreateRaw(this, &FOdysseyViewportDrawingEditorTextureSelectorTab::ShouldFilterTextureAsset))
                                .AllowClear(true)
                                .DisplayUseSelected(true)
                                .DisplayBrowse(true)
                                .EnableContentPicker(true)
                                .DisplayCompactSize(true)
                                .DisplayThumbnail(true)
                                .ThumbnailSizeOverride(FIntPoint(30, 30))
                        ]
            ];
}

TSharedRef<SWidget>
FOdysseyViewportDrawingEditorTextureSelectorTab::CreateMeshComponentMenuWidget()
{
    FMenuBuilder menuBuilder(false, NULL);

    menuBuilder.BeginSection("Select Mesh Component");

    FUIAction changeMeshAction;
    changeMeshAction.ExecuteAction.BindSP( this, &FOdysseyViewportDrawingEditorTextureSelectorTab::OnMeshComponentChanged, FString() );
    changeMeshAction.GetActionCheckState.BindSP( this, &FOdysseyViewportDrawingEditorTextureSelectorTab::GetMeshComponentCheckState, FString() );
    
    menuBuilder.AddMenuEntry(
        LOCTEXT("SelecMeshComponentToggle", "None"),
        LOCTEXT("SelecMeshComponentToggle_Tooltip", "Toggles selection of the mesh components."),
        FSlateIcon(),
        changeMeshAction,
        NAME_None,
        EUserInterfaceActionType::RadioButton
    );

    int32 maxMeshComponent = mEditor->SelectableComponents().Num();
    if( maxMeshComponent > 0)
        menuBuilder.AddMenuSeparator();

    for(int i = 0; i < maxMeshComponent; ++i)
    {
        FUIAction menuAction;
        menuAction.ExecuteAction.BindSP( this, &FOdysseyViewportDrawingEditorTextureSelectorTab::OnMeshComponentChanged, mEditor->SelectableComponents()[i]->GetFName().ToString() );
        menuAction.GetActionCheckState.BindSP( this, &FOdysseyViewportDrawingEditorTextureSelectorTab::GetMeshComponentCheckState, mEditor->SelectableComponents()[i]->GetFName().ToString() );

        menuBuilder.AddMenuEntry(
            FText::Format(LOCTEXT("SelectedMeshComponent", "{0}"), FText::FromName( mEditor->SelectableComponents()[i]->GetFName() ) ),
            FText::Format(LOCTEXT("SelectedMeshComponent_ToolTip", "Set {0} as Mesh Component"), FText::FromName( mEditor->SelectableComponents()[i]->GetFName() ) ),
            FSlateIcon(),
            menuAction,
            NAME_None,
            EUserInterfaceActionType::RadioButton
        );
    }

    menuBuilder.EndSection();


    TSharedRef< SWidget > widget =

    SNew( SVerticalBox )
        + SVerticalBox::Slot()
        .Padding( 2 )
        .AutoHeight()
        [
            SNew(STextBlock)
            .Text(LOCTEXT("Select Mesh", "Select Mesh"))
        ]
        + SVerticalBox::Slot()
        .Padding( 2 )
        .AutoHeight()
        [
            menuBuilder.MakeWidget()
        ];
        
    return widget;
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Widget Getters

FString
FOdysseyViewportDrawingEditorTextureSelectorTab::PaintMaterialPath() const
{
    if ( !mEditor->Material() )
        return FString();

    return mEditor->Material()->GetPathName();
}

FString
FOdysseyViewportDrawingEditorTextureSelectorTab::PaintTexturePath() const
{
    if( !mEditor->Texture() )
        return FString();

    return mEditor->Texture()->GetPathName();
}

bool
FOdysseyViewportDrawingEditorTextureSelectorTab::ShouldFilterTextureAsset(const FAssetData& iAssetData)
{
    mMeshComponentSelectionMenu->DetachWidget();
    mMeshComponentSelectionMenu->AttachWidget( CreateMeshComponentMenuWidget() );
    return !(mEditor->SelectableTextures().ContainsByPredicate([=](const FPaintableTexture& iTexture) { return iTexture.Texture->GetFullName() == iAssetData.GetFullName(); }));
}

bool
FOdysseyViewportDrawingEditorTextureSelectorTab::ShouldFilterMaterialAsset(const FAssetData& iAssetData) const
{
    TArray<UMaterialInterface*> materialsArray;
    mEditor->SelectableMaterials( materialsArray );
    return !(materialsArray.ContainsByPredicate([=](const UMaterialInterface* iMaterial) { return iMaterial->GetFullName() == iAssetData.GetFullName(); }));
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------- Event Listeners

void
FOdysseyViewportDrawingEditorTextureSelectorTab::OnTextureChanged(const FAssetData& iAssetData)
{

    UTexture2D* texture = Cast<UTexture2D>(iAssetData.GetAsset());

    if ( texture )
    {
        //check
        UAssetEditorSubsystem* AssetEditorSubsystem = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>();
        if (texture != mEditor->Texture() && AssetEditorSubsystem->FindEditorForAsset(texture, true) != nullptr)
        {
            FText Title = LOCTEXT("TitleSelectedTextureAlreadyOpenedTitle", "Selected Texture Already Opened");
            FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("SelectedTextureAlreadyOpened", "The selected texture is already opened in an other editor. Please close the editor before selecting this texture."), &Title);
            return;
        }

        mEditor->SetTexture( texture );
    }
}

void
FOdysseyViewportDrawingEditorTextureSelectorTab::OnMaterialChanged(const FAssetData& iAssetData)
{
    UMaterialInterface* material = Cast<UMaterialInterface>(iAssetData.GetAsset());

    if ( material )
    {
        mEditor->SetMaterial( material );
    }
}

void
FOdysseyViewportDrawingEditorTextureSelectorTab::OnMeshComponentChanged(const FString iName)
{
    UE_LOG(LogTemp, Display, TEXT("MeshChanged"));
}

ECheckBoxState
FOdysseyViewportDrawingEditorTextureSelectorTab::GetMeshComponentCheckState(const FString iName)
{
    FString string = mEditor->Component() ? mEditor->Component()->GetName() : FString();
    return ( string == iName ? ECheckBoxState::Checked : ECheckBoxState::Unchecked );
}

#undef LOCTEXT_NAMESPACE

