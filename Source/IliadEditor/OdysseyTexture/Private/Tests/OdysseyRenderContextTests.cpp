// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019
#include "OdysseyRenderContext.h"
#include "OdysseyLayerStackRenderContext.h"
#if WITH_DEV_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"
#include "Engine/Texture2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "OdysseyTextureLayerImageRaster.h"
#include "OdysseyTextureLayerFolder.h"
#include "OdysseyTextureLayerStack.h"
#include "RenderingThread.h"
#include "RenderGraphUtils.h"
#include "TextureResource.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/Package.h"

namespace Odyssey::TextureLayerTiledImageTests
{
UTexture2D* MakeTexture(UObject* Outer, ETextureSourceFormat Format);
UOdysseyTextureLayerImageRaster* MakeLayer(UTexture2D* Texture);
}

namespace Odyssey::RenderContextTests
{
IOdysseyTextureRenderingAbility::FRenderFunction Solid(FLinearColor Color)
{
    return [Color](FRDGBuilder& Graph, ERHIFeatureLevel::Type, FRDGTextureRef Target,
        const FIntRect&, const FIntRect& Destination, const FMatrix&)
    {
        AddClearRenderTargetPass(Graph, Target, Color, Destination);
    };
}
TArray<FLinearColor> Read(IOdysseyTextureRenderingAbility* Renderer, const FOdysseyRenderContext* Context)
{
    TStrongObjectPtr<UTextureRenderTarget2D> Target(NewObject<UTextureRenderTarget2D>());
    Target->InitCustomFormat(73, 67, PF_A32B32G32R32F, true);
    Target->UpdateResourceImmediate(true);
    Renderer->Render_GameThread(Target.Get(), FFrameNumber(0), EOdysseyRenderingType::Render, {}, Context);
    FlushRenderingCommands();
    TArray<FLinearColor> Pixels;
    FReadSurfaceDataFlags Flags(RCM_MinMax);
    Flags.SetLinearToGamma(false);
    Target->GameThread_GetRenderTargetResource()->ReadLinearColorPixels(Pixels, Flags);
    return Pixels;
}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOdysseyRenderContextTest, "Odyssey.Rendering.Context.TextureIntegration",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FOdysseyRenderContextTest::RunTest(const FString& Parameters)
{
    using namespace Odyssey::RenderContextTests;
    using namespace Odyssey::TextureLayerTiledImageTests;
    TStrongObjectPtr<UTexture2D> Texture(MakeTexture(GetTransientPackage(), TSF_BGRA8));
    UOdysseyTextureLayerImageRaster* Layer = MakeLayer(Texture.Get());
    UOdysseyLayerStack* Stack = Layer->GetLayerStack();
    const TArray<FLinearColor> Original = Read(Stack, nullptr);
    if (!TestEqual(TEXT("Read all pixels"), Original.Num(), 73 * 67)) return false;
    const float OriginalOpacity = Layer->GetOpacity();
    FOdysseyRenderContext Context;
    FOdysseyLayerStackRenderContext& LayerContext = Context.FindOrAddExtension<FOdysseyLayerStackRenderContext>();
    LayerContext.bUseDisplayOnlyCurrentLayer = false;
    FOdysseyRenderContext ContextCopy = Context;
    ContextCopy.FindOrAddExtension<FOdysseyLayerStackRenderContext>().bUseDisplayOnlyCurrentLayer = true;
    TestFalse(TEXT("Copied request retains independent layer settings"),
        Context.GetExtension<FOdysseyLayerStackRenderContext>()->bUseDisplayOnlyCurrentLayer);
    FOdysseyRenderOverride& Override = Context.Overrides.FindOrAdd(Layer->GetRenderingId());
    FOdysseyLayerStackRenderOverride& LayerOverride = LayerContext.Overrides.FindOrAdd(Layer->GetRenderingId());
    Override.RenderReplacement = Solid(FLinearColor::Red);
    LayerOverride.Opacity = 0.5f;
    TArray<FLinearColor> Pixels = Read(Stack, &Context);
    TestTrue(TEXT("Parent applies temporary opacity"), Pixels.Num() == Original.Num() && FMath::IsNearlyEqual(Pixels[33 * 73 + 36].A, 0.5f, 0.001f));
    TestEqual(TEXT("Opacity asset is unchanged"), Layer->GetOpacity(), OriginalOpacity);
    LayerOverride.Opacity = 1.0f;
    Override.RenderReplacement = {};
    LayerOverride.Opacity.Reset();
    Pixels = Read(Stack, &Context);
    TestTrue(TEXT("Reset overrides restores document pixels"), Pixels.Num() > 0 && Pixels[33 * 73 + 36].Equals(Original[33 * 73 + 36], 0.001f));
    FOdysseyRenderOverride SavedOverride;
    SavedOverride.RenderReplacement = Solid(FLinearColor::Red);
    Context.Overrides.Add(Layer->GetRenderingId(), SavedOverride);
    TestNotNull(TEXT("Get override"), Context.Overrides.Find(Layer->GetRenderingId()));
    Context.Overrides.Remove(Layer->GetRenderingId());
    Pixels = Read(Stack, &Context);
    TestTrue(TEXT("Removing override restores document pixels"), Pixels.Num() > 0 && Pixels[33 * 73 + 36].Equals(Original[33 * 73 + 36], 0.001f));
    FOdysseyRenderOverride& ActiveOverride = Context.Overrides.FindOrAdd(Layer->GetRenderingId());
    ActiveOverride.RenderReplacement = Solid(FLinearColor::Red);
    LayerOverride.bVisible = false;
    Pixels = Read(Stack, &Context);
    TestTrue(TEXT("Temporary visibility clears output"), Pixels.Num() > 0 && Pixels[33 * 73 + 36].A == 0.0f);
    LayerOverride.bVisible = true;
    FOdysseyRenderPreview Preview;
    int32 Notifications = 0;
    Preview.OnInvalidated().AddLambda([&Notifications](const TArray<FIntRect>&) { ++Notifications; });
    Preview.SetContext(Context);
    Preview.Invalidate({FIntRect(0, 0, 8, 8)});
    Pixels = Read(Stack, &Preview.GetContext());
    TestTrue(TEXT("View has red preview"), Pixels.Num() > 0 && Pixels[33 * 73 + 36].Equals(FLinearColor::Red, 0.001f));
    Pixels = Read(Stack, nullptr);
    TestTrue(TEXT("Other view retains document"), Pixels.Num() > 0 && Pixels[33 * 73 + 36].Equals(Original[33 * 73 + 36], 0.001f));
    Preview.Reset();
    TestEqual(TEXT("Preview revisions"), Preview.GetRevision(), uint64(3));
    TestEqual(TEXT("Local notifications"), Notifications, 3);
    Pixels = Read(Stack, &Preview.GetContext());
    TestTrue(TEXT("Cancel restores original pixels"), Pixels.Num() > 0 && Pixels[33 * 73 + 36].Equals(Original[33 * 73 + 36], 0.001f));

    UOdysseyLayer* Folder = Stack->AddLayer(UOdysseyTextureLayerFolder::StaticClass());
    UOdysseyLayer* Selected = Stack->AddLayer(UOdysseyTextureLayerImageRaster::StaticClass(), Folder);
    Context.Overrides.FindOrAdd(Selected->GetRenderingId()).RenderReplacement = Solid(FLinearColor::Green);
    LayerContext.Overrides.FindOrAdd(Folder->GetRenderingId()).BlendMode = EOdysseyBlendMode::Multiply;
    Pixels = Read(Stack, &Context);
    TestTrue(TEXT("Temporary group multiply uses the lower backdrop"), Pixels.Num() > 0 && Pixels[33 * 73 + 36].R < 0.001f && Pixels[33 * 73 + 36].G < 0.001f);
    LayerContext.Overrides.FindOrAdd(Folder->GetRenderingId()).BlendMode.Reset();
    const FGuid ExcludedId = Layer->GetRenderingId();
    Context.Filter = [ExcludedId](const IOdysseyTextureRenderingAbility& Node,
        const TArray<const IOdysseyTextureRenderingAbility*>& Parents)
    {
        return Node.GetRenderingId() != ExcludedId;
    };
    Pixels = Read(Stack, &Context);
    TestTrue(TEXT("Selected descendant survives unselected ancestor"), Pixels.Num() > 0 && Pixels[33 * 73 + 36].Equals(FLinearColor::Green, 0.001f));
    LayerContext.Overrides.FindOrAdd(Selected->GetRenderingId()).bInheritsAlpha = true;
    Pixels = Read(Stack, &Context);
    TestTrue(TEXT("Alpha inheritance clips against empty isolated backdrop"), Pixels.Num() > 0 && Pixels[33 * 73 + 36].A == 0.0f);
    LayerContext.Overrides.FindOrAdd(Selected->GetRenderingId()).bInheritsAlpha.Reset();
    Context.Overrides.FindOrAdd(Folder->GetRenderingId()).ContentReplacement = Solid(FLinearColor::Blue);
    Pixels = Read(Stack, &Context);
    TestTrue(TEXT("Composite content replacement"), Pixels.Num() > 0 && Pixels[33 * 73 + 36].Equals(FLinearColor::Blue, 0.001f));
    Context.Overrides.FindOrAdd(Folder->GetRenderingId()).ContentReplacement = {};
    Context.Filter = [](const IOdysseyTextureRenderingAbility&, const TArray<const IOdysseyTextureRenderingAbility*>&) { return false; };
    Pixels = Read(Stack, &Context);
    TestTrue(TEXT("Empty selection clears output"), Pixels.Num() > 0 && Pixels[33 * 73 + 36].A == 0.0f);
    Context.Filter = {};
    Context.Overrides.FindOrAdd(Stack->GetRenderingId()).RenderReplacement = Solid(FLinearColor::Blue);
    Pixels = Read(Stack, &Context);
    TestTrue(TEXT("Whole stack replacement"), Pixels.Num() > 0 && Pixels[33 * 73 + 36].Equals(FLinearColor::Blue, 0.001f));
    return true;
}
#endif
