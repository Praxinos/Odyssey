// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2026

#include "OdysseyRasterEditSession.h"

#include "Engine/TextureRenderTarget2D.h"
#include "Math/TranslationMatrix.h"
#include "RenderGraphBuilder.h"
#include "RenderGraphUtils.h"
#include "RenderingThread.h"
#include "TextureResource.h"

#include "OdysseyBlendShader.h"
#include "OdysseyRasterSampling.h"
#include "OdysseyRenderContext.h"
#include "OdysseyTileUtils.h"
#include "OdysseyTiledImage.h"

namespace
{
TArray<TWeakPtr<FOdysseyRasterEditSession>>& ActiveSessions()
{
    static TArray<TWeakPtr<FOdysseyRasterEditSession>> Sessions;
    return Sessions;
}
}

FOdysseyRasterEditSession::~FOdysseyRasterEditSession()
{
    End();
}

FOdysseyRasterEditSession::FOdysseyRasterEditSession(const FGuid& InRenderingId, UOdysseyTiledImage& InImage, UTextureRenderTarget2D& InTarget,
    UTextureRenderTarget2D* InOriginal)
    : RenderingId(InRenderingId)
    , Image(&InImage)
    , Target(&InTarget)
    , Original(InOriginal)
{
}

TSharedPtr<FOdysseyRasterEditSession>
FOdysseyRasterEditSession::Begin(const FGuid& RenderingId, UOdysseyTiledImage& Image, FIntPoint Size, bool bPreserveAlpha)
{
    check(IsInGameThread());
    if (Size.X <= 0 || Size.Y <= 0)
    {
        return nullptr;
    }

    if (TSharedPtr<FOdysseyRasterEditSession> Existing = Find(RenderingId))
    {
        Existing->End();
    }

    const FIntRect Bounds(FIntPoint::ZeroValue, Size);
    UTextureRenderTarget2D* Target = Image.CreateRenderTarget(Size);
    Image.Render(Target, Bounds, FIntPoint::ZeroValue);
    UTextureRenderTarget2D* Original = nullptr;
    if (bPreserveAlpha)
    {
        Original = Image.CreateRenderTarget(Size);
        Image.Render(Original, Bounds, FIntPoint::ZeroValue);
    }
    // The target RHI textures are created and filled on the render thread.
    FlushRenderingCommands();

    TSharedPtr<FOdysseyRasterEditSession> Session = MakeShared<FOdysseyRasterEditSession>(RenderingId, Image, *Target, Original);
    ActiveSessions().Add(Session);
    OnActiveSessionsChanged().Broadcast();
    return Session;
}

TSharedPtr<FOdysseyRasterEditSession>
FOdysseyRasterEditSession::Find(const FGuid& RenderingId)
{
    for (const TWeakPtr<FOdysseyRasterEditSession>& Weak : ActiveSessions())
    {
        TSharedPtr<FOdysseyRasterEditSession> Session = Weak.Pin();
        if (Session && Session->RenderingId == RenderingId)
        {
            return Session;
        }
    }
    return nullptr;
}

void
FOdysseyRasterEditSession::AddOverrides(FOdysseyRenderContext& Context)
{
    for (const TWeakPtr<FOdysseyRasterEditSession>& Weak : ActiveSessions())
    {
        if (TSharedPtr<FOdysseyRasterEditSession> Session = Weak.Pin())
        {
            Context.Overrides.FindOrAdd(Session->RenderingId).ContentReplacement = Session->BuildRenderFunction();
        }
    }
}

FOdysseyRasterEditSession::FOnActiveSessionsChanged&
FOdysseyRasterEditSession::OnActiveSessionsChanged()
{
    static FOnActiveSessionsChanged Delegate;
    return Delegate;
}

FOdysseyRasterEditSession::FOnInvalidated&
FOdysseyRasterEditSession::OnInvalidated()
{
    static FOnInvalidated Delegate;
    return Delegate;
}

const FGuid&
FOdysseyRasterEditSession::GetRenderingId() const
{
    return RenderingId;
}

UTextureRenderTarget2D*
FOdysseyRasterEditSession::GetTarget() const
{
    return Target.Get();
}

bool
FOdysseyRasterEditSession::PreservesAlpha() const
{
    return Original.IsValid();
}

bool
FOdysseyRasterEditSession::IsActive() const
{
    return Target.IsValid();
}

void
FOdysseyRasterEditSession::Invalidate(const TArray<FIntRect>& Rects)
{
    // An empty array would invalidate everything.
    if (!IsActive() || Rects.IsEmpty())
    {
        return;
    }
    // Tools invalidate on every tick: keep the bounds only, so that this does not grow with the edit duration.
    for (const FIntRect& Rect : Rects)
    {
        if (Rect.Width() <= 0 || Rect.Height() <= 0)
        {
            continue;
        }
        if (InvalidBounds.IsSet())
        {
            InvalidBounds->Union(Rect);
        }
        else
        {
            InvalidBounds = Rect;
        }
    }
    OnInvalidated().Broadcast(RenderingId, Rects);
}

bool
FOdysseyRasterEditSession::Commit(const TArray<FIntRect>& Rects)
{
    check(IsInGameThread());
    UOdysseyTiledImage* TiledImage = Image.Get();
    if (!IsActive() || !TiledImage)
    {
        End();
        return false;
    }

    UTextureRenderTarget2D* Source = Target.Get();
    const TArray<FIntRect> Clipped = Odyssey::TileUtils::ClipRects(Rects, FIntRect(0, 0, Source->SizeX, Source->SizeY));
    ApplyOriginalAlpha(Clipped);
    for (const FIntRect& Rect : Clipped)
    {
        TiledImage->CopyFromTexture(Source, Rect, Rect.Min);
    }
    // The tiles are written on the render thread, which reads the target.
    FlushRenderingCommands();
    End();
    if (OnCommitted)
    {
        OnCommitted(Clipped);
    }
    return true;
}

void
FOdysseyRasterEditSession::SetOnCommitted(FOnCommitted InOnCommitted)
{
    OnCommitted = MoveTemp(InOnCommitted);
}

void
FOdysseyRasterEditSession::End()
{
    if (!IsActive())
    {
        return;
    }
    Target.Reset();
    Original.Reset();
    ActiveSessions().RemoveAll([this](const TWeakPtr<FOdysseyRasterEditSession>& Weak)
    {
        return !Weak.IsValid() || Weak.Pin().Get() == this;
    });
    OnActiveSessionsChanged().Broadcast();
    // The views showed the target: they show the tiles again.
    if (InvalidBounds.IsSet())
    {
        const TArray<FIntRect> Rects = { InvalidBounds.GetValue() };
        InvalidBounds.Reset();
        OnInvalidated().Broadcast(RenderingId, Rects);
    }
}

void
FOdysseyRasterEditSession::ApplyOriginalAlpha(const TArray<FIntRect>& Rects) const
{
    if (!Original.IsValid())
    {
        return;
    }

    const FIntRect Bounds(0, 0, Target->SizeX, Target->SizeY);
    TArray<FIntRect> Clipped = Odyssey::TileUtils::ClipRects(Rects, Bounds);
    if (Clipped.IsEmpty())
    {
        return;
    }

    ENQUEUE_RENDER_COMMAND(OdysseyRasterEditSessionApplyOriginalAlpha)(
        [EditedRHI = FTextureRHIRef(Target->GameThread_GetRenderTargetResource()->GetRenderTargetTexture()),
            OriginalRHI = FTextureRHIRef(Original->GameThread_GetRenderTargetResource()->GetRenderTargetTexture()),
            Clipped = MoveTemp(Clipped)](FRHICommandListImmediate& RHICmdList)
        {
            FRDGBuilder GraphBuilder(RHICmdList);
            {
                // The event scope must end before the graph executes.
                RDG_EVENT_SCOPE(GraphBuilder, "Odyssey.RasterEditSession.PreserveAlpha");
                FRDGTextureRef Edited = GraphBuilder.RegisterExternalTexture(CreateRenderTarget(EditedRHI, TEXT("Odyssey.RasterEditSession.Target")));
                FRDGTextureRef OriginalTexture = GraphBuilder.RegisterExternalTexture(CreateRenderTarget(OriginalRHI, TEXT("Odyssey.RasterEditSession.Original")));
                for (const FIntRect& Rect : Clipped)
                {
                    // The colors of the edited pixels (background, Back color mode) with the alpha of the original ones (foreground, Top alpha mode).
                    FOdysseyBlendShader::BlendRect(GraphBuilder, GMaxRHIFeatureLevel, Edited, OriginalTexture, Edited,
                        Rect, Rect, FMatrix::Identity, EOdysseyColorBlendMode::Back, EOdysseyAlphaBlendMode::Top, 1.0f,
                        EOdysseyAntiAliasing::NearestNeighbor);
                }
            }
            GraphBuilder.Execute();
        });
}

IOdysseyTextureRenderingAbility::FRenderFunction
FOdysseyRasterEditSession::BuildRenderFunction() const
{
    // The captured textures stay alive for renders already built, even after the session ended.
    FTextureRHIRef TargetRHI = Target.IsValid() ? Target->GameThread_GetRenderTargetResource()->GetRenderTargetTexture() : nullptr;
    FTextureRHIRef OriginalRHI = Original.IsValid() ? Original->GameThread_GetRenderTargetResource()->GetRenderTargetTexture() : nullptr;
    const FIntPoint Size = Target.IsValid() ? FIntPoint(Target->SizeX, Target->SizeY) : FIntPoint::ZeroValue;
    return [TargetRHI = MoveTemp(TargetRHI), OriginalRHI = MoveTemp(OriginalRHI), Size](FRDGBuilder& GraphBuilder, ERHIFeatureLevel::Type FeatureLevel,
        FRDGTextureRef Destination, const FIntRect& SourceRect, const FIntRect& DestinationRect, const FMatrix& Transform)
    {
        RDG_EVENT_SCOPE(GraphBuilder, "Odyssey.RasterEditSession");
        AddClearRenderTargetPass(GraphBuilder, Destination, FLinearColor::Transparent, DestinationRect);
        if (!TargetRHI)
        {
            return;
        }
        FRDGTextureRef TargetTexture = GraphBuilder.RegisterExternalTexture(CreateRenderTarget(TargetRHI, TEXT("Odyssey.RasterEditSession.Target")));
        if (!OriginalRHI)
        {
            FOdysseyBlendShader::BlendRect(GraphBuilder, FeatureLevel, Destination, TargetTexture, Destination,
                SourceRect, DestinationRect, Transform, EOdysseyColorBlendMode::Normal,
                EOdysseyAlphaBlendMode::Normal, 1.0f, EOdysseyAntiAliasing::Anisotropic);
            return;
        }

        // Preserving alpha: compose the colors of the target with the alpha of the original, over the sampled area only.
        const FIntRect Area = Odyssey::RasterSampling::GetSampledArea(SourceRect, DestinationRect, Transform, Size);
        if (Area.Width() <= 0 || Area.Height() <= 0)
        {
            return;
        }
        FRDGTextureRef Composed = GraphBuilder.CreateTexture(FRDGTextureDesc::Create2D(Area.Size(), TargetTexture->Desc.Format,
            FClearValueBinding::Transparent, TexCreate_ShaderResource | TexCreate_RenderTargetable),
            TEXT("Odyssey.RasterEditSession.Composed"));
        AddCopyTexturePass(GraphBuilder, TargetTexture, Composed, Area.Min, FIntPoint::ZeroValue, Area.Size());
        FRDGTextureRef OriginalTexture = GraphBuilder.RegisterExternalTexture(CreateRenderTarget(OriginalRHI, TEXT("Odyssey.RasterEditSession.Original")));
        // The colors of the edited pixels (background, Back color mode) with the alpha of the original ones (foreground, Top alpha mode).
        FOdysseyBlendShader::BlendRect(GraphBuilder, FeatureLevel, Composed, OriginalTexture, Composed,
            Area, FIntRect(FIntPoint::ZeroValue, Area.Size()), FMatrix::Identity, EOdysseyColorBlendMode::Back,
            EOdysseyAlphaBlendMode::Top, 1.0f, EOdysseyAntiAliasing::NearestNeighbor);
        // The composed texture starts at the area origin in content space.
        const FMatrix FromArea = FTranslationMatrix(FVector(Area.Min.X, Area.Min.Y, 0.0)) * Transform;
        FOdysseyBlendShader::BlendRect(GraphBuilder, FeatureLevel, Destination, Composed, Destination,
            SourceRect, DestinationRect, FromArea, EOdysseyColorBlendMode::Normal,
            EOdysseyAlphaBlendMode::Normal, 1.0f, EOdysseyAntiAliasing::Anisotropic);
    };
}

FOdysseyRasterEditPreview::~FOdysseyRasterEditPreview()
{
    FOdysseyRasterEditSession::OnActiveSessionsChanged().Remove(ActiveSessionsChangedHandle);
    FOdysseyRasterEditSession::OnInvalidated().Remove(InvalidatedHandle);
}

FOdysseyRasterEditPreview::FOdysseyRasterEditPreview(TSharedRef<FOdysseyRenderPreview> InPreview)
    : Preview(MoveTemp(InPreview))
{
    ActiveSessionsChangedHandle = FOdysseyRasterEditSession::OnActiveSessionsChanged().AddRaw(this, &FOdysseyRasterEditPreview::UpdateContext);
    InvalidatedHandle = FOdysseyRasterEditSession::OnInvalidated().AddRaw(this, &FOdysseyRasterEditPreview::Invalidate);
    UpdateContext();
}

const TSharedRef<FOdysseyRenderPreview>&
FOdysseyRasterEditPreview::GetPreview() const
{
    return Preview;
}

void
FOdysseyRasterEditPreview::UpdateContext()
{
    FOdysseyRenderContext Context;
    FOdysseyRasterEditSession::AddOverrides(Context);
    // A session begins with the pixels of its tiles, and invalidates what it changes, also when it ends.
    Preview->SetContext(MoveTemp(Context), false);
}

void
FOdysseyRasterEditPreview::Invalidate(const FGuid& RenderingId, const TArray<FIntRect>& Rects)
{
    Preview->Invalidate(Rects);
}
