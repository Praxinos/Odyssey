// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2026

#include "OdysseyRasterEditSession.h"

#include "Engine/TextureRenderTarget2D.h"
#include "RenderGraphBuilder.h"
#include "RenderGraphUtils.h"
#include "RenderingThread.h"
#include "TextureResource.h"

#include "OdysseyBlendShader.h"
#include "OdysseyRenderContext.h"
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

FOdysseyRasterEditSession::FOdysseyRasterEditSession(const FGuid& InRenderingId, UOdysseyTiledImage& InImage, UTextureRenderTarget2D& InTarget)
    : RenderingId(InRenderingId)
    , Image(&InImage)
    , Target(&InTarget)
{
    TargetRHI = InTarget.GameThread_GetRenderTargetResource()->GetRenderTargetTexture();
}

TSharedPtr<FOdysseyRasterEditSession>
FOdysseyRasterEditSession::Begin(const FGuid& RenderingId, UOdysseyTiledImage& Image, FIntPoint Size)
{
    check(IsInGameThread());
    if (Size.X <= 0 || Size.Y <= 0)
    {
        return nullptr;
    }

    TArray<TWeakPtr<FOdysseyRasterEditSession>>& Sessions = ActiveSessions();
    for (const TWeakPtr<FOdysseyRasterEditSession>& Weak : TArray<TWeakPtr<FOdysseyRasterEditSession>>(Sessions))
    {
        TSharedPtr<FOdysseyRasterEditSession> Existing = Weak.Pin();
        if (Existing && Existing->RenderingId == RenderingId)
        {
            Existing->End();
        }
    }

    UTextureRenderTarget2D* Target = Image.CreateRenderTarget(Size);
    Image.Render(Target, FIntRect(FIntPoint::ZeroValue, Size), FIntPoint::ZeroValue);
    // The target RHI texture is created and filled on the render thread.
    FlushRenderingCommands();

    TSharedPtr<FOdysseyRasterEditSession> Session = MakeShared<FOdysseyRasterEditSession>(RenderingId, Image, *Target);
    Sessions.Add(Session);
    OnActiveSessionsChanged().Broadcast();
    return Session;
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
    InvalidRects.Append(Rects);
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
    const FIntRect Bounds(0, 0, Source->SizeX, Source->SizeY);
    for (FIntRect Rect : Rects)
    {
        Rect.Clip(Bounds);
        if (Rect.Width() > 0 && Rect.Height() > 0)
        {
            TiledImage->CopyFromTexture(Source, Rect, Rect.Min);
        }
    }
    // The tiles are written on the render thread, which reads the target.
    FlushRenderingCommands();
    End();
    return true;
}

void
FOdysseyRasterEditSession::End()
{
    if (!IsActive())
    {
        return;
    }
    Target.Reset();
    ActiveSessions().RemoveAll([this](const TWeakPtr<FOdysseyRasterEditSession>& Weak)
    {
        return !Weak.IsValid() || Weak.Pin().Get() == this;
    });
    OnActiveSessionsChanged().Broadcast();
    // The views showed the target: they show the tiles again.
    TArray<FIntRect> Rects = MoveTemp(InvalidRects);
    if (!Rects.IsEmpty())
    {
        OnInvalidated().Broadcast(RenderingId, Rects);
    }
}

IOdysseyTextureRenderingAbility::FRenderFunction
FOdysseyRasterEditSession::BuildRenderFunction() const
{
    return [SourceRHI = TargetRHI](FRDGBuilder& GraphBuilder, ERHIFeatureLevel::Type FeatureLevel, FRDGTextureRef Destination,
        const FIntRect& SourceRect, const FIntRect& DestinationRect, const FMatrix& Transform)
    {
        RDG_EVENT_SCOPE(GraphBuilder, "Odyssey.RasterEditSession");
        AddClearRenderTargetPass(GraphBuilder, Destination, FLinearColor::Transparent, DestinationRect);
        if (!SourceRHI)
        {
            return;
        }
        FRDGTextureRef Source = GraphBuilder.RegisterExternalTexture(CreateRenderTarget(SourceRHI, TEXT("Odyssey.RasterEditSession.Target")));
        FOdysseyBlendShader::BlendRect(GraphBuilder, FeatureLevel, Destination, Source, Destination,
            SourceRect, DestinationRect, Transform, EOdysseyColorBlendMode::Normal,
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
