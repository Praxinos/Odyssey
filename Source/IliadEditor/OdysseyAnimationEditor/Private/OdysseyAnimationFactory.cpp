// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyAnimationFactory.h"

#include "OdysseyAnimation.h"
#include "OdysseyAnimationLayerImageVector.h"
#include "OdysseyAnimationLayerImageRaster.h"
#include "OdysseyAnimationCellImageVector.h"
#include "OdysseyAnimationCellImageRaster.h"
#include "Widgets/SOdysseyAnimationConfigureWindow.h"
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
    mConfiguration = configurationWindow->GetConfiguration();

    mConfigured = configurationWindow->GetWindowAnswer();

    return configurationWindow->GetWindowAnswer();
}

FString
UOdysseyAnimationFactory::GetDefaultNewAssetName() const
{
    return !mConfiguration.Name.ToString().IsEmpty() ? mConfiguration.Name.ToString() : Super::GetDefaultNewAssetName();
}

UObject*
UOdysseyAnimationFactory::FactoryCreateNew( UClass* iClass, UObject* iParent, FName iName, EObjectFlags iFlags, UObject* iContext, FFeedbackContext* iWarn )
{
    check(iClass->IsChildOf(UOdysseyAnimation::StaticClass()));

    UOdysseyAnimation* animation = NewObject<UOdysseyAnimation>( iParent, iName, iFlags | RF_Transactional );

    //happens when ConfigureProperties is not called
    //Example : In the CreateAnimationAsset blueprint node
    if (!mConfigured)
        return animation;

    mConfigured = false;

    animation->Init(mConfiguration.Width, mConfiguration.Height, mConfiguration.Format, mConfiguration.FramesPerSecond);

    UOdysseyAnimationLayerStack* layerStack = NewObject<UOdysseyAnimationLayerStack>(animation, "LayerStack", RF_Public | RF_Transactional);
    animation->SetLayerStack(layerStack);

    switch (mConfiguration.LayerType)
    {
        case EOdysseyAnimationDefaultLayerType::Raster:
        {
            UOdysseyAnimationLayerImageRaster* layer = Cast<UOdysseyAnimationLayerImageRaster>(layerStack->AddLayer(UOdysseyAnimationLayerImageRaster::StaticClass()));
            layerStack->CurrentLayer = layer;
            layer->AddCell(UOdysseyAnimationCellImageRaster::StaticClass());
        }
        break;

        case EOdysseyAnimationDefaultLayerType::Vector:
        {
            UOdysseyAnimationLayerImageVector* layer = Cast<UOdysseyAnimationLayerImageVector>(layerStack->AddLayer(UOdysseyAnimationLayerImageVector::StaticClass()));
            layerStack->CurrentLayer = layer;
            layer->AddCell(UOdysseyAnimationCellImageVector::StaticClass());
        }
        break;

        default:
            check(false); //should not be called
    }

    //Background Layer
    if (mConfiguration.BackgroundColor != EOdysseyAnimationBackgroundColor::Transparent)
    {
        UOdysseyAnimationLayerImageRaster* backgroundLayer = Cast<UOdysseyAnimationLayerImageRaster>(layerStack->AddLayer(UOdysseyAnimationLayerImageRaster::StaticClass(), nullptr, 1));
        backgroundLayer->PostBehaviour = EOdysseyAnimationLayerImagePostBehaviour::Hold;
        backgroundLayer->Name = LOCTEXT("animation.default-background-layer.name", "Background");

        UOdysseyAnimationCellImageRaster* backgroundCell = Cast<UOdysseyAnimationCellImageRaster>(backgroundLayer->AddCell(UOdysseyAnimationCellImageRaster::StaticClass()));
        TSharedPtr<FOdysseyRasterBlock> backgroundRasterBlock = backgroundCell->GetRasterBlock();

        FLinearColor backgorundColor = mConfiguration.GetBackgroundColor();

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
