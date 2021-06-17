// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "Color/SOdysseyColorSliders.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyViewportDrawingEditorTextureSelectorTab.h"

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
                        .Text(FText::FromString("Select Actor"))
                    ]
                    + SVerticalBox::Slot()
                        .Padding(2)
                        .AutoHeight()
                        [
                            SNew(SObjectPropertyEntryBox)
                            .AllowedClass(AActor::StaticClass())
                            .ObjectPath(this, &FOdysseyViewportDrawingEditorTextureSelectorTab::PaintActorPath)
                            .OnObjectChanged(FOnSetObject::CreateRaw(this, &FOdysseyViewportDrawingEditorTextureSelectorTab::OnActorChanged))
                            .AllowClear(true)
                            .DisplayUseSelected(true)
                            .DisplayBrowse(true)
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
FOdysseyViewportDrawingEditorTextureSelectorTab::OnGetMenuContent()
{
    mMeshSelectorVerticalBox = SNew( SVerticalBox );

    for( int i = 0; i < mEditor->SelectableComponents().Num(); i++ )
    {
        mMeshSelectorVerticalBox->AddSlot()
            .Padding(2)
            .AutoHeight()
            [
                SNew( SButton )
                    .ButtonStyle( FEditorStyle::Get(), "HoverHintOnly" )
                    .ForegroundColor(FEditorStyle::GetColor("PropertyEditor.AssetName.ColorAndOpacity"))
                    .Text( FText::FromString( mEditor->SelectableComponents()[i]->GetName() ) )
                    .OnClicked( this, &FOdysseyViewportDrawingEditorTextureSelectorTab::OnMeshComponentChanged, mEditor->SelectableComponents()[i]->GetName() )
            ];
    }

    return mMeshSelectorVerticalBox->AsShared();
}

void
FOdysseyViewportDrawingEditorTextureSelectorTab::OnMenuClosed( bool iOpen)
{
    if ( iOpen == false )
        mMeshSelectComboButton->SetMenuContent(SNullWidget::NullWidget);
}


TSharedRef<SWidget>
FOdysseyViewportDrawingEditorTextureSelectorTab::GenerateMeshSelectorComboButtonItem( TSharedPtr<FString> iItem )
{
    return SNew( STextBlock )
                .Text( FText::FromString( *( iItem.Get() ) ) )
                .TextStyle( FEditorStyle::Get(), "PropertyEditor.AssetClass" )
                .Font( FEditorStyle::GetFontStyle( "PropertyWindow.NormalFont" ) );
}

FText
FOdysseyViewportDrawingEditorTextureSelectorTab::CreateTextMeshSelector() const
{
    if( mEditor->Component() )
    {
        return FText::FromString( mEditor->Component()->GetName() );
    }
    return FText::FromString( "None" );
}

TSharedRef<SWidget>
FOdysseyViewportDrawingEditorTextureSelectorTab::CreateMeshComponentMenuWidget()
{
mMeshSelectComboButton = SNew(SComboButton)
        .ButtonStyle( FEditorStyle::Get(), "PropertyEditor.AssetComboStyle" )
        .ForegroundColor(FEditorStyle::GetColor("PropertyEditor.AssetName.ColorAndOpacity"))
        .OnGetMenuContent( this, &FOdysseyViewportDrawingEditorTextureSelectorTab::OnGetMenuContent )
        .OnMenuOpenChanged( this, &FOdysseyViewportDrawingEditorTextureSelectorTab::OnMenuClosed )
        .ContentPadding(2.0f)
        .ButtonContent()
        [
            SNew(SHorizontalBox)
                +SHorizontalBox::Slot()
                .FillWidth(1)
                .VAlign(VAlign_Center)
                [
                    SNew( STextBlock )
                       .Text( this, &FOdysseyViewportDrawingEditorTextureSelectorTab::CreateTextMeshSelector )
                       .TextStyle( FEditorStyle::Get(), "PropertyEditor.AssetClass" )
                       .Font( FEditorStyle::GetFontStyle( "PropertyWindow.NormalFont" ) )
                ]
        ];

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
            mMeshSelectComboButton->AsShared()
        ];
        
    return widget;
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Widget Getters

FString
FOdysseyViewportDrawingEditorTextureSelectorTab::PaintActorPath() const
{
    if ( !mEditor->Actor() )
        return FString();

    return mEditor->Actor()->GetPathName();
}

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
FOdysseyViewportDrawingEditorTextureSelectorTab::ShouldFilterMaterialAsset(const FAssetData& iAssetData) const
{
    TArray<UMaterialInterface*> materialsArray;
    mEditor->SelectableMaterials( materialsArray );
    return !(materialsArray.ContainsByPredicate([=](const UMaterialInterface* iMaterial) { return iMaterial->GetFullName() == iAssetData.GetFullName(); }));
}

bool
FOdysseyViewportDrawingEditorTextureSelectorTab::ShouldFilterTextureAsset(const FAssetData& iAssetData) const
{
    return !(mEditor->SelectableTextures().ContainsByPredicate([=](const FPaintableTexture& iTexture) { return iTexture.Texture->GetFullName() == iAssetData.GetFullName(); }));
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------- Event Listeners

void
FOdysseyViewportDrawingEditorTextureSelectorTab::OnActorChanged(const FAssetData& iAssetData)
{
    AActor* actor = Cast<AActor>(iAssetData.GetAsset());

    if ( actor )
    {
        mEditor->SetActor( actor );
    }
}


FReply
FOdysseyViewportDrawingEditorTextureSelectorTab::OnMeshComponentChanged(const FString iName)
{
    if( mEditor->Component()->GetName() == iName ) return FReply::Handled();

    for( int i = 0; i < mEditor->SelectableComponents().Num(); i++ )
    {
        if( mEditor->SelectableComponents()[i]->GetName() == iName )
        {
            mEditor->SetComponent( mEditor->SelectableComponents()[i] );
        }
    }
    return FReply::Handled();
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
FOdysseyViewportDrawingEditorTextureSelectorTab::OnTextureChanged(const FAssetData& iAssetData)
{
    UTexture2D* texture = Cast<UTexture2D>(iAssetData.GetAsset());

    if ( texture )
    {
        //check if texture is already edited by an other editor
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

#undef LOCTEXT_NAMESPACE

