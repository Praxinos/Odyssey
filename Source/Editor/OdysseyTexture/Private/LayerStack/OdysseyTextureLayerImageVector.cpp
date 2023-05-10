// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyTextureLayerImageVector.h"

#include "OdysseyTextureLayerStack.h"
#include "OdysseyPixelFormat.h"
#include "EditorStyleSet.h"
#include "ULISLoaderModule.h"
#include "ULISUtils.h"
#include "Export/OdysseyVectorExport.h"
#include "Import/OdysseyVectorImport.h"

#include "blend2d.h"

#define LOCTEXT_NAMESPACE "UOdysseyTextureLayerImageVector"

UOdysseyTextureLayerImageVector::~UOdysseyTextureLayerImageVector()
{
    // TODO: free the scene

    mScene->mRefreshLayer.Remove(mOnRefreshHandle);
}

UOdysseyTextureLayerImageVector::UOdysseyTextureLayerImageVector()
    : mScene(nullptr)
    , mVEngine(nullptr)
{
	LayerTypeName = LOCTEXT("LayerTypeName", "Vector Image Layer");
    Icon = *FOdysseyStyle::GetBrush( "OdysseyLayerStack.ImageLayer16");
} 

void
UOdysseyTextureLayerImageVector::Init( uint32 iWidth, uint32 iHeight )
{
    BLImageData imgData;

    Width  = iWidth;
    Height = iHeight;

    mVEngine = new FOdysseyVectorEngine( (double)iWidth
                                       , (double)iHeight );

    mScene = new FOdysseyVectorScene();
    mScene->SetEngine( mVEngine );
    mScene->Init( "Vector Scene" );

    // record a callback to refresh the layer when a property of an object's details view is changed
    mOnRefreshHandle = mScene->mRefreshLayer.AddUObject(this, &UOdysseyTextureLayerImageVector::OnRefresh);

    UE_LOG(LogTemp,Warning,TEXT("UOdysseyTextureLayerImageVector::Init %d %d %d"), iWidth, iHeight, mVEngine );

    mVEngine->GetBLImage()->getData( &imgData );

    mBlock = MakeShared<::ULIS::FBlock>(static_cast<uint8*>(imgData.pixelData)
                               , iWidth
                               , iHeight
                               , ::ULIS::eFormat::Format_RGBA8
                               , nullptr);
}

FOdysseyVectorEngine*
UOdysseyTextureLayerImageVector::GetEngine()
{
    return mVEngine;
}

FOdysseyVectorScene*
UOdysseyTextureLayerImageVector::GetScene()
{
    return mScene;
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

TArray<::ULIS::FEvent>
UOdysseyTextureLayerImageVector::RenderImage(TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> ioBlock, const ::ULIS::FRectI& iRect, const ::ULIS::FVec2I& iPos, const TArray<::ULIS::FEvent>& iWaitList)
{
    if (!IsActivated)
        return iWaitList;

    if (!ioBlock)
        return iWaitList;

    // TODO: set region of interest as parameter ?
    mVEngine->Render( mScene, (::ULIS::FRectI&) iRect );

    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(mBlock->Format());

    TArray<::ULIS::FEvent> eventConvertAndExecute = ULISUtils::ConvertAndExecute(ioBlock, mBlock->Format(), iRect, iPos, iWaitList,
        [this, &ctx](TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> ioDest, const ::ULIS::FRectI& iRect, const ::ULIS::FVec2I& iPos, const TArray<::ULIS::FEvent>& iWaitList) -> TArray<::ULIS::FEvent>
        {
            ::ULIS::FEvent eventBlend = FULISEventBuilder().RetainBlock(mBlock).Build();

            ctx.Blend(
                *mBlock,
                *ioDest,
                iRect,
                iPos,
                ::ULIS::eBlendMode(/*BlendMode*/::ULIS::eBlendMode::Blend_Normal),
                ::ULIS::Alpha_Normal,
                /*Opacity*/1.0f,
                ::ULIS::FSchedulePolicy::/*CacheEfficient*/AsyncCacheEfficient,
                iWaitList.Num(),
                iWaitList.GetData(),
                &eventBlend
            );

            return { eventBlend };
        }
    );

    ctx.Flush();
    ctx.Finish();

    return eventConvertAndExecute;
}

void
UOdysseyTextureLayerImageVector::Serialize(FArchive& Ar)
{
    Super::Serialize( Ar );

    if( Ar.IsSaving() )
    {

        FOdysseyVectorExport::Write( mScene, Ar );
    }

    if( Ar.IsLoading() )
    {
        if ( mVEngine == nullptr )
        {
            Init( Width, Height );
        }

        FOdysseyVectorImport::Read( mScene, Ar );
    }

}

void
UOdysseyTextureLayerImageVector::OnRefresh( FOdysseyVectorScene* iScene )
{
    if( mScene == iScene )
    {
        RenderImageChanged(false);
    }
}

#undef LOCTEXT_NAMESPACE
