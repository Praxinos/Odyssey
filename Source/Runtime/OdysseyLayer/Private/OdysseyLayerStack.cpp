// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000


#include "OdysseyLayerStack.h"
#include "OdysseyImageLayer.h"
#include "OdysseyBlock.h"
#include "UObject/UObjectGlobals.h"
#include <ULIS_CORE>

#define LOCTEXT_NAMESPACE "OdysseyLayerStack"


//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyLayerStack::~FOdysseyLayerStack()
{
    delete mResultBlock;

    //TODO: and other block ? layers ?
}

FOdysseyLayerStack::FOdysseyLayerStack()
    : mResultBlock( NULL )
    , mTempBlock( NULL )
    , mCurrentIndex( 0 )
    , mWidth( -1 )
    , mHeight( -1 )
    , mTextureSourceFormat( ETextureSourceFormat::TSF_BGRA8 )
    , mIsInitialized( false )
{
}

FOdysseyLayerStack::FOdysseyLayerStack( int iWidth, int iHeight )
    : mResultBlock( NULL )
    , mTempBlock( NULL )
    , mCurrentIndex( 0 )
    , mWidth( iWidth )
    , mHeight( iHeight )
    , mTextureSourceFormat( ETextureSourceFormat::TSF_BGRA8 )
    , mIsInitialized( true )
{
    Init( mWidth, mHeight ); //TODO: what ? mIsInitialized is already to true ?!
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------- Public API

void
FOdysseyLayerStack::Init( int iWidth, int iHeight )
{
    if( mIsInitialized )
        return;

    mWidth = iWidth;
    mHeight = iHeight;
    mTextureSourceFormat = ETextureSourceFormat::TSF_BGRA8;
    mIsInitialized = true;

    InitResultAndTempBlock();
    AddLayer();
    
    mDrawingUndo = new FOdysseyDrawingUndo(this);
}

void
FOdysseyLayerStack::InitFromData( FOdysseyBlock* iData )
{
    if( mIsInitialized )
        return;

    checkf( iData, L"Cannot Initialize Layer Stack from NULL data" );

    mWidth = iData->Width();
    mHeight = iData->Height();
    mTextureSourceFormat = iData->GetUE4TextureSourceFormat();
    mIsInitialized = true;

    InitResultAndTempBlock();
    AddLayerFromData( iData );
    
    mDrawingUndo = new FOdysseyDrawingUndo(this);
}

FOdysseyBlock*
FOdysseyLayerStack::GetResultBlock()
{
    return mResultBlock;
}

void
FOdysseyLayerStack::ComputeResultBlock()
{
    ::ULIS::FClearFillContext::Clear( mResultBlock->GetIBlock() );
    for( int i = 0; i < mLayers.Num(); ++i )
    {
        IOdysseyLayer::eType type = mLayers[i]->GetType();
        if( type != IOdysseyLayer::eType::kImage || !mLayers[i]->IsVisible() )
            continue;

        FOdysseyImageLayer* imageLayer = static_cast<FOdysseyImageLayer*>( mLayers[i].Get() );

        if( imageLayer )
            ::ULIS::FBlendingContext::Blend( imageLayer->GetBlock()->GetIBlock(), mResultBlock->GetIBlock(), ::ULIS::FRect( 0, 0, mWidth, mHeight ), imageLayer->GetBlendingMode(), ::ULIS::eAlphaMode::kNormal, imageLayer->GetOpacity() );
    }

    mResultBlock->GetIBlock()->Invalidate();
}

void
FOdysseyLayerStack::ComputeResultBlock( const ::ULIS::FRect& iRect )
{
    ::ULIS::FPerformanceOptions performanceOptions;
    performanceOptions.desired_workers = 1;
    ::ULIS::FClearFillContext::ClearRect( mResultBlock->GetIBlock(), iRect, performanceOptions, false );

    for( int i = 0; i < mLayers.Num(); ++i )
    {
        IOdysseyLayer::eType type = mLayers[i]->GetType();
        if( type != IOdysseyLayer::eType::kImage || !mLayers[i]->IsVisible() )
            continue;

        FOdysseyImageLayer* imageLayer = static_cast<FOdysseyImageLayer*>( mLayers[i].Get() );

        if( imageLayer )
            ::ULIS::FBlendingContext::Blend( imageLayer->GetBlock()->GetIBlock(), mResultBlock->GetIBlock(), iRect, imageLayer->GetBlendingMode(), ::ULIS::eAlphaMode::kNormal, imageLayer->GetOpacity(), performanceOptions, false );
    }

    mResultBlock->GetIBlock()->Invalidate( iRect );
}

void
FOdysseyLayerStack::ComputeResultBlockWithTempBuffer( const ::ULIS::FRect& iRect, FOdysseyBlock* iTempBuffer, float iOpacity, ::ULIS::eBlendingMode iMode, ::ULIS::eAlphaMode iAlphaMode )
{
    ::ULIS::FPerformanceOptions performanceOptions;
    performanceOptions.desired_workers = 1;
    ::ULIS::FClearFillContext::ClearRect( mResultBlock->GetIBlock(), iRect, performanceOptions, false );

    for( int i = 0; i < mLayers.Num(); ++i )
    {
        IOdysseyLayer::eType type = mLayers[i]->GetType();
        if( type != IOdysseyLayer::eType::kImage || !mLayers[i]->IsVisible() )
            continue;

        FOdysseyImageLayer* imageLayer = static_cast<FOdysseyImageLayer*>( mLayers[i].Get() );

        if( imageLayer && mCurrentIndex == i && iTempBuffer )
        {
            ::ULIS::FPoint pos( iRect.x, iRect.y );
            ::ULIS::FMakeContext::CopyBlockRectInto( imageLayer->GetBlock()->GetIBlock(), mTempBlock->GetIBlock(), iRect, pos, performanceOptions );
            ::ULIS::FBlendingContext::Blend( iTempBuffer->GetIBlock(), mTempBlock->GetIBlock(), iRect, iMode, imageLayer->IsAlphaLocked() ? ::ULIS::eAlphaMode::kBack : iAlphaMode, iOpacity, performanceOptions, false );
            ::ULIS::FBlendingContext::Blend( mTempBlock->GetIBlock(), mResultBlock->GetIBlock(), iRect, imageLayer->GetBlendingMode(), ::ULIS::eAlphaMode::kNormal, imageLayer->GetOpacity(), performanceOptions, false );
        }
        else if( imageLayer )
        {
            ::ULIS::FBlendingContext::Blend( imageLayer->GetBlock()->GetIBlock(), mResultBlock->GetIBlock(), iRect, imageLayer->GetBlendingMode(), ::ULIS::eAlphaMode::kNormal, imageLayer->GetOpacity(), performanceOptions, false );
        }
    }

    mResultBlock->GetIBlock()->Invalidate( iRect );
}

void
FOdysseyLayerStack::BlendTempBufferOnCurrentBlock( const ::ULIS::FRect& iRect, FOdysseyBlock* iTempBuffer, float iOpacity, ::ULIS::eBlendingMode iMode, ::ULIS::eAlphaMode iAlphaMode )
{
    ::ULIS::FPerformanceOptions performanceOptions;
    performanceOptions.desired_workers = 1;
    ::ULIS::FClearFillContext::ClearRect( mResultBlock->GetIBlock(), iRect, performanceOptions, false );

    IOdysseyLayer::eType type = mLayers[mCurrentIndex]->GetType();
    if( type != IOdysseyLayer::eType::kImage || !mLayers[mCurrentIndex]->IsVisible() )
        return;

    FOdysseyImageLayer* imageLayer = static_cast<FOdysseyImageLayer*>( mLayers[mCurrentIndex].Get() );

    if( imageLayer && iTempBuffer )
        ::ULIS::FBlendingContext::Blend( iTempBuffer->GetIBlock(), imageLayer->GetBlock()->GetIBlock(), iRect, iMode, imageLayer->IsAlphaLocked() ? ::ULIS::eAlphaMode::kBack : iAlphaMode, iOpacity, performanceOptions, false );

    ComputeResultBlock( iRect );
}

int
FOdysseyLayerStack::Width() const
{
    return mWidth;
}

int
FOdysseyLayerStack::Height() const
{
    return mHeight;
}

FVector2D
FOdysseyLayerStack::Size() const
{
    return FVector2D( mWidth, mHeight );
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------- Public Array Tampon Methods

FOdysseyImageLayer*
FOdysseyLayerStack::AddLayer( int iAtIndex )
{
    if( iAtIndex >= 0 && iAtIndex < mLayers.Num() )
    {
        FOdysseyImageLayer* layer = new FOdysseyImageLayer( GetNextLayerName(), FVector2D( mWidth, mHeight ), mTextureSourceFormat );
        mLayers.EmplaceAt( iAtIndex, TSharedPtr< FOdysseyImageLayer >( layer ) );
        return layer;
    }

    mLayers.Add( TSharedPtr< FOdysseyImageLayer >( new FOdysseyImageLayer( GetNextLayerName(), FVector2D( mWidth, mHeight ), mTextureSourceFormat ) ) );
    return static_cast<FOdysseyImageLayer*>( mLayers.Last().Get() );
}

FOdysseyImageLayer*
FOdysseyLayerStack::AddLayerFromData( FOdysseyBlock* iData, FName iName, int iAtIndex )
{
    assert( iData->GetUE4TextureSourceFormat() == mTextureSourceFormat );

    FOdysseyBlock* explicitCopyResized = new FOdysseyBlock( mWidth, mHeight, mTextureSourceFormat );
    ::ULIS::FMakeContext::CopyBlockInto( iData->GetIBlock(), explicitCopyResized->GetIBlock() );

    if( iAtIndex >= 0 && iAtIndex < mLayers.Num() )
    {
        FOdysseyImageLayer* layer = new FOdysseyImageLayer( iName.IsNone() ? GetNextLayerName() : iName, explicitCopyResized );
        mLayers.EmplaceAt( iAtIndex, TSharedPtr< FOdysseyImageLayer >( layer ) );
        return layer;
    }

    mLayers.Add( TSharedPtr< FOdysseyImageLayer >( new FOdysseyImageLayer( iName.IsNone() ? GetNextLayerName() : iName, explicitCopyResized ) ) );
    return static_cast<FOdysseyImageLayer*>( mLayers.Last().Get() );
}

TArray< TSharedPtr< IOdysseyLayer > >*
FOdysseyLayerStack::GetLayers()
{
    return &mLayers;
}

void
FOdysseyLayerStack::DeleteLayer( IOdysseyLayer* iLayerToDelete )
{
    for( int i = 0; i < mLayers.Num(); i++ )
    {
        if( mLayers[i].Get() == iLayerToDelete )
        {
            mLayers.RemoveAt( i );

            if( i > 0 )
                mCurrentIndex--;
            else
                mCurrentIndex = 0;

            break;
        }
    }
}

void
FOdysseyLayerStack::DeleteLayer( int iIndexLayerToDelete )
{
    if( iIndexLayerToDelete >= 0 && iIndexLayerToDelete < mLayers.Num() )
        mLayers.RemoveAt( iIndexLayerToDelete );

    if( iIndexLayerToDelete > 0 )
        mCurrentIndex--;
    else
        mCurrentIndex = 0;
}

void FOdysseyLayerStack::MergeDownLayer( IOdysseyLayer* iLayerToMergeDown )
{
    ::ULIS::FClearFillContext::Clear( mResultBlock->GetIBlock() );
    for( int i = mLayers.Num() - 1; i >= 0; i-- )
    {
        if( mLayers[i].Get() == iLayerToMergeDown && i != 0 && mLayers[i - 1]->GetType() == IOdysseyLayer::eType::kImage )
        {
            FOdysseyImageLayer* imageLayer1 = static_cast<FOdysseyImageLayer*>( mLayers[i].Get() );
            FOdysseyImageLayer* imageLayer2 = static_cast<FOdysseyImageLayer*>( mLayers[i - 1].Get() );

            if( imageLayer1 && imageLayer2 )
            {
                //UE_LOG(LogTemp, Display, TEXT("Merging %s into %s"), *(imageLayer1->GetName()).ToString(), *(imageLayer2->GetName()).ToString())
                ::ULIS::FBlendingContext::Blend( imageLayer1->GetBlock()->GetIBlock(), imageLayer2->GetBlock()->GetIBlock(), ::ULIS::FRect( 0, 0, mWidth, mHeight ), imageLayer1->GetBlendingMode(), ::ULIS::eAlphaMode::kNormal, 1.f );
            }
            DeleteLayer( i );
            break;
        }
    }
    ComputeResultBlock();
}

void FOdysseyLayerStack::DuplicateLayer( IOdysseyLayer* iLayerToDuplicate )
{
    ::ULIS::FClearFillContext::Clear( mResultBlock->GetIBlock() );
    for( int i = mLayers.Num() - 1; i >= 0; i-- )
    {
        if( mLayers[i].Get() == iLayerToDuplicate && mLayers[i].Get()->GetType() == IOdysseyLayer::eType::kImage )
        {
            FOdysseyImageLayer* imageLayer = static_cast<FOdysseyImageLayer*>( mLayers[i].Get() );

            FOdysseyImageLayer* copiedLayer = AddLayerFromData( imageLayer->GetBlock(), FName( *( imageLayer->GetName().ToString() + FString( "_Copy" ) ) ), i + 1 );

            copiedLayer->CopyPropertiesFrom( *imageLayer );

            //UE_LOG(LogTemp, Display, TEXT("opacity %lf, blendingMode %s, locked %i, visible %i"), copiedLayer->GetOpacity(), *(copiedLayer->GetBlendingModeAsText().ToString()), copiedLayer->IsLocked(), copiedLayer->IsVisible() );

            break;
        }
    }
    for( int i = mLayers.Num() - 1; i >= 0; i-- )
    {
        FOdysseyImageLayer* imageLayer = static_cast<FOdysseyImageLayer*>( mLayers[i].Get() );

        //UE_LOG(LogTemp, Display, TEXT("2 opacity %lf, blendingMode %s, locked %i, visible %i"), imageLayer->GetOpacity(), *(imageLayer->GetBlendingModeAsText().ToString()), imageLayer->IsLocked(), imageLayer->IsVisible() );
    }
    ComputeResultBlock();
}

int
FOdysseyLayerStack::GetCurrentLayerIndex() const
{
    return mCurrentIndex;
}

void
FOdysseyLayerStack::SetCurrentLayerIndex( int iIndex )
{
    if( iIndex < 0 || iIndex >= mLayers.Num() )
        return;

    mCurrentIndex = iIndex;
}

TSharedPtr< IOdysseyLayer >
FOdysseyLayerStack::GetCurrentLayer() const
{
    return mLayers[mCurrentIndex];
}

void
FOdysseyLayerStack::ClearCurrentLayer()
{
    if( mLayers[mCurrentIndex]->GetType() == FOdysseyImageLayer::eType::kImage )
    {
        FOdysseyImageLayer* imageLayer = static_cast<FOdysseyImageLayer*>( mLayers[mCurrentIndex].Get() );
        ::ULIS::FClearFillContext::Clear( imageLayer->GetBlock()->GetIBlock() );
        ComputeResultBlock();
    }
}

void
FOdysseyLayerStack::FillCurrentLayerWithColor( const ::ULIS::CColor& iColor )
{
    if( mLayers[mCurrentIndex]->GetType() == FOdysseyImageLayer::eType::kImage )
    {
        FOdysseyImageLayer* imageLayer = static_cast<FOdysseyImageLayer*>( mLayers[mCurrentIndex].Get() );
        ::ULIS::FClearFillContext::Fill( imageLayer->GetBlock()->GetIBlock(), iColor );
        ComputeResultBlock();
    }
}

TArray< TSharedPtr< FText > >
FOdysseyLayerStack::GetBlendingModesAsText()
{
    TArray< TSharedPtr< FText > > array;
    for( int i = 0; i < ( int )::ULIS::eBlendingMode::kNumBlendingModes; ++i )
        array.Add( MakeShared< FText >( FText::FromString( ANSI_TO_TCHAR( ::ULIS::kwBlendingMode[i] ) ) ) );

    return array;
}

//--------------------------------------------------------------------------------------
//-------------------------------------------------------------------------- Private API
FName
FOdysseyLayerStack::GetNextLayerName()
{
    return FName( *( FString( "Layer " ) + FString::FromInt( mLayers.Num() ) ) );
}

void
FOdysseyLayerStack::InitResultAndTempBlock()
{
    if( !mIsInitialized )
        return;

    mResultBlock = new FOdysseyBlock( mWidth, mHeight, mTextureSourceFormat );
    mTempBlock = new FOdysseyBlock( mWidth, mHeight, mTextureSourceFormat );
    ::ULIS::FClearFillContext::Clear( mResultBlock->GetIBlock() );
    ::ULIS::FClearFillContext::Clear( mTempBlock->GetIBlock() );
}

//---


//FODysseyDrawingUndo ---------
FOdysseyDrawingUndo::FOdysseyDrawingUndo( FOdysseyLayerStack* iLayerStack )
{
    mLayerStackPtr = iLayerStack;
    FOdysseyImageLayer* imageLayer = static_cast<FOdysseyImageLayer*>( mLayerStackPtr->GetCurrentLayer().Get() );

    
    mData = TArray<uint8>();
    
    //We reserve the maximum memory needed for a undo
    mData.Reserve( imageLayer->GetBlock()->GetIBlock()->BytesTotal() );

    UE_LOG(LogTemp, Display, TEXT("total: %d"), mData.Num() );
    UE_LOG(LogTemp, Display, TEXT("totalMemory: %d"), mData.GetAllocatedSize() );

    mSavePath = "/Users/praxinos/Documents/TestSave/MySave.save";

    //SaveData();
    //LoadData();
}

FOdysseyDrawingUndo::~FOdysseyDrawingUndo()
{
    
}

bool
FOdysseyDrawingUndo::Clear()
{
    FPlatformFileManager::Get().GetPlatformFile().DeleteFile(*mSavePath);
    return true;
}

bool
FOdysseyDrawingUndo::SaveData( uint8 iXTile, uint8 iYTile, uint8 iSizeX, uint8 iSizeY )
{
    UE_LOG(LogTemp, Display, TEXT("SaveStart") );
    FOdysseyImageLayer* imageLayer = static_cast<FOdysseyImageLayer*>( mLayerStackPtr->GetCurrentLayer().Get() );
    mTileData = ::ULIS::FMakeContext::CopyBlockRect( imageLayer->GetBlock()->GetIBlock(), ::ULIS::FRect( iXTile * iSizeX, iYTile * iSizeY, iSizeX, iSizeY ) );
    
    TArray<uint8> array = TArray<uint8>();
    array.AddUninitialized(mTileData->BytesTotal());
    
    FMemory::Memcpy(array.GetData(), mTileData->DataPtr(), mTileData->BytesTotal());
    
    UPTRINT address = (UPTRINT)imageLayer;
    
    FBufferArchive mToBinary;
    mToBinary << address;

    FFileHelper::SaveArrayToFile( mToBinary, *mSavePath, &IFileManager::Get()/*, EFileWrite::FILEWRITE_Append */);
    UE_LOG(LogTemp, Display, TEXT("SaveEnd") );

    return true;
}

bool
FOdysseyDrawingUndo::LoadData()
{
    UE_LOG(LogTemp, Display, TEXT("LoadStart") );
    
	TArray<uint8> TheBinaryArray;
    FFileHelper::LoadFileToArray(TheBinaryArray, *mSavePath);
    
    FMemoryReader Ar = FMemoryReader(TheBinaryArray, true); //true, free data after done
    Ar.Seek(0); //make sure we are at the beginning
    UPTRINT address;
    Ar << address;
    
    FOdysseyImageLayer* imageLayer = static_cast<FOdysseyImageLayer*>( mLayerStackPtr->GetCurrentLayer().Get() );
    UPTRINT address2 = (UPTRINT)imageLayer;

    if(address == address2)
        UE_LOG(LogTemp, Display, TEXT("It works !") );

    /*FOdysseyImageLayer* imageLayer = static_cast<FOdysseyImageLayer*>( mLayerStackPtr->GetCurrentLayer().Get() );

	TArray<uint8> TheBinaryArray;
    FFileHelper::LoadFileToArray(TheBinaryArray, *mSavePath);
    
    for( int i = 0; i < TheBinaryArray.Num() - 4; i++)
    {
        *(mTileData->DataPtr() + i) = TheBinaryArray[i + 4];
    }
    
    ::ULIS::FMakeContext::CopyBlockRectInto( mTileData, imageLayer->GetBlock()->GetIBlock(), ::ULIS::FRect( 0, 0, 1023, 1023 ), ::ULIS::FPoint(0,0) );

    UE_LOG(LogTemp, Display, TEXT("LoadEnd") );*/

    return true;
}






#undef LOCTEXT_NAMESPACE
