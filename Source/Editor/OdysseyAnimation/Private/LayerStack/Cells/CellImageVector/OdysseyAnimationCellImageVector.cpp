// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "LayerStack/Cells/CellImageVector/OdysseyAnimationCellImageVector.h"

#include "LayerStack/Cells/CellImageVector/OdysseyAnimationCellImageVectorImageRenderer.h"
#include "ULISLoaderModule.h"
#include "OdysseyMediaVector.h"
#include "OdysseyVectorBlock.h"
#include "Import/v2/OdysseyVectorImport.h"
#include "Import/v1/OdysseyVectorImport.h"

#define LOCTEXT_NAMESPACE "FOdysseyAnimationCellImageVector"

TSharedRef<FOdysseyAnimationCellImageVector>
FOdysseyAnimationCellImageVector::Create(UOdysseyAnimationLayerImageVector* iLayer, int iWidth, int iHeight)
{
    TSharedRef<FOdysseyAnimationCellImageVector> cell = MakeShared<FOdysseyAnimationCellImageVector>(iLayer);
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

FOdysseyAnimationCellImageVector::FOdysseyAnimationCellImageVector(UOdysseyAnimationLayerImageVector* iLayer)
    : mLayer(iLayer)
    , mEngine(nullptr)
    , mVectorBlockId(FGuid::NewGuid())
    , mWidth(0)
    , mHeight(0)
    , mMediaVector(nullptr)
{
}

void
FOdysseyAnimationCellImageVector::Init(int iWidth, int iHeight)
{
    UOdysseyAnimation* animation = mLayer->GetAnimation();

    BLImageData imgData;

    mWidth = iWidth;
    mHeight = iHeight;

    mEngine = new FOdysseyVectorEngine( new FOdysseyVectorScene( "Scene" )
                                       , (double)iWidth
                                       , (double)iHeight );

    // bind refresh function to delegates on existing vector scenes at load. Needed to refresh necessary widgets.
    UOdysseyAnimationLayerImageVector::OnIsColoredChanged().AddRaw( this, &FOdysseyAnimationCellImageVector::OnIsColoredChanged );

    mEngine->GetBLImage()->getData( &imgData );

    mVectorBlock = MakeShared<FOdysseyVectorBlock>();
    mVectorBlock->Init(mVectorBlockId, mEngine, iWidth, iHeight, animation->Format());
    mVectorBlock->SetRenderFlags(mLayer->IsColored ? 0 : FOdysseyVectorObject::DRAWING_IGNORECOLOR);
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

void
FOdysseyAnimationCellImageVector::Serialize(FArchive& Ar)
{
    FOdysseyAnimationCell::Serialize(Ar);
    
    Ar << mWidth;
    Ar << mHeight;
    Ar << mVectorBlockId;

    if( Ar.IsSaving() )
    {
        FOdysseyVectorExportV2::Write( mEngine ? mEngine->GetScene() : nullptr, Ar );
    }

    if( Ar.IsLoading() )
    {
        uint32 chunkID;
        uint64 chunkLen;
        uint64 chunkEnd;

        // Reads the first chunk (CHUNK_VECTOR_MAGIC)
        Ar << chunkID;
        Ar << chunkLen;

        chunkEnd = Ar.Tell() + chunkLen;

        if ( mEngine == nullptr )
        {
            Init( mWidth, mHeight );
        }

        switch( chunkID )
        {
            case FOdysseyVectorExportV1::CHUNK_VECTOR_MAGIC_V1 :
                UE_LOG(LogTemp, Warning, TEXT("CHUNK_VECTOR_MAGIC_V1") );

                FOdysseyVectorImportV1::Read( mEngine->GetScene(), Ar, chunkEnd );
            break;

            case FOdysseyVectorExportV2::CHUNK_VECTOR_MAGIC_V2 :
            {
                FOdysseyVectorImportV2 importerV2 = FOdysseyVectorImportV2();

                UE_LOG(LogTemp, Warning, TEXT("CHUNK_VECTOR_MAGIC_V2") );

                importerV2.Read( mEngine->GetScene(), Ar, chunkEnd );
            }
            break;

            default:
                Ar.Seek( chunkEnd );
            break;
        }

        mEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                       | FOdysseyVectorEngine::SIGNAL_SCENE_HIERARCHY );
    }
}

void
FOdysseyAnimationCellImageVector::OnIsColoredChanged(UOdysseyAnimationLayerImageVector* iLayer)
{
    if (iLayer != mLayer)
        return;

    mVectorBlock->SetRenderFlags(mLayer->IsColored ? 0 : FOdysseyVectorObject::DRAWING_IGNORECOLOR);
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
    TSharedRef<FOdysseyAnimationCellImageVector> cell = FOdysseyAnimationCellImageVector::Create(mLayer, mWidth, mHeight);
    FOdysseyVectorScene* newScene = static_cast<FOdysseyVectorScene*>(mEngine->GetScene()->Copy());
    cell->GetEngine()->SetScene(newScene);
    newScene->Update( FOdysseyVectorObject::UPDATEPAINTGROUPS );

    return cell;
}

void
FOdysseyAnimationCellImageVector::OnVectorBlockInvalidated(bool iIsInteractive)
{
    ImageRenderingChanged(iIsInteractive);
}

#undef LOCTEXT_NAMESPACE