// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#include "OdysseyLayerStack.h"

#include "HAL/PlatformFilemanager.h"
#include "Misc/FileHelper.h"
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
    : IOdysseySerializable( 1 ) //Version of FOdysseyLayerStack
    , mResultBlock( NULL )
    , mTempBlock( NULL )
    , mLayers( new FOdysseyNTree<IOdysseyLayer*>(NULL))
    , mCurrentLayer( mLayers )
    , mWidth( -1 )
    , mHeight( -1 )
    , mTextureSourceFormat( ETextureSourceFormat::TSF_BGRA8 )
    , mIsInitialized( false )
{
}

FOdysseyLayerStack::FOdysseyLayerStack( int iWidth, int iHeight )
    : IOdysseySerializable( 1 ) //Version of FOdysseyLayerStack
    , mResultBlock( NULL )
    , mTempBlock( NULL )
    , mLayers( new FOdysseyNTree<IOdysseyLayer*>(NULL) )
    , mCurrentLayer( mLayers )
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
    
    //Is not 0 if it comes from an existing OdysseyTexture
    if( mLayers->GetNodes()->Num() == 0 )
        AddLayer( mLayers );
    
    mCurrentLayer = mLayers->GetNodes()->GetData()[0];
    
    mDrawingUndo = new FOdysseyDrawingUndo(this);
}

void
FOdysseyLayerStack::InitFromData( FOdysseyBlock* iData )
{
    if( mIsInitialized )
        return;

    checkf( iData, TEXT("Cannot Initialize Layer Stack from NULL data") );

    mWidth = iData->Width();
    mHeight = iData->Height();
    mTextureSourceFormat = iData->GetUE4TextureSourceFormat();
    mIsInitialized = true;

    InitResultAndTempBlock();
    
    //Is not 0 if it comes from an existing OdysseyTexture
    if( mLayers->GetNodes()->Num() == 0 )
        AddLayerFromData( iData, mLayers );
    
    mCurrentLayer = mLayers->GetNodes()->GetData()[0];
    
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
    TArray< IOdysseyLayer* > layers = TArray<IOdysseyLayer*>();
    mLayers->DepthFirstSearchTree( &layers );
    
    ::ULIS::FClearFillContext::Clear( mResultBlock->GetIBlock() );
    for( int i = 0; i < layers.Num(); ++i )
    {
        IOdysseyLayer::eType type = layers[i]->GetType();
        if( type != IOdysseyLayer::eType::kImage || !layers[i]->IsVisible() )
            continue;

        FOdysseyImageLayer* imageLayer = static_cast<FOdysseyImageLayer*>( layers[i] );

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

    TArray< IOdysseyLayer* > layers = TArray<IOdysseyLayer*>();
    mLayers->DepthFirstSearchTree( &layers );
    
    for( int i = 0; i < layers.Num(); ++i )
    {
        IOdysseyLayer::eType type = layers[i]->GetType();
        if( type != IOdysseyLayer::eType::kImage || !layers[i]->IsVisible() )
            continue;

        FOdysseyImageLayer* imageLayer = static_cast<FOdysseyImageLayer*>( layers[i] );

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

    TArray< IOdysseyLayer* > layers = TArray<IOdysseyLayer*>();
    mLayers->DepthFirstSearchTree( &layers );
    
    for( int i = 0; i < layers.Num(); ++i )
    {
        IOdysseyLayer::eType type = layers[i]->GetType();
        if( type != IOdysseyLayer::eType::kImage || !layers[i]->IsVisible() )
            continue;

        FOdysseyImageLayer* imageLayer = static_cast<FOdysseyImageLayer*>( layers[i] );

        if( imageLayer && imageLayer == mCurrentLayer->GetNodeContent() && iTempBuffer )
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

    TArray< IOdysseyLayer* > layers = TArray<IOdysseyLayer*>();
    mLayers->DepthFirstSearchTree( &layers );
    
    IOdysseyLayer::eType type = mCurrentLayer->GetNodeContent()->GetType();
    if( type != IOdysseyLayer::eType::kImage || !mCurrentLayer->GetNodeContent()->IsVisible() )
        return;

    FOdysseyImageLayer* imageLayer = static_cast<FOdysseyImageLayer*>( mCurrentLayer->GetNodeContent() );

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
FOdysseyLayerStack::AddLayer( FOdysseyNTree< IOdysseyLayer* >* iPosition, int iAtIndex )
{
    FOdysseyImageLayer* layer = new FOdysseyImageLayer( GetNextLayerName(), FVector2D( mWidth, mHeight ), mTextureSourceFormat );
    iPosition->AddNode( layer, iAtIndex );
    return layer;
}

FOdysseyImageLayer*
FOdysseyLayerStack::AddLayer( int iAtIndex )
{
    FOdysseyImageLayer* layer = new FOdysseyImageLayer( GetNextLayerName(), FVector2D( mWidth, mHeight ), mTextureSourceFormat );
    mCurrentLayer->AddNode( layer, iAtIndex );
    return layer;
}

FOdysseyImageLayer*
FOdysseyLayerStack::AddLayerFromData( FOdysseyBlock* iData, FOdysseyNTree< IOdysseyLayer* >* iPosition, FName iName, int iAtIndex )
{
    assert( iData->GetUE4TextureSourceFormat() == mTextureSourceFormat );

    FOdysseyBlock* explicitCopyResized = new FOdysseyBlock( mWidth, mHeight, mTextureSourceFormat );
    ::ULIS::FMakeContext::CopyBlockInto( iData->GetIBlock(), explicitCopyResized->GetIBlock() );

    FOdysseyImageLayer* layer = new FOdysseyImageLayer( iName.IsNone() ? GetNextLayerName() : iName, explicitCopyResized );
    iPosition->AddNode( layer, iAtIndex );
    return layer;
}

FOdysseyImageLayer*
FOdysseyLayerStack::AddLayerFromData( FOdysseyBlock* iData, FName iName, int iAtIndex )
{
    assert( iData->GetUE4TextureSourceFormat() == mTextureSourceFormat );

    FOdysseyBlock* explicitCopyResized = new FOdysseyBlock( mWidth, mHeight, mTextureSourceFormat );
    ::ULIS::FMakeContext::CopyBlockInto( iData->GetIBlock(), explicitCopyResized->GetIBlock() );

    FOdysseyImageLayer* layer = new FOdysseyImageLayer( iName.IsNone() ? GetNextLayerName() : iName, explicitCopyResized );
    mCurrentLayer->AddNode( layer, iAtIndex );
    return layer;
}

FOdysseyNTree< IOdysseyLayer* >*
FOdysseyLayerStack::GetLayers()
{
    return mLayers;
}

void
FOdysseyLayerStack::DeleteLayer( IOdysseyLayer* iLayerToDelete )
{
    mLayers->DeleteNodeIfExist( &iLayerToDelete );
}

/*
void
FOdysseyLayerStack::DeleteLayer( int iIndexLayerToDelete )
{
    if( iIndexLayerToDelete >= 0 && iIndexLayerToDelete < mLayers.Num() )
        mLayers.RemoveAt( iIndexLayerToDelete );

    if( iIndexLayerToDelete > 0 )
        mCurrentIndex--;
    else
        mCurrentIndex = 0;
}*/

void FOdysseyLayerStack::MergeDownLayer( IOdysseyLayer* iLayerToMergeDown )
{
    TArray< IOdysseyLayer* > layers = TArray<IOdysseyLayer*>();
    mLayers->DepthFirstSearchTree( &layers );
    
    ::ULIS::FClearFillContext::Clear( mResultBlock->GetIBlock() );
    for( int i = layers.Num() - 1; i >= 0; i-- )
    {
        if( layers[i] == iLayerToMergeDown && i != 0 && layers[i - 1]->GetType() == IOdysseyLayer::eType::kImage )
        {
            FOdysseyImageLayer* imageLayer1 = static_cast<FOdysseyImageLayer*>( layers[i] );
            FOdysseyImageLayer* imageLayer2 = static_cast<FOdysseyImageLayer*>( layers[i - 1] );

            if( imageLayer1 && imageLayer2 )
            {
                ::ULIS::FBlendingContext::Blend( imageLayer1->GetBlock()->GetIBlock(), imageLayer2->GetBlock()->GetIBlock(), ::ULIS::FRect( 0, 0, mWidth, mHeight ), imageLayer1->GetBlendingMode(), ::ULIS::eAlphaMode::kNormal, 1.f );
            }
            DeleteLayer( layers[i] );
            break;
        }
    }
    ComputeResultBlock();
}

void FOdysseyLayerStack::DuplicateLayer( IOdysseyLayer* iLayerToDuplicate )
{
    TArray< IOdysseyLayer* > layers = TArray<IOdysseyLayer*>();
    mLayers->DepthFirstSearchTree( &layers );
    
    ::ULIS::FClearFillContext::Clear( mResultBlock->GetIBlock() );
    for( int i = layers.Num() - 1; i >= 0; i-- )
    {
        if( layers[i] == iLayerToDuplicate && layers[i]->GetType() == IOdysseyLayer::eType::kImage )
        {
            FOdysseyImageLayer* imageLayer = static_cast<FOdysseyImageLayer*>( layers[i] );

            FOdysseyImageLayer* copiedLayer = AddLayerFromData( imageLayer->GetBlock(), mLayers, FName( *( imageLayer->GetName().ToString() + FString( "_Copy" ) ) ), i + 1 );

            copiedLayer->CopyPropertiesFrom( *imageLayer );

            break;
        }
    }
    for( int i = layers.Num() - 1; i >= 0; i-- )
    {
        FOdysseyImageLayer* imageLayer = static_cast<FOdysseyImageLayer*>( layers[i] );
    }
    ComputeResultBlock();
}


int
FOdysseyLayerStack::GetCurrentLayerAsIndex() const
{
    TArray< IOdysseyLayer* > layers = TArray<IOdysseyLayer*>();
    mLayers->DepthFirstSearchTree( &layers );
    
    int currentIndex = -1;
    
    for( int i = 0; i < layers.Num(); i++ )
    {
        if( mCurrentLayer->GetNodeContent() == layers[i] )
            currentIndex = i;
        break;
    }
    
    return currentIndex;
}

void
FOdysseyLayerStack::SetCurrentLayer( IOdysseyLayer* iLayer  )
{
    mCurrentLayer = mLayers->FindNode( iLayer );
}


 
void
FOdysseyLayerStack::SetCurrentLayer( FOdysseyNTree< IOdysseyLayer* >* iLayer )
{
    mCurrentLayer = iLayer;
}


FOdysseyNTree< IOdysseyLayer* >*
FOdysseyLayerStack::GetCurrentLayer() const
{
    return mCurrentLayer;
}


void
FOdysseyLayerStack::ClearCurrentLayer()
{
    if( mCurrentLayer->GetNodeContent()->GetType() == FOdysseyImageLayer::eType::kImage )
    {
        FOdysseyImageLayer* imageLayer = static_cast<FOdysseyImageLayer*>( mCurrentLayer->GetNodeContent() );
        ::ULIS::FClearFillContext::Clear( imageLayer->GetBlock()->GetIBlock() );
        ComputeResultBlock();
    }
}

void
FOdysseyLayerStack::FillCurrentLayerWithColor( const ::ULIS::CColor& iColor )
{
    if( mCurrentLayer->GetNodeContent()->GetType() == FOdysseyImageLayer::eType::kImage )
    {
        FOdysseyImageLayer* imageLayer = static_cast<FOdysseyImageLayer*>( mCurrentLayer->GetNodeContent() );
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

ETextureSourceFormat
FOdysseyLayerStack::GetTextureSourceFormat()
{
    return mTextureSourceFormat;
}

//--------------------------------------------------------------------------------------
//-------------------------------------------------------------------------- Private API
FName
FOdysseyLayerStack::GetNextLayerName()
{
    TArray< IOdysseyLayer* > layers = TArray<IOdysseyLayer*>();
    mLayers->DepthFirstSearchTree( &layers );
    
    return FName( *( FString( "Layer " ) + FString::FromInt( layers.Num() ) ) );
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
    FOdysseyImageLayer* imageLayer = static_cast<FOdysseyImageLayer*>( mLayerStackPtr->GetCurrentLayer()->GetNodeContent() );

    mData = TArray<uint8>();
    
    //We reserve the maximum memory needed for a undo
    mData.Reserve( imageLayer->GetBlock()->GetIBlock()->BytesTotal() );

    mUndoPath = FPaths::Combine( FPaths::EngineSavedDir(), TEXT("undos.save") );
    mRedoPath = FPaths::Combine( FPaths::EngineSavedDir(), TEXT("redos.save") );
    
    Clear();
}

FOdysseyDrawingUndo::~FOdysseyDrawingUndo()
{
    
}

void
FOdysseyDrawingUndo::StartRecord()
{
    mToBinary.Seek(0);
    mToBinary.Empty();
    mToBinary.FArchive::Reset();

    //If we make a record while we're not at the end of the stack, we delete all records after this one
    if( mCurrentIndex == 0 )
    {
        Clear();
    }
    else if( mCurrentIndex < mUndosPositions.Num() - 1 )
    {
        mUndosPositions.SetNum( mCurrentIndex + 1 );
        mNumberBlocksUndo.SetNum( mCurrentIndex + 1 );
        mNumberBlocksUndo[ mCurrentIndex ] = 0;
        mNumberBlocksRedo.SetNum( mCurrentIndex + 1 );
        mNumberBlocksRedo[ mCurrentIndex ] = 0;
    }
}

    
void
FOdysseyDrawingUndo::EndRecord()
{
    if( mNumberBlocksUndo[mCurrentIndex] != 0 )
    {
        IPlatformFile& platformFile = FPlatformFileManager::Get().GetPlatformFile();
        IFileHandle* fileHandle = platformFile.OpenWrite(*mUndoPath, true);
        fileHandle->Seek( mUndosPositions[mCurrentIndex] );
        fileHandle->Write( mToBinary.GetData(), mToBinary.Num() );
        fileHandle->Flush( true );

        
        mUndosPositions.Add( mUndosPositions[mCurrentIndex] + mToBinary.Num() );
        mNumberBlocksUndo.Add(0);
        mNumberBlocksRedo.Add(0);
        mCurrentIndex++;

        delete fileHandle;
    }
}


void
FOdysseyDrawingUndo::StartRecordRedo()
{
    mToBinary.Seek(0);
    mToBinary.Empty();
    mToBinary.FArchive::Reset();
}


void
FOdysseyDrawingUndo::EndRecordRedo()
{
    if( mNumberBlocksRedo[mCurrentIndex] != 0 )
    {
        IPlatformFile& platformFile = FPlatformFileManager::Get().GetPlatformFile();
        IFileHandle* fileHandle = platformFile.OpenWrite(*mRedoPath, true);
        fileHandle->Seek( mUndosPositions[mCurrentIndex] );
        fileHandle->Write( mToBinary.GetData(), mToBinary.Num() );
        fileHandle->Flush(true);
        
        delete fileHandle;
    }
}

bool
FOdysseyDrawingUndo::Clear()
{
    mToBinary.FArchive::Reset();
    mToBinary.Seek(0);
    FPlatformFileManager::Get().GetPlatformFile().DeleteFile(*mUndoPath);
    FPlatformFileManager::Get().GetPlatformFile().DeleteFile(*mRedoPath);
    mCurrentIndex = 0;
    mUndosPositions.Empty();
    mUndosPositions.Add(0);
    mNumberBlocksUndo.Empty();
    mNumberBlocksUndo.Add(0);
    mNumberBlocksRedo.Empty();
    mNumberBlocksRedo.Add(0);
    return true;
}

void FOdysseyDrawingUndo::Check()
{
    UE_LOG( LogTemp, Display, TEXT("Global:"));
    UE_LOG( LogTemp, Display, TEXT("mCurrentIndex: %d"), mCurrentIndex );
    for( int i = 0; i < mUndosPositions.Num(); i++ )
    {
        UE_LOG( LogTemp, Display, TEXT("mUndosPositions[%d]: %lld"), i, mUndosPositions[i] );
    }


    UE_LOG( LogTemp, Display, TEXT("Undo:"));
    for( int i = 0; i < mNumberBlocksUndo.Num(); i++ )
    {
        UE_LOG( LogTemp, Display, TEXT("mNumberBlocksUndo[%d]: %d"), i, mNumberBlocksUndo[i] );
    }
    
    UE_LOG( LogTemp, Display, TEXT("Redo:"));
    for( int i = 0; i < mNumberBlocksRedo.Num(); i++ )
    {
        UE_LOG( LogTemp, Display, TEXT("mNumberBlocksRedo[%d]: %d"), i, mNumberBlocksRedo[i] );
    }
}


bool
FOdysseyDrawingUndo::SaveDataRedo( UPTRINT iAddress, uint8 iXTile, uint8 iYTile, unsigned int iSizeX, unsigned int iSizeY )
{
    TArray< IOdysseyLayer* > layers = TArray<IOdysseyLayer*>();
    mLayerStackPtr->GetLayers()->DepthFirstSearchTree( &layers );
    
    FOdysseyImageLayer* imageLayer = nullptr;
    for( int j = 0; j < layers.Num(); j++)
    {
        if( iAddress == (UPTRINT) layers[j] )
        {
            imageLayer = static_cast<FOdysseyImageLayer*> (layers[j]);
            break;
        }
    }
    
    if( imageLayer == nullptr )
        return false;

    mTileData = ::ULIS::FMakeContext::CopyBlockRect( imageLayer->GetBlock()->GetIBlock(), ::ULIS::FRect( iXTile * iSizeX, iYTile * iSizeY, iSizeX, iSizeY ) );
           
    TArray<uint8> array = TArray<uint8>();
    array.AddUninitialized(mTileData->BytesTotal());
    
    FMemory::Memcpy(array.GetData(), mTileData->DataPtr(), mTileData->BytesTotal());

    UPTRINT address = (UPTRINT)imageLayer;
    mToBinary << address;
    mToBinary << iXTile;
    mToBinary << iYTile;
    mToBinary << iSizeX;
    mToBinary << iSizeY;
    mToBinary << array;
        
    mNumberBlocksRedo[mCurrentIndex]++;
    
    return true;
}

bool
FOdysseyDrawingUndo::SaveData( uint8 iXTile, uint8 iYTile, unsigned int iSizeX, unsigned int iSizeY )
{
    FOdysseyImageLayer* imageLayer = static_cast<FOdysseyImageLayer*>( mLayerStackPtr->GetCurrentLayer()->GetNodeContent() );
    mTileData = ::ULIS::FMakeContext::CopyBlockRect( imageLayer->GetBlock()->GetIBlock(), ::ULIS::FRect( iXTile * iSizeX, iYTile * iSizeY, iSizeX, iSizeY ) );
    
    TArray<uint8> array = TArray<uint8>();
    array.AddUninitialized(mTileData->BytesTotal());

    FMemory::Memcpy(array.GetData(), mTileData->DataPtr(), mTileData->BytesTotal());
    
    UPTRINT address = (UPTRINT)imageLayer;
    mToBinary << address;
    mToBinary << iXTile;
    mToBinary << iYTile;
    mToBinary << iSizeX;
    mToBinary << iSizeY;
    mToBinary << array;

    mNumberBlocksUndo[mCurrentIndex]++;
    
    return true;
}

bool
FOdysseyDrawingUndo::LoadData()
{
    if( mCurrentIndex > 0 )
        mCurrentIndex--;
    
    bool bSaveForRedo = mNumberBlocksRedo[mCurrentIndex] == 0;
        
    if( bSaveForRedo )
    {
        StartRecordRedo();
    }

    UPTRINT address;
    uint8 tileX = 0;
    uint8 tileY = 0;
    unsigned int sizeX;
    unsigned int sizeY;
    
	TArray<uint8> TheBinaryArray;
    FFileHelper::LoadFileToArray(TheBinaryArray, *mUndoPath);
    
    FMemoryReader Ar = FMemoryReader(TheBinaryArray );
    Ar.Seek(mUndosPositions[mCurrentIndex]);
    
    for( int i = 0; i < mNumberBlocksUndo[mCurrentIndex]; i++)
    {
        Ar << address;
        Ar << tileX;
        Ar << tileY;
        Ar << sizeX;
        Ar << sizeY;

        if( bSaveForRedo )
        {
            SaveDataRedo( address, tileX, tileY, sizeX, sizeY );
        }
        
        Ar << mData;

        //Should be out of this loop
        FOdysseyImageLayer* imageLayer = nullptr;
        TArray< IOdysseyLayer* > layers = TArray<IOdysseyLayer*>();
        mLayerStackPtr->GetLayers()->DepthFirstSearchTree( &layers );
        
        for( int j = 0; j < layers.Num(); j++)
        {
            if( address == (UPTRINT) layers[j] )
            {
                imageLayer = static_cast<FOdysseyImageLayer*> (layers[j]);
                break;
            }
        }

        if( imageLayer == nullptr )
            return false;
        //---

        
        //Useless, I just want mTileData at the right size for the next undo, to change
        if( i == 0 )
            mTileData = ::ULIS::FMakeContext::CopyBlockRect( imageLayer->GetBlock()->GetIBlock(), ::ULIS::FRect( tileX * sizeX, tileY * sizeY, sizeX, sizeY ) );

        if( mData.Num() > 0 && tileX >= 0 && tileY >= 0 && sizeX > 0 && sizeY > 0 )
        {
            for( int j = 0; j < mData.Num(); j++)
            {
                *(mTileData->DataPtr() + j) = mData[j];
            }
    
            ::ULIS::FMakeContext::CopyBlockRectInto( mTileData, imageLayer->GetBlock()->GetIBlock(), ::ULIS::FRect(0, 0, sizeX, sizeY ), ::ULIS::FPoint( tileX * sizeX, tileY * sizeY ) );
            mLayerStackPtr->ComputeResultBlock( ::ULIS::FRect( tileX * sizeX, tileY * sizeY, sizeX, sizeY ));
        }
    }
    
    if( bSaveForRedo )
    {
        EndRecordRedo();
    }

    return true;
}


bool
FOdysseyDrawingUndo::Redo()
{
    UPTRINT address;
    uint8 tileX;
    uint8 tileY;
    unsigned int sizeX;
    unsigned int sizeY;
    
	TArray<uint8> TheBinaryArray;
    FFileHelper::LoadFileToArray(TheBinaryArray, *mRedoPath);
    
    FMemoryReader Ar = FMemoryReader(TheBinaryArray );
    Ar.Seek(mUndosPositions[mCurrentIndex]);
    
    for( int i = 0; i < mNumberBlocksRedo[mCurrentIndex]; i++)
    {
        Ar << address;
        Ar << tileX;
        Ar << tileY;
        Ar << sizeX;
        Ar << sizeY;
        Ar << mData;
        
        //Should be out of this loop
        FOdysseyImageLayer* imageLayer = nullptr;
        TArray< IOdysseyLayer* > layers = TArray<IOdysseyLayer*>();
        mLayerStackPtr->GetLayers()->DepthFirstSearchTree( &layers );
        
        for( int j = 0; j < layers.Num(); j++)
        {
            if( address == (UPTRINT) layers[j] )
            {
                imageLayer = static_cast<FOdysseyImageLayer*> (layers[j]);
                break;
            }
        }

        if( imageLayer == nullptr )
            return false;
        //---

        
        //Useless, I just want mTileData at the right size for the next undo, to change
        if( i == 0 )
            mTileData = ::ULIS::FMakeContext::CopyBlockRect( imageLayer->GetBlock()->GetIBlock(), ::ULIS::FRect( tileX * sizeX, tileY * sizeY, sizeX, sizeY ) );

        if( mData.Num() > 0 && tileX >= 0 && tileY >= 0 && sizeX > 0 && sizeY > 0 )
        {
            for( int j = 0; j < mData.Num(); j++)
            {
                *(mTileData->DataPtr() + j) = mData[j];
            }
        
            ::ULIS::FMakeContext::CopyBlockRectInto( mTileData, imageLayer->GetBlock()->GetIBlock(), ::ULIS::FRect(0, 0, sizeX, sizeY ), ::ULIS::FPoint( tileX * sizeX, tileY * sizeY ) );
            mLayerStackPtr->ComputeResultBlock( ::ULIS::FRect( tileX * sizeX, tileY * sizeY, sizeX, sizeY ));
        }
    }
    
    if( mCurrentIndex < (mUndosPositions.Num() - 1) )
        mCurrentIndex++;

    return true;
}




#undef LOCTEXT_NAMESPACE
