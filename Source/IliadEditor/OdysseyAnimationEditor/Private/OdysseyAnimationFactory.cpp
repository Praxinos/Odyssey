// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyAnimationFactory.h"

#include "OdysseyAnimation.h"
#include "OdysseyAnimationLayerImageVector.h"
#include "OdysseyAnimationLayerImageRaster.h"
#include "OdysseyAnimationCellImageVector.h"
#include "OdysseyAnimationCellImageRaster.h"
#include "Widgets/SOdysseyAnimationConfigureWindow.h"
#include "OdysseyRasterBlock.h"
#include "OdysseyRasterBlockMutator.h"
#include "ULISLoaderModule.h"

#include "Editor.h"
#include "EditorStyleSet.h"

#define LOCTEXT_NAMESPACE "Animation"

/////////////////////////////////////////////////////
// UOdysseyAnimationFactory
UOdysseyAnimationFactory::UOdysseyAnimationFactory( const FObjectInitializer& iObjectInitializer )
    : Super( iObjectInitializer )
{
    // From UFactory
    bCreateNew = true;
    bEditAfterNew = true;
    SupportedClass = UOdysseyAnimation::StaticClass();
}

bool UOdysseyAnimationFactory::ConfigureProperties()
{
    //We go in here before creating the asset: Meaning we can have any modal window here.
    //If return false, we don't create the object, if true, we create it
    TSharedPtr<SOdysseyAnimationConfigureWindow> configurationWindow = SNew( SOdysseyAnimationConfigureWindow );
    GEditor->EditorAddModalWindow( configurationWindow.ToSharedRef() );

    if( !configurationWindow->GetWindowAnswer() )
        return false;

    //---

    FOdysseyAnimationConfiguration configuration = configurationWindow->GetConfiguration();

    DefaultName = configuration.Name;
    Width = configuration.Settings.Width;
    Height = configuration.Settings.Height;
    Format = configuration.Settings.Format;
    FrameRate = configuration.Settings.FrameRate;
    DefaultLayerClass = configuration.Settings.DefaultLayerClass;
    LayerBackgroundColor.Reset();
    if( configuration.Settings.AddLayerBackground )
    {
        LayerBackgroundColor = configuration.Settings.LayerBackgroundColor;
    }

    return true;
}

FString
UOdysseyAnimationFactory::GetDefaultNewAssetName() const
{
    return DefaultName.Len() ? DefaultName : Super::GetDefaultNewAssetName();
}

UObject*
UOdysseyAnimationFactory::FactoryCreateNew( UClass* iClass, UObject* iParent, FName iName, EObjectFlags iFlags, UObject* iContext, FFeedbackContext* iWarn )
{
    check(iClass->IsChildOf(UOdysseyAnimation::StaticClass()));

    UOdysseyAnimation* animation = NewObject<UOdysseyAnimation>( iParent, iName, iFlags | RF_Transactional );

    animation->Init( Width, Height, Format, FrameRate.AsDecimal() );
    UOdysseyLayerStack* layerStack = animation->GetLayerStack();

    UOdysseyAnimationLayer* layer = Cast<UOdysseyAnimationLayer>( layerStack->AddLayer( *DefaultLayerClass ) );
    layerStack->SetCurrentLayer( layer );
    layer->AddCell( layer->GetDefaultCellClass() );

    //Background Layer
    if( LayerBackgroundColor.IsSet() )
    {
        UOdysseyAnimationLayerImageRaster* backgroundLayer = Cast<UOdysseyAnimationLayerImageRaster>(layerStack->AddLayer(UOdysseyAnimationLayerImageRaster::StaticClass(), nullptr, 1));
        backgroundLayer->SetPostBehaviour(EOdysseyLayerImagePostBehaviour::Hold);
        backgroundLayer->SetLayerName(LOCTEXT("animation.default-background-layer.name", "Background"));

        UOdysseyAnimationCellImageRaster* backgroundCell = Cast<UOdysseyAnimationCellImageRaster>(backgroundLayer->AddCell(UOdysseyAnimationCellImageRaster::StaticClass()));
        TSharedPtr<FOdysseyRasterBlock> backgroundRasterBlock = backgroundCell->GetRasterBlock();

        FLinearColor backgorundColor = *LayerBackgroundColor;

        FOdysseyRasterBlockMutator rasterBlockMutator(backgroundRasterBlock);
        rasterBlockMutator.EditTilesFromRects(
            { backgroundRasterBlock->GetRect() },
            [backgorundColor](TSharedPtr<::ULIS::FBlock> ioBlock, const FOdysseyInvalidTileMap& iInvalidTileMap) -> TArray<::ULIS::FEvent>
            {
                ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(ioBlock->Format());
                ::ULIS::FColor color( ::ULIS::FColor::FromRGBAF( backgorundColor.R, backgorundColor.G, backgorundColor.B, backgorundColor.A ) );
                ctx.Fill(*ioBlock, color);
                ctx.Finish();
                return {};
            }
        );
    }

    return animation;
}

#undef LOCTEXT_NAMESPACE
