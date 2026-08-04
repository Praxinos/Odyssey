// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyVirtualRenderTargetFinalizer.h"

#include "CanvasItem.h"
#include "CanvasTypes.h"
#include "Engine.h"
#include "RenderGraphBuilder.h"
#include "ScreenPass.h"

FOdysseyVirtualRenderTargetFinalizer::~FOdysseyVirtualRenderTargetFinalizer()
{
}

FOdysseyVirtualRenderTargetFinalizer::FOdysseyVirtualRenderTargetFinalizer(const FVTProducerDescription& InProducerDesc)
    : ProducerDesc(InProducerDesc)
{
}

void
FOdysseyVirtualRenderTargetFinalizer::RenderFinalize(FRDGBuilder& GraphBuilder, ISceneRenderer* SceneRenderingContext)
{
}

void
FOdysseyVirtualRenderTargetFinalizer::Finalize(FRDGBuilder& GraphBuilder)
{
    for (FTileEntry const& Tile : TilesToRender)
    {
        /**
         * Size : 4096x4096
         * Format : BGRA8
         *
         */
        IPooledRenderTarget* DestRenderTarget = Tile.Target.PooledRenderTarget;

        const float X = (float)FMath::ReverseMortonCode2_64(Tile.vAddress);
        const float Y = (float)FMath::ReverseMortonCode2_64(Tile.vAddress >> 1);
        /*
        const float DivisorX = (float)ProducerDesc.BlockWidthInTiles / (float)(1 << Tile.vLevel);
        const float DivisorY = (float)ProducerDesc.BlockHeightInTiles / (float)(1 << Tile.vLevel);

        const FVector2f UV(X / DivisorX, Y / DivisorY);
        const FVector2f UVSize(1.f / DivisorX, 1.f / DivisorY);
        const FVector2f UVBorder = UVSize * ((float)ProducerDesc.TileBorderSize / (float)ProducerDesc.TileSize);
        const FBox2f UVRange(UV - UVBorder, UV + UVSize + UVBorder);*/

        const int32 FullTileSize = ProducerDesc.TileSize + 2 * ProducerDesc.TileBorderSize;
        const FIntPoint DestinationPos(Tile.Target.pPageLocation.X * FullTileSize, Tile.Target.pPageLocation.Y * FullTileSize);
        const FIntPoint DestinationSize(FullTileSize, FullTileSize);
        const FIntRect DestRect(DestinationPos, DestinationPos + DestinationSize);

        FRDGTextureRef TileRenderTarget = GraphBuilder.RegisterExternalTexture(DestRenderTarget, ERDGTextureFlags::None);

        FRDGTextureDesc TextRenderTargetDesc = FRDGTextureDesc::Create2D(
            FIntPoint(FullTileSize, FullTileSize),
            TileRenderTarget->Desc.Format,
            FClearValueBinding::Transparent,
            ETextureCreateFlags::RenderTargetable | ETextureCreateFlags::ShaderResource
        );
        FRDGTextureRef TextRenderTarget = GraphBuilder.CreateTexture(TextRenderTargetDesc, TEXT("FOdysseyVirtualRenderTargetFinalizer.TextRenderTarget"));
        AddClearRenderTargetPass(GraphBuilder, TextRenderTarget, FLinearColor::Green);

        FCanvas* TextCanvas = FCanvas::Create(GraphBuilder, TextRenderTarget, nullptr, FGameTime(), GMaxRHIFeatureLevel);

        FCanvasTextItem PosTextItem(
            FVector2D(10,10),
            FText::Format(FText::FromString("Pos : {0},{1}"), FText::AsNumber(X), FText::AsNumber(Y)),
            GEngine->GetSmallFont(),
            FLinearColor::Red
        );

        FCanvasTextItem VLevelTextItem(
            FVector2D(10,20),
            FText::Format(FText::FromString("vLevel : {0}"), FText::AsNumber(Tile.vLevel)),
            GEngine->GetSmallFont(),
            FLinearColor::Red
        );

        TextCanvas->DrawItem(PosTextItem);
        TextCanvas->DrawItem(VLevelTextItem);

        TextCanvas->Flush_RenderThread(GraphBuilder);

        AddDrawTexturePass(
            GraphBuilder,
            FScreenPassViewInfo(),
            TextRenderTarget,
            TileRenderTarget,
            FIntPoint::ZeroValue,
            DestinationPos,
            DestinationSize
        );
    }

    TilesToRender.Reset();
}
