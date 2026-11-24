// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyAnimationLayer.h"

#include "Misc/TransactionObjectEvent.h"
#include "RenderGraphBuilder.h"
#include "RenderGraphUtils.h"

#include "OdysseyAnimationCell.h"
#include "OdysseyAnimationLayerStack.h"
#include "OdysseyAnimationRenderContext.h"
#include "OdysseyBlendColorShader.h"
#include "OdysseyBlendShader.h"
#include "OdysseyLayerStackRenderContext.h"
#include "OdysseyRenderContext.h"

#include "OdysseyLighttable.h"
#include "UObject/OdysseyObjectEditorUtils.h"

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

        UOdysseyAnimationCell* cell = GetCellAtFrame(frame);
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
}

TArray<FGuid>
UOdysseyAnimationLayer::GetRenderingComposition(uint64 iRenderType, int iFrame) const
{
    TArray<FGuid> idComposition = { GetRenderingId() };

    bool showLighttable = iRenderType & EOdysseyRenderingType::Editor && Lighttable.bIsActivated;
    if ( showLighttable && Lighttable.DisplayPosition == EOdysseyLighttableDisplayPosition::UnderLayer )
        idComposition.Append(GetLighttableImageRenderingComposition(iFrame));

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

        UOdysseyAnimationCell* cell = GetCellAtFrame(frame);
        if ( cell )
        {
            int cellFrame = frame - cell->GetFrameRange().GetLowerBoundValue();
            idComposition.Append(cell->GetRenderingComposition(iRenderType, cellFrame));
        }
    }

    if ( showLighttable && Lighttable.DisplayPosition == EOdysseyLighttableDisplayPosition::AboveLayer )
        idComposition.Append(GetLighttableImageRenderingComposition(iFrame));

    return idComposition;
}

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
    UOdysseyAnimationCell* cell = GetCellAtFrame(Frame.Value);
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
                UOdysseyAnimationCell* keyCell = GetCells()[keyCellIndex];
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
                UOdysseyAnimationCell* keyCell = GetCells()[keyCellIndex];
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
    UOdysseyAnimationCell* cell = GetCellAtFrame(iFrameIndex);
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
                UOdysseyAnimationCell* keyCell = GetCells()[keyCellIndex];
                if (keyCell)
                    idComposition.Append(keyCell->GetRenderingComposition(EOdysseyRenderingType::Render, 0));
            }
        }

        if (Lighttable.NextKeys[i].bIsActivated)
        {
            int keyCellIndex = cell->GetIndexInLayer() + i + 1;
            if (keyCellIndex >= 0 && keyCellIndex < Cells.Num())
            {
                UOdysseyAnimationCell* keyCell = GetCells()[keyCellIndex];
                if (keyCell)
                    idComposition.Append(keyCell->GetRenderingComposition(EOdysseyRenderingType::Render, 0));
            }
        }
    }

    return idComposition;
}

UOdysseyAnimationLayer::FOnDisplayCellNamesChanged&
UOdysseyAnimationLayer::OnDisplayCellNamesChanged()
{
    static FOnDisplayCellNamesChanged onDisplayCellNamesChanged;
    return onDisplayCellNamesChanged;
}

UOdysseyAnimationLayer::FOnCellsChanged&
UOdysseyAnimationLayer::OnCellsChanged()
{
    static FOnCellsChanged OnCellsChangedDelegate;
    return OnCellsChangedDelegate;
}

void
UOdysseyAnimationLayer::PostInitProperties()
{
    Super::PostInitProperties();

    //Activate first previous and first next keys in the lighttable by default
    Lighttable.PreviousKeys[0].bIsActivated = true;
    Lighttable.NextKeys[0].bIsActivated = true;

    for (int i = 0; i < 5; i++)
    {
        Lighttable.PreviousKeys[i].Opacity = (5 - i) * 10.f;
        Lighttable.NextKeys[i].Opacity = (5 - i) * 10.f;
    }
}

void
UOdysseyAnimationLayer::InvalidateCellsFrameRanges()
{
    mCellsFrameRanges.Empty();
}

FInt32Range
UOdysseyAnimationLayer::GetFrameRange() const
{
    TArray<FInt32Range> ranges;
    const TArray<UOdysseyLayer*> layers = Children;
    for (UOdysseyLayer* layer : layers)
    {
        ranges.Add(layer->GetFrameRange());
    }

    if (!Cells.IsEmpty())
    {
        const TArray<FInt32Range>& cellFrameRanges = GetCellsFrameRanges();
        ranges.Add(FInt32Range::Inclusive( cellFrameRanges[0].GetLowerBoundValue(), cellFrameRanges.Last().GetUpperBoundValue() ));
    }

    return FInt32Range::Hull(ranges);
}

TSubclassOf<UOdysseyAnimationCell>
UOdysseyAnimationLayer::GetDefaultCellClass() const
{
    return DefaultCellClass;
}

void
UOdysseyAnimationLayer::CellsChanged()
{
    UpdateCellsIndexInLayer();
    InvalidateCellsFrameRanges();
    OnCellsChanged().Broadcast(this, false);
    RenderingCompositionChanged();
}

void
UOdysseyAnimationLayer::CellsChangedInteractive()
{
    UpdateCellsIndexInLayer();
    InvalidateCellsFrameRanges();
    OnCellsChanged().Broadcast(this, true);
    RenderingCompositionChanged(true);
}

int
UOdysseyAnimationLayer::GetPreBehaviourFrame(EOdysseyLayerImagePostBehaviour iBehaviour, int iFrame) const
{
    FInt32Range frameRange = GetFrameRange();

    //PreBehaviour
    int frame = iFrame;
    switch(iBehaviour)
    {
        default:
        case EOdysseyLayerImagePostBehaviour::None:
        break;

        case EOdysseyLayerImagePostBehaviour::Hold:
        {
            frame = frameRange.GetLowerBoundValue();
        }
        break;
        case EOdysseyLayerImagePostBehaviour::Loop:
        {
            int offsetFromStart = frameRange.GetLowerBoundValue() - iFrame;
            int layerLength = frameRange.GetUpperBoundValue() - frameRange.GetLowerBoundValue() + 1;
            frame = frameRange.GetUpperBoundValue() - ((offsetFromStart - 1) % layerLength);
        }
        break;
        case EOdysseyLayerImagePostBehaviour::PingPong:
        {
            int offsetFromStart = frameRange.GetLowerBoundValue() - iFrame;
            int layerLength = frameRange.GetUpperBoundValue() - frameRange.GetLowerBoundValue() + 1;

            if (layerLength <= 1)
            {
                frame = frameRange.GetLowerBoundValue();
                break;
            }

            int offset = ((offsetFromStart - 1) % (layerLength - 1)) + 1;
            int forwardFrame = frameRange.GetLowerBoundValue() + offset;
            int backwardFrame = frameRange.GetUpperBoundValue() - offset;

            int direction = layerLength > 0 ? ((offsetFromStart - 1) / (layerLength - 1)) % 2 : 0;
            frame = direction == 0 ? forwardFrame : backwardFrame;
        }
        break;
    }

    return frame;
}

int
UOdysseyAnimationLayer::GetPostBehaviourFrame(EOdysseyLayerImagePostBehaviour iBehaviour, int iFrame) const
{
    //PreBehaviour
    FInt32Range frameRange = GetFrameRange();

    int frame = iFrame;
    switch(iBehaviour)
    {
        default:
        case EOdysseyLayerImagePostBehaviour::None:
        break;

        case EOdysseyLayerImagePostBehaviour::Hold:
        {
            frame = frameRange.GetUpperBoundValue();
        }
        break;
        case EOdysseyLayerImagePostBehaviour::Loop:
        {
            int offsetFromEnd = iFrame - frameRange.GetUpperBoundValue();
            int layerLength = frameRange.GetUpperBoundValue() - frameRange.GetLowerBoundValue() + 1;
            frame = frameRange.GetLowerBoundValue() + (offsetFromEnd - 1) % layerLength;
        }
        break;
        case EOdysseyLayerImagePostBehaviour::PingPong:
        {
            int offsetFromEnd = iFrame - frameRange.GetUpperBoundValue();
            int layerLength = frameRange.GetUpperBoundValue() - frameRange.GetLowerBoundValue() + 1;

            if (layerLength <= 1)
            {
                frame = frameRange.GetLowerBoundValue();
                break;
            }

            int offset = ((offsetFromEnd - 1) % (layerLength - 1)) + 1;
            int forwardFrame = frameRange.GetLowerBoundValue() + offset;
            int backwardFrame = frameRange.GetUpperBoundValue() - offset;

            int direction = layerLength > 0 ? ((offsetFromEnd - 1) / (layerLength - 1)) % 2 : 0;
            frame = direction == 0 ? backwardFrame : forwardFrame;
        }
        break;
    }

    return frame;
}

const TArray<UOdysseyAnimationCell*>&
UOdysseyAnimationLayer::GetCells() const
{
    return Cells;
}

UOdysseyAnimationCell*
UOdysseyAnimationLayer::GetCellAtFrame(int Frame) const
{
    int cellIndex = GetCellIndexAtFrame(Frame);
    if (cellIndex == INDEX_NONE)
        return nullptr;

    return Cells[cellIndex];
}

int
UOdysseyAnimationLayer::GetCellIndexAtFrame(int Frame) const
{
    //TODO: Binary search using GetFrameRanges()
    //TODO: Use Prolly Search, as most of the time in animation the data is approximately uniformly distributed
    //see : https://www.dolthub.com/blog/2024-05-13-binary-vs-prolly/

    if( Frame < CellsOffset )
        return INDEX_NONE;

    int frameIndex = CellsOffset;
    for (int i = 0; i < Cells.Num(); i++)
    {
        UOdysseyAnimationCell* cell = Cells[i];
        int cellExposure = 1;
        if (cell)
            cellExposure = cell->GetExposure();

        if ( frameIndex + cellExposure - 1 >= Frame)
            return i;

        frameIndex += cellExposure;
    }

    return INDEX_NONE;
}

bool
UOdysseyAnimationLayer::HasCellAtFrame(int Frame) const
{
    return !!GetCellAtFrame(Frame);
}

const TArray<FInt32Range>&
UOdysseyAnimationLayer::GetCellsFrameRanges() const
{
    if (mCellsFrameRanges.Num() != Cells.Num())
    {
        mCellsFrameRanges.Empty();
        uint32 startFrame = CellsOffset;
        for (TObjectPtr<UOdysseyAnimationCell> cell : Cells)
        {
            int cellExposure = 1;
            if (cell)
                cellExposure = cell->GetExposure();

            FInt32Range frameRange = FInt32Range::Inclusive(startFrame, startFrame + cellExposure - 1);
            mCellsFrameRanges.Add(frameRange);
            startFrame += cellExposure;
        }
    }

    return mCellsFrameRanges;
}

void
UOdysseyAnimationLayer::AddCellInteractive(int Index)
{
    AddCellsInteractive(Index);
}

void
UOdysseyAnimationLayer::AddCellsInteractive(int Index, int Count)
{
    if (Index < 0 )
    {
        Index = Cells.Num();
    }
    else
    {
        Index = FMath::Clamp(Index, 0, Cells.Num());
    }

    TArray<UOdysseyAnimationCell*> cells;
    for (int i = 0; i < Count; i++)
    {
        //Create the Layer
        cells.Add(nullptr);
    }

    Cells.Insert(cells, Index);
    CellsChangedInteractive();
}

UOdysseyAnimationCell*
UOdysseyAnimationLayer::AddCell(TSubclassOf<UOdysseyAnimationCell> CellType, int Index)
{
    TArray<UOdysseyAnimationCell*> cells = AddCells(CellType, Index);
    if (cells.IsEmpty())
        return nullptr;

    return cells[0];
}

TArray<UOdysseyAnimationCell*>
UOdysseyAnimationLayer::AddCells(TSubclassOf<UOdysseyAnimationCell> CellType, int Index, int Count)
{
    if( !IsEditable() )
        return {};

    UClass* cellType = CellType.Get();

    //No cellType
    if ( !cellType )
        return {};

    //cellType Not supported
    if (!SupportedCellTypes.Contains(cellType))
        return {};

    Modify();

    if (Index < 0 )
    {
        Index = Cells.Num();
    }
    else
    {
        Index = FMath::Clamp(Index, 0, Cells.Num());
    }

    TArray<UOdysseyAnimationCell*> cells;
    for (int i = 0; i < Count; i++)
    {
        //Create the Layer
        UOdysseyAnimationCell* cell = NewObject<UOdysseyAnimationCell>(this, cellType, NAME_None, RF_Public | RF_Transactional);
        if (!cell )
            return cells;

        cells.Add(cell);
    }

    Cells.Insert(cells, Index);
    CellsChanged();

    return cells;
}

void
UOdysseyAnimationLayer::RemoveCell(UOdysseyAnimationCell* Cell)
{
    if (!Cell) //if you need to remove a null cell use RemoveCellAtIndex
        return;

    RemoveCells({Cell});
    checkf(Cell->GetLayer() == this, TEXT("Layer does not contain the cell to remove"));
}

void
UOdysseyAnimationLayer::RemoveCells(const TArray<UOdysseyAnimationCell*>& iCells)
{
    if( !IsEditable() )
        return;

    UOdysseyLayerStack* layerStack = GetLayerStack();
    if(!layerStack)
        return;

    Modify();

    TArray<UOdysseyAnimationCell*> cells;
    for (UOdysseyAnimationCell* cell : iCells)
    {
        if (!cell) //if you need to remove a null cell use RemoveCellAtIndex
            continue;

        if (!ensureMsgf(cell->GetLayer() == this, TEXT("Layer does not contain the cell to remove")))
            continue;

        cell->Modify();
        cell->IndexInLayer = INDEX_NONE;
        cell->MarkAsGarbage();
        cells.Add(cell);
    }

    Cells.RemoveAll([&](UOdysseyAnimationCell* iCell) { return cells.Contains(iCell); });
    CellsChanged();
}

void
UOdysseyAnimationLayer::RemoveCellAtIndex(int Index)
{
    if( !IsEditable() )
        return;

    UOdysseyLayerStack* layerStack = GetLayerStack();
    if(!layerStack)
        return;

    if (Index < 0 || Index >= Cells.Num())
        return;

    Modify();

    if (Cells[Index])
    {
        Cells[Index]->Modify();
        Cells[Index]->IndexInLayer = INDEX_NONE;
        Cells[Index]->MarkAsGarbage();
    }

    Cells.RemoveAt(Index);
    CellsChanged();
}

void
UOdysseyAnimationLayer::RemoveAllCells()
{
    if( !IsEditable() )
        return;

    UOdysseyLayerStack* layerStack = GetLayerStack();
    if(!layerStack)
        return;

    Modify();

    Cells.Empty();
    CellsChanged();
}

void
UOdysseyAnimationLayer::UpdateCellsIndexInLayer()
{
    for (int i = 0; i < Cells.Num(); i++)
    {
        if (!Cells[i])
            continue;

        if (Cells[i]->IndexInLayer == i)
            continue;

        Cells[i]->Modify();
        Cells[i]->IndexInLayer = i;
    }
}

bool
UOdysseyAnimationLayer::AreCellsContiguous( const TArray<UOdysseyAnimationCell*>& iCells  )
{
    TArray<UOdysseyAnimationCell*> unusedSortedCells;

    return AreCellsContiguous( iCells, unusedSortedCells );
}

bool
UOdysseyAnimationLayer::AreCellsContiguous( const TArray<UOdysseyAnimationCell*>& iCells
                                 , TArray<UOdysseyAnimationCell*>& oSortedCells  )
{
    oSortedCells = iCells;

    oSortedCells.Sort( []( const UOdysseyAnimationCell& iA, const UOdysseyAnimationCell& iB ) -> bool
    {
        return iA.GetIndexInLayer() < iB.GetIndexInLayer();
    } );

    // check contiguity
    for( int i = 0, n = 1; n < oSortedCells.Num(); i++, n++ )
    {
        if( oSortedCells[i]->GetIndexInLayer() != ( oSortedCells[n]->GetIndexInLayer() - 1 ) )
        {
            oSortedCells.Empty();

            return false;
        }
    }

    return true;
}

TArray<TArray<UOdysseyAnimationCell*>> UOdysseyAnimationLayer::GetCellsContiguousGroups( const TArray<UOdysseyAnimationCell*>& iCells )
{
    TArray<UOdysseyAnimationCell*> sortedCells = iCells;

    sortedCells.Sort( []( const UOdysseyAnimationCell& iA, const UOdysseyAnimationCell& iB ) -> bool
                      {
                          return iA.GetIndexInLayer() < iB.GetIndexInLayer();
                      } );

    if( sortedCells.IsEmpty() )
        return TArray<TArray<UOdysseyAnimationCell*>>();

    TArray<TArray<UOdysseyAnimationCell*>> cellGroups;
    cellGroups.Emplace(); // At least 1 group as sortedCells are not empty here
    for( UOdysseyAnimationCell* cell : sortedCells )
    {
        TArray<UOdysseyAnimationCell*>& lastGroup = cellGroups.Last();
        if( lastGroup.IsEmpty() )
        {
            lastGroup.Add( cell );
            continue;
        }

        if( lastGroup.Last()->GetIndexInLayer() == cell->GetIndexInLayer() - 1 )
        {
            lastGroup.Add( cell );
        }
        else
        {
            cellGroups.Emplace();
            cellGroups.Last().Add( cell );
        }
    }

    return cellGroups;
}

bool
UOdysseyAnimationLayer::ReverseCells( const TArray<UOdysseyAnimationCell*>& iCellsToReverse )
{
    if( !IsEditable() )
        return false;

    if( iCellsToReverse.Num() >= 2 )
    {
        TArray<UOdysseyAnimationCell*> sortedSelectedCells;

        Modify();

        if( AreCellsContiguous( iCellsToReverse, sortedSelectedCells ) )
        {
            for( int i = 0; i < sortedSelectedCells.Num(); i++ )
            {
                int n = sortedSelectedCells.Num() - i - 1;
                int originalIndex = sortedSelectedCells[i]->GetIndexInLayer();
                int reversedIndex = sortedSelectedCells[n]->GetIndexInLayer();

                Cells[originalIndex] = sortedSelectedCells[n];
                Cells[reversedIndex] = sortedSelectedCells[i];
            }

            UpdateCellsIndexInLayer();
            CellsChanged();

            return true; // success
        }
    }

    return false; // failure
}

UOdysseyAnimationCell*
UOdysseyAnimationLayer::CopyCell(UOdysseyAnimationCell* Cell, int Index)
{
    if( !IsEditable() )
        return nullptr;

    //No Layer
    if(!Cell)
        return nullptr;

    Modify();

    if (Index < 0 )
    {
        Index = Cells.Num();
    }
    else
    {
        Index = FMath::Clamp(Index, 0, Cells.Num());
    }

    //Duplicate the cell
    FObjectDuplicationParameters params(Cell, this);
    UOdysseyAnimationCell* cellCopy = Cast<UOdysseyAnimationCell>(StaticDuplicateObjectEx(params));

    Cells.Insert(cellCopy, Index);
    CellsChanged();

    return cellCopy;
}

UOdysseyAnimationCell*
UOdysseyAnimationLayer::CopyCellToLayer(UOdysseyAnimationCell* iCell, UOdysseyAnimationLayer* iToLayer, int iIndex) const
{
    //No Layer
    if(!iCell)
        return nullptr;

    if( !iToLayer->IsEditable() )
        return nullptr;

    iToLayer->Modify();

    if (iIndex < 0 )
    {
        iIndex = iToLayer->Cells.Num();
    }
    else
    {
        iIndex = FMath::Clamp(iIndex, 0, iToLayer->Cells.Num());
    }

    //Duplicate the cell
    FObjectDuplicationParameters params(iCell, iToLayer );
    UOdysseyAnimationCell* cellCopy = Cast<UOdysseyAnimationCell>(StaticDuplicateObjectEx(params));

    iToLayer->Cells.Insert( cellCopy, iIndex );
    iToLayer->CellsChanged();

    return cellCopy;
}

TArray<UOdysseyAnimationCell*>
UOdysseyAnimationLayer::CopyCells(TArray<UOdysseyAnimationCell*> iCells, int Index)
{
    if( !IsEditable() )
        return {};

    if (Index < 0 )
    {
        Index = Cells.Num();
    }
    else
    {
        Index = FMath::Clamp(Index, 0, Cells.Num());
    }

    //Sanitize Layers array
    iCells.RemoveAll(
        [this](const UOdysseyAnimationCell* iCell)
        {
            return !iCell;
        }
    );

    //No Layers
    if (iCells.IsEmpty())
        return {};

    Modify();

    TArray<UOdysseyAnimationCell*> cellCopies;
    for (UOdysseyAnimationCell* cell : iCells)
    {
        FObjectDuplicationParameters params(cell, this);
        UOdysseyAnimationCell* cellCopy = Cast<UOdysseyAnimationCell>(StaticDuplicateObjectEx(params));
        cellCopies.Add(cellCopy);
    }

    Cells.Insert(cellCopies, Index);
    CellsChanged();

    return cellCopies;
}

bool
UOdysseyAnimationLayer::ShouldDisplayCellNames() const
{
    return bDisplayCellNames;
}

int
UOdysseyAnimationLayer::GetCellsOffset() const
{
    return CellsOffset;
}

EOdysseyLayerImagePostBehaviour
UOdysseyAnimationLayer::GetPreBehaviour() const
{
    return PreBehaviour;
}

EOdysseyLayerImagePostBehaviour
UOdysseyAnimationLayer::GetPostBehaviour() const
{
    return PostBehaviour;
}

const TArray<TSubclassOf<UOdysseyAnimationCell>>&
UOdysseyAnimationLayer::GetSupportedCellTypes() const
{
    return SupportedCellTypes;
}

void
UOdysseyAnimationLayer::SetCellsOffset(int Value)
{
    if( !IsEditable() )
        return;

    Modify();

    CellsOffset = Value;
    InvalidateCellsFrameRanges();
    RenderingCompositionChanged();
}

void
UOdysseyAnimationLayer::SetPreBehaviour(EOdysseyLayerImagePostBehaviour Value)
{
    if( !IsEditable() )
        return;

    Modify();

    PreBehaviour = Value;
    RenderingCompositionChanged();
}

void
UOdysseyAnimationLayer::SetPostBehaviour(EOdysseyLayerImagePostBehaviour Value)
{
    if( !IsEditable() )
        return;

    Modify();

    PostBehaviour = Value;
    RenderingCompositionChanged();
}

void
UOdysseyAnimationLayer::SetDisplayCellNames(bool Value)
{
    bDisplayCellNames = Value;
    OnDisplayCellNamesChanged().Broadcast(this);
}

void
UOdysseyAnimationLayer::SetCellsOffsetInteractive(float Value)
{
    if( !IsEditable() )
        return;

    Modify();

    CellsOffset = Value;
    InvalidateCellsFrameRanges();
    RenderingCompositionChanged(true);
}

TArray<FName>
UOdysseyAnimationLayer::GetRows() const
{
    return {
        "Main",
        "Blend",
        "CellNames",
        "Lighttable",
        "OutOfPegs",
    };
}

int
UOdysseyAnimationLayer::GetRowHeight(FName iSubRowName) const
{
    if (iSubRowName == "Main")
        return 20;

    if (iSubRowName == "Blend")
        return 20;

    if (iSubRowName == "Lighttable")
        return 40;

    if (iSubRowName == "OutOfPegs")
        return 20;

    if (iSubRowName == "CellNames")
        return 20;

    return 0;
}

bool
UOdysseyAnimationLayer::IsRowVisible(FName iSubRowName) const
{
    if (iSubRowName == "Main")
        return true;

    if (iSubRowName == "Blend")
        return bDisplayOptions;

    if (iSubRowName == "Lighttable")
        return IsActivatedRecursively() && ShouldDisplayOptions() && bHasLighttable && Lighttable.bIsActivated;

    if (iSubRowName == "OutOfPegs")
        return IsActivatedRecursively() && ShouldDisplayOptions() && bHasLighttable && Lighttable.bIsActivated;

    if (iSubRowName == "CellNames")
        return IsActivatedRecursively() && ShouldDisplayOptions() && bDisplayCellNames;

    return 0;
}

FOdysseyLighttable
UOdysseyAnimationLayer::GetLighttable() const
{
    return Lighttable;
}

bool
UOdysseyAnimationLayer::HasLighttable() const
{
    return bHasLighttable;
}

void
UOdysseyAnimationLayer::SetLighttable(FOdysseyLighttable Value)
{
    if (!bHasLighttable)
        return;

    Lighttable = Value;
    RenderingCompositionChanged(); //Composition could change if lighttable or a key is activated/inactivated
    RenderingChanged(); //ImageRendering changes without a composition change when any other param is changed
    mOnLighttableChanged.Broadcast();
}

void
UOdysseyAnimationLayer::SetLighttableInteractive(FOdysseyLighttable Value)
{
    if (!bHasLighttable)
        return;

    Lighttable = Value;
    RenderingCompositionChanged(true); //Composition could change if lighttable or a key is activated/inactivated
    RenderingChanged(true); //ImageRendering changes without a composition change when any other param is changed
    mOnLighttableChanged.Broadcast();
}

FSimpleMulticastDelegate&
UOdysseyAnimationLayer::OnLighttableChanged()
{
    return mOnLighttableChanged;
}

void
UOdysseyAnimationLayer::PostTransacted(const FTransactionObjectEvent& iTransactionEvent)
{
    Super::PostTransacted(iTransactionEvent);
    if (iTransactionEvent.GetEventType() != ETransactionObjectEventType::UndoRedo)
        return;
    const TArray<FName>& changedPropertyNames = iTransactionEvent.GetChangedProperties();
    if ( changedPropertyNames.Contains(GET_MEMBER_NAME_CHECKED(UOdysseyAnimationLayer, Cells))
        || changedPropertyNames.Contains(GET_MEMBER_NAME_CHECKED(UOdysseyAnimationLayer, CellsOffset) )
    )
    {
        InvalidateCellsFrameRanges();
    }

    if (changedPropertyNames.Contains(GET_MEMBER_NAME_CHECKED(UOdysseyAnimationLayer, Cells))
        || changedPropertyNames.Contains(GET_MEMBER_NAME_CHECKED(UOdysseyAnimationLayer, CellsOffset))
        || changedPropertyNames.Contains(GET_MEMBER_NAME_CHECKED(UOdysseyAnimationLayer, PreBehaviour))
        || changedPropertyNames.Contains(GET_MEMBER_NAME_CHECKED(UOdysseyAnimationLayer, PostBehaviour)))
    {
        RenderingCompositionChanged();
    }
    if (changedPropertyNames.Contains(GET_MEMBER_NAME_CHECKED(UOdysseyAnimationLayer, Lighttable)))
    {
        RenderingCompositionChanged(); //Composition could change if lighttable or a key is activated/inactivated
        RenderingChanged(); //ImageRendering changes without a composition change when any other param is changed
        mOnLighttableChanged.Broadcast();
    }
}
