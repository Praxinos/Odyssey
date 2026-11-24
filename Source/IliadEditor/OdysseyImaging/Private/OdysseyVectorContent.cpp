// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2026

#include "OdysseyVectorContent.h"

#include "Engine/TextureRenderTarget2D.h"
#include "HAL/CriticalSection.h"
#include "HAL/PlatformMisc.h"
#include "Misc/App.h"
#include "Misc/ScopeLock.h"
#include "UObject/Package.h"

#include "OdysseyRasterEditSession.h"
#include "OdysseyTiledImageRendering.h"
#include "OdysseyTiledImage.h"
#include "OdysseyTiledImageBlock.h"
#include "OdysseyVectorCell.h"
#include "OdysseyVectorEngine.h"
#include "ULISLoaderModule.h"

#include "blend2d.h"

/**
 * A canvas-sized image the scenes are rendered into, with the engine drawing them.
 * Shared by the contents of the same size; renders are serialized by its mutex.
 * The engine only holds buffers used during a render, so it is shared too.
 */
struct FOdysseyVectorContentRasterizer
{
    explicit FOdysseyVectorContentRasterizer(FIntPoint InSize)
        : Size(InSize)
        , Image(InSize.X, InSize.Y, BL_FORMAT_PRGB32)
    {
    }

    FIntPoint Size;
    FCriticalSection Mutex;
    BLImage Image;
    BLContext Context;
    FOdysseyVectorEngine Engine;
};

namespace
{

/** Returns the rasterizer of the given size, shared with every content of that size. */
TSharedRef<FOdysseyVectorContentRasterizer> FindOrCreateRasterizer(FIntPoint Size)
{
    static FCriticalSection RegistryMutex;
    static TMap<FIntPoint, TWeakPtr<FOdysseyVectorContentRasterizer>> Registry;

    FScopeLock Lock(&RegistryMutex);
    for (TMap<FIntPoint, TWeakPtr<FOdysseyVectorContentRasterizer>>::TIterator It = Registry.CreateIterator(); It; ++It)
    {
        if (!It.Value().IsValid())
        {
            It.RemoveCurrent();
        }
    }
    if (TSharedPtr<FOdysseyVectorContentRasterizer> Existing = Registry.FindRef(Size).Pin())
    {
        return Existing.ToSharedRef();
    }
    TSharedRef<FOdysseyVectorContentRasterizer> Created = MakeShared<FOdysseyVectorContentRasterizer>(Size);
    Registry.Add(Size, Created);
    return Created;
}

/** Returns the format of the tiles: the render is 8-bit, decoded when it holds sRGB encoded colors. */
EOdysseyTiledImageFormat TileFormat(const FOdysseyVectorContent::FDescription& Description)
{
    return Odyssey::TiledImage::TiledImageFormatForULISFormat(::ULIS::Format_BGRA8, Description.bSRGB);
}

void AddToBounds(TOptional<FIntRect>& Bounds, const FIntRect& Rect)
{
    if (Rect.Width() <= 0 || Rect.Height() <= 0)
    {
        return;
    }
    if (Bounds.IsSet())
    {
        Bounds->Union(Rect);
    }
    else
    {
        Bounds = Rect;
    }
}
}

FOdysseyVectorContent::~FOdysseyVectorContent()
{
    if (Cell)
    {
        Cell->OnRequestRedrawDelegate().RemoveAll(this);
    }
    if (Session)
    {
        Session->End();
    }
}

FOdysseyVectorContent::FOdysseyVectorContent(const FGuid& InRenderingId, TSharedPtr<FOdysseyVectorCell> InCell,
    FGetDescription InGetDescription, FGetDrawingFlags InGetDrawingFlags, FOnChanged InOnChanged)
    : RenderingId(InRenderingId)
    , Cell(MoveTemp(InCell))
    , GetDescription(MoveTemp(InGetDescription))
    , GetDrawingFlags(MoveTemp(InGetDrawingFlags))
    , OnChanged(MoveTemp(InOnChanged))
{
    if (Cell)
    {
        Cell->OnRequestRedrawDelegate().AddRaw(this, &FOdysseyVectorContent::OnRequestRedraw);
    }
}

const TSharedPtr<FOdysseyVectorCell>&
FOdysseyVectorContent::GetCell() const
{
    return Cell;
}

UOdysseyTiledImage*
FOdysseyVectorContent::GetTiledImage() const
{
    return Image;
}

bool
FOdysseyVectorContent::EnsureRendered()
{
    check(IsInGameThread());
    // The tiles are created and written on the GPU.
    if (!FApp::CanEverRender())
    {
        return false;
    }
    const FDescription Description = GetDescription();
    if (!Description.IsValid())
    {
        return false;
    }

    const EOdysseyTiledImageFormat Format = TileFormat(Description);
    const uint64 DrawingFlags = GetDrawingFlags ? GetDrawingFlags() : 0;
    if (!Image)
    {
        // The render is rebuilt from the scene: the image is never saved, and writing its tiles never dirties a package.
        // Not transactional: the undo of a vector edit redraws the scene.
        Image = NewObject<UOdysseyTiledImage>(GetTransientPackage(), NAME_None, RF_Transient);
        Image->Initialize(UOdysseyTiledImage::DefaultTileSize, Format);
        RenderedDescription.Reset();
    }

    const FIntRect Canvas(FIntPoint::ZeroValue, Description.Size);
    TOptional<FIntRect> Rect;
    if (!RenderedDescription.IsSet() || !(RenderedDescription.GetValue() == Description) || Image->GetFormat() != Format)
    {
        // The session holds a render of the previous description.
        EndSession();
        Image->Initialize(UOdysseyTiledImage::DefaultTileSize, Format);
        Rect = Canvas;
    }
    else if (DrawingFlags != RenderedDrawingFlags)
    {
        Rect = Canvas;
    }
    else if (PendingRect.IsSet())
    {
        Rect = PendingRect.GetValue();
        Rect->Clip(Canvas);
    }
    RenderedDescription = Description;
    RenderedDrawingFlags = DrawingFlags;
    PendingRect.Reset();

    if (Rect.IsSet() && Rect->Width() > 0 && Rect->Height() > 0)
    {
        const bool bRendered = RenderScene(Rect.GetValue(), Description.Size, DrawingFlags, [this, &Rect, &Description](const ::ULIS::FBlock& Block)
        {
            Odyssey::TiledImage::CopyFromBlock(*Image, Block, TArray<FIntRect>{ Rect.GetValue() }, Description.bSRGB);
        });
        if (!bRendered)
        {
            // e.g. the cell is not in a layer yet: rendered later.
            PendingRect = Rect;
        }
    }
    return true;
}

bool
FOdysseyVectorContent::BuildRenderFunction(IOdysseyTextureRenderingAbility::FRenderFunction& OutRenderFunction, const FMatrix& ContentTransform)
{
    // Tiles are written on the game thread only: other threads render them as they are.
    if (IsInGameThread())
    {
        EnsureRendered();
    }
    if (!Image || !FApp::CanEverRender())
    {
        return false;
    }
    // Uncommitted edits are shown by the views, through the content replacement of the edit session.
    return Odyssey::TiledImageRendering::BuildTiledImageRenderFunction(*Image, GetDescription().Size, ContentTransform, OutRenderFunction);
}

void
FOdysseyVectorContent::OnRequestRedraw(FOdysseyVectorGroupPaint* Scene, uint64 RedrawFlags)
{
    const FDescription Description = GetDescription();
    // Read before any render, which resets the area the cell asks to redraw.
    const FIntRect Rect = GetRedrawRect(Description.Size);

    if ((RedrawFlags & FOdysseyVectorCell::REDRAW_INTERACTIVE) && Description.IsValid() && IsInGameThread() && FApp::CanEverRender())
    {
        RenderInteractive(Rect, Description);
        return;
    }

    // The areas shown by the session are rendered into the tiles with the others, when the content is displayed.
    AddPending(Rect);
    TArray<FIntRect> Rects;
    if (TOptional<FIntRect> SessionRect = EndSession())
    {
        Rects.Add(SessionRect.GetValue());
    }
    if (Rect.Width() > 0 && Rect.Height() > 0)
    {
        Rects.Add(Rect);
    }
    if (!Rects.IsEmpty())
    {
        OnChanged(Rects);
    }
}

void
FOdysseyVectorContent::RenderInteractive(const FIntRect& Rect, const FDescription& Description)
{
    if (Rect.Width() <= 0 || Rect.Height() <= 0)
    {
        return;
    }
    if (Session && !Session->IsActive())
    {
        // Ended by someone else: the areas it showed go to the tiles.
        EndSession();
    }
    // The session starts from the tiles: bring them up to date first.
    if (!EnsureRendered())
    {
        return;
    }
    if (!Session)
    {
        Session = FOdysseyRasterEditSession::Begin(RenderingId, *Image, Description.Size);
        SessionBounds.Reset();
        if (!Session)
        {
            return;
        }
    }

    UTextureRenderTarget2D* Target = Session->GetTarget();
    const EOdysseyTiledImageFormat Format = Image->GetFormat();
    const bool bRendered = RenderScene(Rect, Description.Size, RenderedDrawingFlags, [Target, Format, &Rect, &Description](const ::ULIS::FBlock& Block)
    {
        Odyssey::TiledImage::CopyBlockToRenderTarget(*Target, Format, Block, { Rect }, Description.bSRGB);
    });
    if (!bRendered)
    {
        return;
    }
    AddToBounds(SessionBounds, Rect);
    Session->Invalidate({ Rect });
}

TOptional<FIntRect>
FOdysseyVectorContent::EndSession()
{
    TOptional<FIntRect> Bounds = SessionBounds;
    SessionBounds.Reset();
    // Pending before the session ends: ending it makes the views render the tiles again.
    if (Bounds.IsSet())
    {
        AddPending(Bounds.GetValue());
    }
    if (TSharedPtr<FOdysseyRasterEditSession> Ended = MoveTemp(Session))
    {
        Ended->End();
    }
    Session.Reset();
    return Bounds;
}

bool
FOdysseyVectorContent::RenderScene(const FIntRect& Rect, FIntPoint Size, uint64 DrawingFlags, TFunctionRef<void(const ::ULIS::FBlock&)> Consume)
{
    TRACE_CPUPROFILER_EVENT_SCOPE(FOdysseyVectorContent::RenderScene);
    if (!Cell || Rect.Width() <= 0 || Rect.Height() <= 0)
    {
        return false;
    }
    if (!Rasterizer || Rasterizer->Size != Size)
    {
        Rasterizer = FindOrCreateRasterizer(Size);
    }

    // The rasterizer is shared: render, unpremultiply and copy before another content renders into it.
    FScopeLock Lock(&Rasterizer->Mutex);
    BLContextCreateInfo CreateInfo{};
    CreateInfo.thread_count = FPlatformMisc::NumberOfCoresIncludingHyperthreads();
    Rasterizer->Context.begin(Rasterizer->Image, CreateInfo);
    // The engine draws in canvas coordinates, within the rect, and resets the area the cell asks to redraw.
    const ::ULIS::FRectD Rendered = Rasterizer->Engine.Render(&Rasterizer->Context,
        ::ULIS::FRectD(Rect.Min.X, Rect.Min.Y, Rect.Width(), Rect.Height()), Cell.Get(), DrawingFlags);
    Rasterizer->Context.end();
    // Without a render, the rasterizer image holds the pixels of another content.
    if (Rendered.Area() <= 0.0)
    {
        return false;
    }

    BLImageData Data;
    Rasterizer->Image.get_data(&Data);
    ::ULIS::FBlock Block(static_cast<uint8*>(Data.pixel_data), Size.X, Size.Y, ::ULIS::Format_BGRA8);
    Block.SetBytesPerScanline(Data.stride);
    ::ULIS::FContext& Context = IULISLoaderModule::StaticFindOrAddContext(::ULIS::Format_BGRA8);
    Context.Unpremultiply(Block, ::ULIS::FRectI(Rect.Min.X, Rect.Min.Y, Rect.Width(), Rect.Height()));
    Context.Finish();
    Consume(Block);
    return true;
}

FIntRect
FOdysseyVectorContent::GetRedrawRect(FIntPoint Size) const
{
    const FIntRect Canvas(FIntPoint::ZeroValue, Size);
    if (!Cell)
    {
        return Canvas;
    }
    // As FOdysseyVectorEngine::SanitizeRect(): an area without size stands for the whole canvas.
    const ::ULIS::FRectD Invalidated = Cell->GetInvalidatedRect();
    if (Invalidated.Area() == 0.0)
    {
        return Canvas;
    }
    FIntRect Rect(FMath::FloorToInt(Invalidated.x), FMath::FloorToInt(Invalidated.y),
        FMath::CeilToInt(Invalidated.x + Invalidated.w), FMath::CeilToInt(Invalidated.y + Invalidated.h));
    Rect.Clip(Canvas);
    return Rect;
}

void
FOdysseyVectorContent::AddPending(const FIntRect& Rect)
{
    AddToBounds(PendingRect, Rect);
}

void
FOdysseyVectorContent::AddReferencedObjects(FReferenceCollector& Collector)
{
    Collector.AddReferencedObject(Image);
}

FString
FOdysseyVectorContent::GetReferencerName() const
{
    return TEXT("FOdysseyVectorContent");
}
