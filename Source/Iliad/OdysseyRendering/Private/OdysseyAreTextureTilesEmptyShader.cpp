// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyAreTextureTilesEmptyShader.h"
#include "GlobalShader.h"
#include "RenderGraphBuilder.h"
#include "RenderGraphEvent.h"
#include "RenderGraphUtils.h"
#include "RHIGPUReadback.h"
#include "ScreenPass.h"

DECLARE_STATS_GROUP(TEXT("OdysseyAreTextureTilesEmptyShader"), STATGROUP_OdysseyAreTextureTilesEmptyShader, STATCAT_Advanced);
DECLARE_CYCLE_STAT(TEXT("OdysseyAreTextureTilesEmptyShader Execute"), STAT_OdysseyAreTextureTilesEmptyShader_Execute, STATGROUP_OdysseyAreTextureTilesEmptyShader);

// This class carries our parameter declarations and acts as the bridge between cpp and HLSL.
class FOdysseyAreTextureTilesEmptyShader : public FGlobalShader
{
public:

    DECLARE_GLOBAL_SHADER(FOdysseyAreTextureTilesEmptyShader);
    SHADER_USE_PARAMETER_STRUCT(FOdysseyAreTextureTilesEmptyShader, FGlobalShader);

    BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
        SHADER_PARAMETER_RDG_TEXTURE_SRV(Texture2D, InputTexture)
        SHADER_PARAMETER_RDG_BUFFER_SRV(StructuredBuffer<int2>, TilePositions) //TilePositions are defined in pixels
        SHADER_PARAMETER(uint32, TileSize) //TileSize is defines in pixels
        SHADER_PARAMETER(uint32, NumTiles)
        SHADER_PARAMETER_RDG_BUFFER_UAV(RWBuffer<uint>, IsTileEmpty)
    END_SHADER_PARAMETER_STRUCT()

public:
    static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
    {
        return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5) && !IsConsolePlatform(Parameters.Platform);
    }

    static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
    {
        FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);
    }

private:
};

// This will tell the engine to create the shader and where the shader entry point is.
//                            ShaderType                            ShaderPath                     Shader function name    Type
IMPLEMENT_GLOBAL_SHADER(FOdysseyAreTextureTilesEmptyShader, "/OdysseyShaders/Private/OdysseyAreTextureTilesEmpty.usf", "MainCS", SF_Compute);

namespace Odyssey::Shaders {

void
AddAreTextureTilesEmptyPass(FRDGBuilder& GraphBuilder, ERHIFeatureLevel::Type InFeatureLevel, FRDGTextureRef InTexture, const TArray<FIntPoint>& InTilePositions, uint32 InTileSize, TSharedPtr<FRHIGPUBufferReadback> InReadBack)
{
    SCOPE_CYCLE_COUNTER(STAT_OdysseyAreTextureTilesEmptyShader_Execute);
    DECLARE_GPU_STAT(OdysseyAreTextureTilesEmptyShader);
    RDG_EVENT_SCOPE(GraphBuilder, "OdysseyAreTextureTilesEmptyShader");
    RDG_EVENT_SCOPE_STAT(GraphBuilder, OdysseyAreTextureTilesEmptyShader, "OdysseyAreTextureTilesEmptyShader");

    if (InTilePositions.IsEmpty() || !InReadBack.IsValid())
        return;

    TShaderMapRef<FOdysseyAreTextureTilesEmptyShader> ComputeShader(GetGlobalShaderMap(InFeatureLevel));
    if (!ComputeShader.IsValid())
        return;

    //
    // Upload rectangle data.
    //
    TArray<FIntVector2> TilePositionsData;

    TilePositionsData.Reserve(InTilePositions.Num());

    for (const FIntPoint& TilePosition : InTilePositions)
    {
        TilePositionsData.Emplace(TilePosition.X, TilePosition.Y);
    }

    FRDGBufferRef TilePositionBuffer = CreateStructuredBuffer(
        GraphBuilder,
        TEXT("OdysseyTextureEmptyTilePositions"),
        sizeof(FIntVector2),
        TilePositionsData.Num(),
        TilePositionsData.GetData(),
        sizeof(FIntVector2) *
        TilePositionsData.Num()
    );

    FRDGBufferSRVRef TilePositionSRV = GraphBuilder.CreateSRV(TilePositionBuffer);

    //
    // One uint per rectangle.
    //
    FRDGBufferRef ResultBuffer = GraphBuilder.CreateBuffer(
        FRDGBufferDesc::CreateBufferDesc(
            sizeof(uint32),
            InTilePositions.Num()
        ),
        TEXT("OdysseyTextureRectsEmptyResult")
    );

    FRDGBufferUAVRef ResultUAV = GraphBuilder.CreateUAV(ResultBuffer, PF_R32_UINT);

    //
    // Initialize every rect as empty.
    //
    AddClearUAVPass(GraphBuilder, ResultUAV, 1u);

    FRDGTextureSRVDesc SRVDesc = FRDGTextureSRVDesc::CreateForMipLevel(InTexture, 0);

    FOdysseyAreTextureTilesEmptyShader::FParameters* PassParameters = GraphBuilder.AllocParameters<FOdysseyAreTextureTilesEmptyShader::FParameters>();
    PassParameters->InputTexture = GraphBuilder.CreateSRV(SRVDesc);
    PassParameters->TilePositions = TilePositionSRV;
    PassParameters->NumTiles = InTilePositions.Num();
    PassParameters->TileSize = InTileSize;
    PassParameters->IsTileEmpty = ResultUAV;

    FComputeShaderUtils::AddPass(
        GraphBuilder,
        RDG_EVENT_NAME("ExecuteOdysseyAreTextureTilesEmptyShader"),
        GSupportsEfficientAsyncCompute ? ERDGPassFlags::AsyncCompute : ERDGPassFlags::Compute,
        ComputeShader,
        PassParameters,
        FIntVector(
            InTilePositions.Num(),
            1,
            1
        )
        /*FIntVector(
            FMath::DivideAndRoundUp(InTileSize,(uint32)8),
            FMath::DivideAndRoundUp(InTileSize,(uint32)8),
            InTilePositions.Num()
        )*/
    );

    AddEnqueueCopyPass(
        GraphBuilder,
        InReadBack.Get(),
        ResultBuffer,
        sizeof(uint32) * InTilePositions.Num()
    );
}

}
