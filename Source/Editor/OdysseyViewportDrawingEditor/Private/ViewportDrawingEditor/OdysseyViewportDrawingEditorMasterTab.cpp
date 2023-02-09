// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyViewportDrawingEditorMasterTab.h"

#include "Color/SOdysseyColorSliders.h"
#include "Editor/UnrealEdEngine.h"
#include "OdysseyPainterEditor.h"
#include "UnrealEdGlobals.h"
#include "Widgets/Layout/SSeparator.h"
#include "EditorStyleSet.h"

#define LOCTEXT_NAMESPACE "OdysseyViewportDrawingEditorMasterTab"

/////////////////////////////////////////////////////
// FOdysseyViewportDrawingEditorMasterTab
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyViewportDrawingEditorMasterTab::~FOdysseyViewportDrawingEditorMasterTab()
{
}

FOdysseyViewportDrawingEditorMasterTab::FOdysseyViewportDrawingEditorMasterTab(FOdysseyViewportDrawingEditor* iEditor)
    : FOdysseyEditorTab(TEXT("OdysseyPainterEditor_TextureSelector"),
                            LOCTEXT( "OdysseyViewportDrawingEditorMasterTab", "Texture Selector" ),
                            FSlateIcon( "OdysseyStyle", "PainterEditor.TextureSelector_16" ))
    , mEditor(iEditor)
{
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------- FOdysseyEditorTab interface

TSharedPtr<SWidget>
FOdysseyViewportDrawingEditorMasterTab::CreateWidget()
{
    mOptions.Empty();
    mOptions.Add(MakeShared< EOdysseyViewportDrawingPaintingAdapterMethod >(EOdysseyViewportDrawingPaintingAdapterMethod::OdysseyTextureBased));
    mOptions.Add(MakeShared< EOdysseyViewportDrawingPaintingAdapterMethod >(EOdysseyViewportDrawingPaintingAdapterMethod::OdysseyMeshBased));
    mOptions.Add(MakeShared< EOdysseyViewportDrawingPaintingAdapterMethod >(EOdysseyViewportDrawingPaintingAdapterMethod::OdysseyScreenBased));

    mThumbnailPool = MakeShareable( new FAssetThumbnailPool( 50 ) );

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
                            .ObjectPath(this, &FOdysseyViewportDrawingEditorMasterTab::PaintActorPath)
                            .OnObjectChanged(FOnSetObject::CreateRaw(this, &FOdysseyViewportDrawingEditorMasterTab::OnActorChanged))
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
                            .AllowedClass(UMaterialInterface::StaticClass())
                            .ObjectPath(this, &FOdysseyViewportDrawingEditorMasterTab::PaintMaterialPath)
                            .OnObjectChanged(FOnSetObject::CreateRaw(this, &FOdysseyViewportDrawingEditorMasterTab::OnMaterialChanged))
                            .OnShouldFilterAsset(FOnShouldFilterAsset::CreateRaw(this, &FOdysseyViewportDrawingEditorMasterTab::ShouldFilterMaterialAsset))
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
                                .AllowedClass(UTexture2D::StaticClass())
                                .ObjectPath(this, &FOdysseyViewportDrawingEditorMasterTab::PaintTexturePath)
                                .OnObjectChanged(FOnSetObject::CreateRaw(this, &FOdysseyViewportDrawingEditorMasterTab::OnTextureChanged))
                                .OnShouldFilterAsset(FOnShouldFilterAsset::CreateRaw(this, &FOdysseyViewportDrawingEditorMasterTab::ShouldFilterTextureAsset))
                                .DisplayBrowse(true)
                                .EnableContentPicker(true)
                                .DisplayCompactSize(true)
                                .DisplayThumbnail(true)
                                .ThumbnailSizeOverride(FIntPoint(70, 70))
                                .ThumbnailPool( mThumbnailPool )
                        ]
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
                            .Text(FText::FromString("Select painting method"))
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
                            .OnGenerateWidget(this, &FOdysseyViewportDrawingEditorMasterTab::GeneratePaintingMethodComboBoxItem)
                            .OnSelectionChanged(this, &FOdysseyViewportDrawingEditorMasterTab::ChangeSelectionPaintingMethodComboBoxItem)
                            [
                                SNew(STextBlock)
                                .TextStyle(FAppStyle::Get(), "PropertyEditor.AssetClass")
                                .Font(FAppStyle::GetFontStyle("PropertyWindow.NormalFont"))
                                .Text_Lambda([=] { return FOdysseyViewportDrawingEditorMasterTab::GetMethodAsText(mEditor->PaintingAdapterMethod());})
                            ]
                        ]
                    //---
            ];
}

TSharedRef<SWidget>
FOdysseyViewportDrawingEditorMasterTab::OnGetMenuContent()
{
    mMeshSelectorVerticalBox = SNew( SVerticalBox );

    for( int i = 0; i < mEditor->SelectableComponents().Num(); i++ )
    {
        mMeshSelectorVerticalBox->AddSlot()
            .Padding(2)
            .AutoHeight()
            [
                SNew( SButton )
                    .ButtonStyle( FAppStyle::Get(), "HoverHintOnly" )
                    .ForegroundColor( FAppStyle::GetColor("PropertyEditor.AssetName.ColorAndOpacity"))
                    .Text( FText::FromString( mEditor->SelectableComponents()[i]->GetName() ) )
                    .OnClicked( this, &FOdysseyViewportDrawingEditorMasterTab::OnMeshComponentChanged, mEditor->SelectableComponents()[i]->GetName() )
            ];
    }

    return mMeshSelectorVerticalBox->AsShared();
}

void
FOdysseyViewportDrawingEditorMasterTab::OnMenuClosed( bool iOpen)
{
    if ( iOpen == false )
        mMeshSelectComboButton->SetMenuContent(SNullWidget::NullWidget);
}


FText
FOdysseyViewportDrawingEditorMasterTab::CreateTextMeshSelector() const
{
    if( mEditor->Component() )
    {
        return FText::FromString( mEditor->Component()->GetName() );
    }
    return FText::FromString( "None" );
}

TSharedRef<SWidget>
FOdysseyViewportDrawingEditorMasterTab::CreateMeshComponentMenuWidget()
{
    mMeshSelectComboButton = SNew(SComboButton)
        .ButtonStyle( FAppStyle::Get(), "PropertyEditor.AssetComboStyle" )
        .ForegroundColor( FAppStyle::GetColor("PropertyEditor.AssetName.ColorAndOpacity") )
        .OnGetMenuContent( this, &FOdysseyViewportDrawingEditorMasterTab::OnGetMenuContent )
        .OnMenuOpenChanged( this, &FOdysseyViewportDrawingEditorMasterTab::OnMenuClosed )
        .ContentPadding(2.0f)
        .ButtonContent()
        [
            SNew(SHorizontalBox)
                +SHorizontalBox::Slot()
                .FillWidth(1)
                .VAlign(VAlign_Center)
                [
                    SNew( STextBlock )
                       .Text( this, &FOdysseyViewportDrawingEditorMasterTab::CreateTextMeshSelector )
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

TSharedRef<SWidget> FOdysseyViewportDrawingEditorMasterTab::GeneratePaintingMethodComboBoxItem(TSharedPtr<EOdysseyViewportDrawingPaintingAdapterMethod> iItem)
{
    return  SNew(STextBlock)
        .Text(FOdysseyViewportDrawingEditorMasterTab::GetMethodAsText(*(iItem.Get())));
}

void FOdysseyViewportDrawingEditorMasterTab::ChangeSelectionPaintingMethodComboBoxItem(TSharedPtr<EOdysseyViewportDrawingPaintingAdapterMethod> iNewSelection, ESelectInfo::Type iSelectInfo)
{
    mEditor->SetPaintingAdapterMethod(*(iNewSelection.Get()));
}

FText FOdysseyViewportDrawingEditorMasterTab::GetMethodAsText(EOdysseyViewportDrawingPaintingAdapterMethod iMethod)
{
    switch (iMethod)
    {
        case OdysseyTextureBased:        return LOCTEXT("OdysseyTextureBased", "Texture Based");
        case OdysseyMeshBased:           return LOCTEXT("OdysseyMeshBased", "Mesh Based");
        case OdysseyScreenBased:         return LOCTEXT("OdysseyScreenBased", "Screen Based");
    }

    return LOCTEXT("OdysseyInvalid", "Invalid");
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Widget Getters

FString
FOdysseyViewportDrawingEditorMasterTab::PaintActorPath() const
{
    if ( !mEditor->Actor() )
        return FString();

    return mEditor->Actor()->GetPathName();
}

FString
FOdysseyViewportDrawingEditorMasterTab::PaintMaterialPath() const
{
    if ( !mEditor->Material() )
        return FString();

    return mEditor->Material()->GetPathName();
}

FString
FOdysseyViewportDrawingEditorMasterTab::PaintTexturePath() const
{
    if( !mEditor->Texture() )
        return FString();

    return mEditor->Texture()->GetPathName();
}

bool
FOdysseyViewportDrawingEditorMasterTab::ShouldFilterMaterialAsset(const FAssetData& iAssetData) const
{
    TArray<UMaterialInterface*> materialsArray;
    mEditor->SelectableMaterials( materialsArray );
    return !(materialsArray.ContainsByPredicate([=](const UMaterialInterface* iMaterial) { return iMaterial->GetFullName() == iAssetData.GetFullName(); }));
}

bool
FOdysseyViewportDrawingEditorMasterTab::ShouldFilterTextureAsset(const FAssetData& iAssetData) const
{
    return !(mEditor->SelectableTextures().ContainsByPredicate([=](const FPaintableTexture& iTexture) { return iTexture.Texture->GetFullName() == iAssetData.GetFullName(); }));
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------- Event Listeners

void
FOdysseyViewportDrawingEditorMasterTab::OnActorChanged(const FAssetData& iAssetData)
{
    AActor* actor = Cast<AActor>(iAssetData.GetAsset());

    if ( actor )
    {
        GEditor->SelectNone( false, true, false );
        GUnrealEd->SelectActor(actor, true, true, true);
    }
}


FReply
FOdysseyViewportDrawingEditorMasterTab::OnMeshComponentChanged(const FString iName)
{
    if( mEditor->Component()->GetName() == iName ) return FReply::Handled();

    for( int i = 0; i < mEditor->SelectableComponents().Num(); i++ )
    {
        if( mEditor->SelectableComponents()[i]->GetName() == iName )
        {
            mEditor->SetComponent( mEditor->SelectableComponents()[i] );
        }
    }

    mEditor->GetGUI()->GetTopTab()->SetMeshMaxSize( mEditor->GetMeshComponentMaxSize() );
    return FReply::Handled();
}

void
FOdysseyViewportDrawingEditorMasterTab::OnMaterialChanged(const FAssetData& iAssetData)
{
    UMaterialInterface* material = Cast<UMaterialInterface>(iAssetData.GetAsset());

    if ( material )
    {
        mEditor->SetMaterial( material );
    }
    
}

void
FOdysseyViewportDrawingEditorMasterTab::OnTextureChanged(const FAssetData& iAssetData)
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

