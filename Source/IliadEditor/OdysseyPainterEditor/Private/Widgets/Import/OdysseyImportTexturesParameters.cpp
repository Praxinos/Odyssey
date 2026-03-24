// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyImportTexturesParameters.h"
#include "Engine/TextureRenderTarget2D.h"
#include "CanvasItem.h"
#include "CanvasRender.h"
#include "CanvasTypes.h"
#include "RenderGraphBuilder.h"
#include "RenderGraphUtils.h"
#include "ScreenPass.h"
#include "Factories/TextureFactory.h"
#include "OdysseyScanCleanerShader.h"

#define LOCTEXT_NAMESPACE "PainterEditor"


FOdysseyImportTexturesParametersGC::FOdysseyImportTexturesParametersGC(FOdysseyImportTexturesParameters* iParameters)
    : mParameters(iParameters)
{
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------ FGCObject API
void
FOdysseyImportTexturesParametersGC::AddReferencedObjects( FReferenceCollector& ioCollector )
{
    ioCollector.AddReferencedObjects( mParameters->mSourceTextures );
    ioCollector.AddReferencedObject( mParameters->mScanCleanerCurve );
}

FString FOdysseyImportTexturesParametersGC::GetReferencerName() const
{
    return TEXT("FOdysseyImportTexturesParametersGC");
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------- FOdysseyImportTexturesParameters

FOdysseyImportTexturesParameters::~FOdysseyImportTexturesParameters()
{
    mScanCleanerCurve->OnUpdateCurve.RemoveAll(this);
}

FOdysseyImportTexturesParameters::FOdysseyImportTexturesParameters()
    : mGC(this)
{
    mScanCleanerCurve = NewObject<UCurveFloat>();
    mScanCleanerCurve->FloatCurve.AddKey(0.f, 0.f);
    mScanCleanerCurve->FloatCurve.AddKey(0.25f, 0.f);
    mScanCleanerCurve->FloatCurve.AddKey(0.75f, 1.f);
    mScanCleanerCurve->FloatCurve.AddKey(1.f, 1.f);
    mScanCleanerCurve->OnUpdateCurve.AddRaw(this, &FOdysseyImportTexturesParameters::OnUpdateCurve);

    mScanCleanerCurveTexture = NewObject<UTexture2D>();

    UpdateScanCleanerCurveTextures();
}

void
FOdysseyImportTexturesParameters::Init(const TArray< UTexture2D* >& iTextures, uint32 iDestinationWidth, uint32 iDestinationHeight)
{
    mSourceTextures = iTextures;
    mDestinationWidth = iDestinationWidth;
    mDestinationHeight = iDestinationHeight;
}

void
FOdysseyImportTexturesParameters::Init(const TArray<FString>& iFilenames, uint32 iDestinationWidth, uint32 iDestinationHeight)
{
    mDestinationWidth = iDestinationWidth;
    mDestinationHeight = iDestinationHeight;

    mSourceTextures.Empty();

    //Convert to textures
    FScopedSlowTask progressBar(iFilenames.Num(), LOCTEXT("texture-editor.import-images.progress-bar.title", "Importing Images"));
    progressBar.MakeDialog();

    TStrongObjectPtr<UTextureFactory> TextureFactory(NewObject<UTextureFactory>());
    TArray<TStrongObjectPtr<UTexture2D>> importedTextures;
    importedTextures.Reserve(iFilenames.Num());
    for (const FString& filename : iFilenames)
    {
        progressBar.EnterProgressFrame();

        UObject* importedObject = UFactory::StaticImportObject(UTexture2D::StaticClass(), GetTransientPackage(), NAME_None, EObjectFlags::RF_NoFlags, *filename, nullptr, TextureFactory.Get());
        UTexture2D* importedTexture = Cast<UTexture2D>(importedObject);
        if (!importedTexture)
            continue;

        importedTextures.Emplace(importedTexture);
    }

    TArray<UTexture2D*> textures;

    for (int i = 0; i < importedTextures.Num(); i++)
    {
        mSourceTextures.Add(importedTextures[i].Get());
    }
}

void
FOdysseyImportTexturesParameters::UpdateScanCleanerCurveTextures()
{
    FOdysseyScanCleanerShader::InitTextureFromCurves(
        mScanCleanerCurveTexture.Get(),
        mScanCleanerCurve->FloatCurve
    );
}

void
FOdysseyImportTexturesParameters::OnUpdateCurve( UCurveBase* Curve, EPropertyChangeType::Type ChangeType)
{
    UpdateScanCleanerCurveTextures();
}

UTextureRenderTarget2D*
FOdysseyImportTexturesParameters::CreateRT() const
{
    UTextureRenderTarget2D* renderTarget = NewObject<UTextureRenderTarget2D>();
    renderTarget->InitAutoFormat(mDestinationWidth, mDestinationHeight);
    renderTarget->UpdateResourceImmediate();
    return renderTarget;
}

FVector2D
FOdysseyImportTexturesParameters::GetTextureScaledSize(int iSourceTextureIndex) const
{
    UTexture2D* sourceTexture = mSourceTextures[iSourceTextureIndex];
    sourceTexture->BlockOnAnyAsyncBuild();
    sourceTexture->SetForceMipLevelsToBeResident( 30.0f );
    sourceTexture->WaitForStreaming();

    FVector2D size(sourceTexture->GetSurfaceWidth(), sourceTexture->GetSurfaceHeight());

    switch(mScaling)
    {
        case EOdysseyImportTextureScaling::None: break;

        case EOdysseyImportTextureScaling::Scale:
        {
            size = FVector2D(mDestinationWidth, mDestinationHeight);
        }
        break;

        case EOdysseyImportTextureScaling::ScaleAndFit:
        {
            float ratio = FMath::Min(float(mDestinationWidth) / sourceTexture->GetSurfaceWidth(), float(mDestinationHeight) / sourceTexture->GetSurfaceHeight());
            size = FVector2D(sourceTexture->GetSurfaceWidth() * ratio, sourceTexture->GetSurfaceHeight() * ratio);
        }
        break;
    }

    return size;
}

FVector2D
FOdysseyImportTexturesParameters::GetTexturePosition(const FVector2D& iTextureSize) const
{
    FVector2D texturePosition;
    switch(mAlignment)
    {
        case EOdysseyImportTextureAlignment::TopLeft: texturePosition = FVector2D::ZeroVector; break;
        case EOdysseyImportTextureAlignment::Top: texturePosition = FVector2D((mDestinationWidth - iTextureSize.X) / 2.f, 0); break;
        case EOdysseyImportTextureAlignment::TopRight: texturePosition = FVector2D(mDestinationWidth - iTextureSize.X, 0); break;
        case EOdysseyImportTextureAlignment::Left: texturePosition = FVector2D(0, (mDestinationHeight - iTextureSize.Y) / 2.f); break;
        case EOdysseyImportTextureAlignment::Center: texturePosition = FVector2D((mDestinationWidth - iTextureSize.X) / 2.f, (mDestinationHeight - iTextureSize.Y) / 2.f); break;
        case EOdysseyImportTextureAlignment::Right: texturePosition = FVector2D(mDestinationWidth - iTextureSize.X, (mDestinationHeight - iTextureSize.Y) / 2.f); break;
        case EOdysseyImportTextureAlignment::BottomLeft: texturePosition = FVector2D(0, mDestinationHeight - iTextureSize.Y); break;
        case EOdysseyImportTextureAlignment::Bottom: texturePosition = FVector2D((mDestinationWidth - iTextureSize.X) / 2.f, mDestinationHeight - iTextureSize.Y); break;
        case EOdysseyImportTextureAlignment::BottomRight: texturePosition = FVector2D(mDestinationWidth - iTextureSize.X, mDestinationHeight - iTextureSize.Y); break;
    }

    return texturePosition;
}

FVector2D
FOdysseyImportTexturesParameters::GetTexturePosition(int iSourceTextureIndex) const
{
    FVector2D textureSize = GetTextureScaledSize(iSourceTextureIndex);
    return GetTexturePosition(textureSize);
}


FString
FOdysseyImportTexturesParameters::GetTextureName(int iSourceTextureIndex) const
{
    UTexture2D* sourceTexture = mSourceTextures[iSourceTextureIndex];
    return sourceTexture->GetName();
}

class FOdysseyCanvasRenderTarget final : public FRenderTarget
{
public:
    FOdysseyCanvasRenderTarget(FRDGTextureRef InRDGTexture)
        : RDGTexture(InRDGTexture)
    {}

    FIntPoint GetSizeXY() const override
    {
        return RDGTexture->Desc.Extent;
    }

    const FTextureRHIRef& GetRenderTargetTexture() const override
    {
        static FTextureRHIRef NullRef;
        return NullRef;
    }

    FRDGTextureRef GetRenderTargetTexture(FRDGBuilder&) const override
    {
        return RDGTexture;
    }

    virtual float GetDisplayGamma() const override
    {
        //PATCH: We had to add that to Odyssey
        //so that Canvas can retrieve the right Gamma
        //instead of the default 2.2 gamma

        return 1.0f; //not portable, we assume we need this exact value hear to work in linear gamma
    }

    FRDGTextureRef RDGTexture;
};

void
FOdysseyImportTexturesParameters::Render(UTextureRenderTarget2D* oRenderTarget, int iSourceTextureIndex) const
{
    UTexture2D* sourceTexture = mSourceTextures[iSourceTextureIndex];
    sourceTexture->BlockOnAnyAsyncBuild();
    sourceTexture->SetForceMipLevelsToBeResident( 30.0f );
    sourceTexture->WaitForStreaming();

    UTexture2D* scanCleanerCurveTextureR = mScanCleanerCurveTexture;
    scanCleanerCurveTextureR->BlockOnAnyAsyncBuild();
    scanCleanerCurveTextureR->SetForceMipLevelsToBeResident( 30.0f );
    scanCleanerCurveTextureR->WaitForStreaming();

    FTextureRenderTargetResource* renderTargetResource = oRenderTarget->GameThread_GetRenderTargetResource();

    FVector2D scaledSize = GetTextureScaledSize(iSourceTextureIndex);
    FVector2D texturePosition = GetTexturePosition(scaledSize);

    //Clear RenderTarget
    ENQUEUE_RENDER_COMMAND(SOdysseyImportTexturesDialog_Render)(
        [
            sourceTextureRHI = sourceTexture->GetResource()->TextureRHI,
            adjustCurveTextureRHI = mScanCleanerCurveTexture->GetResource()->TextureRHI,
            renderTargetResource,
            colorSaturation = mScanCleanerColorSaturation,
            colorValue = mScanCleanerColorValue,
            isScanCleanerActivated = mIsScanCleanerActivated,
            scaledSize,
            texturePosition,
            resamplingMethod = mResamplingMethod
        ](FRHICommandListImmediate& RHICmdList)
        {
            TRefCountPtr<IPooledRenderTarget> extractedSourceRenderTarget;

            { //Step 1 : Scan Cleaner
                FRDGBuilder graphBuilder(RHICmdList);
                FRDGTextureRef sourceTexture = graphBuilder.RegisterExternalTexture(CreateRenderTarget(sourceTextureRHI, TEXT("SOdysseyImportTexturesDialog_Render::sourceTexture")));
                FRDGTextureRef adjustCurveTexture = graphBuilder.RegisterExternalTexture(CreateRenderTarget(adjustCurveTextureRHI, TEXT("SOdysseyImportTexturesDialog_Render::adjustCurveTexture")));

                ETextureCreateFlags textureFlags = ETextureCreateFlags::ShaderResource | ETextureCreateFlags::RenderTargetable;
                textureFlags |= sourceTexture->Desc.Flags & ETextureCreateFlags::SRGB;
                FRDGTextureDesc sourceTextureDesc = FRDGTextureDesc::Create2D(
                    sourceTexture->Desc.Extent,
                    PF_FloatRGBA,
                    FClearValueBinding::Transparent,
                    textureFlags
                );
                FRDGTextureRef sourceRenderTarget = graphBuilder.CreateTexture(sourceTextureDesc, TEXT("SOdysseyImportTexturesDialog_Render::sourceRenderTarget"));

                if (isScanCleanerActivated)
                {
                    FOdysseyScanCleanerShader::ScanCleaner(
                        graphBuilder,
                        GMaxRHIFeatureLevel,
                        sourceTexture,
                        adjustCurveTexture,
                        sourceRenderTarget,
                        colorSaturation,
                        colorValue
                    );
                }
                else
                {
                    AddDrawTexturePass(
                        graphBuilder,
                        FScreenPassViewInfo(),
                        sourceTexture,
                        sourceRenderTarget,
                        FIntPoint(0, 0),
                        FIntPoint(0, 0),
                        sourceTexture->Desc.Extent
                    );
                    /*AddCopyTexturePass(
                        graphBuilder,
                        sourceTexture,
                        sourceRenderTarget
                    );*/
                }

                graphBuilder.QueueTextureExtraction(sourceRenderTarget, &extractedSourceRenderTarget);
                graphBuilder.Execute();
            }

            { //Step 2 : Positioning
                FRDGBuilder graphBuilder(RHICmdList);
                FRDGTextureRef destinationTexture = renderTargetResource->GetRenderTargetTexture( graphBuilder );
                AddClearRenderTargetPass(graphBuilder, destinationTexture, FLinearColor::Transparent);

                /*TEST: Canvas */
                FRDGTextureRef sourceRenderTarget = graphBuilder.RegisterExternalTexture(extractedSourceRenderTarget);

                //FCanvas* canvas = FCanvas::Create(graphBuilder, destinationTexture, nullptr, FGameTime(), GMaxRHIFeatureLevel);

                //PATCH: FOdysseyCanvasRenderTarget allows us to use the correct Gamma value here
                FCanvas* canvas = graphBuilder.AllocObject<FCanvas>(
                    graphBuilder.AllocObject<FOdysseyCanvasRenderTarget>(destinationTexture),
                    nullptr,
                    FGameTime(),
                    GMaxRHIFeatureLevel,
                    1.0f
                );
                //PATCH: END

                FTexture* tileTexture = graphBuilder.AllocObject<FTexture>();

                tileTexture->TextureRHI = sourceRenderTarget->GetRHI(); //sourceTextureRHI;
                tileTexture->SamplerStateRHI = Odyssey::GetSamplerStateForAntiAliasing(resamplingMethod);

                FCanvasTileItem TileItem(
                    texturePosition,
                    tileTexture,
                    scaledSize,
                    FColor::White
                );

                canvas->DrawItem(TileItem);
                canvas->Flush_RenderThread(graphBuilder);

                /** END TEST */

                graphBuilder.Execute();
            }
        }
    );
}

uint32
FOdysseyImportTexturesParameters::GetDestinationWidth() const
{
    return mDestinationWidth;
}

uint32
FOdysseyImportTexturesParameters::GetDestinationHeight() const
{
    return mDestinationHeight;
}

TArray< UTexture2D* >
FOdysseyImportTexturesParameters::GetSourceTextures() const
{
    return mSourceTextures;
}

EOdysseyImportTextureAlignment
FOdysseyImportTexturesParameters::GetAlignment() const
{
    return mAlignment;
}

EOdysseyImportTextureScaling
FOdysseyImportTexturesParameters::GetScaling() const
{
    return mScaling;
}

EOdysseyAntiAliasing
FOdysseyImportTexturesParameters::GetResamplingMethod() const
{
    return mResamplingMethod;
}

bool
FOdysseyImportTexturesParameters::GetIsScanCleanerActivated() const
{
    return mIsScanCleanerActivated;
}

UCurveFloat*
FOdysseyImportTexturesParameters::GetScanCleanerCurve() const
{
    return mScanCleanerCurve.Get();
}

float
FOdysseyImportTexturesParameters::GetScanCleanerColorSaturation() const
{
    return mScanCleanerColorSaturation;
}

float
FOdysseyImportTexturesParameters::GetScanCleanerColorValue() const
{
    return mScanCleanerColorValue;
}

void
FOdysseyImportTexturesParameters::SetScanCleanerColorSaturation(float iSaturation)
{
    mScanCleanerColorSaturation = iSaturation;
}

void
FOdysseyImportTexturesParameters::SetScanCleanerColorValue(float iValue)
{
    mScanCleanerColorValue = iValue;
}

void
FOdysseyImportTexturesParameters::SetIsScanCleanerActivated(bool iIsActivated)
{
    mIsScanCleanerActivated = iIsActivated;
}

void
FOdysseyImportTexturesParameters::SetAlignment(EOdysseyImportTextureAlignment iAlignment)
{
    mAlignment = iAlignment;
}

void
FOdysseyImportTexturesParameters::SetScaling(EOdysseyImportTextureScaling iScaling)
{
    mScaling = iScaling;
}

void
FOdysseyImportTexturesParameters::SetResamplingMethod(EOdysseyAntiAliasing iMethod)
{
    mResamplingMethod = iMethod;
}

#undef LOCTEXT_NAMESPACE
