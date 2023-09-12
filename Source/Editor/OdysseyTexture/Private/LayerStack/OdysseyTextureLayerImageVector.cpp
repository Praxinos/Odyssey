// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyTextureLayerImageVector.h"

#include "OdysseyTextureLayerStack.h"
#include "OdysseyPixelFormat.h"
#include "EditorStyleSet.h"
#include "OdysseyMediaVector.h"
#include "ULISLoaderModule.h"
#include "ULISUtils.h"
#include "Import/v2/OdysseyVectorImport.h"
#include "Import/v1/OdysseyVectorImport.h"
#include "LayerStack/OdysseyTextureLayerImageVectorImageRenderer.h"

#include "blend2d.h"

#define LOCTEXT_NAMESPACE "UOdysseyTextureLayerImageVector"

UOdysseyTextureLayerImageVector::FOnBlendModeChanged&
UOdysseyTextureLayerImageVector::OnBlendModeChanged()
{
    static FOnBlendModeChanged onBlendModeChanged;
    return onBlendModeChanged;
}

UOdysseyTextureLayerImageVector::FOnOpacityChanged&
UOdysseyTextureLayerImageVector::OnOpacityChanged()
{
    static FOnOpacityChanged onOpacityChanged;
    return onOpacityChanged;
}

UOdysseyTextureLayerImageVector::~UOdysseyTextureLayerImageVector()
{
    // TODO: free the scene

    //mScene->OnUpdateDelegate().Remove(mOnRefreshHandle);

    // bind refresh function to delegates on existing vector scenes at load. Needed to refresh necessary widgets.
    FOdysseyVectorEngine::OnSignalDelegate().RemoveAll( this );
}

UOdysseyTextureLayerImageVector::UOdysseyTextureLayerImageVector()
    : mEngine(nullptr)
{
	LayerTypeName = LOCTEXT("LayerTypeName", "Vector Image Layer");
    Icon = *FOdysseyStyle::GetBrush( "OdysseyLayerStack.LayerVector16");
}

void
UOdysseyTextureLayerImageVector::Init( uint32 iWidth, uint32 iHeight )
{
    BLImageData imgData;

    Width  = iWidth;
    Height = iHeight;

    mEngine = new FOdysseyVectorEngine( new FOdysseyVectorScene( "Scene" )
                                       , (double)iWidth
                                       , (double)iHeight );
//UE_LOG(LogTemp, Warning, TEXT("UOdysseyTextureLayerImageVector::Init %X"), mEngine );
    // record a callback to refresh the layer when a property of an object's details view is changed
    //mOnRefreshHandle = mScene->OnUpdateDelegate().AddUObject( this, &UOdysseyTextureLayerImageVector::OnRefresh );
    
    // bind refresh function to delegates on existing vector scenes at load. Needed to refresh necessary widgets.
    FOdysseyVectorEngine::OnSignalDelegate().AddUObject( this, &UOdysseyTextureLayerImageVector::OnVectorSceneSignal );

    //UE_LOG(LogTemp,Warning,TEXT("UOdysseyTextureLayerImageVector::Init %d %d %d"), iWidth, iHeight, mVEngine );

    mEngine->GetBLImage()->getData( &imgData );

    mBlock = MakeShared<::ULIS::FBlock>(static_cast<uint8*>(imgData.pixelData)
                               , iWidth
                               , iHeight
    // ::ULIS::eFormat::Format_BGRA8 is the same as Blend2D's BL_FORMAT_PRGB32
                               , ::ULIS::eFormat::Format_BGRA8
                               , nullptr);
}

FOdysseyVectorEngine*
UOdysseyTextureLayerImageVector::GetEngine()
{
    return mEngine;
}

FOdysseyMediaProvider
UOdysseyTextureLayerImageVector::GetMediaProvider(uint32 iFrameIndex) const
{
    FOdysseyMediaProvider mediaProvider;
    TSharedPtr<FOdysseyMediaVector> mediaVector = MakeShared<FOdysseyMediaVector>(mEngine->GetScene());

    bool isActive = UOdysseyLayerFunctionLibrary::IsLayerActivatedInStack(this);
    bool isLocked = UOdysseyLayerFunctionLibrary::IsLayerLockedInStack(this);
    mediaVector->IsLocked(!isActive || isLocked);

    mediaProvider.Add(mediaVector);
    return mediaProvider;
}

void
UOdysseyTextureLayerImageVector::OnCreated_Implementation()
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetLayerStack());
    if(!layerStack)
        return;

    UTexture2D* texture = layerStack->GetTexture();

    Init( texture->Source.GetSizeX(), texture->Source.GetSizeY() );
}

void
UOdysseyTextureLayerImageVector::Serialize(FArchive& Ar)
{
    Super::Serialize( Ar );

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
            // commented out: at that point, the texture owning the layer stack doe snot have width and height values. 
            // This should be changed. As a bypass, I store dimensions in Width and Height UProperties.
            //UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetLayerStack());
            //if(!layerStack)
            //    return;
            //UTexture2D* texture = layerStack->GetTexture();

            //Init( texture->Source.GetSizeX(), texture->Source.GetSizeY() );

            Init( Width, Height );
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
UOdysseyTextureLayerImageVector::PropertyChanged(const FName& iPropertyName)
{
    Super::PropertyChanged(iPropertyName);
    if(iPropertyName == "IsColored")
        IsColoredChanged();
    if (iPropertyName == "BlendMode")
        BlendModeChanged();
    if (iPropertyName == "Opacity")
        OpacityChanged();
}

void
UOdysseyTextureLayerImageVector::IsColoredChanged()
{
    mEngine->Invalidate(); //Force engine invalidation here, because IsColored is not a part of the engine, but still needs the engine to redraw itself
    ImageRenderingChanged();
}

void
UOdysseyTextureLayerImageVector::OpacityChanged()
{
    OnOpacityChanged().Broadcast(this);
    
    ImageRenderingChanged();
}

void
UOdysseyTextureLayerImageVector::BlendModeChanged()
{
    OnBlendModeChanged().Broadcast(this);
    
    ImageRenderingChanged();
}

void
UOdysseyTextureLayerImageVector::OnVectorSceneSignal( FOdysseyVectorScene* iScene, uint64 iSignalFlags )
{
    if (mEngine->GetScene() != iScene)
        return;

    if( iSignalFlags & FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW )
    {
        mEngine->Invalidate();
        ImageRenderingChanged(iSignalFlags & FOdysseyVectorEngine::SIGNAL_INTERACTIVE);
    }
}

TSharedPtr<IOdysseyImageRenderer>
UOdysseyTextureLayerImageVector::BuildImageRenderer(IOdysseyImageRenderer::eRenderType iRenderType) const
{
    return MakeShared<FOdysseyTextureLayerImageVectorImageRenderer>(this, mBlock, iRenderType, GetImageRenderingRects());
}

TArray<FGuid>
UOdysseyTextureLayerImageVector::GetImageRenderingComposition(IOdysseyImageRenderer::eRenderType iRenderType) const
{
    return { GetImageRenderingId() };
}

::ULIS::eBlendMode
UOdysseyTextureLayerImageVector::GetImageRenderingBlendMode() const
{
    return (::ULIS::eBlendMode)BlendMode;
}

float
UOdysseyTextureLayerImageVector::GetImageRenderingOpacity() const
{
    return Opacity;
}

#undef LOCTEXT_NAMESPACE
