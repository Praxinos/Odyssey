// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyTextureLayerImageVector.h"

#include "OdysseyTextureLayerStack.h"
#include "OdysseyPixelFormat.h"
#include "EditorStyleSet.h"
#include "OdysseyMediaVector.h"
#include "OdysseyVectorBlock.h"
#include "ULISLoaderModule.h"
#include "ULISUtils.h"
#include "LayerStack/OdysseyTextureLayerImageVectorImageRenderer.h"
#include "OdysseyTextureLayerImageVectorImport.h"
#include "OdysseyTextureLayerImageVectorExport.h"
// from module OdysseyVector
#include "Import/v1/OdysseyVectorImport.h"
#include "Import/v2/OdysseyVectorImport.h"
// from module OdysseyFile
#include "OdysseyFile.h"

#include "blend2d.h"

#define LOCTEXT_NAMESPACE "Texture"

UOdysseyTextureLayerImageVector::~UOdysseyTextureLayerImageVector()
{
    // bind refresh function to delegates on existing vector scenes at load. Needed to refresh necessary widgets.
    if (mVectorBlock)
        mVectorBlock->OnInvalidated().RemoveAll( this );
}

UOdysseyTextureLayerImageVector::UOdysseyTextureLayerImageVector()
    : mEngine(nullptr)
{
    LayerTypeName = LOCTEXT("layer-image-vector.type", "Vector Image Layer");
    Icon = FSlateIcon("OdysseyStyle", "OdysseyLayerStack.LayerVector16");
}

void
UOdysseyTextureLayerImageVector::Init( uint32 iWidth, uint32 iHeight )
{
    Width  = iWidth;
    Height = iHeight;

    mEngine = new FOdysseyVectorEngine( new FOdysseyVectorGroupPaint( "Scene" )
                                       , (double)iWidth
                                       , (double)iHeight );
}



FOdysseyVectorEngine*
UOdysseyTextureLayerImageVector::GetEngine()
{
    return mEngine;
}

FOdysseyMediaProvider
UOdysseyTextureLayerImageVector::GetMediaProvider(uint32 iFrameIndex) const
{
    bool isActive = IsActivatedRecursively();
    bool isLocked = IsLockedRecursively();

    FOdysseyMediaProvider mediaProvider;
    mediaProvider.IsLocked(!isActive || isLocked);

    TSharedPtr<FOdysseyMediaVector> mediaVector = MakeShared<FOdysseyMediaVector>(mEngine->GetScene());
    mediaProvider.Add(mediaVector);
    return mediaProvider;
}

void
UOdysseyTextureLayerImageVector::PostInitProperties()
{
    Super::PostInitProperties();

    if (GetFlags() & RF_ClassDefaultObject)
        return;

    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetLayerStack());
    if(!layerStack)
        return;

    UTexture2D* texture = layerStack->GetTexture();
    if( texture && texture->Source.GetFormat() != TSF_Invalid )
    {
        Init( texture->Source.GetSizeX(), texture->Source.GetSizeY() );

        ::ULIS::eFormat format = ULISFormatForTextureSourceFormat(texture->Source.GetFormat());
        //let's ensure the format has alpha, so add alpha channel of needed
        format = static_cast< ::ULIS::eFormat >(format | ULIS_W_ALPHA( 1 ) );

        mVectorBlockId = FGuid::NewGuid();
        mVectorBlock = MakeShared<FOdysseyVectorBlock>();
        mVectorBlock->Init(mVectorBlockId, mEngine, Width, Height, format);

        mVectorBlock->OnInvalidated().AddUObject(this, &UOdysseyTextureLayerImageVector::OnVectorBlockInvalidated);
    }
}

void
UOdysseyTextureLayerImageVector::PostLoad()
{
    Super::PostLoad();

    if (GetFlags() & RF_ClassDefaultObject)
        return;

    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetLayerStack());
    UTexture2D* texture = layerStack->GetTexture();

    if( texture && texture->Source.GetFormat() != TSF_Invalid )
    {
        ::ULIS::eFormat format = ULISFormatForTextureSourceFormat(texture->Source.GetFormat());
        //let's ensure the format has alpha, so add alpha channel of needed
        format = static_cast< ::ULIS::eFormat >(format | ULIS_W_ALPHA( 1 ) );

        mVectorBlock = MakeShared<FOdysseyVectorBlock>();
        mVectorBlock->Init(mVectorBlockId, mEngine, Width, Height, format);
        mVectorBlock->OnInvalidated().AddUObject(this, &UOdysseyTextureLayerImageVector::OnVectorBlockInvalidated);
    }
}

void
UOdysseyTextureLayerImageVector::PostDuplicate(bool bDuplicateForPIE)
{
    Super::PostDuplicate(bDuplicateForPIE);

    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetLayerStack());
    UTexture2D* texture = layerStack->GetTexture();
    if( texture && texture->Source.GetFormat() != TSF_Invalid )
    {
        Width = texture->Source.GetSizeX();
        Height = texture->Source.GetSizeY();
        mVectorBlockId = FGuid::NewGuid();
    }
}

void
UOdysseyTextureLayerImageVector::Serialize(FArchive& Ar)
{
    Super::Serialize( Ar );

    if( Ar.IsSaving() )
    {
        FOdysseyTextureLayerImageVectorExport::Write( this, Ar );
    }

    if( Ar.IsLoading() )
    {
        if ( mEngine == nullptr )
        {
            // commented out: at that point, the texture owning the layer stack doe snot have width and height values.
            // This should be changed. As a bypass, I store dimensions in Width and Height UProperties.
            //UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetLayerStack());
            //if(!layerStack)
            //    return;
            //UTexture2D* texture = layerStack->GetTexture();

            //Init( texture->Source.GetSizeX(), texture->Source.GetSizeY() );

            Init( Width, Height );
        }

        if (!FOdysseyTextureLayerImageVectorImport::Read( this, Ar ))
        {
            //Old Style Chunk Loading
            uint32 chunkID;
            uint64 chunkLen;
            uint64 chunkEnd;

            // Reads the first chunk (CHUNK_VECTOR_MAGIC)
            Ar << chunkID;
            Ar << chunkLen;

            chunkEnd = Ar.Tell() + chunkLen;

            switch( chunkID )
            {
                case FOdysseyFile::VectorV1::CHUNK_VECTOR_MAGIC_V1 :
                    //UE_LOG(LogTemp, Warning, TEXT("CHUNK_VECTOR_MAGIC_V1") );

                    FOdysseyVectorImportV1::Read( mEngine->GetScene(), Ar, chunkEnd );
                break;

                case FOdysseyFile::VectorV2::CHUNK_VECTOR_MAGIC_V2 :
                {
                    FOdysseyVectorImportV2 importerV2 = FOdysseyVectorImportV2();

                    //UE_LOG(LogTemp, Warning, TEXT("CHUNK_VECTOR_MAGIC_V2") );

                    importerV2.Read( mEngine->GetScene(), Ar, chunkEnd );
                }
                break;

                default:
                    Ar.Seek( chunkEnd );
                break;
            }
        }
        mEngine->Signal( FOdysseyVectorEngine::SIGNAL_ALL );
    }
}

void
UOdysseyTextureLayerImageVector::PropertyChanged(const FName& iPropertyName, const FName& iMemberPropertyName, bool iIsInteractive)
{
    Super::PropertyChanged(iPropertyName, iMemberPropertyName, iIsInteractive);
    if(iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyTextureLayerImageVector, IsWireframe))
        IsWireframeChanged();
    if(iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyTextureLayerImageVector, IsColored))
        IsColoredChanged();
}

void
UOdysseyTextureLayerImageVector::PostPropertyChanged(const FName& iPropertyName, bool iIsInteractive)
{
    Super::PostPropertyChanged(iPropertyName, iIsInteractive);
    if(iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyTextureLayerImageVector, IsWireframe))
        ImageRenderingChanged();
    if(iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyTextureLayerImageVector, IsColored))
        ImageRenderingChanged();
}

void
UOdysseyTextureLayerImageVector::IsWireframeChanged()
{
    mEngine->Invalidate(); //Force engine invalidation here, because IsColored is not a part of the engine, but still needs the engine to redraw itself
    mEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW );
}

void
UOdysseyTextureLayerImageVector::IsColoredChanged()
{
    mEngine->Invalidate(); //Force engine invalidation here, because IsColored is not a part of the engine, but still needs the engine to redraw itself
    mEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW );
}

TSharedPtr<IOdysseyImageRenderer>
UOdysseyTextureLayerImageVector::BuildImageRenderer(IOdysseyImageRenderer::eRenderType iRenderType, int iFrame, FImageRendererFilter iFilter) const
{
    if (iFilter.IsBound() && !iFilter.Execute(this))
        return nullptr;

    if (!mVectorBlock)
        return nullptr;

    return MakeShared<FOdysseyTextureLayerImageVectorImageRenderer>(this, mVectorBlock, iRenderType, GetImageRenderingRects(), iFilter);
}

TArray<FGuid>
UOdysseyTextureLayerImageVector::GetImageRenderingComposition(IOdysseyImageRenderer::eRenderType iRenderType, int iFrame) const
{
    return { GetImageRenderingId() };
}

void
UOdysseyTextureLayerImageVector::OnVectorBlockInvalidated( const TArray<::ULIS::FRectI>& iRects, bool iIsInteractive)
{
    ImageRenderingChanged(iRects, iIsInteractive);
}

void
UOdysseyTextureLayerImageVector::Merge(const TArray<UOdysseyLayer*>& iLayers)
{
    FOdysseyVectorGroupPaint* destinationScene = mEngine->GetScene();

    for( int i = 0; i < iLayers.Num(); i++ )
    {
        UOdysseyTextureLayerImageVector* vectorLayer = Cast<UOdysseyTextureLayerImageVector>(iLayers[i]);
        if (!vectorLayer)
            continue;

        FOdysseyVectorGroupPaint* scene = vectorLayer->GetEngine()->GetScene();
        for( FOdysseyVectorObject* child : scene->GetChildrenList() )
        {
            FOdysseyVectorObject* copiedChild = child->Copy();
            destinationScene->AppendChild( copiedChild );

            scene->CopyBuckets( destinationScene, false );
        }
    }

    destinationScene->UpdateMatrix();
    destinationScene->Update( FOdysseyVectorObject::UPDATEPAINTGROUPS );

    mEngine->Signal( FOdysseyVectorEngine::SIGNAL_ALL );
}

void
UOdysseyTextureLayerImageVector::IsWireframeBlueprintSetter(bool Value)
{
    FOdysseyObjectEditorUtils::SetPropertyValue(this, GET_MEMBER_NAME_CHECKED(UOdysseyTextureLayerImageVector, IsWireframe), Value);
}

void
UOdysseyTextureLayerImageVector::IsColoredBlueprintSetter(bool Value)
{
    FOdysseyObjectEditorUtils::SetPropertyValue(this, GET_MEMBER_NAME_CHECKED(UOdysseyTextureLayerImageVector, IsColored), Value);
}

#undef LOCTEXT_NAMESPACE
