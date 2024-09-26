// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "LayerStack/Cells/CellImageVector/OdysseyAnimationCellImageVector.h"

#include "LayerStack/Cells/CellImageVector/OdysseyAnimationCellImageVectorImageRenderer.h"
#include "LayerStack/Cells/CellImageVector/OdysseyAnimationCellImageVectorExport.h"
#include "LayerStack/Cells/CellImageVector/OdysseyAnimationCellImageVectorImport.h"
#include "LayerStack/Layers/LayerImageVector/OdysseyAnimationLayerImageVector.h"
#include "LayerStack/Cells/OdysseyAnimationCellsContainer.h"
#include "ULISLoaderModule.h"
#include "OdysseyMediaVector.h"
#include "OdysseyVectorBlock.h"
#include "Misc/OdysseyDuplicate.h"
#include "OdysseyAnimation.h"
// from module OdysseyFile
#include "OdysseyFile.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorRoot.h"
#include "OdysseyVectorGroupPaint.h"

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
    UOdysseyAnimationLayerImageVector::OnIsWireframeChanged().RemoveAll( this );

    GetLayer()->GetSharedEnv()->RemoveChild( mRoot );
    delete mRoot;

    mRoot = nullptr;
}

FOdysseyAnimationCellImageVector::FOdysseyAnimationCellImageVector(UOdysseyAnimationLayerImageVector* iLayer, int iLength)
    : FOdysseyAnimationCell(iLength, iLayer)
    , mRoot(nullptr)
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
    UOdysseyLayer* layer = FOdysseyAnimationCell::GetLayer();
    if (!layer)
        return nullptr;
    return Cast<UOdysseyAnimationLayerImageVector>(layer);
}

void
FOdysseyAnimationCellImageVector::Init(int iWidth, int iHeight)
{
    UOdysseyAnimation* animation = GetLayer()->GetAnimation();

    mWidth = iWidth;
    mHeight = iHeight;

    mRoot = new FOdysseyVectorRoot( this
                                  , new FOdysseyVectorGroupPaint( "Scene" )
                                  , (double)iWidth
                                  , (double)iHeight );

    GetLayer()->GetSharedEnv()->AppendChild( mRoot );

    // bind refresh function to delegates on existing vector scenes at load. Needed to refresh necessary widgets.
    UOdysseyAnimationLayerImageVector::OnIsColoredChanged().AddRaw( this, &FOdysseyAnimationCellImageVector::OnIsColoredChanged );
    UOdysseyAnimationLayerImageVector::OnIsWireframeChanged().AddRaw( this, &FOdysseyAnimationCellImageVector::OnIsWireframeChanged );

    mVectorBlock = MakeShared<FOdysseyVectorBlock>();
    mVectorBlock->Init(mVectorBlockId, mRoot->GetEngine(), iWidth, iHeight, animation->Format());
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
    return mRoot->GetEngine();
}

FOdysseyVectorRoot*
FOdysseyAnimationCellImageVector::GetRoot() const
{
    return mRoot;
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
        if ( mRoot == nullptr )
        {
            UOdysseyAnimation* animation = GetLayer()->GetAnimation();

            Init( animation->Width(), animation->Height() );
        }

        if (!FOdysseyAnimationCellImageVectorImport::Read( this, Ar ))
        {
            //Old Style No Chunk Loading
            checkf(false, TEXT("Failed to read chunks"));
        }
    }
}

void
FOdysseyAnimationCellImageVector::PostLoad()
{
    mRoot->GetEngine()->Invalidate( 0 );
    FOdysseyVectorEngine::Notify( mRoot->GetScene(), FOdysseyVectorEngine::NOTIFY_ALL );
}

void
FOdysseyAnimationCellImageVector::OnIsColoredChanged(UOdysseyAnimationLayerImageVector* iLayer)
{
    if (iLayer != GetLayer())
        return;

    mRoot->GetEngine()->Invalidate( 0 );
}

void
FOdysseyAnimationCellImageVector::OnIsWireframeChanged(UOdysseyAnimationLayerImageVector* iLayer)
{
    if (iLayer != GetLayer())
        return;

    mRoot->GetEngine()->Invalidate( 0 );
}

bool
FOdysseyAnimationCellImageVector::IsImageRenderingGameThreadOnly() const
{
    TSharedPtr<FOdysseyMediaVector> mediaVector = mMediaVector.Pin();
    return !!mediaVector;
}

TSharedPtr<IOdysseyImageRenderer>
FOdysseyAnimationCellImageVector::BuildImageRenderer(IOdysseyImageRenderer::eRenderType iRenderType, int iFrame, FImageRendererFilter iFilter) const
{
    if (iFilter.IsBound() && !iFilter.Execute(this))
        return nullptr;
    
    return MakeShared<FOdysseyAnimationCellImageVectorImageRenderer>(SharedThis(this), iFrame, iRenderType, GetImageRenderingRects(), iFilter);
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

    TSharedPtr<FOdysseyMediaVector> mediaVector = MakeShared<FOdysseyMediaVector>(mRoot->GetScene());
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
    FOdysseyVectorGroupPaint* newScene = static_cast<FOdysseyVectorGroupPaint*>(mRoot->GetScene()->Copy());
    cell->GetRoot()->SetScene(newScene);
    newScene->UpdateMatrix();
    newScene->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );

    return cell;
}

void
FOdysseyAnimationCellImageVector::OnVectorBlockInvalidated( const TArray<::ULIS::FRectI>& iRects, bool iIsInteractive)
{
    ImageRenderingChanged( iRects, iIsInteractive);
}

// Implements Interface IOdysseyVectorAnimationCell::GetCellByIndex
IOdysseyVectorAnimationCell*
FOdysseyAnimationCellImageVector::GetCellByIndex( uint32 iIndex )
{
    uint32 cellCount = GetLayer()->GetCellsContainer()->GetCells().Num();

    if( ( iIndex >= 0 ) && ( iIndex < cellCount ) )
    {
        FOdysseyAnimationCell* cell = GetLayer()->GetCellsContainer()->GetCells()[iIndex].Get();

        return static_cast<FOdysseyAnimationCellImageVector*>(cell);
    }

    return nullptr;
}

// Implements Interface IOdysseyVectorAnimationCell::GetLastCell
IOdysseyVectorAnimationCell*
FOdysseyAnimationCellImageVector::GetLastCell()
{
    FOdysseyAnimationCell* lastCell = GetLayer()->GetCellsContainer()->GetCells().Last().Get();

    return static_cast<FOdysseyAnimationCellImageVector*>(lastCell);
}

// Implements Interface IOdysseyVectorAnimationCell::GetLastCell
IOdysseyVectorAnimationCell*
FOdysseyAnimationCellImageVector::GetFirstCell()
{
    FOdysseyAnimationCell* firstCell = GetLayer()->GetCellsContainer()->GetCells()[0].Get();

    return static_cast<FOdysseyAnimationCellImageVector*>(firstCell);
}

// Implements Interface IOdysseyVectorAnimationCell::GetEngine
FOdysseyVectorEngine* 
FOdysseyAnimationCellImageVector::GetEngine()
{
    return mRoot->GetEngine();
}

// Implements Interface IOdysseyVectorAnimationCell::GetIndex
int32 
FOdysseyAnimationCellImageVector::GetIndex()
{
    return GetLayer()->GetCellsContainer()->GetCellIndex(SharedThis(this));
}

// Implements Interface IOdysseyVectorAnimationCell::GetLength
uint32
FOdysseyAnimationCellImageVector::GetLength()
{
    return FOdysseyAnimationCell::GetLength();
}

// Implements Interface IOdysseyVectorAnimationCell::GetFrame
uint32
FOdysseyAnimationCellImageVector::GetFrame()
{
    return GetLayer()->GetCellsContainer()->GetCellFrame(SharedThis(this));
}
