// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyTiledRLE.h"
#include "GlobalShader.h"
#include "RenderGraphBuilder.h"
#include "RenderGraphEvent.h"
#include "RenderGraphUtils.h"
#include "Engine/TextureRenderTarget2D.h"

DECLARE_STATS_GROUP(TEXT("OdysseyDecompressTiledRLEShader"), STATGROUP_OdysseyDecompressTiledRLEShader, STATCAT_Advanced);
DECLARE_CYCLE_STAT(TEXT("OdysseyDecompressTiledRLEShader Execute"), STAT_OdysseyDecompressTiledRLEShader_Execute, STATGROUP_OdysseyDecompressTiledRLEShader);

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
        SHADER_PARAMETER(uint32, BytesPerComponent)
        SHADER_PARAMETER_RDG_BUFFER_SRV(StructuredBuffer<FTileDescriptor>, TileDescriptors) //Contains informations about tiles (which tile contains data and where to find taht data in the RLEBuffer
        SHADER_PARAMETER_RDG_BUFFER_SRV(StructuredBuffer<int32>, RLEPositions) //Contains each valid tile data compressed in RLE
        SHADER_PARAMETER_RDG_BUFFER_SRV(StructuredBuffer<uint32>, RLEData) //Contains each valid tile data compressed in RLE
        SHADER_PARAMETER_RDG_BUFFER_SRV(StructuredBuffer<uint32>, SequenceData) //Contains each valid tile data compressed in RLE
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

    FRDGBufferRef tileDescriptorsBuffer = CreateUploadBuffer(iGraphBuilder, TEXT("TileDescriptors"), sizeof(FTileDescriptor), iBuffer.TileDescriptors.Num(), iBuffer.TileDescriptors.GetData(), iBuffer.TileDescriptors.Num() * sizeof(FTileDescriptor));
    FRDGBufferRef rlePositionsBuffer = CreateUploadBuffer(iGraphBuilder, TEXT("RLEPositions"), sizeof(int32), iBuffer.RLEPositions.Num(), iBuffer.RLEPositions.GetData(), iBuffer.RLEPositions.Num() * sizeof(int32));
    FRDGBufferRef rleDataBuffer = CreateUploadBuffer(iGraphBuilder, TEXT("RLEData"), sizeof(uint32), iBuffer.RLEData.Num(), iBuffer.RLEData.GetData(), iBuffer.RLEData.Num() * sizeof(uint32));
    FRDGBufferRef sequenceDataBuffer = CreateUploadBuffer(iGraphBuilder, TEXT("SequenceData"), sizeof(uint32), iBuffer.SequenceData.Num(), iBuffer.SequenceData.GetData(), iBuffer.SequenceData.Num() * sizeof(uint32));

    PassParameters->TileWidth = iBuffer.TileWidth;
    PassParameters->TileHeight = iBuffer.TileHeight;
    PassParameters->TextureWidth = iBuffer.TextureWidth;
    PassParameters->TextureHeight = iBuffer.TextureHeight;
    PassParameters->BytesPerComponent = iBuffer.BytesPerComponent;
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
    [iBuffer, pixelFormat](FRHICommandListImmediate& RHICmdList)
    {
        FRDGBuilder graphBuilder(RHICmdList);
        FRDGTextureRef outputTexture = DecompressRenderThread(graphBuilder, iBuffer, pixelFormat);
        graphBuilder.Execute();
    });
}

FOdysseyTiledRLE::FRLECompressedBuffer
FOdysseyTiledRLE::Compress(UTexture* iTexture, const FCompressionParams& Params)
{
    return FRLECompressedBuffer();
}

FIntPoint
GetNumTiles(const FOdysseyTiledRLE::FRLECompressedBuffer& iBuffer)
{
    return FIntPoint(
        FMath::Max(uint32(1), ((iBuffer.TextureWidth - 1) / iBuffer.TileWidth) + 1),
        FMath::Max(uint32(1), ((iBuffer.TextureHeight - 1) / iBuffer.TileHeight) + 1)
    );
}

void
FOdysseyTiledRLE::TestDecompressTiledRLEShader(uint32 iTextureWidth, uint32 iTextureHeight, FRLECompressedBuffer& oRLEBuffer)
{
    oRLEBuffer.TextureHeight= iTextureWidth;
    oRLEBuffer.TextureWidth= iTextureHeight;
    oRLEBuffer.TileWidth = 64;
    oRLEBuffer.TileHeight = 64;
    oRLEBuffer.BytesPerComponent = 1;

    FIntPoint numTiles = GetNumTiles(oRLEBuffer);

    //1st Tile
    oRLEBuffer.TileDescriptors.AddZeroed(numTiles.X * numTiles.Y);
    for (int i = 0; i < numTiles.X * numTiles.Y; i++)
    {
        //Tile is empty by default, we use negative values to identify empty tiles
        oRLEBuffer.TileDescriptors[i] = { /*position*/-1, /*count*/0 };
    }
    oRLEBuffer.TileDescriptors[0] = { /*position*/0, /*count*/4 };
    oRLEBuffer.TileDescriptors[1] = { /*position*/5, /*count*/1 };

    uint8 one = 255;
    uint8 zero = 0;
    //FFloat16 one(1.0f);
    //FFloat16 zero(0.0f);

    uint32 numPixelsInTile = oRLEBuffer.TileWidth * oRLEBuffer.TileHeight;

    oRLEBuffer.RLEPositions.Add(0); //start from first section
    oRLEBuffer.RLEPositions.Add(numPixelsInTile * 1 / 4); //start from second section
    oRLEBuffer.RLEPositions.Add(numPixelsInTile * 2 / 4); //start from third section
    oRLEBuffer.RLEPositions.Add(numPixelsInTile * 3 / 4); //start from fourth section
    oRLEBuffer.RLEPositions.Add(numPixelsInTile); //end at last section of the tile

    oRLEBuffer.RLEData.AddUninitialized(4 * oRLEBuffer.BytesPerComponent);
    FMemory::Memcpy(oRLEBuffer.RLEData.GetData() + (0 * oRLEBuffer.BytesPerComponent), &one, oRLEBuffer.BytesPerComponent);
    FMemory::Memcpy(oRLEBuffer.RLEData.GetData() + (1 * oRLEBuffer.BytesPerComponent), &zero, oRLEBuffer.BytesPerComponent);
    FMemory::Memcpy(oRLEBuffer.RLEData.GetData() + (2 * oRLEBuffer.BytesPerComponent), &zero, oRLEBuffer.BytesPerComponent);
    FMemory::Memcpy(oRLEBuffer.RLEData.GetData() + (3 * oRLEBuffer.BytesPerComponent), &one, oRLEBuffer.BytesPerComponent);

    oRLEBuffer.RLEData.AddUninitialized(4 * oRLEBuffer.BytesPerComponent);
    FMemory::Memcpy(oRLEBuffer.RLEData.GetData() + (4 * oRLEBuffer.BytesPerComponent), &zero, oRLEBuffer.BytesPerComponent);
    FMemory::Memcpy(oRLEBuffer.RLEData.GetData() + (5 * oRLEBuffer.BytesPerComponent), &one, oRLEBuffer.BytesPerComponent);
    FMemory::Memcpy(oRLEBuffer.RLEData.GetData() + (6 * oRLEBuffer.BytesPerComponent), &zero, oRLEBuffer.BytesPerComponent);
    FMemory::Memcpy(oRLEBuffer.RLEData.GetData() + (7 * oRLEBuffer.BytesPerComponent), &one, oRLEBuffer.BytesPerComponent);

    oRLEBuffer.RLEData.AddUninitialized(4 * oRLEBuffer.BytesPerComponent);
    FMemory::Memcpy(oRLEBuffer.RLEData.GetData() + (8 * oRLEBuffer.BytesPerComponent), &zero, oRLEBuffer.BytesPerComponent);
    FMemory::Memcpy(oRLEBuffer.RLEData.GetData() + (9 * oRLEBuffer.BytesPerComponent), &zero, oRLEBuffer.BytesPerComponent);
    FMemory::Memcpy(oRLEBuffer.RLEData.GetData() + (10 * oRLEBuffer.BytesPerComponent), &one, oRLEBuffer.BytesPerComponent);
    FMemory::Memcpy(oRLEBuffer.RLEData.GetData() + (11 * oRLEBuffer.BytesPerComponent), &one, oRLEBuffer.BytesPerComponent);

    oRLEBuffer.RLEData.AddUninitialized(4 * oRLEBuffer.BytesPerComponent);
    FMemory::Memcpy(oRLEBuffer.RLEData.GetData() + (12 * oRLEBuffer.BytesPerComponent), &one, oRLEBuffer.BytesPerComponent);
    FMemory::Memcpy(oRLEBuffer.RLEData.GetData() + (13 * oRLEBuffer.BytesPerComponent), &one, oRLEBuffer.BytesPerComponent);
    FMemory::Memcpy(oRLEBuffer.RLEData.GetData() + (14 * oRLEBuffer.BytesPerComponent), &zero, oRLEBuffer.BytesPerComponent);
    FMemory::Memcpy(oRLEBuffer.RLEData.GetData() + (15 * oRLEBuffer.BytesPerComponent), &one, oRLEBuffer.BytesPerComponent);

    //Sequence Tile
    oRLEBuffer.RLEPositions.Add(0); //start of sequence tile
    oRLEBuffer.RLEPositions.Add(numPixelsInTile * -1); //end at last section of the tile

    oRLEBuffer.RLEData.AddUninitialized(4 * oRLEBuffer.BytesPerComponent);
    uint32 pointerToSequence = 0;
    FMemory::Memcpy(oRLEBuffer.RLEData.GetData() + (16 * oRLEBuffer.BytesPerComponent), &pointerToSequence, sizeof(pointerToSequence));

    oRLEBuffer.SequenceData.AddUninitialized(numPixelsInTile * 4 * oRLEBuffer.BytesPerComponent);
    for (uint32 i = 0; i < numPixelsInTile; i++)
    {
        if (i&1)
        {
            FMemory::Memcpy(oRLEBuffer.SequenceData.GetData() + (i * 4 + 0) * oRLEBuffer.BytesPerComponent, &one, oRLEBuffer.BytesPerComponent);
            FMemory::Memcpy(oRLEBuffer.SequenceData.GetData() + (i * 4 + 1) * oRLEBuffer.BytesPerComponent, &zero, oRLEBuffer.BytesPerComponent);
            FMemory::Memcpy(oRLEBuffer.SequenceData.GetData() + (i * 4 + 2) * oRLEBuffer.BytesPerComponent, &zero, oRLEBuffer.BytesPerComponent);
            FMemory::Memcpy(oRLEBuffer.SequenceData.GetData() + (i * 4 + 3) * oRLEBuffer.BytesPerComponent, &one, oRLEBuffer.BytesPerComponent);
        }
        else
        {
            FMemory::Memcpy(oRLEBuffer.SequenceData.GetData() + (i * 4 + 0) * oRLEBuffer.BytesPerComponent, &zero, oRLEBuffer.BytesPerComponent);
            FMemory::Memcpy(oRLEBuffer.SequenceData.GetData() + (i * 4 + 1) * oRLEBuffer.BytesPerComponent, &one, oRLEBuffer.BytesPerComponent);
            FMemory::Memcpy(oRLEBuffer.SequenceData.GetData() + (i * 4 + 2) * oRLEBuffer.BytesPerComponent, &zero, oRLEBuffer.BytesPerComponent);
            FMemory::Memcpy(oRLEBuffer.SequenceData.GetData() + (i * 4 + 3) * oRLEBuffer.BytesPerComponent, &one, oRLEBuffer.BytesPerComponent);
        }
    }

    if (oRLEBuffer.TileDescriptors.IsEmpty()) //a buffer must contain something otherwise we get an error.
        oRLEBuffer.TileDescriptors.AddUninitialized();

    if (oRLEBuffer.RLEPositions.IsEmpty()) //a buffer must contain something otherwise we get an error.
        oRLEBuffer.RLEPositions.AddUninitialized();

    if (oRLEBuffer.RLEData.IsEmpty()) //a buffer must contain something otherwise we get an error.
        oRLEBuffer.RLEData.AddUninitialized();

    if (oRLEBuffer.SequenceData.IsEmpty()) //a buffer must contain something otherwise we get an error.
        oRLEBuffer.SequenceData.AddUninitialized();
}
