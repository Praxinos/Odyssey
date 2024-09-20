// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyAnimationCellThumbnailRenderer.h"
#include "CanvasItem.h"
#include "CanvasTypes.h"
#include "TextureCompiler.h"
#include "ThumbnailRendering/ThumbnailManager.h"
#include "ImageUtils.h"
#include "OdysseyPainterEditorSettings.h"

#define THUMBNAIL_RENDER_SIZE 64


void
UOdysseyAnimationCellThumbnailRenderer::PostInitProperties()
{
	Super::PostInitProperties();
	const UOdysseyPainterEditorSettings& settings = *GetDefault< UOdysseyPainterEditorSettings >();
	mCheckerboardTexture = FImageUtils::CreateCheckerboardTexture( settings.CheckerColorOne, settings.CheckerColorTwo, 16 );
}

void
UOdysseyAnimationCellThumbnailRenderer::GetThumbnailSize(UObject* Object, float Zoom, uint32& OutWidth, uint32& OutHeight) const
{
	UOdysseyAnimationCell* cell = Cast<UOdysseyAnimationCell>(Object);
	if (!cell)
		return;

	UOdysseyAnimation* 	animation = cell->GetAnimation();
	if (!animation)
		return;

	float ratio = (float)animation->GetWidth() / (float)animation->GetHeight();

	static const uint32 baseSize = THUMBNAIL_RENDER_SIZE;

	OutWidth = (uint32)(THUMBNAIL_RENDER_SIZE * Zoom * ratio);
	OutHeight =(uint32)(THUMBNAIL_RENDER_SIZE * Zoom);
}

void
UOdysseyAnimationCellThumbnailRenderer::Draw(UObject* Object, int32 X, int32 Y, uint32 Width, uint32 Height, FRenderTarget* Viewport, FCanvas* Canvas, bool bAdditionalViewFamily)
{
	FCanvasTileItem checkboardTileItem(
		FVector2D(X, Y),
		mCheckerboardTexture->GetResource(),
		FVector2D( Width, Height ),
		FVector2D( 0.f, 0.f ),
		FVector2D( Width / mCheckerboardTexture->GetSizeX(), Height / mCheckerboardTexture->GetSizeY() ),
		FLinearColor::White
	);
	checkboardTileItem.BlendMode = SE_BLEND_Opaque;
	Canvas->DrawItem( checkboardTileItem );

	UOdysseyAnimationCell* cell = Cast<UOdysseyAnimationCell>(Object);
	if (!cell)
		return;

	FOdysseyAnimationCellThumbnailProxy::Get().GetMutex().Lock();

	//No Thumbnail => Display a checkboard
	if (!cell->mThumbnail)
	{
		cell->UndirtyThumbnail();
		FOdysseyAnimationCellThumbnailProxy::Get().InvalidateCell(cell);
		FOdysseyAnimationCellThumbnailProxy::Get().GetMutex().Unlock();
		return;
	}

	//If the Thumbnail is dirty, start rendering it and continue displaying the old thumbnail
	if (cell->IsThumbnailDirty())
	{
		cell->UndirtyThumbnail();
		FOdysseyAnimationCellThumbnailProxy::Get().InvalidateCell(cell);
	}

	//Thumbnail, but no texture available for its size => create the texture directly from block data
	TSharedPtr<::ULIS::FBlock> block = cell->mThumbnail->GetBlock();
	FVector2D thumbnailSize(block->Width(), block->Height());
	if (!Textures.Contains(thumbnailSize))
	{
		TConstArrayView64<uint8> data(block->Bits(), block->BytesTotal());
		UTexture2D* texture = UTexture2D::CreateTransient(
			thumbnailSize.X,
			thumbnailSize.Y,
			EPixelFormat::PF_B8G8R8A8,
			NAME_None,
			data
		);
		Textures.Add(thumbnailSize, texture);

		FOdysseyAnimationCellThumbnailProxy::Get().GetMutex().Unlock();

		FCanvasTileItem tileItem(
			FVector2D(X, Y),
			texture->GetResource(),
			FVector2D( Width, Height ),
			FLinearColor::White );
		tileItem.BlendMode = SE_BLEND_Translucent;
		Canvas->DrawItem( tileItem );

		return;
	}

	//Thumbnail and Texture available => copy the content of the thumbnail in the texture and display it
	UTexture2D* texture = Textures[thumbnailSize];

	FUpdateTextureRegion2D* region = new FUpdateTextureRegion2D( 0, 0, 0, 0, block->Width(), block->Height() );
	int blockBytes = GPixelFormats[ EPixelFormat::PF_B8G8R8A8 ].BlockBytes;
	texture->UpdateTextureRegions(
		  0
		, 1
		, region
		, block->Width() * blockBytes
		, blockBytes
		, block->Bits()
		, []( uint8*, const FUpdateTextureRegion2D* iRegions ) {
			delete iRegions;
		}
	);

    FRenderCommandFence fence;
    fence.BeginFence();
    fence.Wait();

	FOdysseyAnimationCellThumbnailProxy::Get().GetMutex().Unlock();

	FCanvasTileItem tileItem(
		FVector2D(X, Y),
		texture->GetResource(),
		FVector2D( Width, Height ),
		FLinearColor::White );
	tileItem.BlendMode = SE_BLEND_Translucent;
	Canvas->DrawItem( tileItem );
}


FOdysseyAnimationCellThumbnailProxy&
FOdysseyAnimationCellThumbnailProxy::Get()
{
	static FOdysseyAnimationCellThumbnailProxy proxy;
	return proxy;
}

FOdysseyAnimationCellThumbnailProxy::~FOdysseyAnimationCellThumbnailProxy()
{
	delete mThread;
	mThread = nullptr;
}

FOdysseyAnimationCellThumbnailProxy::FOdysseyAnimationCellThumbnailProxy()
{
	//this line starts the thread which will execute Init() => Run()
    mThread = FRunnableThread::Create(this, TEXT("FOdysseyAnimationCellThumbnailProxy"), 0, TPri_BelowNormal); //windows default = 8mb for thread, could specify more
}

bool
FOdysseyAnimationCellThumbnailProxy::Init()
{
    return true;
}

void
FOdysseyAnimationCellThumbnailProxy::InvalidateCell(UOdysseyAnimationCell* iCell)
{
	if(mRenderers.Contains(iCell))
	{
		mRenderers[iCell] = iCell->BuildImageRenderer(IOdysseyImageRenderer::eRenderType::Render, 0);
	}
	else
	{
		mRenderers.Add(iCell, iCell->BuildImageRenderer(IOdysseyImageRenderer::eRenderType::Render, 0));
	}


	UOdysseyAnimation* animation = iCell->GetAnimation();

	FOdysseyAnimationCellThumbnailTask task;
	task.mCell = iCell;
	task.mCellWidth = animation->GetWidth();
	task.mCellHeight = animation->GetHeight();
	task.mCellFormat = animation->GetFormat();
	mQueue.Enqueue(task);
}

uint32
FOdysseyAnimationCellThumbnailProxy::Run()
{
    FOptionalTaskTagScope Scope(ETaskTag::EParallelGameThread);
    //While not told to stop this thread 
    while (mStopTaskCounter.GetValue() == 0)
    {
        FOdysseyAnimationCellThumbnailTask task;
        if (!mQueue.Dequeue(task)) //PendingBlockData is a ThreadSafe queue
        {
            FPlatformProcess::Sleep(0.03); //Arbitrary number
            continue;
        }   

		if (!task.mCell)
			continue;

		mMutex.Lock();
		if (!mRenderers.Contains(task.mCell))
		{
			mMutex.Unlock();
			continue;
		}
		TSharedPtr<IOdysseyImageRenderer> renderer = mRenderers[task.mCell];
		mRenderers.Remove(task.mCell);
		mMutex.Unlock();
		

		TSharedPtr<::ULIS::FBlock> block = MakeShared<::ULIS::FBlock>(task.mCellWidth, task.mCellHeight, task.mCellFormat);

		::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(block->Format());

		renderer->Lock();
		renderer->Init();

		FOdysseyImageRendererCopyParams params(block,  {block->Rect()} );
		renderer->Copy(params, {});
		ctx.Finish();

		renderer->Unlock();

		if (block->Format() != ::ULIS::Format_BGRA8)
		{
			TSharedPtr<::ULIS::FBlock> oldBlock = block;
			block = MakeShared<::ULIS::FBlock>(task.mCellWidth, task.mCellHeight, ::ULIS::Format_BGRA8);
			ctx.ConvertFormat(*oldBlock, *block);
			ctx.Finish();
		}

		mMutex.Lock();
		
		TSharedPtr<FOdysseyRasterBlock> rasterBlock = task.mCell->mThumbnail;
		if (!rasterBlock)
		{
			float ratio = (float)task.mCellWidth / (float)task.mCellHeight;
			int width = (int)(THUMBNAIL_RENDER_SIZE * ratio);
			rasterBlock = MakeShared<FOdysseyRasterBlock>(task.mCell, width, THUMBNAIL_RENDER_SIZE, ::ULIS::Format_BGRA8);
			task.mCell->mThumbnail = rasterBlock;
		}
		FOdysseyRasterBlockMutator mutator(task.mCell->mThumbnail, false);
		mutator.EditTilesFromRects(
			{ task.mCell->mThumbnail->GetRect() },
			[&block, &ctx](TSharedPtr<::ULIS::FBlock> iBlock, const FULISInvalidTileMap& iTileMap) -> TArray<::ULIS::FEvent>
			{
				ctx.Resize(
					*block,
					*iBlock,
					::ULIS::FRectI::Auto,
					::ULIS::FRectF::Auto,
					::ULIS::Resampling_Bicubic
				);
				ctx.Finish();
				return {};
			}
		);

		mMutex.Unlock();

		//ensure cell is still valid when calling OnThumbnailChanged() in GameThread
		mFinishedCellsMutex.Lock();
		mFinishedCells.Add(task.mCell);
		mFinishedCellsMutex.Unlock();
		
		AsyncTask(
			ENamedThreads::GameThread,
			[this, cell = task.mCell]()
			{
				mFinishedCellsMutex.Lock();
				mFinishedCells.Remove(cell);
				cell->OnThumbnailChanged().Broadcast();
				mFinishedCellsMutex.Unlock();
			}
		);
    }
    return 0;
}

void
FOdysseyAnimationCellThumbnailProxy::Stop()
{
    mStopTaskCounter.Increment();
    mThread->WaitForCompletion();
}

FCriticalSection&
FOdysseyAnimationCellThumbnailProxy::GetMutex()
{
	return mMutex;
}


//--------------------------------------------------------------------------------------
//------------------------------------------------------------- FGCObject implementation

void
FOdysseyAnimationCellThumbnailProxy::AddReferencedObjects(FReferenceCollector& Collector)
{
	mMutex.Lock();
	mFinishedCellsMutex.Lock();

	TArray<UOdysseyAnimationCell*> cells;
	mRenderers.GenerateKeyArray(cells);
	Collector.AddReferencedObjects(cells);
	Collector.AddReferencedObjects(mFinishedCells);

	mFinishedCellsMutex.Unlock();
	mMutex.Unlock();
}

FString
FOdysseyAnimationCellThumbnailProxy::GetReferencerName() const
{
    return "FOdysseyAnimationCellThumbnailProxy";
}