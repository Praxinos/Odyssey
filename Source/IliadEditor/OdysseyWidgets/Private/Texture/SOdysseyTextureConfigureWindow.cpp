// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Texture/SOdysseyTextureConfigureWindow.h"

#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SNumericEntryBox.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SUniformGridPanel.h"
#include "Widgets/Layout/SSeparator.h"
#include "IStructureDetailsView.h"
#include "OdysseyPixelFormat.h"
#include "ULISLoaderModule.h"
#include "LayerStack/OdysseyTextureLayerImageRaster.h"
#include "OdysseySurfaceTexture2DEditable.h"
#include "OdysseyTextureLayerStackUserData.h"
#include "OdysseyRasterBlockMutator.h"
#include <ULIS>

/////////////////////////////////////////////////////
// Defines
#define LOCTEXT_NAMESPACE "Texture"

#define MAX_CANVAS_SIZE 8192
#define MIN_CANVAS_SIZE 1

//---

ETextureSourceFormat
FOdysseyTextureConfiguration::TextureSourceFormat() const
{
    switch(Format)
    {
        case EOdysseyTextureSourceFormat::kG8: return TSF_G8;
        case EOdysseyTextureSourceFormat::kG16: return TSF_G16;
        case EOdysseyTextureSourceFormat::kBGRA8: return TSF_BGRA8;
        case EOdysseyTextureSourceFormat::kBGRE8: return TSF_BGRE8;
        case EOdysseyTextureSourceFormat::kRGBA16: return TSF_RGBA16;
        case EOdysseyTextureSourceFormat::kRGBA16F: return TSF_RGBA16F;
        case EOdysseyTextureSourceFormat::kCustom: return CustomFormat;
    }

    check(false); //should not be called
    return TSF_BGRA8;
}

FLinearColor
FOdysseyTextureConfiguration::GetBackgroundColor() const
{
    switch(BackgroundColor)
    {
        case EOdysseyTextureBackgroundColor::kTransparent:  return FLinearColor( 0.f, 0.f, 0.f, 0.f );
        case EOdysseyTextureBackgroundColor::kWhite:        return FLinearColor( 1.f, 1.f, 1.f );
        case EOdysseyTextureBackgroundColor::kNormal:       return FLinearColor( .5f, .5f, 1.f );
        default: break;
    }

    check(false); //should not be called
    return FLinearColor();
}

UTexture2D*
FOdysseyTextureConfiguration::CreateTexture(UObject* iParent, FName iName, EObjectFlags iFlags) const
{
    int textureWidth = Width;
    int textureHeight = Height;
    ETextureSourceFormat textureFormat = TextureSourceFormat();

    //IsImageInfoValid() in ImageCore.h allows use of sRGB only on G8 and BGRA8 textures
    bool useSRGB = textureFormat == ETextureSourceFormat::TSF_BGRA8 || textureFormat == ETextureSourceFormat::TSF_G8;
    FString defaultName = Name.ToString();
    FLinearColor backgroundColor = GetBackgroundColor();
    EOdysseyTextureDefaultLayerType defaultLayerType = LayerType;

    // Init internal data
    ::ULIS::FBlock block( textureWidth, textureHeight, ULISFormatForTextureSourceFormat(textureFormat) );
    ::ULIS::FColor color( ::ULIS::FColor::FromRGBAF( backgroundColor.R, backgroundColor.G, backgroundColor.B, backgroundColor.A ) );

    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(block.Format());
    ctx.Fill(block, color );
    ctx.Finish();

    UTexture2D* texture = NewObject<UTexture2D>( iParent, iName, iFlags );
    texture->SRGB = useSRGB;
    InitTextureWithBlockData(&block, texture, textureFormat);

    //Init user data
    UOdysseyTextureLayerStackUserData* userData = NewObject<UOdysseyTextureLayerStackUserData>(texture, NAME_None, RF_Public);

    if (defaultLayerType == EOdysseyTextureDefaultLayerType::kRaster)
    {
        userData->InitWithDefaultLayerStack();
    }
    else if (defaultLayerType == EOdysseyTextureDefaultLayerType::kVector)
    {
        userData->InitWithEmptyVectorLayer();
    }
    else if (defaultLayerType == EOdysseyTextureDefaultLayerType::kNone)
    {
        userData->InitWithEmptyLayerStack();
    }

    // Notify for changes
    texture->AddAssetUserData( userData );

    if (BackgroundColor != EOdysseyTextureBackgroundColor::kTransparent)
    {
        UOdysseyLayerStack* layerStack = userData->GetLayerStack();

        //Add first layer image


        //Set the layer as Current Layer
        UOdysseyTextureLayerImageRaster* layer = Cast<UOdysseyTextureLayerImageRaster>(layerStack->AddLayer(UOdysseyTextureLayerImageRaster::StaticClass(), nullptr, 1));
        layer->Name = LOCTEXT("texture.default-background-layer.name", "Background");

        //Fill LayerImage with content of Texture
        TSharedPtr<FOdysseyRasterBlock> rasterBlock = layer->GetRasterBlock();
        FOdysseyRasterBlockMutator rasterBlockMutator(rasterBlock, false);
        rasterBlockMutator.EditTilesFromRects(
            { ::ULIS::FRectI::FromXYWH(0, 0, rasterBlock->GetWidth(), rasterBlock->GetHeight()) },
            [&](TSharedPtr<::ULIS::FBlock> iBlock, const FOdysseyInvalidTileMap& iTileMap) -> TArray<ULIS::FEvent>
            {
                FillOdysseyBlockFromUTextureData(iBlock.Get(), texture, iBlock->Format());
                return {};
            }
        );
        rasterBlockMutator.Commit();
    }

    texture->PostEditChange();

    return texture;
}

void
SOdysseyTextureConfigureWindow::Construct(const FArguments& iArgs, const FOdysseyTextureConfiguration& iDefaultConfiguration)
{
    mConfiguration = iDefaultConfiguration;
    Construct(iArgs);
}

void
SOdysseyTextureConfigureWindow::Construct( const FArguments& iArgs)
{
    mWindowAnswer = false;

    FStructureDetailsViewArgs structureDetailsViewArgs;
    FDetailsViewArgs detailsViewArgs;
    detailsViewArgs.bAllowSearch = false;
    detailsViewArgs.bShowScrollBar = false;

    FPropertyEditorModule& propertyEditor = FModuleManager::Get().LoadModuleChecked<FPropertyEditorModule>(TEXT("PropertyEditor"));
    TSharedRef<FStructOnScope> structOnScope = MakeShared<FStructOnScope>(FOdysseyTextureConfiguration::StaticStruct(), reinterpret_cast<uint8*>(&mConfiguration));
    TSharedPtr<IStructureDetailsView> configurationDetailsView = propertyEditor.CreateStructureDetailView(detailsViewArgs, structureDetailsViewArgs, structOnScope);

    //---

    SWindow::Construct( SWindow::FArguments()
        .Title( LOCTEXT( "configure-window.title", "Create Texture Asset" ) )
        // Remove the empty space at the bottom but doesn't scale anymore
        //.SizingRule( ESizingRule::FixedSize )
        //.ClientSize( FVector2D( 300, 180 ) )
        .SizingRule( ESizingRule::Autosized )
        .SupportsMinimize( false )
        .SupportsMaximize( false )
        [
            SNew(SVerticalBox)
            +SVerticalBox::Slot()
            [
                configurationDetailsView->GetWidget().ToSharedRef()
            ]
            +SVerticalBox::Slot()
            .AutoHeight()
            .HAlign( HAlign_Center )
            .VAlign( VAlign_Center )
            [
                SNew( SUniformGridPanel )
                .MinDesiredSlotHeight( FCoreStyle::Get().GetFloat( "StandardDialog.MinDesiredSlotHeight" ) )
                .MinDesiredSlotWidth( FCoreStyle::Get().GetFloat( "StandardDialog.MinDesiredSlotWidth" ) )
                .SlotPadding( FCoreStyle::Get().GetMargin( "StandardDialog.SlotPadding" ) )

                +SUniformGridPanel::Slot( 0, 0 )
                [
                    SNew( SButton )
                    .ContentPadding( FCoreStyle::Get().GetMargin( "StandardDialog.ContentPadding" ) )
                    .HAlign( HAlign_Center )
                    .Text( LOCTEXT( "configure-window.create-asset", "Create Asset" ) )
                    .OnClicked_Raw( this, &SOdysseyTextureConfigureWindow::OnAccept )
                ]

                +SUniformGridPanel::Slot( 1, 0 )
                [
                    SNew( SButton )
                    .ContentPadding( FCoreStyle::Get().GetMargin( "StandardDialog.ContentPadding" ) )
                    .HAlign( HAlign_Center )
                    .Text( LOCTEXT( "configure-window.cancel", "Cancel" ) )
                    .OnClicked_Raw( this, &SOdysseyTextureConfigureWindow::OnCancel )
                ]
            ]
        ]
    );
}

//---

bool
SOdysseyTextureConfigureWindow::GetWindowAnswer()
{
    return mWindowAnswer;
}

const FOdysseyTextureConfiguration&
SOdysseyTextureConfigureWindow::GetConfiguration() const
{
    return mConfiguration;
}

//---

FReply
SOdysseyTextureConfigureWindow::OnAccept()
{
    mWindowAnswer = true;
    RequestDestroyWindow();

    return FReply::Handled();
}

FReply
SOdysseyTextureConfigureWindow::OnCancel()
{
    mWindowAnswer = false;
    RequestDestroyWindow();

    return FReply::Handled();
}

#undef LOCTEXT_NAMESPACE
