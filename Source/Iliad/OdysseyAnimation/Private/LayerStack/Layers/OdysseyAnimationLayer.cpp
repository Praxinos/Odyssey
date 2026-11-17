// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyAnimationLayer.h"

#include "RenderGraphBuilder.h"
#include "RenderGraphUtils.h"

#include "OdysseyAnimationCell.h"
#include "OdysseyAnimationLayerStack.h"
#include "OdysseyAnimationRenderContext.h"
#include "OdysseyBlendColorShader.h"
#include "OdysseyBlendShader.h"
#include "OdysseyLayerCell.h"
#include "OdysseyLayerStackRenderContext.h"
#include "OdysseyRenderContext.h"

#if WITH_EDITOR
#include "OdysseyLighttable.h"
#include "UObject/OdysseyObjectEditorUtils.h"
#endif

UOdysseyAnimation*
UOdysseyAnimationLayer::GetAnimation() const
{
    UOdysseyAnimationLayerStack* layerStack = Cast<UOdysseyAnimationLayerStack>(GetLayerStack());
    if(!layerStack)
        return nullptr;

    return layerStack->GetAnimation();
}

UOdysseyAnimationLayerStack*
UOdysseyAnimationLayer::GetLayerStack() const
{
    return Cast<UOdysseyAnimationLayerStack>(UOdysseyLayer::GetLayerStack());
}

bool
UOdysseyAnimationLayer::BuildRenderPipelineInternal(
    IOdysseyTextureRenderingAbility::FRenderFunction& oRenderFunction,
    const TArray<const IOdysseyTextureRenderingAbility*>& iParents, const FOdysseyRenderContext* RenderContext
) const
{
    const FOdysseyAnimationRenderContext* AnimationContext = RenderContext ? RenderContext->GetExtension<FOdysseyAnimationRenderContext>() : nullptr;
    const FFrameNumber Frame = AnimationContext ? AnimationContext->Frame.Get(FFrameNumber(0)) : FFrameNumber(0);
    const uint64 Type = RenderContext ? RenderContext->Type : EOdysseyRenderingType::Render;
    const FOdysseyRenderOverride* ContentOverride = RenderContext ? RenderContext->Find(*this) : nullptr;
    IOdysseyTextureRenderingAbility::FRenderFunction renderFunction;
    if (ContentOverride && ContentOverride->ContentReplacement)
    {
        renderFunction = ContentOverride->ContentReplacement;
    }
    else if ( bCanHaveChildren )
    {
        if (!BuildRenderChildrenPipeline(renderFunction, iParents, RenderContext))
            return false;
    }
    else
    {
        FInt32Range frameRange = GetFrameRange();
        int frame = Frame.Value;
        if ( frame < frameRange.GetLowerBoundValue() )
        {
            frame = GetPreBehaviourFrame(PreBehaviour, frame);
        }
        else if ( frame > frameRange.GetUpperBoundValue() )
        {
            frame = GetPostBehaviourFrame(PostBehaviour, frame);
        }

        if ( frame == INDEX_NONE )
            return false;

        UOdysseyLayerCell* cell = GetCellAtFrame(frame);
        if ( !cell )
            return false;

        FOdysseyRenderContext CellContext = RenderContext ? *RenderContext : FOdysseyRenderContext();
        CellContext.FindOrAddExtension<FOdysseyAnimationRenderContext>().Frame = FFrameNumber(frame);
        if(!cell->BuildRenderPipeline(renderFunction, iParents, &CellContext))
            return false;
    }

    if (ContentOverride && ContentOverride->ContentDecorator)
    {
        renderFunction = ContentOverride->ContentDecorator(MoveTemp(renderFunction));
    }
    if (!renderFunction) return false;
#if WITH_EDITOR
    const FOdysseyLayerStackRenderContext* LayerContext = RenderContext ? RenderContext->GetExtension<FOdysseyLayerStackRenderContext>() : nullptr;
    const FOdysseyLayerStackRenderOverride* LayerOverride = LayerContext ? LayerContext->Find(GetRenderingId()) : nullptr;
    const FOdysseyLighttable EffectiveLighttable = LayerOverride && LayerOverride->Lighttable ? *LayerOverride->Lighttable : Lighttable;
    bool showLighttable = Type & EOdysseyRenderingType::Editor && EffectiveLighttable.bIsActivated;
    EOdysseyLighttableDisplayPosition lighttablePosition = EffectiveLighttable.DisplayPosition;

    IOdysseyTextureRenderingAbility::FRenderFunction lighttableRenderFunction;
    if (showLighttable)
        showLighttable = BuildLighttableRenderPipeline(lighttableRenderFunction, iParents, RenderContext);

    oRenderFunction = [this, renderFunction, showLighttable, lighttablePosition, lighttableRenderFunction](
            FRDGBuilder& iGraphBuilder,
            ERHIFeatureLevel::Type iFeatureLevel,
            FRDGTextureRef iDestinationTexture,
            const FIntRect& iSrcRect,
            const FIntRect& iDstRect,
            const FMatrix& iSrcTransform
        )
        {
            AddClearRenderTargetPass(iGraphBuilder, iDestinationTexture, FLinearColor::Transparent, iDstRect);

            FRDGTextureDesc renderTextureDesc = FRDGTextureDesc::Create2D(
                iDestinationTexture->Desc.Extent,
                iDestinationTexture->Desc.Format,
                FClearValueBinding::Transparent,
                ETextureCreateFlags::ShaderResource | ETextureCreateFlags::RenderTargetable
            );

            FRDGTextureRef renderTexture = iGraphBuilder.CreateTexture(renderTextureDesc, TEXT("UOdysseyAnimationLayer::renderTexture"));

            renderFunction(
                iGraphBuilder,
                iFeatureLevel,
                renderTexture,
                iSrcRect,
                iDstRect,
                iSrcTransform
            );

            FRDGTextureRef lighttableTexture = nullptr;
            if ( showLighttable )
            {
                FRDGTextureDesc lighttableTextureDesc = FRDGTextureDesc::Create2D(
                    iDestinationTexture->Desc.Extent,
                    iDestinationTexture->Desc.Format,
                    FClearValueBinding::Transparent,
                    ETextureCreateFlags::ShaderResource | ETextureCreateFlags::RenderTargetable
                );

                lighttableTexture = iGraphBuilder.CreateTexture(lighttableTextureDesc, TEXT("UOdysseyAnimationLayer::LighttableTexture"));

                lighttableRenderFunction(
                    iGraphBuilder,
                    iFeatureLevel,
                    lighttableTexture,
                    iSrcRect,
                    iDstRect,
                    iSrcTransform
                );
            }

            if ( showLighttable && lighttablePosition == EOdysseyLighttableDisplayPosition::UnderLayer )
            {
                FOdysseyBlendShader::BlendRect(
                    iGraphBuilder,
                    iFeatureLevel,
                    iDestinationTexture,
                    lighttableTexture,
                    iDestinationTexture,
                    iDstRect,
                    iDstRect,
                    FMatrix::Identity,
                    EOdysseyColorBlendMode::Normal,
                    EOdysseyAlphaBlendMode::Normal,
                    1.0f,
                    EOdysseyAntiAliasing::AnisotropicLinear
                );
            }

            FOdysseyBlendShader::BlendRect(
                iGraphBuilder,
                iFeatureLevel,
                iDestinationTexture,
                renderTexture,
                iDestinationTexture,
                iDstRect,
                iDstRect,
                FMatrix::Identity,
                EOdysseyColorBlendMode::Normal,
                EOdysseyAlphaBlendMode::Normal,
                1.0f,
                EOdysseyAntiAliasing::AnisotropicLinear
            );

            if ( showLighttable && lighttablePosition == EOdysseyLighttableDisplayPosition::AboveLayer )
            {
                FOdysseyBlendShader::BlendRect(
                    iGraphBuilder,
                    iFeatureLevel,
                    iDestinationTexture,
                    lighttableTexture,
                    iDestinationTexture,
                    iDstRect,
                    iDstRect,
                    FMatrix::Identity,
                    EOdysseyColorBlendMode::Normal,
                    EOdysseyAlphaBlendMode::Normal,
                    1.0f,
                    EOdysseyAntiAliasing::AnisotropicLinear
                );
            }
        };
    return true;
#else
    oRenderFunction = renderFunction;
    return true;
#endif
}

TArray<FGuid>
UOdysseyAnimationLayer::GetRenderingComposition(uint64 iRenderType, int iFrame) const
{
    TArray<FGuid> idComposition = { GetRenderingId() };

#if WITH_EDITOR
    bool showLighttable = iRenderType & EOdysseyRenderingType::Editor && Lighttable.bIsActivated;
    if ( showLighttable && Lighttable.DisplayPosition == EOdysseyLighttableDisplayPosition::UnderLayer )
        idComposition.Append(GetLighttableImageRenderingComposition(iFrame));
#endif

    if (bCanHaveChildren)
    {
        const TArray<UOdysseyLayer*>& children = GetChildren();
        for (UOdysseyLayer* child : children)
        {
            if (!child->IsActivated())
                continue;

            idComposition.Append(child->GetRenderingComposition(iRenderType, iFrame));
        }
    }
    else
    {
        int frame = iFrame;
        FInt32Range frameRange = GetFrameRange();
        if ( iFrame < frameRange.GetLowerBoundValue() )
        {
            frame = GetPreBehaviourFrame(PreBehaviour, iFrame);
        }
        else if ( iFrame > frameRange.GetUpperBoundValue() )
        {
            frame = GetPostBehaviourFrame(PostBehaviour, iFrame);
        }

        UOdysseyLayerCell* cell = GetCellAtFrame(frame);
        if ( cell )
        {
            int cellFrame = frame - cell->GetFrameRange().GetLowerBoundValue();
            idComposition.Append(cell->GetRenderingComposition(iRenderType, cellFrame));
        }
    }

#if WITH_EDITOR
    if ( showLighttable && Lighttable.DisplayPosition == EOdysseyLighttableDisplayPosition::AboveLayer )
        idComposition.Append(GetLighttableImageRenderingComposition(iFrame));
#endif

    return idComposition;
}

#if WITH_EDITOR
bool
UOdysseyAnimationLayer::BuildLighttableRenderPipeline(
    IOdysseyTextureRenderingAbility::FRenderFunction& oRenderFunction,
    const TArray<const IOdysseyTextureRenderingAbility*>& iParents, const FOdysseyRenderContext* RenderContext
) const
{
    const FOdysseyAnimationRenderContext* AnimationContext = RenderContext ? RenderContext->GetExtension<FOdysseyAnimationRenderContext>() : nullptr;
    const FFrameNumber Frame = AnimationContext ? AnimationContext->Frame.Get(FFrameNumber(0)) : FFrameNumber(0);
    const FOdysseyLayerStackRenderContext* LayerContext = RenderContext ? RenderContext->GetExtension<FOdysseyLayerStackRenderContext>() : nullptr;
    const FOdysseyLayerStackRenderOverride* LayerOverride = LayerContext ? LayerContext->Find(GetRenderingId()) : nullptr;
    const FOdysseyLighttable EffectiveLighttable = LayerOverride && LayerOverride->Lighttable ? *LayerOverride->Lighttable : Lighttable;
    FOdysseyRenderContext LighttableContext = RenderContext ? *RenderContext : FOdysseyRenderContext();
    LighttableContext.Type = EOdysseyRenderingType::Render;
    FOdysseyLayerStackRenderContext& LighttableLayerContext = LighttableContext.FindOrAddExtension<FOdysseyLayerStackRenderContext>();
    LighttableLayerContext.bLighttablePass = true;
    for (TMap<FGuid, FOdysseyRenderOverride>::TIterator OverrideIt = LighttableContext.Overrides.CreateIterator(); OverrideIt; ++OverrideIt)
    {
        if (!LighttableLayerContext.Find(OverrideIt.Key()))
        {
            OverrideIt.RemoveCurrent();
        }
    }
    UOdysseyLayerCell* cell = GetCellAtFrame(Frame.Value);
    if (!cell)
        return false;

    struct FLighttableKeyRenderParams
    {
        IOdysseyTextureRenderingAbility::FRenderFunction RenderFunction;
        float Opacity;
        FLinearColor Color;
        float Contrast;
        FMatrix Transform;
    };

    TArray<FLighttableKeyRenderParams> keysRenderParams;
    for (int i = 9; i >= 0; i--)
    {
        if (EffectiveLighttable.PreviousKeys[i].bIsActivated)
        {
            int keyCellIndex = cell->GetIndexInLayer() - i - 1;
            if (keyCellIndex >= 0 && keyCellIndex < Cells.Num())
            {
                UOdysseyLayerCell* keyCell = GetCells()[keyCellIndex];
                if (keyCell)
                {
                    const int frame = Frame.Value - cell->GetFrameRange().GetLowerBoundValue();
                    LighttableContext.FindOrAddExtension<FOdysseyAnimationRenderContext>().Frame = FFrameNumber(keyCell->GetFrameRange().GetLowerBoundValue() + frame);
                    IOdysseyTextureRenderingAbility::FRenderFunction renderFunction;
                    if (keyCell->BuildRenderPipeline(renderFunction, iParents, &LighttableContext))
                    {
                        keysRenderParams.Add(
                            {
                                renderFunction,
                                EffectiveLighttable.PreviousKeys[i].Opacity / 100.f,
                                EffectiveLighttable.PreviousKeysColor,
                                EffectiveLighttable.PreviousKeysContrast / 100.f,
                                keyCell->OutOfPegsTransform()
                            }
                        );
                    }
                }
            }
        }

        if (EffectiveLighttable.NextKeys[i].bIsActivated)
        {
            int keyCellIndex = cell->GetIndexInLayer() + i + 1;
            if (keyCellIndex >= 0 && keyCellIndex < Cells.Num())
            {
                UOdysseyLayerCell* keyCell = GetCells()[keyCellIndex];
                if (keyCell)
                {
                    const int frame = Frame.Value - cell->GetFrameRange().GetLowerBoundValue();
                    LighttableContext.FindOrAddExtension<FOdysseyAnimationRenderContext>().Frame = FFrameNumber(keyCell->GetFrameRange().GetLowerBoundValue() + frame);
                    IOdysseyTextureRenderingAbility::FRenderFunction renderFunction;
                    if (keyCell->BuildRenderPipeline(renderFunction, iParents, &LighttableContext))
                    {
                        keysRenderParams.Add(
                            {
                                renderFunction,
                                EffectiveLighttable.NextKeys[i].Opacity / 100.f,
                                EffectiveLighttable.NextKeysColor,
                                EffectiveLighttable.NextKeysContrast / 100.f,
                                keyCell->OutOfPegsTransform()
                            }
                        );
                    }
                }
            }
        }
    }

    if (keysRenderParams.IsEmpty())
        return false;

    oRenderFunction = [this, keysRenderParams](
            FRDGBuilder& iGraphBuilder,
            ERHIFeatureLevel::Type iFeatureLevel,
            FRDGTextureRef iDestinationTexture,
            const FIntRect& iSrcRect,
            const FIntRect& iDstRect,
            const FMatrix& iSrcTransform
        )
        {
            AddClearRenderTargetPass(iGraphBuilder, iDestinationTexture, FLinearColor::Transparent, iDstRect);

            for ( const FLighttableKeyRenderParams& keyRenderParams : keysRenderParams )
            {
                FRDGTextureDesc desc = FRDGTextureDesc::Create2D(
                    iDestinationTexture->Desc.Extent,
                    iDestinationTexture->Desc.Format,
                    FClearValueBinding::Transparent,
                    ETextureCreateFlags::ShaderResource | ETextureCreateFlags::RenderTargetable
                );

                FRDGTextureRef keyTexture = iGraphBuilder.CreateTexture(desc, TEXT("UOdysseyAnimationLayer::Lighttable::keyTexture"));
                keyRenderParams.RenderFunction(iGraphBuilder, iFeatureLevel, keyTexture, iSrcRect, iDstRect, iSrcTransform * keyRenderParams.Transform);

                //Turn to black and white
                FOdysseyBlendColorShader::BlendRect(
                    iGraphBuilder,
                    iFeatureLevel,
                    keyTexture,
                    FLinearColor::Black,
                    keyTexture,
                    iDstRect,
                    EOdysseyColorBlendMode::Saturation,
                    EOdysseyAlphaBlendMode::Back,
                    1.f
                );

                //Apply Contrast
                FOdysseyBlendColorShader::BlendRect(
                    iGraphBuilder,
                    iFeatureLevel,
                    keyTexture,
                    FLinearColor::Black,
                    keyTexture,
                    iDstRect,
                    EOdysseyColorBlendMode::Luminosity,
                    EOdysseyAlphaBlendMode::Back,
                    1.f - keyRenderParams.Contrast
                );

                FOdysseyBlendColorShader::BlendRect(
                    iGraphBuilder,
                    iFeatureLevel,
                    keyTexture,
                    keyRenderParams.Color,
                    keyTexture,
                    iDstRect,
                    EOdysseyColorBlendMode::Screen,
                    EOdysseyAlphaBlendMode::Back,
                    1.f
                );

                FOdysseyBlendShader::BlendRect(
                    iGraphBuilder,
                    iFeatureLevel,
                    iDestinationTexture,
                    keyTexture,
                    iDestinationTexture,
                    iDstRect,
                    iDstRect,
                    FMatrix::Identity,
                    EOdysseyColorBlendMode::Normal,
                    EOdysseyAlphaBlendMode::Normal,
                    keyRenderParams.Opacity,
                    EOdysseyAntiAliasing::AnisotropicLinear
                );
            }
        };

    return true;
}

TArray<FGuid>
UOdysseyAnimationLayer::GetLighttableImageRenderingComposition(int iFrameIndex) const
{
    UOdysseyLayerCell* cell = GetCellAtFrame(iFrameIndex);
    if (!cell)
        return {};

    TArray<FGuid> idComposition = {};
    for (int i = 9; i >= 0; i--)
    {
        if (Lighttable.PreviousKeys[i].bIsActivated)
        {
            int keyCellIndex = cell->GetIndexInLayer() - i - 1;
            if (keyCellIndex >= 0 && keyCellIndex < Cells.Num())
            {
                UOdysseyLayerCell* keyCell = GetCells()[keyCellIndex];
                if (keyCell)
                    idComposition.Append(keyCell->GetRenderingComposition(EOdysseyRenderingType::Render, 0));
            }
        }

        if (Lighttable.NextKeys[i].bIsActivated)
        {
            int keyCellIndex = cell->GetIndexInLayer() + i + 1;
            if (keyCellIndex >= 0 && keyCellIndex < Cells.Num())
            {
                UOdysseyLayerCell* keyCell = GetCells()[keyCellIndex];
                if (keyCell)
                    idComposition.Append(keyCell->GetRenderingComposition(EOdysseyRenderingType::Render, 0));
            }
        }
    }

    return idComposition;
}
#endif
