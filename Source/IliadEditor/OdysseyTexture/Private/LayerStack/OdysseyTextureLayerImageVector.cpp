// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyTextureLayerImageVector.h"

#include "OdysseyTextureLayerStack.h"
#include "OdysseyPixelFormat.h"
#include "OdysseyMediaVector.h"
#include "OdysseyVectorBlock.h"
#include "ULISLoaderModule.h"
#include "ULISUtils.h"
#include "OdysseyTextureLayerImageVectorImageRenderer.h"
#include "OdysseyTextureLayerImageVectorImport.h"
#include "OdysseyTextureLayerImageVectorExport.h"
// from module OdysseyVector
#include "Import/v1/OdysseyVectorImport.h"
#include "Import/v2/OdysseyVectorImport.h"
// from module OdysseyFile
#include "OdysseyFile.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorCell.h"
#include "OdysseyVectorGroupPaint.h"
#include "UObject/OdysseyObjectEditorUtils.h"

#include "blend2d.h"

#define LOCTEXT_NAMESPACE "Texture"

UOdysseyTextureLayerImageVector::~UOdysseyTextureLayerImageVector()
{
    // bind refresh function to delegates on existing vector scenes at load. Needed to refresh necessary widgets.
    if (mVectorBlock)
        mVectorBlock->OnInvalidated().RemoveAll( this );

    if (mVectorCell)
    {
        if( mVectorCell->GetParent() )
        {
            mVectorCell->GetParent()->RemoveChild( mVectorCell.Get() );
        }
    }
}

UOdysseyTextureLayerImageVector::UOdysseyTextureLayerImageVector()
    : mVectorLayer( this )
{
    LayerTypeName = LOCTEXT("layer-image-vector.type", "Vector Image Layer");
    Icon = FSlateIcon("OdysseyStyle", "OdysseyLayerStack.LayerVector16");
}

void
UOdysseyTextureLayerImageVector::Init( uint32 iWidth, uint32 iHeight )
{
    Width  = iWidth;
    Height = iHeight;

    mVectorCell = MakeShared<FOdysseyVectorCell>( this
                                                , new FOdysseyVectorGroupPaint( "Scene" ) );

    mVectorLayer.AppendChild( mVectorCell.Get() );
}

FOdysseyVectorCell*
UOdysseyTextureLayerImageVector::GetVectorCell()
{
    return mVectorCell.Get();
}

FOdysseyMediaProvider
UOdysseyTextureLayerImageVector::GetMediaProvider(uint32 iFrameIndex) const
{
    bool isActive = IsActivatedRecursively();
    bool isLocked = IsLockedRecursively();

    FOdysseyMediaProvider mediaProvider;
    mediaProvider.IsLocked(!isActive || isLocked);

    TSharedPtr<FOdysseyMediaVector> mediaVector = MakeShared<FOdysseyMediaVector>(mVectorCell->GetScene());
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
        mVectorBlock->Init(mVectorBlockId, mVectorCell, Width, Height, format);
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
        mVectorBlock->Init(mVectorBlockId, mVectorCell, Width, Height, format);
        mVectorBlock->OnInvalidated().AddUObject(this, &UOdysseyTextureLayerImageVector::OnVectorBlockInvalidated);
    }

    // textures must be assigned to brushes in PostLoad and not in Serialize(), because the UAsset won't be fully loaded
    // and there dimensions would be 0 at that point.
    mImporterV2.PostLoadTextures();
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

FOdysseyVectorImportV2*
UOdysseyTextureLayerImageVector::GetImporterV2()
{
    return &mImporterV2;
}

void
UOdysseyTextureLayerImageVector::Serialize(FArchive& Ar)
{
    Super::Serialize( Ar );

    if( Ar.IsSaving() )
    {
        if ( mVectorCell != nullptr )
        {
            FOdysseyTextureLayerImageVectorExport::Write( this, Ar );
        }
    }

    if( Ar.IsLoading() )
    {
        if ( mVectorCell == nullptr )
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

                    FOdysseyVectorImportV1::Read( mVectorCell->GetScene(), Ar, chunkEnd );
                break;

                case FOdysseyFile::VectorV2::CHUNK_VECTOR_MAGIC_V2 :
                {
                    //UE_LOG(LogTemp, Warning, TEXT("CHUNK_VECTOR_MAGIC_V2") );

                    mImporterV2.Read( mVectorCell->GetScene(), Ar, chunkEnd );
                }
                break;

                default:
                    Ar.Seek( chunkEnd );
                break;
            }
        }

        FOdysseyVectorEngine::Notify( mVectorCell->GetScene(), FOdysseyVectorEngine::NOTIFY_ALL );
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
        RenderingChanged();
    if(iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyTextureLayerImageVector, IsColored))
        RenderingChanged();
}

void
UOdysseyTextureLayerImageVector::IsWireframeChanged()
{
    mVectorCell->GetLayer()->RequestRedraw( mVectorCell.Get(), 0 );
}

void
UOdysseyTextureLayerImageVector::IsColoredChanged()
{
    mVectorCell->GetLayer()->RequestRedraw( mVectorCell.Get(), 0 );
}

TSharedPtr<IOdysseyImageRenderer>
UOdysseyTextureLayerImageVector::BuildImageRenderer(EOdysseyRenderingType iRenderType, int iFrame, FImageRendererFilter iFilter) const
{
    if (iFilter.IsBound() && !iFilter.Execute(this))
        return nullptr;

    if (!mVectorBlock)
        return nullptr;

    return MakeShared<FOdysseyTextureLayerImageVectorImageRenderer>(this, mVectorBlock, iRenderType, GetRenderingRects(), iFilter);
}

TArray<FGuid>
UOdysseyTextureLayerImageVector::GetRenderingComposition(EOdysseyRenderingType iRenderType, int iFrame) const
{
    return { GetRenderingId() };
}

void
UOdysseyTextureLayerImageVector::OnVectorBlockInvalidated( const TArray<::ULIS::FRectI>& iRects, bool iIsInteractive)
{
    RenderingChanged(::ULISUtils::ToIntRects(iRects), iIsInteractive);
}

void
UOdysseyTextureLayerImageVector::Merge(const TArray<UOdysseyLayer*>& iLayers)
{
    FOdysseyVectorGroupPaint* destinationScene = mVectorCell->GetScene();

    for( int i = 0; i < iLayers.Num(); i++ )
    {
        UOdysseyTextureLayerImageVector* vectorLayer = Cast<UOdysseyTextureLayerImageVector>(iLayers[i]);
        if (!vectorLayer)
            continue;

        FOdysseyVectorGroupPaint* scene = vectorLayer->GetVectorCell()->GetScene();

        for( FOdysseyVectorObject* child : scene->GetChildrenList() )
        {
            FOdysseyVectorObject* copiedChild = child->Copy();
            destinationScene->AppendChild( copiedChild );

            scene->CopyBuckets( destinationScene, false );
        }
    }

    destinationScene->UpdateMatrix();

    destinationScene->GetLayer()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
    destinationScene->GetLayer()->RequestRedraw( destinationScene->GetCell(), 0 );

    FOdysseyVectorEngine::Notify( mVectorCell->GetScene(), FOdysseyVectorEngine::NOTIFY_ALL );
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

// Implements Interface IOdysseyVectorLayer::GetWidth
uint32
UOdysseyTextureLayerImageVector::GetWidth()
{
    return ( uint32 ) Width;
}

// Implements Interface IOdysseyVectorLayer::GetHeight
uint32
UOdysseyTextureLayerImageVector::GetHeight()
{
    return ( uint32 ) Height;
}

// Implements Interface IOdysseyVectorLayer::GetCellByIndex
FOdysseyVectorCell*
UOdysseyTextureLayerImageVector::GetCellByIndex( uint32 iIndex )
{
    return mVectorCell.Get();
}

// Implements Interface IOdysseyVectorLayer::Contains
bool
UOdysseyTextureLayerImageVector::Contains( FOdysseyVectorCell* iCandidateCell )
{
    return ( mVectorCell.Get() == iCandidateCell ) ? true : false;
}

// Implements Interface IOdysseyVectorLayer::GetLastCell
FOdysseyVectorCell*
UOdysseyTextureLayerImageVector::GetLastCell()
{
    return mVectorCell.Get();
}

// Implements Interface IOdysseyVectorLayer::GetLastCell
FOdysseyVectorCell*
UOdysseyTextureLayerImageVector::GetFirstCell()
{
    return mVectorCell.Get();
}

// Implements Interface IOdysseyVectorCell::GetIndex
int32
UOdysseyTextureLayerImageVector::GetIndex()
{
    return 0;
}

// Implements Interface IOdysseyVectorCell::GetLength
uint32
UOdysseyTextureLayerImageVector::GetLength()
{
    return 1;
}

// Implements Interface IOdysseyVectorCell::GetFrame
uint32
UOdysseyTextureLayerImageVector::GetFrame()
{
    return 0;
}

#undef LOCTEXT_NAMESPACE
