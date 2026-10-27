// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyAnimationBlueprintLibrary.h"

#include "OdysseyAnimationCellImageRaster.h"
#include "OdysseyAnimationUtils.h"
#include "OdysseyRasterBlock.h"
#include "OdysseyRasterBlockMutator.h"
#include "ULISLoaderModule.h"

//static
UOdysseyAnimation*
UOdysseyAnimationBlueprintLibrary::GetLayerStackAnimation(UOdysseyLayerStack* Layerstack)
{
    return Odyssey::AnimationUtils::GetLayerStackAnimation(Layerstack);
}

//static
UOdysseyAnimation*
UOdysseyAnimationBlueprintLibrary::GetLayerAnimation(UOdysseyLayer* Layer)
{
    return Odyssey::AnimationUtils::GetLayerAnimation(Layer);
}

//static
UOdysseyAnimation*
UOdysseyAnimationBlueprintLibrary::GetCellAnimation(UOdysseyLayerCell* Cell)
{
    return Odyssey::AnimationUtils::GetCellAnimation(Cell);
}

//---

//static
void
UOdysseyAnimationBlueprintLibrary::Fill( UOdysseyAnimationCellImageRaster* iCell, const FLinearColor& iColor )
{
    //FScopedTransaction ScopedTransaction( LOCTEXT( "animation-cell-raster.fill", "Fill" ) );

    TSharedPtr<FOdysseyRasterBlock> rasterBlock = iCell->GetRasterBlock();

    FOdysseyRasterBlockMutator mutator( rasterBlock );
    mutator.EditTilesFromRects(
        { ::ULIS::FRectI::FromXYWH( 0, 0, rasterBlock->GetWidth(), rasterBlock->GetHeight() ) },
        [&]( TSharedPtr<::ULIS::FBlock> iBlock, const FOdysseyInvalidTileMap& iTileMap ) -> TArray<::ULIS::FEvent>
        {
            ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext( rasterBlock->GetFormat() );
            ::ULIS::FEvent eventFill;
            ctx.Fill( *iBlock, ::ULIS::FColor::FromRGBAF( iColor.R, iColor.G, iColor.B, iColor.A ), ::ULIS::FRectI::Auto, ::ULIS::FSchedulePolicy::CacheEfficient, 0, nullptr, &eventFill );
            return { eventFill };
        }
    );
    mutator.Commit();
}
