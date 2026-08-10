// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyTiledRLE.h"
#include "GlobalShader.h"
#include "RenderGraphBuilder.h"
#include "RenderGraphEvent.h"
#include "RenderGraphUtils.h"
#include "ScreenPass.h"
#include "Engine/TextureRenderTarget2D.h"

DECLARE_STATS_GROUP(TEXT("OdysseyDecompressTiledRLEShader"), STATGROUP_OdysseyDecompressTiledRLEShader, STATCAT_Advanced);
DECLARE_CYCLE_STAT(TEXT("OdysseyDecompressTiledRLEShader Execute"), STAT_OdysseyDecompressTiledRLEShader_Execute, STATGROUP_OdysseyDecompressTiledRLEShader);
DECLARE_CYCLE_STAT(TEXT("OdysseyTiledRLE UnloadRenderTarget"), STAT_OdysseyTiledRLE_UnloadRenderTarget, STATGROUP_OdysseyDecompressTiledRLEShader);

// This class carries our parameter declarations and acts as the bridge between cpp and HLSL.
class FOdysseyDecompressTiledRLEShader : public FGlobalShader
{
public:

    DECLARE_GLOBAL_SHADER(FOdysseyDecompressTiledRLEShader);
    SHADER_USE_PARAMETER_STRUCT(FOdysseyDecompressTiledRLEShader, FGlobalShader);

    BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
        SHADER_PARAMETER(uint32, TileWidth) //Width of 1 tile in pixels
        SHADER_PARAMETER(uint32, TileHeight) //Height of 1 tile in pixels
        SHADER_PARAMETER(uint32, TextureHeight) //Width of the final Texture
        SHADER_PARAMETER(uint32, TextureWidth) //Height of the final Texture
        SHADER_PARAMETER(uint32, ComponentsPerPixel)
        SHADER_PARAMETER(uint32, BytesPerComponent)
        SHADER_PARAMETER(uint32, RedIndex)
        SHADER_PARAMETER(uint32, GreenIndex)
        SHADER_PARAMETER(uint32, BlueIndex)
        SHADER_PARAMETER(uint32, AlphaIndex)
        SHADER_PARAMETER_RDG_BUFFER_SRV(StructuredBuffer<FTileDescriptor>, TileDescriptors) //Contains informations about tiles (which tile contains data and where to find taht data in the RLEBuffer
        SHADER_PARAMETER_RDG_BUFFER_SRV(StructuredBuffer<int32>, RLEPositions) //Contains each valid tile data compressed in RLE
        SHADER_PARAMETER_RDG_BUFFER_SRV(StructuredBuffer<uint8>, RLEData) //Contains each valid tile data compressed in RLE
        SHADER_PARAMETER_RDG_BUFFER_SRV(StructuredBuffer<uint8>, SequenceData) //Contains each valid tile data compressed in RLE
        SHADER_PARAMETER_RDG_TEXTURE_UAV(RWTexture2D, OutputTexture) //The output Texture where the uncompressed data will be stored
    END_SHADER_PARAMETER_STRUCT()

public:
    static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
    {
        return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5) && !IsConsolePlatform(Parameters.Platform);
    }

    static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
    {
        FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);
        /*const FPermutationDomain PermutationVector(Parameters.PermutationId);
        OutEnvironment.SetDefine(TEXT("THREADS_X"), NUM_THREADS_OdysseyDecompressTiledRLEShader_X);
        OutEnvironment.SetDefine(TEXT("THREADS_Y"), NUM_THREADS_OdysseyDecompressTiledRLEShader_Y);
        OutEnvironment.SetDefine(TEXT("THREADS_Z"), NUM_THREADS_OdysseyDecompressTiledRLEShader_Z); */
    }

private:
};

// This will tell the engine to create the shader and where the shader entry point is.
//                            ShaderType                            ShaderPath                     Shader function name    Type
IMPLEMENT_GLOBAL_SHADER(FOdysseyDecompressTiledRLEShader, "/OdysseyShaders/Private/OdysseyDecompressTiledRLEShader.usf", "Main", SF_Compute);

FIntPoint
GetNumTiles(const FOdysseyTiledRLE::FRLECompressedBuffer& iBuffer)
{
    return FIntPoint(
        FMath::Max(uint32(1), ((iBuffer.TextureWidth - 1) / iBuffer.TileWidth) + 1),
        FMath::Max(uint32(1), ((iBuffer.TextureHeight - 1) / iBuffer.TileHeight) + 1)
    );
}

FRDGTextureRef
FOdysseyTiledRLE::DecompressRenderThread(FRDGBuilder& iGraphBuilder, const FRLECompressedBuffer& iBuffer, EPixelFormat iPixelFormat)
{
    SCOPE_CYCLE_COUNTER(STAT_OdysseyDecompressTiledRLEShader_Execute);
    DECLARE_GPU_STAT(OdysseyDecompressTiledRLEShader);
    RDG_EVENT_SCOPE(iGraphBuilder, "OdysseyDecompressTiledRLEShader");
    RDG_GPU_STAT_SCOPE(iGraphBuilder, OdysseyDecompressTiledRLEShader);

    TShaderMapRef<FOdysseyDecompressTiledRLEShader> ComputeShader(GetGlobalShaderMap(GMaxRHIFeatureLevel));
    if (!ComputeShader.IsValid())
        return nullptr;

    FOdysseyDecompressTiledRLEShader::FParameters* PassParameters = iGraphBuilder.AllocParameters<FOdysseyDecompressTiledRLEShader::FParameters>();
    FRDGTextureDesc desc = FRDGTextureDesc::Create2D(
        FIntPoint(iBuffer.TextureWidth, iBuffer.TextureHeight),
        iPixelFormat,
        FClearValueBinding::Transparent,
        ETextureCreateFlags::ShaderResource | ETextureCreateFlags::RenderTargetable | ETextureCreateFlags::UAV
    );
    FRDGTextureRef outputTexture = iGraphBuilder.CreateTexture(desc, TEXT("FOdysseyTiledRLE::outputTexture"));
    AddClearRenderTargetPass(iGraphBuilder, outputTexture, FLinearColor::Transparent);

    FRDGBufferRef tileDescriptorsBuffer = CreateUploadBuffer(iGraphBuilder, TEXT("TileDescriptors"), sizeof(FTileDescriptor), iBuffer.TileDescriptors.Num(), iBuffer.TileDescriptors.GetData(), iBuffer.TileDescriptors.Num() * sizeof(FTileDescriptor));
    FRDGBufferRef rlePositionsBuffer = CreateUploadBuffer(iGraphBuilder, TEXT("RLEPositions"), sizeof(int32), iBuffer.RLEPositions.Num(), iBuffer.RLEPositions.GetData(), iBuffer.RLEPositions.Num() * sizeof(int32));
    FRDGBufferRef rleDataBuffer = CreateUploadBuffer(iGraphBuilder, TEXT("RLEData"), sizeof(uint8), iBuffer.RLEData.Num(), iBuffer.RLEData.GetData(), iBuffer.RLEData.Num());
    FRDGBufferRef sequenceDataBuffer = CreateUploadBuffer(iGraphBuilder, TEXT("SequenceData"), sizeof(uint8), iBuffer.SequenceData.Num(), iBuffer.SequenceData.GetData(), iBuffer.SequenceData.Num());

    PassParameters->TileWidth = iBuffer.TileWidth;
    PassParameters->TileHeight = iBuffer.TileHeight;
    PassParameters->TextureWidth = iBuffer.TextureWidth;
    PassParameters->TextureHeight = iBuffer.TextureHeight;
    PassParameters->ComponentsPerPixel = iBuffer.ComponentsPerPixel;
    PassParameters->BytesPerComponent = iBuffer.BytesPerComponent;
    PassParameters->RedIndex = iBuffer.RedIndex;
    PassParameters->GreenIndex = iBuffer.GreenIndex;
    PassParameters->BlueIndex = iBuffer.BlueIndex;
    PassParameters->AlphaIndex = iBuffer.AlphaIndex;
    PassParameters->TileDescriptors = iGraphBuilder.CreateSRV(FRDGBufferSRVDesc(tileDescriptorsBuffer, PF_R32_SINT));
    PassParameters->RLEPositions = iGraphBuilder.CreateSRV(FRDGBufferSRVDesc(rlePositionsBuffer, PF_R32_SINT));
    PassParameters->RLEData = iGraphBuilder.CreateSRV(FRDGBufferSRVDesc(rleDataBuffer, PF_R32_UINT));
    PassParameters->SequenceData = iGraphBuilder.CreateSRV(FRDGBufferSRVDesc(sequenceDataBuffer, PF_R32_UINT));
    PassParameters->OutputTexture = iGraphBuilder.CreateUAV(FRDGTextureUAVDesc(outputTexture), ERDGUnorderedAccessViewFlags::None);

    int TileMapWidth = ((iBuffer.TextureWidth - 1) / iBuffer.TileWidth) + 1;
    int TileMapHeight = ((iBuffer.TextureHeight - 1) / iBuffer.TileHeight) + 1;

    auto GroupCount = FIntVector(TileMapWidth, TileMapHeight, 1);
    FComputeShaderUtils::AddPass(
        iGraphBuilder,
        RDG_EVENT_NAME("ExecuteOdysseyDecompressTiledRLEShader"),
        GSupportsEfficientAsyncCompute ? ERDGPassFlags::AsyncCompute : ERDGPassFlags::Compute,
        ComputeShader,
        PassParameters,
        GroupCount
    );

    return outputTexture;
}

// Executes this shader on the render thread from the game thread via EnqueueRenderThreadCommand
void
FOdysseyTiledRLE::DecompressGameThread(const FRLECompressedBuffer& iBuffer, UTextureRenderTarget2D* oTexture)
{
    //TODO: Test if oTexture has the size expected by iBuffer

    EPixelFormat pixelFormat = oTexture->GetFormat();

    ENQUEUE_RENDER_COMMAND(SceneDrawCompletion)(
    [oTexture, iBuffer, pixelFormat](FRHICommandListImmediate& RHICmdList)
    {
        FRDGBuilder graphBuilder(RHICmdList);
        FRDGTextureRef outputTexture = DecompressRenderThread(graphBuilder, iBuffer, pixelFormat);
        FRDGTextureRef destinationTexture = oTexture->GetRenderTargetResource()->GetRenderTargetTexture( graphBuilder );

        AddDrawTexturePass(
            graphBuilder,
            FScreenPassViewInfo(),
            outputTexture,
            destinationTexture
        );

        graphBuilder.Execute();
    });
}

FIntPoint GetTileSize(uint32 iTileX, uint32 iTileY, FOdysseyTiledRLE::FRLECompressedBuffer RLEBuffer)
{
    FIntPoint endTilePos = FIntPoint((iTileX + 1) * RLEBuffer.TileWidth, (iTileY + 1) * RLEBuffer.TileHeight);
    return FIntPoint(
        RLEBuffer.TileWidth - FMath::Max(int(0), int(endTilePos.X - RLEBuffer.TextureWidth)),
        RLEBuffer.TileHeight - FMath::Max(int(0), int(endTilePos.Y - RLEBuffer.TextureHeight))
    );
}

bool
CompressTile(uint32 iTileX, uint32 iTileY, const FUniqueBuffer& iRawBuffer, FOdysseyTiledRLE::FRLECompressedBuffer& ioRLEBuffer, uint32& oPositionCount, bool& oEndsWithRepetition)
{
    uint32 bytesPerPixel = ioRLEBuffer.BytesPerComponent * ioRLEBuffer.ComponentsPerPixel;
    uint32 tileOffset = iTileY * ioRLEBuffer.TextureWidth * ioRLEBuffer.TileHeight + iTileX * ioRLEBuffer.TileWidth;
    uint8* tileData = (uint8*)iRawBuffer.GetData() + tileOffset * bytesPerPixel;
    FIntPoint tileSize = GetTileSize(iTileX, iTileY, ioRLEBuffer);

    oPositionCount = 0;
    int currentRLEPosition = 0;
    uint8* currentPixelData = tileData;
    int x = 1; //allows us to skip the first pixel as we need to start comparisons at the second pixel
    uint32 sequenceCount = 0;
    uint32 repetitionCount = 0;
    for (int y = 0; y < tileSize.Y; y++)
    {
        uint8* lineData = tileData + y * ioRLEBuffer.TextureWidth * bytesPerPixel;
        while(x < tileSize.X)
        {
            uint8* pixelData = lineData + x * bytesPerPixel;
            if (FMemory::Memcmp( currentPixelData, pixelData, bytesPerPixel ) == 0)
            {
                //Same Pixel
                if (sequenceCount > 0) //it's the end of a sequence
                {
                    ioRLEBuffer.RLEPositions.Add(currentRLEPosition);
                    oPositionCount++;

                    ioRLEBuffer.RLEData.AddUninitialized(bytesPerPixel);
                    uint8* rlePixelData = ioRLEBuffer.RLEData.GetData() + ioRLEBuffer.RLEData.Num() - bytesPerPixel;
                    uint32 sequenceDataPtr = (ioRLEBuffer.SequenceData.Num() - sequenceCount * bytesPerPixel) / bytesPerPixel;
                    FMemory::Memcpy( rlePixelData, &sequenceDataPtr, 4 ); //pointers to sequenceData are always 4 bytes integers

                    sequenceCount = 0;
                    currentRLEPosition = (y * tileSize.X + x - 1) * -1; //sequence positions are negative in RLEPositions
                    repetitionCount++;
                }
                else // it's a repetition
                {
                    repetitionCount++;
                }

            }
            else
            {
                //Different Pixels
                if (repetitionCount > 0) //it's the end of a repetiton
                {
                    ioRLEBuffer.RLEPositions.Add(currentRLEPosition);
                    oPositionCount++;

                    ioRLEBuffer.RLEData.AddUninitialized(bytesPerPixel);
                    uint8* rlePixelData = ioRLEBuffer.RLEData.GetData() + ioRLEBuffer.RLEData.Num() - bytesPerPixel;
                    FMemory::Memcpy( rlePixelData, currentPixelData, bytesPerPixel );

                    currentPixelData = pixelData;
                    repetitionCount = 0;
                    currentRLEPosition = y * tileSize.X + x;
                }
                else //It's a sequence
                {
                    ioRLEBuffer.SequenceData.AddUninitialized(bytesPerPixel);
                    uint8* sequencePixelData = ioRLEBuffer.SequenceData.GetData() + ioRLEBuffer.SequenceData.Num() - bytesPerPixel;
                    FMemory::Memcpy( sequencePixelData, currentPixelData, bytesPerPixel );
                    sequenceCount++;

                    currentPixelData = pixelData;
                }
            }

            x++;
        }
        x = 0;
    }

    //Check if the whole tile was empty
    if (currentPixelData == tileData && FMemory::MemIsZero(tileData, bytesPerPixel))
        return false;

    //Manage end of tile data
    if (repetitionCount > 0)
    {
        //end the repetiton
        ioRLEBuffer.RLEPositions.Add(currentRLEPosition);
        oPositionCount++;

        ioRLEBuffer.RLEData.AddUninitialized(bytesPerPixel);
        uint8* rlePixelData = ioRLEBuffer.RLEData.GetData() + ioRLEBuffer.RLEData.Num() - bytesPerPixel;
        FMemory::Memcpy( rlePixelData, currentPixelData, bytesPerPixel );

        oEndsWithRepetition = true;
    }
    else //it's a sequence
    {
        ioRLEBuffer.RLEPositions.Add(currentRLEPosition);
        oPositionCount++;

        ioRLEBuffer.SequenceData.AddUninitialized(bytesPerPixel);
        uint8* sequencePixelData = ioRLEBuffer.SequenceData.GetData() + ioRLEBuffer.SequenceData.Num() - bytesPerPixel;
        FMemory::Memcpy( sequencePixelData, currentPixelData, bytesPerPixel );
        sequenceCount++;

        ioRLEBuffer.RLEData.AddUninitialized(bytesPerPixel);
        uint8* rlePixelData = ioRLEBuffer.RLEData.GetData() + ioRLEBuffer.RLEData.Num() - bytesPerPixel;
        uint32 sequenceDataPtr = (ioRLEBuffer.SequenceData.Num() - sequenceCount * bytesPerPixel) / bytesPerPixel;
        FMemory::Memcpy( rlePixelData, &sequenceDataPtr, 4 ); //pointers to sequenceData are always 4 bytes integers

        oEndsWithRepetition = false;
    }

    return true;
}

void
CompressTiles(const FUniqueBuffer& iRawBuffer, FOdysseyTiledRLE::FRLECompressedBuffer& ioRLEBuffer)
{
    FIntPoint numTiles = GetNumTiles(ioRLEBuffer);

    //Initialize TileDescriptors array
    ioRLEBuffer.TileDescriptors.AddZeroed(numTiles.X * numTiles.Y);
    for (int i = 0; i < numTiles.X * numTiles.Y; i++)
    {
        //Tile is empty by default, we use negative values to identify empty tiles
        ioRLEBuffer.TileDescriptors[i] = { /*position*/-1, /*count*/0 };
    }

    //Compress each tile
    int32 position = 0;
    for (int tileY = 0; tileY < numTiles.Y; tileY++)
    {
        for (int tileX = 0; tileX < numTiles.X; tileX++)
        {
            uint32 positionCount;
            bool endsWithRepetition;
            if (!CompressTile(tileX, tileY, iRawBuffer, ioRLEBuffer, positionCount, endsWithRepetition))
                continue; //tile is empty

            ioRLEBuffer.TileDescriptors[tileY * numTiles.X + tileX] = { position, positionCount, endsWithRepetition }; //-1 because we do not count the last position of the tile
            position += positionCount;
        }
    }

    if (ioRLEBuffer.TileDescriptors.IsEmpty()) //a buffer must contain something otherwise we get an error.
        ioRLEBuffer.TileDescriptors.AddUninitialized();

    if (ioRLEBuffer.RLEPositions.IsEmpty()) //a buffer must contain something otherwise we get an error.
        ioRLEBuffer.RLEPositions.AddUninitialized();

    if (ioRLEBuffer.RLEData.IsEmpty()) //a buffer must contain something otherwise we get an error.
        ioRLEBuffer.RLEData.AddUninitialized();

    if (ioRLEBuffer.SequenceData.IsEmpty()) //a buffer must contain something otherwise we get an error.
        ioRLEBuffer.SequenceData.AddUninitialized();
}

TSharedRef<TPromise<FOdysseyTiledRLE::FRLECompressedBuffer>>
FOdysseyTiledRLE::Compress(UTexture* iTexture, const FCompressionParams& Params)
{
    TSharedRef<TPromise<FRLECompressedBuffer>> promise = MakeShared<TPromise<FRLECompressedBuffer>>();

    ENQUEUE_RENDER_COMMAND(UOdysseyCachedRenderTarget_UnloadRenderTarget)(
        [
            promise,
            textureWidth = iTexture->GetSurfaceWidth(),
            textureHeight = iTexture->GetSurfaceHeight(),
            source = iTexture,
            params = Params
        ](FRHICommandListImmediate& RHICmdList)
        {
            SCOPE_CYCLE_COUNTER(STAT_OdysseyTiledRLE_UnloadRenderTarget);
            DECLARE_GPU_STAT(UOdysseyCachedRenderTarget_UnloadRenderTarget);

            uint32 bytesPerPixel = params.ComponentsPerPixel * params.BytesPerComponent;
            uint32 stride = bytesPerPixel * textureWidth;
            uint32 totalBytes = stride * textureHeight;

            //Read Texture into a buffer

            FTextureRHIRef sourceTexture = source->GetResource()->TextureRHI;
            FRHILockTextureArgs LockArgs = FRHILockTextureArgs::Lock2D(sourceTexture, 0, RLM_ReadOnly, false);
            FRHILockTextureResult LockResult = RHICmdList.LockTexture(LockArgs);

            FUniqueBuffer rawBuffer = FUniqueBuffer::Alloc(totalBytes);
            for (uint32 y = 0; y < textureHeight; y++)
            {
                void* src = (uint8*)(LockResult.Data) + LockResult.Stride * y;
                void* dst = (uint8*)(rawBuffer.GetData()) + stride * y;
                FMemory::Memcpy(dst, src, stride);
            }
            RHICmdList.UnlockTexture(LockArgs);

            //Compress the buffer
            FRLECompressedBuffer RLEBuffer;

            RLEBuffer.TextureWidth = source->GetSurfaceWidth();
            RLEBuffer.TextureHeight = source->GetSurfaceHeight();
            RLEBuffer.TileWidth = params.TileWidth;
            RLEBuffer.TileHeight = params.TileHeight;
            RLEBuffer.ComponentsPerPixel = params.ComponentsPerPixel;
            RLEBuffer.BytesPerComponent = params.BytesPerComponent;
            RLEBuffer.RedIndex = params.RedIndex;
            RLEBuffer.GreenIndex = params.GreenIndex;
            RLEBuffer.BlueIndex = params.BlueIndex;
            RLEBuffer.AlphaIndex = params.AlphaIndex;

            CompressTiles(rawBuffer, RLEBuffer);

            promise->SetValue(MoveTemp(RLEBuffer));
        }
    );

    return promise;
}

void
FOdysseyTiledRLE::TestDecompressTiledRLEShader(uint32 iTextureWidth, uint32 iTextureHeight, FRLECompressedBuffer& oRLEBuffer)
{
    oRLEBuffer.TextureWidth= iTextureWidth;
    oRLEBuffer.TextureHeight= iTextureHeight;
    oRLEBuffer.TileWidth = 64;
    oRLEBuffer.TileHeight = 64;
    oRLEBuffer.BytesPerComponent = 1;
    oRLEBuffer.ComponentsPerPixel = 4;

    FIntPoint numTiles = GetNumTiles(oRLEBuffer);

    //1st Tile
    oRLEBuffer.TileDescriptors.AddZeroed(numTiles.X * numTiles.Y);
    for (int i = 0; i < numTiles.X * numTiles.Y; i++)
    {
        //Tile is empty by default, we use negative values to identify empty tiles
        oRLEBuffer.TileDescriptors[i] = { /*position*/-1, /*count*/0 };
    }
    oRLEBuffer.TileDescriptors[0] = { /*position*/0, /*count*/4 };

    uint8 one = 255;
    uint8 rand = 16;
    uint8 zero = 0;
    //FFloat16 one(1.0f);
    //FFloat16 zero(0.0f);

    uint32 numPixelsInTile = oRLEBuffer.TileWidth * oRLEBuffer.TileHeight;

    oRLEBuffer.RLEPositions.Add(0); //start from first section
    oRLEBuffer.RLEPositions.Add(numPixelsInTile * 1 / 4); //start from second section
    oRLEBuffer.RLEPositions.Add(numPixelsInTile * 2 / 4); //start from third section (sequence)
    oRLEBuffer.RLEPositions.Add(numPixelsInTile * 3 / 4 * -1); //start from third section (sequence)
    oRLEBuffer.RLEPositions.Add(numPixelsInTile); //end at last section of the tile

    oRLEBuffer.RLEData.AddUninitialized(4 * oRLEBuffer.BytesPerComponent);
    FMemory::Memcpy(oRLEBuffer.RLEData.GetData() + (0 * oRLEBuffer.BytesPerComponent), &rand, oRLEBuffer.BytesPerComponent);
    FMemory::Memcpy(oRLEBuffer.RLEData.GetData() + (1 * oRLEBuffer.BytesPerComponent), &rand, oRLEBuffer.BytesPerComponent);
    FMemory::Memcpy(oRLEBuffer.RLEData.GetData() + (2 * oRLEBuffer.BytesPerComponent), &zero, oRLEBuffer.BytesPerComponent);
    FMemory::Memcpy(oRLEBuffer.RLEData.GetData() + (3 * oRLEBuffer.BytesPerComponent), &one, oRLEBuffer.BytesPerComponent);

    oRLEBuffer.RLEData.AddUninitialized(4 * oRLEBuffer.BytesPerComponent);
    FMemory::Memcpy(oRLEBuffer.RLEData.GetData() + (4 * oRLEBuffer.BytesPerComponent), &zero, oRLEBuffer.BytesPerComponent);
    FMemory::Memcpy(oRLEBuffer.RLEData.GetData() + (5 * oRLEBuffer.BytesPerComponent), &rand, oRLEBuffer.BytesPerComponent);
    FMemory::Memcpy(oRLEBuffer.RLEData.GetData() + (6 * oRLEBuffer.BytesPerComponent), &rand, oRLEBuffer.BytesPerComponent);
    FMemory::Memcpy(oRLEBuffer.RLEData.GetData() + (7 * oRLEBuffer.BytesPerComponent), &one, oRLEBuffer.BytesPerComponent);

    uint32 pointerToSequence = 0;
    oRLEBuffer.RLEData.AddUninitialized(sizeof(pointerToSequence));
    FMemory::Memcpy(oRLEBuffer.RLEData.GetData() + (8 * oRLEBuffer.BytesPerComponent), &pointerToSequence, sizeof(pointerToSequence));

    oRLEBuffer.SequenceData.AddUninitialized(numPixelsInTile / 2 * 4 * oRLEBuffer.BytesPerComponent);
    for (uint32 i = 0; i < numPixelsInTile; i++)
    {
        if (i&1)
        {
            FMemory::Memcpy(oRLEBuffer.SequenceData.GetData() + (i * 4 + 0) * oRLEBuffer.BytesPerComponent, &rand, oRLEBuffer.BytesPerComponent);
            FMemory::Memcpy(oRLEBuffer.SequenceData.GetData() + (i * 4 + 1) * oRLEBuffer.BytesPerComponent, &rand, oRLEBuffer.BytesPerComponent);
            FMemory::Memcpy(oRLEBuffer.SequenceData.GetData() + (i * 4 + 2) * oRLEBuffer.BytesPerComponent, &zero, oRLEBuffer.BytesPerComponent);
            FMemory::Memcpy(oRLEBuffer.SequenceData.GetData() + (i * 4 + 3) * oRLEBuffer.BytesPerComponent, &one, oRLEBuffer.BytesPerComponent);
        }
        else
        {
            FMemory::Memcpy(oRLEBuffer.SequenceData.GetData() + (i * 4 + 0) * oRLEBuffer.BytesPerComponent, &zero, oRLEBuffer.BytesPerComponent);
            FMemory::Memcpy(oRLEBuffer.SequenceData.GetData() + (i * 4 + 1) * oRLEBuffer.BytesPerComponent, &rand, oRLEBuffer.BytesPerComponent);
            FMemory::Memcpy(oRLEBuffer.SequenceData.GetData() + (i * 4 + 2) * oRLEBuffer.BytesPerComponent, &rand, oRLEBuffer.BytesPerComponent);
            FMemory::Memcpy(oRLEBuffer.SequenceData.GetData() + (i * 4 + 3) * oRLEBuffer.BytesPerComponent, &one, oRLEBuffer.BytesPerComponent);
        }
    }

    oRLEBuffer.RLEData.AddUninitialized(4 * oRLEBuffer.BytesPerComponent);
    FMemory::Memcpy(oRLEBuffer.RLEData.GetData() + (12 * oRLEBuffer.BytesPerComponent), &zero, oRLEBuffer.BytesPerComponent);
    FMemory::Memcpy(oRLEBuffer.RLEData.GetData() + (13 * oRLEBuffer.BytesPerComponent), &rand, oRLEBuffer.BytesPerComponent);
    FMemory::Memcpy(oRLEBuffer.RLEData.GetData() + (14 * oRLEBuffer.BytesPerComponent), &rand, oRLEBuffer.BytesPerComponent);
    FMemory::Memcpy(oRLEBuffer.RLEData.GetData() + (15 * oRLEBuffer.BytesPerComponent), &one, oRLEBuffer.BytesPerComponent);

    if (oRLEBuffer.TileDescriptors.IsEmpty()) //a buffer must contain something otherwise we get an error.
        oRLEBuffer.TileDescriptors.AddUninitialized();

    if (oRLEBuffer.RLEPositions.IsEmpty()) //a buffer must contain something otherwise we get an error.
        oRLEBuffer.RLEPositions.AddUninitialized();

    if (oRLEBuffer.RLEData.IsEmpty()) //a buffer must contain something otherwise we get an error.
        oRLEBuffer.RLEData.AddUninitialized();

    if (oRLEBuffer.SequenceData.IsEmpty()) //a buffer must contain something otherwise we get an error.
        oRLEBuffer.SequenceData.AddUninitialized();
}
