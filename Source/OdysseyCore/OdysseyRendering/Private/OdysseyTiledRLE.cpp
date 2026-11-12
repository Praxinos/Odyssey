// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyTiledRLE.h"
#include "GlobalShader.h"
#include "RenderGraphBuilder.h"
#include "RenderGraphEvent.h"
#include "RenderGraphUtils.h"

DECLARE_STATS_GROUP(TEXT("OdysseyDecompressTiledRLEShader"), STATGROUP_OdysseyDecompressTiledRLEShader, STATCAT_Advanced);
DECLARE_CYCLE_STAT(TEXT("OdysseyDecompressTiledRLEShader Execute"), STAT_OdysseyDecompressTiledRLEShader_Execute, STATGROUP_OdysseyDecompressTiledRLEShader);

// This class carries our parameter declarations and acts as the bridge between cpp and HLSL.
class FOdysseyDecompressTiledRLEShader : public FGlobalShader
{
public:

    DECLARE_GLOBAL_SHADER(FOdysseyDecompressTiledRLEShader);
    SHADER_USE_PARAMETER_STRUCT(FOdysseyDecompressTiledRLEShader, FGlobalShader);


    /* class FOdysseyDecompressTiledRLEShader_Perm_TEST : SHADER_PERMUTATION_INT("TEST", 1);
    using FPermutationDomain = TShaderPermutationDomain<
        FOdysseyDecompressTiledRLEShader_Perm_TEST
    >; */

    //Weights 32Ko for an empty 4096x4096 image with 64x64 tiles
    struct FTileDescriptor
    {
        uint32 Position;
        uint32 Count;
    };

    BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
        SHADER_PARAMETER(uint32, TileWidth) //Width of 1 tile in pixels
        SHADER_PARAMETER(uint32, TileHeight) //Height of 1 tile in pixels
        SHADER_PARAMETER(uint32, TextureHeight) //Width of the final Texture
        SHADER_PARAMETER(uint32, TextureWidth) //Height of the final Texture
        SHADER_PARAMETER(uint32, ValueSize) //0 = RGBA8, 1 = RGBA16F, 2 = RGBA32F
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
FOdysseyTiledRLE::DecompressRenderThread(FRDGBuilder& iGraphBuilder, const FRLEBuffer& iBuffer, const FCompressionParams& Params)
{
    SCOPE_CYCLE_COUNTER(STAT_OdysseyDecompressTiledRLEShader_Execute);
    DECLARE_GPU_STAT(OdysseyDecompressTiledRLEShader);
    RDG_EVENT_SCOPE(iGraphBuilder, "OdysseyDecompressTiledRLEShader");
    RDG_GPU_STAT_SCOPE(iGraphBuilder, OdysseyDecompressTiledRLEShader);

    //typename FOdysseyDecompressTiledRLEShader::FPermutationDomain PermutationVector;

    // Add any static permutation options here
    // PermutationVector.Set<FOdysseyDecompressTiledRLEShader::FMyPermutationName>(12345);

    TShaderMapRef<FOdysseyDecompressTiledRLEShader> ComputeShader(GetGlobalShaderMap(GMaxRHIFeatureLevel)/*, PermutationVector*/);
    if (!ComputeShader.IsValid())
        return nullptr;

    FOdysseyDecompressTiledRLEShader::FParameters* PassParameters = iGraphBuilder.AllocParameters<FOdysseyDecompressTiledRLEShader::FParameters>();
    FRDGTextureDesc desc = FRDGTextureDesc::Create2D(
        FIntPoint(Params.TextureWidth, Params.TextureHeight),
        Params.PixelFormat,
        FClearValueBinding::Transparent,
        ETextureCreateFlags::ShaderResource | ETextureCreateFlags::RenderTargetable | ETextureCreateFlags::UAV
    );
    FRDGTextureRef outputTexture = iGraphBuilder.CreateTexture(desc, TEXT("FOdysseyTiledRLE::outputTexture"));
    AddClearRenderTargetPass(iGraphBuilder, outputTexture, FLinearColor::Transparent);

    PassParameters->TileWidth = Params.TileWidth;
    PassParameters->TileHeight = Params.TileHeight;
    PassParameters->TextureWidth = Params.TextureWidth;
    PassParameters->TextureHeight = Params.TextureHeight;
    PassParameters->ValueSize = 1;

    TArray<int32> rlePositions;
    TArray<uint8> rleData;

    uint8 one = 255;
    uint8 zero = 0;
    PassParameters->ValueSize = 1;

    //uint16 one = FFloat16(1.0f).Encoded;
    //uint16 zero = FFloat16(0.0f).Encoded;
    //PassParameters->ValueSize = 2;

    //FFloat16 one(1.0f);
    //FFloat16 zero(0.0f);
    //PassParameters->ValueSize = 2;

    rlePositions.Add(0);
    rleData.AddUninitialized(4 * PassParameters->ValueSize);
    FMemory::Memcpy(rleData.GetData() + (0 * PassParameters->ValueSize), &one, PassParameters->ValueSize);
    FMemory::Memcpy(rleData.GetData() + (1 * PassParameters->ValueSize), &zero, PassParameters->ValueSize);
    FMemory::Memcpy(rleData.GetData() + (2 * PassParameters->ValueSize), &zero, PassParameters->ValueSize);
    FMemory::Memcpy(rleData.GetData() + (3 * PassParameters->ValueSize), &one, PassParameters->ValueSize);

    rlePositions.Add(64 * 16);
    rleData.AddUninitialized(4 * PassParameters->ValueSize);
    FMemory::Memcpy(rleData.GetData() + (4 * PassParameters->ValueSize), &zero, PassParameters->ValueSize);
    FMemory::Memcpy(rleData.GetData() + (5 * PassParameters->ValueSize), &one, PassParameters->ValueSize);
    FMemory::Memcpy(rleData.GetData() + (6 * PassParameters->ValueSize), &zero, PassParameters->ValueSize);
    FMemory::Memcpy(rleData.GetData() + (7 * PassParameters->ValueSize), &one, PassParameters->ValueSize);

    rlePositions.Add(64 * 32);
    rleData.AddUninitialized(4 * PassParameters->ValueSize);
    FMemory::Memcpy(rleData.GetData() + (8 * PassParameters->ValueSize), &zero, PassParameters->ValueSize);
    FMemory::Memcpy(rleData.GetData() + (9 * PassParameters->ValueSize), &zero, PassParameters->ValueSize);
    FMemory::Memcpy(rleData.GetData() + (10 * PassParameters->ValueSize), &one, PassParameters->ValueSize);
    FMemory::Memcpy(rleData.GetData() + (11 * PassParameters->ValueSize), &one, PassParameters->ValueSize);

    rlePositions.Add(64 * 48);
    rleData.AddUninitialized(4 * PassParameters->ValueSize);
    FMemory::Memcpy(rleData.GetData() + (12 * PassParameters->ValueSize), &one, PassParameters->ValueSize);
    FMemory::Memcpy(rleData.GetData() + (13 * PassParameters->ValueSize), &one, PassParameters->ValueSize);
    FMemory::Memcpy(rleData.GetData() + (14 * PassParameters->ValueSize), &zero, PassParameters->ValueSize);
    FMemory::Memcpy(rleData.GetData() + (15 * PassParameters->ValueSize), &one, PassParameters->ValueSize);

    FRDGBufferRef rlePositionsBuffer = CreateUploadBuffer(iGraphBuilder, TEXT("RLEPositions"), sizeof(uint32), rlePositions.Num(), rlePositions.GetData(), rlePositions.Num() * sizeof(uint32));
    FRDGBufferRef rleDataBuffer = CreateUploadBuffer(iGraphBuilder, TEXT("RLEData"), sizeof(uint32), rleData.Num(), rleData.GetData(), rleData.Num() * sizeof(uint32));

    PassParameters->OutputTexture = iGraphBuilder.CreateUAV(FRDGTextureUAVDesc(outputTexture), ERDGUnorderedAccessViewFlags::None);
    PassParameters->RLEPositions = iGraphBuilder.CreateSRV(FRDGBufferSRVDesc(rlePositionsBuffer, PF_R32_SINT));
    PassParameters->RLEData = iGraphBuilder.CreateSRV(FRDGBufferSRVDesc(rleDataBuffer, PF_R32_UINT));


    /* const void* RawData = (void*)Params.Input;
    int NumInputs = 2;
    int InputSize = sizeof(int);
    FRDGBufferRef InputBuffer = CreateUploadBuffer(iGraphBuilder, TEXT("InputBuffer"), InputSize, NumInputs, RawData, InputSize * NumInputs);

    PassParameters->Input = iGraphBuilder.CreateSRV(FRDGBufferSRVDesc(InputBuffer, PF_R32_SINT));

    FRDGBufferRef OutputBuffer = iGraphBuilder.CreateBuffer(
        FRDGBufferDesc::CreateBufferDesc(sizeof(int32), 1),
        TEXT("OutputBuffer"));

    PassParameters->Output = iGraphBuilder.CreateUAV(FRDGBufferUAVDesc(OutputBuffer, PF_R32_SINT) ); */

    int TileMapWidth = ((Params.TextureWidth - 1) / Params.TileWidth) + 1;
    int TileMapHeight = ((Params.TextureHeight - 1) / Params.TileHeight) + 1;

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
FOdysseyTiledRLE::DecompressGameThread(const FRLEBuffer& iBuffer, const FCompressionParams& Params, UTextureRenderTarget2D* oTexture)
{
    ENQUEUE_RENDER_COMMAND(SceneDrawCompletion)(
    [iBuffer, Params](FRHICommandListImmediate& RHICmdList)
    {
        FRDGBuilder graphBuilder(RHICmdList);
        FRDGTextureRef outputTexture = DecompressRenderThread(graphBuilder, iBuffer, Params);
        graphBuilder.Execute();
    });
}

FOdysseyTiledRLE::FRLEBuffer
FOdysseyTiledRLE::Compress(UTexture* iTexture, const FCompressionParams& Params)
{
    return FRLEBuffer();
}
