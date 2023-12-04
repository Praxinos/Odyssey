// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "LayerStack/Cells/CellImageVector/OdysseyAnimationCellImageVector.h"

#include "LayerStack/Cells/CellImageVector/OdysseyAnimationCellImageVectorImageRenderer.h"
#include "LayerStack/Cells/CellImageVector/OdysseyAnimationCellImageVectorExport.h"
#include "LayerStack/Cells/CellImageVector/OdysseyAnimationCellImageVectorImport.h"
#include "LayerStack/Layers/LayerImageVector/OdysseyAnimationLayerImageVector.h"
#include "ULISLoaderModule.h"
#include "OdysseyMediaVector.h"
#include "OdysseyVectorBlock.h"
#include "Misc/OdysseyDuplicate.h"
// from module OdysseyFile
#include "OdysseyFile.h"

#define LOCTEXT_NAMESPACE "FOdysseyAnimationCellImageVector"

TSharedRef<FOdysseyAnimationCellImageVector>
FOdysseyAnimationCellImageVector::Create(UOdysseyAnimationLayerImageVector* iLayer, int iLength, int iWidth, int iHeight)
{
    TSharedRef<FOdysseyAnimationCellImageVector> cell = MakeShared<FOdysseyAnimationCellImageVector>(iLayer, iLength);
    cell->Init(iWidth, iHeight);
    return cell;
}

const FName&
FOdysseyAnimationCellImageVector::StaticType()
{
    static FName type = TEXT("FOdysseyAnimationCellImageVector");
    return type;
}

FOdysseyAnimationCellImageVector::~FOdysseyAnimationCellImageVector()
{
    if (mVectorBlock)
        mVectorBlock->OnInvalidated().RemoveAll( this );

    UOdysseyAnimationLayerImageVector::OnIsColoredChanged().RemoveAll( this );
    delete mEngine;
    mEngine = nullptr;
}

FOdysseyAnimationCellImageVector::FOdysseyAnimationCellImageVector(UOdysseyAnimationLayerImageVector* iLayer, int iLength)
    : FOdysseyAnimationCell(iLength, iLayer)
    , mEngine(nullptr)
    , mVectorBlockId(FGuid::NewGuid())
    , mWidth(0)
    , mHeight(0)
    , mMediaVector(nullptr)
{
}

TSharedPtr<FOdysseyAnimationCell>
FOdysseyAnimationCellImageVector::Clone(UOdysseyAnimationLayer* iLayer, int iLength) const
{
    TSharedPtr<FOdysseyAnimationCellImageVector> cloneCell = MakeShared<FOdysseyAnimationCellImageVector>(Cast<UOdysseyAnimationLayerImageVector>(iLayer), 1);
    ::Odyssey::Duplicate(const_cast<FOdysseyAnimationCellImageVector*>(this), cloneCell.Get());
    cloneCell->mLength = iLength;
    return cloneCell;
}

UOdysseyAnimationLayerImageVector*
FOdysseyAnimationCellImageVector::GetLayer() const
{
    return Cast<UOdysseyAnimationLayerImageVector>(FOdysseyAnimationCell::GetLayer());
}

void
FOdysseyAnimationCellImageVector::Init(int iWidth, int iHeight)
{
    UOdysseyAnimation* animation = GetLayer()->GetAnimation();

    mWidth = iWidth;
    mHeight = iHeight;

    mEngine = new FOdysseyVectorEngine( new FOdysseyVectorGroupPaint( "Scene" )
                                       , (double)iWidth
                                       , (double)iHeight );

    // bind refresh function to delegates on existing vector scenes at load. Needed to refresh necessary widgets.
    UOdysseyAnimationLayerImageVector::OnIsColoredChanged().AddRaw( this, &FOdysseyAnimationCellImageVector::OnIsColoredChanged );

    mVectorBlock = MakeShared<FOdysseyVectorBlock>();
    mVectorBlock->Init(mVectorBlockId, mEngine, iWidth, iHeight, animation->Format());
    mVectorBlock->OnInvalidated().AddRaw(this, &FOdysseyAnimationCellImageVector::OnVectorBlockInvalidated);
}

const FName&
FOdysseyAnimationCellImageVector::GetType() const
{
    return StaticType();
}

FOdysseyVectorEngine*
FOdysseyAnimationCellImageVector::GetEngine() const
{
    return mEngine;
}

TSharedPtr<FOdysseyVectorBlock>
FOdysseyAnimationCellImageVector::GetVectorBlock() const
{
    return mVectorBlock;
}

uint32
FOdysseyAnimationCellImageVector::GetWidth()
{
    return mWidth;
}

uint32
FOdysseyAnimationCellImageVector::GetHeight()
{
    return mHeight;
}

FGuid
FOdysseyAnimationCellImageVector::GetVectorBlockId()
{
    return mVectorBlockId;
}

void
FOdysseyAnimationCellImageVector::SetWidth( uint32 iWidth )
{
    mWidth = iWidth;
}

void
FOdysseyAnimationCellImageVector::SetHeight( uint32 iHeight )
{
    mHeight = iHeight;
}

void
FOdysseyAnimationCellImageVector::SetVectorBlockId( FGuid iVectorBlockId )
{
    mVectorBlockId = iVectorBlockId;
}

void
FOdysseyAnimationCellImageVector::Serialize(FArchive& Ar)
{
    FOdysseyAnimationCell::Serialize(Ar);

    if( Ar.IsSaving() )
    {
        FOdysseyAnimationCellImageVectorExport::Write( this, Ar );
    }

    if( Ar.IsLoading() )
    {
        if (!FOdysseyAnimationCellImageVectorImport::Read( this, Ar ))
        {
            //Old Style No Chunk Loading
            checkf(false, TEXT("Failed to read chunks"));
        }

        mEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                       | FOdysseyVectorEngine::SIGNAL_SCENE_HIERARCHY );
    }
}

void
FOdysseyAnimationCellImageVector::OnIsColoredChanged(UOdysseyAnimationLayerImageVector* iLayer)
{
    if (iLayer != GetLayer())
        return;

    mEngine->Invalidate();

    mEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW );

    ImageRenderingChanged();
}

bool
FOdysseyAnimationCellImageVector::IsImageRenderingGameThreadOnly() const
{
    TSharedPtr<FOdysseyMediaVector> mediaVector = mMediaVector.Pin();
    return !!mediaVector;
}

TSharedPtr<IOdysseyImageRenderer>
FOdysseyAnimationCellImageVector::BuildImageRenderer(IOdysseyImageRenderer::eRenderType iRenderType, int iFrame) const
{
    return MakeShared<FOdysseyAnimationCellImageVectorImageRenderer>(SharedThis(this), iFrame, iRenderType, GetImageRenderingRects());
}

TArray<FGuid>
FOdysseyAnimationCellImageVector::GetImageRenderingComposition(IOdysseyImageRenderer::eRenderType iRenderType, int iFrameIndex) const
{
    return { GetImageRenderingId() };
}

TArray<::ULIS::FRectI>
FOdysseyAnimationCellImageVector::GetImageRenderingRects() const
{
    return { ::ULIS::FRectI::FromXYWH(0, 0, mWidth, mHeight) };
}

FOdysseyMediaProvider
FOdysseyAnimationCellImageVector::GetMediaProvider(uint32 iFrameIndex) const
{
    //Don't create a mediaRaster if there is an image render in use
    FScopeLock lock(&mImageRenderingMutex);

    TSharedPtr<FOdysseyMediaVector> mediaVector = MakeShared<FOdysseyMediaVector>(mEngine->GetScene());
    mMediaVector = mediaVector;
    FOdysseyMediaProvider mediaProvider;
    mediaProvider.Add(mediaVector);
    return mediaProvider;
}

FCriticalSection*
FOdysseyAnimationCellImageVector::GetImageRenderingMutex() const
{
    return &mImageRenderingMutex;
}

TSharedPtr<FOdysseyAnimationCell>
FOdysseyAnimationCellImageVector::CreateCellFromFrame(uint32 iFrameIndex) const
{
    //Copy Current Cell block at given frameindex
    //Create a new Vector cell from the given block
    TSharedRef<FOdysseyAnimationCellImageVector> cell = FOdysseyAnimationCellImageVector::Create(GetLayer(), 1, mWidth, mHeight);
    FOdysseyVectorGroupPaint* newScene = static_cast<FOdysseyVectorGroupPaint*>(mEngine->GetScene()->Copy());
    cell->GetEngine()->SetScene(newScene);
    newScene->UpdateMatrix();
    newScene->Update( FOdysseyVectorObject::UPDATEPAINTGROUPS );

    return cell;
}

void
FOdysseyAnimationCellImageVector::OnVectorBlockInvalidated(bool iIsInteractive)
{
    ImageRenderingChanged(iIsInteractive);
}

#undef LOCTEXT_NAMESPACE