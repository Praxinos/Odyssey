// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "LayerStack/Cells/CellImageVector/OdysseyAnimationCellImageVector.h"

#include "LayerStack/Cells/CellImageVector/OdysseyAnimationCellImageVectorImageRenderer.h"
#include "LayerStack/Cells/CellImageVector/OdysseyAnimationCellImageVectorExport.h"
#include "LayerStack/Cells/CellImageVector/OdysseyAnimationCellImageVectorImport.h"
#include "ULISLoaderModule.h"
#include "OdysseyMediaVector.h"
#include "OdysseyVectorBlock.h"
// from module OdysseyFile
#include "OdysseyFile.h"

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
    
//    Ar << mWidth;
//    Ar << mHeight;
//    Ar << mVectorBlockId;

    if( Ar.IsSaving() )
    {
        FOdysseyAnimationCellImageVectorExport::Write( this, Ar );
    }

    if( Ar.IsLoading() )
    {
        uint32 chunkID;
        uint64 chunkLen;
        uint64 chunkEnd;

        // Reads the first chunk (FOdysseyFile::AnimationCellImageVector::CHUNK_ROOT)
        Ar << chunkID;
        Ar << chunkLen;

        chunkEnd = Ar.Tell() + chunkLen;

        switch( chunkID )
        {
            case FOdysseyFile::Animation::CHUNK_CELLIMAGEVECTOR :
                UE_LOG(LogTemp, Warning, TEXT("CHUNK_CELLIMAGEVECTOR") );

                FOdysseyAnimationCellImageVectorImport::Read( this, Ar, chunkEnd );
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

TSharedPtr<IOdysseyImageRenderer>
FOdysseyAnimationCellImageVector::BuildImageRenderer(IOdysseyImageRenderer::eRenderType iRenderType, int iFrame) const
{
    bool renderHUD = iRenderType == IOdysseyImageRenderer::eRenderType::Editor && mLayer->GetLayerStack()->CurrentLayer.Get() == mLayer;
    return MakeShared<FOdysseyAnimationCellImageVectorImageRenderer>(/*this, */mVectorBlock, renderHUD, iRenderType, GetImageRenderingRects());
    //return MakeShared<FOdysseyAnimationCellImageVectorImageRenderer>(mEngine, mBlock, renderHUD, mLayer->IsColored, iRenderType, GetImageRenderingRects());
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
    TSharedPtr<FOdysseyMediaVector> mediaVector = MakeShared<FOdysseyMediaVector>(mEngine->GetScene());
    FOdysseyMediaProvider mediaProvider;
    mediaProvider.Add(mediaVector);
    return mediaProvider;
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