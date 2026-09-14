// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyIsTextureEmptyShader.h"
#include "GlobalShader.h"
#include "RenderGraphBuilder.h"
#include "RenderGraphEvent.h"
#include "RenderGraphUtils.h"
#include "RHIGPUReadback.h"
#include "ScreenPass.h"

DECLARE_STATS_GROUP(TEXT("OdysseyIsTextureEmptyShader"), STATGROUP_OdysseyIsTextureEmptyShader, STATCAT_Advanced);
DECLARE_CYCLE_STAT(TEXT("OdysseyIsTextureEmptyShader Execute"), STAT_OdysseyIsTextureEmptyShader_Execute, STATGROUP_OdysseyIsTextureEmptyShader);

// This class carries our parameter declarations and acts as the bridge between cpp and HLSL.
class FOdysseyIsTextureEmptyShader : public FGlobalShader
{
public:

    DECLARE_GLOBAL_SHADER(FOdysseyIsTextureEmptyShader);
    SHADER_USE_PARAMETER_STRUCT(FOdysseyIsTextureEmptyShader, FGlobalShader);

    BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
        SHADER_PARAMETER_RDG_TEXTURE(Texture2D, InputTexture)
        SHADER_PARAMETER_RDG_BUFFER_UAV(RWBuffer<uint>, TextureIsEmpty)
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
IMPLEMENT_GLOBAL_SHADER(FOdysseyIsTextureEmptyShader, "/OdysseyShaders/Private/OdysseyIsTextureEmpty.usf", "MainCS", SF_Compute);

namespace Odyssey::Shaders {

void
AddIsTextureEmptyPass(FRDGBuilder& GraphBuilder, ERHIFeatureLevel::Type InFeatureLevel, FRDGTextureRef InTexture, TSharedPtr<FRHIGPUBufferReadback> InReadBack)
{
    SCOPE_CYCLE_COUNTER(STAT_OdysseyIsTextureEmptyShader_Execute);
    DECLARE_GPU_STAT(OdysseyIsTextureEmptyShader);
    RDG_EVENT_SCOPE(GraphBuilder, "OdysseyIsTextureEmptyShader");
    RDG_EVENT_SCOPE_STAT(GraphBuilder, OdysseyIsTextureEmptyShader, "OdysseyIsTextureEmptyShader");

    TShaderMapRef<FOdysseyIsTextureEmptyShader> ComputeShader(GetGlobalShaderMap(GMaxRHIFeatureLevel));
    if (!ComputeShader.IsValid())
        return;

    //
    // One uint:
    //   1 = texture is empty
    //   0 = texture contains at least one pixel with alpha > 0
    //
    FRDGBufferRef TextureIsEmptyBuffer = GraphBuilder.CreateBuffer(
        FRDGBufferDesc::CreateBufferDesc(sizeof(uint32), 1),
        TEXT("OdysseyIsTextureEmptyResult"));

    FRDGBufferUAVRef TextureIsEmptyUAV = GraphBuilder.CreateUAV(
        TextureIsEmptyBuffer,
        PF_R32_UINT);

    //
    // Initialize result to 1.
    // By default we consider the texture empty
    // The shader will overwrite the result to 0 if the texture has non empty pixels
    //
    AddClearUAVPass(
        GraphBuilder,
        TextureIsEmptyUAV,
        1u);

    FOdysseyIsTextureEmptyShader::FParameters* PassParameters = GraphBuilder.AllocParameters<FOdysseyIsTextureEmptyShader::FParameters>();
    PassParameters->InputTexture = InTexture;
    PassParameters->TextureIsEmpty = TextureIsEmptyUAV;

    const FRDGTextureDesc& TextureDesc = InTexture->Desc;

    FComputeShaderUtils::AddPass(
        GraphBuilder,
        RDG_EVENT_NAME("ExecuteOdysseyIsTextureEmptyShader"),
        GSupportsEfficientAsyncCompute ? ERDGPassFlags::AsyncCompute : ERDGPassFlags::Compute,
        ComputeShader,
        PassParameters,
        FComputeShaderUtils::GetGroupCount(
            TextureDesc.Extent,
            FIntPoint(8, 8)
        )
    );

    AddEnqueueCopyPass(
        GraphBuilder,
        InReadBack.Get(),
        TextureIsEmptyBuffer,
        sizeof(uint32)
    );
}

}
