// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2026

#include "OdysseyColorSpaceShader.h"

#include "OpenColorIOColorSpace.h"
#include "OpenColorIOConfiguration.h"
#include "OpenColorIORendering.h"
#include "OpenColorIOShaderCompilationManager.h"
#include "OpenColorIOWrapper.h"
#include "RenderGraphBuilder.h"
#include "RenderingThread.h"
#include "RenderGraphUtils.h"
#include "ScreenPass.h"
#include "ShaderCompiler.h"
#include "UObject/Package.h"

DEFINE_LOG_CATEGORY_STATIC(LogOdysseyColorSpace, Log, All);

namespace
{
/** The built-in configuration holding the color spaces, and its names for them, newest first. */
const TCHAR* ConfigurationPath = TEXT("ocio://cg-config-latest");
const TCHAR* LinearColorSpaceNames[] = { TEXT("Linear Rec.709 (sRGB)"), TEXT("lin_rec709_srgb") };
const TCHAR* EncodedColorSpaceNames[] = { TEXT("sRGB Encoded Rec.709 (sRGB)"), TEXT("sRGB - Texture"), TEXT("srgb_tex") };

/** The OpenColorIO configuration and transforms converting between linear and sRGB encoded colors. */
class FOdysseyColorManagement
{
public:
    static FOdysseyColorManagement& Get()
    {
        static FOdysseyColorManagement Instance;
        return Instance;
    }

public:
    bool IsAvailable()
    {
        check(IsInGameThread());
        if (!bInitialized)
        {
            bInitialized = true;
            Initialize();
        }
        return bAvailable;
    }

    /** Returns the resources rendering a conversion; invalid when unavailable. */
    FOpenColorIORenderPassResources GetPassResources(EOdysseyColorConversion Conversion)
    {
        if (!IsAvailable() || Conversion == EOdysseyColorConversion::None)
        {
            return FOpenColorIORenderPassResources();
        }
        return FOpenColorIORendering::GetRenderPassResources(Conversion == EOdysseyColorConversion::Encode ? EncodeSettings : DecodeSettings, GMaxRHIFeatureLevel);
    }

private:
    /** Returns the first of Candidates the configuration defines. */
    static FString FindColorSpace(FOpenColorIOWrapperConfig& Wrapper, TArrayView<const TCHAR* const> Candidates)
    {
        for (const TCHAR* Candidate : Candidates)
        {
            for (int32 Index = 0; Index < Wrapper.GetNumColorSpaces(); ++Index)
            {
                if (Wrapper.GetColorSpaceName(Index) == Candidate)
                {
                    return Candidate;
                }
            }
        }
        return FString();
    }

    /** Processes the compilation of the transform shaders and the creation of their lookup textures until a transform is ready. */
    bool WaitUntilReady(const FOpenColorIOColorConversionSettings& Settings)
    {
        constexpr double TimeoutSeconds = 120.0;
        const double Deadline = FPlatformTime::Seconds() + TimeoutSeconds;
        while (!Configuration->IsTransformReady(Settings))
        {
            if (!Configuration->FindTransform(Settings) || FPlatformTime::Seconds() > Deadline)
            {
                return false;
            }
            if (GShaderCompilingManager)
            {
                GShaderCompilingManager->ProcessAsyncResults(false, true);
            }
            GOpenColorIOShaderCompilationManager.ProcessAsyncResults();
            FlushRenderingCommands();
            FPlatformProcess::Sleep(0.01f);
        }
        return true;
    }

    void Initialize()
    {
        // Kept for the whole session: it holds the transform shaders.
        Configuration = NewObject<UOpenColorIOConfiguration>(GetTransientPackage(), NAME_None, RF_Transient);
        Configuration->AddToRoot();
        Configuration->SetConfigurationFile(FFilePath{ ConfigurationPath });
        FOpenColorIOWrapperConfig* Wrapper = Configuration->GetOrCreateConfigWrapper();
        if (!Wrapper)
        {
            UE_LOG(LogOdysseyColorSpace, Error, TEXT("OpenColorIO configuration %s cannot be loaded: colors are not converted."), ConfigurationPath);
            return;
        }

        const FString LinearName = FindColorSpace(*Wrapper, LinearColorSpaceNames);
        const FString EncodedName = FindColorSpace(*Wrapper, EncodedColorSpaceNames);
        if (LinearName.IsEmpty() || EncodedName.IsEmpty())
        {
            UE_LOG(LogOdysseyColorSpace, Error, TEXT("OpenColorIO configuration %s has no linear or sRGB encoded Rec.709 color space: colors are not converted."), ConfigurationPath);
            return;
        }

        // Declaring the color spaces creates the transforms between them.
        const FOpenColorIOColorSpace LinearSpace(LinearName, INDEX_NONE, FString());
        const FOpenColorIOColorSpace EncodedSpace(EncodedName, INDEX_NONE, FString());
        Configuration->DesiredColorSpaces = { LinearSpace, EncodedSpace };
        Configuration->ReloadExistingColorspaces(true);

        EncodeSettings.ConfigurationSource = Configuration;
        EncodeSettings.SourceColorSpace = LinearSpace;
        EncodeSettings.DestinationColorSpace = EncodedSpace;
        DecodeSettings.ConfigurationSource = Configuration;
        DecodeSettings.SourceColorSpace = EncodedSpace;
        DecodeSettings.DestinationColorSpace = LinearSpace;

        // Renders need the transform shaders now, rather than once their asynchronous compilation is done.
        for (const FOpenColorIOColorConversionSettings& Settings : { EncodeSettings, DecodeSettings })
        {
            if (!WaitUntilReady(Settings))
            {
                UE_LOG(LogOdysseyColorSpace, Error, TEXT("OpenColorIO transform %s is unavailable: colors are not converted."), *Settings.ToString());
                return;
            }
        }
        bAvailable = true;
    }

private:
    UOpenColorIOConfiguration* Configuration = nullptr;
    FOpenColorIOColorConversionSettings EncodeSettings;
    FOpenColorIOColorConversionSettings DecodeSettings;
    bool bInitialized = false;
    bool bAvailable = false;
};
}

IOdysseyTextureRenderingAbility::FRenderFunction
FOdysseyColorSpaceShader::WrapRenderFunction(IOdysseyTextureRenderingAbility::FRenderFunction RenderFunction, EOdysseyColorConversion Conversion)
{
    if (!RenderFunction || Conversion == EOdysseyColorConversion::None)
    {
        return RenderFunction;
    }

    FOpenColorIORenderPassResources PassResources = FOdysseyColorManagement::Get().GetPassResources(Conversion);
    if (!PassResources.IsValid())
    {
        return RenderFunction;
    }

    return [RenderFunction = MoveTemp(RenderFunction), PassResources = MoveTemp(PassResources)](FRDGBuilder& GraphBuilder, ERHIFeatureLevel::Type FeatureLevel,
        FRDGTextureRef Destination, const FIntRect& SourceRect, const FIntRect& DestinationRect, const FMatrix& Transform)
    {
        // Full float precision, so that the conversion is not altered by an intermediate quantization.
        FRDGTextureRef Intermediate = GraphBuilder.CreateTexture(FRDGTextureDesc::Create2D(Destination->Desc.Extent,
            PF_A32B32G32R32F, FClearValueBinding::Transparent, TexCreate_ShaderResource | TexCreate_RenderTargetable),
            TEXT("Odyssey.ColorSpace.Intermediate"));
        AddClearRenderTargetPass(GraphBuilder, Intermediate, FLinearColor::Transparent, DestinationRect);
        RenderFunction(GraphBuilder, FeatureLevel, Intermediate, SourceRect, DestinationRect, Transform);

        FIntRect Viewport = DestinationRect;
        Viewport.Clip(FIntRect(FIntPoint::ZeroValue, Destination->Desc.Extent));
        if (Viewport.Width() <= 0 || Viewport.Height() <= 0)
        {
            return;
        }
        // A gamma of 1 applies the transform only.
        FOpenColorIORendering::AddPass_RenderThread(GraphBuilder, FScreenPassViewInfo(FeatureLevel), FeatureLevel,
            FScreenPassTexture(Intermediate, Viewport), FScreenPassRenderTarget(Destination, Viewport, ERenderTargetLoadAction::ELoad),
            PassResources, 1.0f, EOpenColorIOTransformAlpha::None);
    };
}

bool
FOdysseyColorSpaceShader::IsAvailable()
{
    return FOdysseyColorManagement::Get().IsAvailable();
}
