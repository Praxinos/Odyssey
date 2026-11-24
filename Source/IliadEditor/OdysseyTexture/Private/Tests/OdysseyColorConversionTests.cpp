// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2026

#if WITH_DEV_AUTOMATION_TESTS
#include "Engine/TextureRenderTarget2D.h"
#include "Misc/AutomationTest.h"
#include "RenderGraphBuilder.h"
#include "RenderGraphUtils.h"
#include "RenderingThread.h"
#include "TextureResource.h"
#include "UObject/StrongObjectPtr.h"

#include "OdysseyColorConversion.h"
#include "OdysseyColorConversionPass.h"
#include "OdysseyTextureRenderingAbility.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOdysseyColorConversionDestinationAreaTest, "Odyssey.Rendering.ColorConversion.DestinationArea", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

/**
 * A converted render into an area of a larger destination converts that area only, at its place, and leaves the rest of
 * the destination unchanged, including an area partly outside the destination.
 */
bool FOdysseyColorConversionDestinationAreaTest::RunTest(const FString& Parameters)
{
    if (!TestTrue(TEXT("OpenColorIO converts between linear and sRGB encoded colors"), FOdysseyColorConversions::IsAvailable()))
    {
        return false;
    }

    constexpr int32 Size = 64;
    const FLinearColor Background(0.0f, 0.0f, 1.0f, 1.0f);
    const FLinearColor Content(0.5f, 0.5f, 0.5f, 1.0f);

    // A render function filling the destination area it is given with the content color.
    IOdysseyTextureRenderingAbility::FRenderFunction Fill = [Content](FRDGBuilder& GraphBuilder, ERHIFeatureLevel::Type,
        FRDGTextureRef Destination, const FIntRect&, const FIntRect& DestinationRect, const FMatrix&)
    {
        AddClearRenderTargetPass(GraphBuilder, Destination, Content, DestinationRect);
    };
    IOdysseyTextureRenderingAbility::FRenderFunction Converted = IOdysseyTextureRenderingAbility::BuildColorConvertRenderFunction(Fill, EOdysseyColorConversion::Encode);

    for (const FIntRect& Area : { FIntRect(16, 8, 40, 30), FIntRect(48, 50, 80, 70) })
    {
        TStrongObjectPtr<UTextureRenderTarget2D> Target(NewObject<UTextureRenderTarget2D>());
        Target->InitCustomFormat(Size, Size, PF_A32B32G32R32F, true);
        Target->UpdateResourceImmediate(true);
        ENQUEUE_RENDER_COMMAND(OdysseyColorConversionTest)(
            [Target, Converted, Area, Background](FRHICommandListImmediate& RHICmdList)
            {
                FRDGBuilder Graph(RHICmdList);
                FRDGTextureRef Destination = Target->GetRenderTargetResource()->GetRenderTargetTexture(Graph);
                AddClearRenderTargetPass(Graph, Destination, Background);
                Converted(Graph, GMaxRHIFeatureLevel, Destination, FIntRect(FIntPoint::ZeroValue, Area.Size()), Area, FMatrix::Identity);
                Graph.Execute();
            });
        FlushRenderingCommands();

        TArray<FLinearColor> Pixels;
        FReadSurfaceDataFlags Flags(RCM_MinMax);
        Flags.SetLinearToGamma(false);
        Target->GameThread_GetRenderTargetResource()->ReadLinearColorPixels(Pixels, Flags);
        if (!TestEqual(TEXT("Pixel count"), Pixels.Num(), Size * Size))
        {
            return false;
        }

        const float Encoded = ApplyColorConversion(Content.R, EOdysseyColorConversion::Encode);
        FIntRect Visible = Area;
        Visible.Clip(FIntRect(0, 0, Size, Size));
        const FIntPoint Inside = (Visible.Min + Visible.Max) / 2;
        const FLinearColor& InsideColor = Pixels[Inside.Y * Size + Inside.X];
        TestTrue(FString::Printf(TEXT("The area %s is converted, got %s"), *Area.ToString(), *InsideColor.ToString()),
            FMath::IsNearlyEqual(InsideColor.R, Encoded, 0.01f) && FMath::IsNearlyEqual(InsideColor.A, 1.0f, 0.001f));
        TestTrue(FString::Printf(TEXT("The first pixel of the area %s is converted"), *Area.ToString()),
            FMath::IsNearlyEqual(Pixels[Visible.Min.Y * Size + Visible.Min.X].R, Encoded, 0.01f));
        const FLinearColor& Outside = Pixels[2 * Size + 2];
        TestTrue(FString::Printf(TEXT("Pixels outside the area %s are unchanged, got %s"), *Area.ToString(), *Outside.ToString()),
            Outside.Equals(Background, 0.001f));
    }
    return true;
}

#endif
