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
    delete mTempBlock;
    delete mLayers;
    delete mDrawingUndo;
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
    , mIsInitialized( false )
{
    Init( mWidth, mHeight );
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
        AddImageLayer( mLayers );
    
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
        AddImageLayerFromData( iData, mLayers );
    
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
    ::ULIS::FPerformanceOptions performanceOptions;
    performanceOptions.desired_workers = 1;
    ::ULIS::FRect canvasRect = ::ULIS::FRect( 0, 0, mWidth, mHeight );
    ::ULIS::FClearFillContext::Clear( mResultBlock->GetIBlock(), performanceOptions, false );
    
    TArray< IOdysseyLayer* > layers = TArray<IOdysseyLayer*>();
    mLayers->DepthFirstSearchTree( &layers, false );
    
    TMap< FOdysseyNTree<IOdysseyLayer*>*, FOdysseyBlock* > folderBlocks = TMap< FOdysseyNTree<IOdysseyLayer*>*, FOdysseyBlock* >();
    
    for( int i = layers.Num() - 1; i >= 0 ; i-- )
    {
        IOdysseyLayer::eType type = layers[i]->GetType();
        if( type == IOdysseyLayer::eType::kImage && layers[i]->IsVisible() )
        {
            FOdysseyImageLayer* imageLayer = static_cast<FOdysseyImageLayer*>( layers[i] );
            FOdysseyNTree<IOdysseyLayer*>* imageNode = mLayers->FindNode(imageLayer);
            FOdysseyNTree<IOdysseyLayer*>* parentNode = imageNode->GetParent();
            
            if( parentNode->GetNodeContent() != NULL ) //This means the imageLayer is inside a folder
            {
                bool visible = true;
                while( parentNode->GetNodeContent() != NULL )
                {
                    //If a folder above isn't visible, we don't add this image layer to the result block
                    if( !parentNode->GetNodeContent()->IsVisible() )
                        visible = false;
                    
                    parentNode = parentNode->GetParent();
                }
                if( visible )
                {
                    if( folderBlocks.Contains(imageNode->GetParent() ) )
                    {
                        ::ULIS::FBlendingContext::Blend( imageLayer->GetBlock()->GetIBlock(), folderBlocks[imageNode->GetParent()]->GetIBlock(), canvasRect, imageLayer->GetBlendingMode(), ::ULIS::eAlphaMode::kNormal, imageLayer->GetOpacity(), performanceOptions, false );
                    }
                    else
                    {
                        FOdysseyBlock* block = new FOdysseyBlock( mWidth, mHeight, mTextureSourceFormat );
                        ::ULIS::FClearFillContext::Clear( block->GetIBlock(), performanceOptions, false  );
                        ::ULIS::FBlendingContext::Blend( imageLayer->GetBlock()->GetIBlock(), block->GetIBlock(), canvasRect, imageLayer->GetBlendingMode(), ::ULIS::eAlphaMode::kNormal, imageLayer->GetOpacity(), performanceOptions, false );
                        
                        folderBlocks.Add(imageNode->GetParent(), block );
                    }
                }
            }
            else //Image layer is at the root
            {
                ::ULIS::FBlendingContext::Blend( imageLayer->GetBlock()->GetIBlock(), mResultBlock->GetIBlock(), canvasRect, imageLayer->GetBlendingMode(), ::ULIS::eAlphaMode::kNormal, imageLayer->GetOpacity(), performanceOptions, false );
            }
        }
        else if( type == IOdysseyLayer::eType::kFolder && layers[i]->IsVisible() )
        {
            FOdysseyNTree<IOdysseyLayer*>* folderNode = mLayers->FindNode( layers[i] );
            FOdysseyFolderLayer* folderLayer = static_cast<FOdysseyFolderLayer*>( layers[i] );

            if( folderBlocks.Contains( folderNode ) ) //Time to blend the folder unto the resultBlock
            {
                ::ULIS::FBlendingContext::Blend( folderBlocks[folderNode]->GetIBlock(), mResultBlock->GetIBlock(), canvasRect, folderLayer->GetBlendingMode(), ::ULIS::eAlphaMode::kNormal, folderLayer->GetOpacity(), performanceOptions, false  );
            }
        }
    }

    mResultBlock->GetIBlock()->Invalidate();
    
    for (auto& elem : folderBlocks)
    {
        delete elem.Value;
    }
}

void
FOdysseyLayerStack::ComputeResultBlock( const ::ULIS::FRect& iRect )
{
    ::ULIS::FPerformanceOptions performanceOptions;
    performanceOptions.desired_workers = 1;
    ::ULIS::FClearFillContext::ClearRect( mResultBlock->GetIBlock(), iRect, performanceOptions, false );
    
    TArray< IOdysseyLayer* > layers = TArray<IOdysseyLayer*>();
    mLayers->DepthFirstSearchTree( &layers, false );
    
    TMap< FOdysseyNTree<IOdysseyLayer*>*, FOdysseyBlock* > folderBlocks = TMap< FOdysseyNTree<IOdysseyLayer*>*, FOdysseyBlock* >();
    
    for( int i = layers.Num() - 1; i >= 0 ; i-- )
    {
        IOdysseyLayer::eType type = layers[i]->GetType();
        if( type == IOdysseyLayer::eType::kImage && layers[i]->IsVisible() )
        {
            FOdysseyImageLayer* imageLayer = static_cast<FOdysseyImageLayer*>( layers[i] );
            FOdysseyNTree<IOdysseyLayer*>* imageNode = mLayers->FindNode(imageLayer);
            FOdysseyNTree<IOdysseyLayer*>* parentNode = imageNode->GetParent();
            
            if( parentNode->GetNodeContent() != NULL ) //This means the imageLayer is inside a folder
            {
                bool visible = true;
                while( parentNode->GetNodeContent() != NULL )
                {
                    //If a folder above isn't visible, we don't add this image layer to the result block
                    if( !parentNode->GetNodeContent()->IsVisible() )
                        visible = false;
                    
                    parentNode = parentNode->GetParent();
                }
                if( visible )
                {
                    if( folderBlocks.Contains(imageNode->GetParent() ) )
                    {
                        ::ULIS::FBlendingContext::Blend( imageLayer->GetBlock()->GetIBlock(), folderBlocks[imageNode->GetParent()]->GetIBlock(), iRect, imageLayer->GetBlendingMode(), ::ULIS::eAlphaMode::kNormal, imageLayer->GetOpacity(), performanceOptions, false  );
                    }
                    else
                    {
                        FOdysseyBlock* block = new FOdysseyBlock( mWidth, mHeight, mTextureSourceFormat );
                        ::ULIS::FClearFillContext::Clear( block->GetIBlock(), performanceOptions, false  );
                        ::ULIS::FBlendingContext::Blend( imageLayer->GetBlock()->GetIBlock(), block->GetIBlock(), iRect, imageLayer->GetBlendingMode(), ::ULIS::eAlphaMode::kNormal, imageLayer->GetOpacity(), performanceOptions, false  );
                        
                        folderBlocks.Add(imageNode->GetParent(), block );
                    }
                }
            }
            else //Image layer is at the root
            {
                ::ULIS::FBlendingContext::Blend( imageLayer->GetBlock()->GetIBlock(), mResultBlock->GetIBlock(), iRect, imageLayer->GetBlendingMode(), ::ULIS::eAlphaMode::kNormal, imageLayer->GetOpacity(), performanceOptions, false  );
            }
        }
        else if( type == IOdysseyLayer::eType::kFolder && layers[i]->IsVisible() )
        {
            FOdysseyNTree<IOdysseyLayer*>* folderNode = mLayers->FindNode( layers[i] );
            FOdysseyFolderLayer* folderLayer = static_cast<FOdysseyFolderLayer*>( layers[i] );

            if( folderBlocks.Contains( folderNode ) ) //Time to blend the folder unto the resultBlock
            {
                ::ULIS::FBlendingContext::Blend( folderBlocks[folderNode]->GetIBlock(), mResultBlock->GetIBlock(), iRect, folderLayer->GetBlendingMode(), ::ULIS::eAlphaMode::kNormal, folderLayer->GetOpacity(), performanceOptions, false  );
            }
        }
    }

    mResultBlock->GetIBlock()->Invalidate(iRect);
    
    for (auto& elem : folderBlocks)
    {
        delete elem.Value;
    }
}

void
FOdysseyLayerStack::ComputeResultBlockWithTempBuffer( const ::ULIS::FRect& iRect, FOdysseyBlock* iTempBuffer, float iOpacity, ::ULIS::eBlendingMode iMode, ::ULIS::eAlphaMode iAlphaMode )
{
    ::ULIS::FPerformanceOptions performanceOptions;
    performanceOptions.desired_workers = 1;
    ::ULIS::FClearFillContext::ClearRect( mResultBlock->GetIBlock(), iRect, performanceOptions, false );

    TArray< IOdysseyLayer* > layers = TArray<IOdysseyLayer*>();
    mLayers->DepthFirstSearchTree( &layers, false );
    
    TMap< FOdysseyNTree<IOdysseyLayer*>*, FOdysseyBlock* > folderBlocks = TMap< FOdysseyNTree<IOdysseyLayer*>*, FOdysseyBlock* >();
    
    for( int i = layers.Num() - 1; i >= 0 ; i-- )
    {
        IOdysseyLayer::eType type = layers[i]->GetType();
        if( type == IOdysseyLayer::eType::kImage && layers[i]->IsVisible() )
        {
            FOdysseyImageLayer* imageLayer = static_cast<FOdysseyImageLayer*>( layers[i] );
            FOdysseyNTree<IOdysseyLayer*>* imageNode = mLayers->FindNode(imageLayer);
            FOdysseyNTree<IOdysseyLayer*>* parentNode = imageNode->GetParent();
            
            if( parentNode->GetNodeContent() != NULL ) //This means the imageLayer is inside a folder
            {
                bool visible = true;
                while( parentNode->GetNodeContent() != NULL )
                {
                    //If a folder above isn't visible, we don't add this image layer to the result block
                    if( !parentNode->GetNodeContent()->IsVisible() )
                        visible = false;
                    
                    parentNode = parentNode->GetParent();
                }
                if( visible )
                {
                    if( folderBlocks.Contains(imageNode->GetParent() ) )
                    {
                        if( imageLayer && imageLayer == mCurrentLayer->GetNodeContent() && iTempBuffer )
                        {
                            ::ULIS::FPoint pos( iRect.x, iRect.y );
                            ::ULIS::FMakeContext::CopyBlockRectInto( imageLayer->GetBlock()->GetIBlock(), mTempBlock->GetIBlock(), iRect, pos, performanceOptions );
                            ::ULIS::FBlendingContext::Blend( iTempBuffer->GetIBlock(), mTempBlock->GetIBlock(), iRect, iMode, imageLayer->IsAlphaLocked() ? ::ULIS::eAlphaMode::kBack : iAlphaMode, iOpacity, performanceOptions, false );
                            ::ULIS::FBlendingContext::Blend( mTempBlock->GetIBlock(), folderBlocks[imageNode->GetParent()]->GetIBlock(), iRect, imageLayer->GetBlendingMode(), ::ULIS::eAlphaMode::kNormal, imageLayer->GetOpacity(), performanceOptions, false );
                        }
                        else
                        {
                            ::ULIS::FBlendingContext::Blend( imageLayer->GetBlock()->GetIBlock(), folderBlocks[imageNode->GetParent()]->GetIBlock(), iRect, imageLayer->GetBlendingMode(), ::ULIS::eAlphaMode::kNormal, imageLayer->GetOpacity(), performanceOptions, false );
                        }
                    }
                    else
                    {
                        if( imageLayer && imageLayer == mCurrentLayer->GetNodeContent() && iTempBuffer )
                        {
                            FOdysseyBlock* block = new FOdysseyBlock( mWidth, mHeight, mTextureSourceFormat );
                            ::ULIS::FClearFillContext::Clear( block->GetIBlock(), performanceOptions, false  );
                            
                            ::ULIS::FPoint pos( iRect.x, iRect.y );
                            ::ULIS::FMakeContext::CopyBlockRectInto( imageLayer->GetBlock()->GetIBlock(), mTempBlock->GetIBlock(), iRect, pos, performanceOptions );
                            ::ULIS::FBlendingContext::Blend( iTempBuffer->GetIBlock(), mTempBlock->GetIBlock(), iRect, iMode, imageLayer->IsAlphaLocked() ? ::ULIS::eAlphaMode::kBack : iAlphaMode, iOpacity, performanceOptions, false );
                            ::ULIS::FBlendingContext::Blend( mTempBlock->GetIBlock(), block->GetIBlock(), iRect, imageLayer->GetBlendingMode(), ::ULIS::eAlphaMode::kNormal, imageLayer->GetOpacity(), performanceOptions, false );
                            
                            folderBlocks.Add(imageNode->GetParent(), block );
                        }
                        else
                        {
                            FOdysseyBlock* block = new FOdysseyBlock( mWidth, mHeight, mTextureSourceFormat );
                            ::ULIS::FClearFillContext::Clear( block->GetIBlock(), performanceOptions, false );
                            
                            ::ULIS::FBlendingContext::Blend( imageLayer->GetBlock()->GetIBlock(), block->GetIBlock(), iRect, imageLayer->GetBlendingMode(), ::ULIS::eAlphaMode::kNormal, imageLayer->GetOpacity(), performanceOptions, false );
                            
                            folderBlocks.Add(imageNode->GetParent(), block );
                        }
                    }
                }
            }
            else //Image layer is at the root
            {
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
        }
        else if( type == IOdysseyLayer::eType::kFolder && layers[i]->IsVisible() )
        {
            FOdysseyNTree<IOdysseyLayer*>* folderNode = mLayers->FindNode( layers[i] );
            FOdysseyFolderLayer* folderLayer = static_cast<FOdysseyFolderLayer*>( layers[i] );

            if( folderBlocks.Contains( folderNode ) ) //Time to blend the folder unto the resultBlock
            {
                ::ULIS::FBlendingContext::Blend( folderBlocks[folderNode]->GetIBlock(), mResultBlock->GetIBlock(), iRect, folderLayer->GetBlendingMode(), ::ULIS::eAlphaMode::kNormal, folderLayer->GetOpacity(), performanceOptions, false );
            }
        }
    }

    mResultBlock->GetIBlock()->Invalidate(iRect);
    
    for (auto& elem : folderBlocks)
    {
        delete elem.Value;
    }
}

void
FOdysseyLayerStack::BlendTempBufferOnCurrentBlock( const ::ULIS::FRect& iRect, FOdysseyBlock* iTempBuffer, float iOpacity, ::ULIS::eBlendingMode iMode, ::ULIS::eAlphaMode iAlphaMode )
{
    ::ULIS::FPerformanceOptions performanceOptions;
    performanceOptions.desired_workers = 1;
    ::ULIS::FClearFillContext::ClearRect( mResultBlock->GetIBlock(), iRect, performanceOptions, false );

    TArray< IOdysseyLayer* > layers = TArray<IOdysseyLayer*>();
    mLayers->DepthFirstSearchTree( &layers, false );
    
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
FOdysseyLayerStack::AddImageLayer( FOdysseyNTree< IOdysseyLayer* >* iPosition, int iAtIndex )
{
    FOdysseyImageLayer* layer = new FOdysseyImageLayer( GetNextLayerName(), FVector2D( mWidth, mHeight ), mTextureSourceFormat );
    mCurrentLayer = iPosition->AddNode( layer, iAtIndex );
    return layer;
}

FOdysseyImageLayer*
FOdysseyLayerStack::AddImageLayer( int iAtIndex )
{
    FOdysseyImageLayer* layer = new FOdysseyImageLayer( GetNextLayerName(), FVector2D( mWidth, mHeight ), mTextureSourceFormat );
    mCurrentLayer = mLayers->AddNode( layer, iAtIndex );
    return layer;
}

FOdysseyImageLayer*
FOdysseyLayerStack::AddImageLayerFromData( FOdysseyBlock* iData, FOdysseyNTree< IOdysseyLayer* >* iPosition, FName iName, int iAtIndex )
{
    assert( iData->GetUE4TextureSourceFormat() == mTextureSourceFormat );

    FOdysseyBlock* explicitCopyResized = new FOdysseyBlock( mWidth, mHeight, mTextureSourceFormat );
    ::ULIS::FMakeContext::CopyBlockInto( iData->GetIBlock(), explicitCopyResized->GetIBlock() );

    FOdysseyImageLayer* layer = new FOdysseyImageLayer( iName.IsNone() ? GetNextLayerName() : iName, explicitCopyResized );
    iPosition->AddNode( layer, iAtIndex );
    return layer;
}

FOdysseyImageLayer*
FOdysseyLayerStack::AddImageLayerFromData( FOdysseyBlock* iData, FName iName, int iAtIndex )
{
    assert( iData->GetUE4TextureSourceFormat() == mTextureSourceFormat );

    FOdysseyBlock* explicitCopyResized = new FOdysseyBlock( mWidth, mHeight, mTextureSourceFormat );
    ::ULIS::FMakeContext::CopyBlockInto( iData->GetIBlock(), explicitCopyResized->GetIBlock() );

    FOdysseyImageLayer* layer = new FOdysseyImageLayer( iName.IsNone() ? GetNextLayerName() : iName, explicitCopyResized );
    mCurrentLayer->AddNode( layer, iAtIndex );
    return layer;
}

FOdysseyFolderLayer*
FOdysseyLayerStack::AddFolderLayer( FOdysseyNTree< IOdysseyLayer* >* iPosition, FName iName, int iAtIndex )
{
    if( iName == FName() )
        iName = GetNextLayerName();
    
    FOdysseyFolderLayer* layer = new FOdysseyFolderLayer( iName );
    mCurrentLayer = iPosition->AddNode( layer, iAtIndex );
    return layer;
}

FOdysseyFolderLayer*
FOdysseyLayerStack::AddFolderLayer( FName iName, int iAtIndex )
{
    if( iName == FName() )
        iName = GetNextLayerName();
    
    FOdysseyFolderLayer* layer = new FOdysseyFolderLayer( iName );
    mCurrentLayer = mLayers->AddNode( layer, iAtIndex );
    return layer;
}

FOdysseyNTree< IOdysseyLayer* >*
FOdysseyLayerStack::GetLayers()
{
    return mLayers;
}

FOdysseyNTree< IOdysseyLayer* >*
FOdysseyLayerStack::GetCurrentLayer() const
{
    return mCurrentLayer;
}

int
FOdysseyLayerStack::GetCurrentLayerAsIndex() const
{
    TArray< IOdysseyLayer* > layers = TArray<IOdysseyLayer*>();
    mLayers->DepthFirstSearchTree( &layers, false );
    
    for( int i = 0; i < layers.Num(); i++)
    {
        if( layers[i] == mCurrentLayer->GetNodeContent() )
            return i;
    }
    return 0;
}

FOdysseyNTree< IOdysseyLayer* >*
FOdysseyLayerStack::GetCurrentLayerFromIndex( int iIndex ) const
{
    TArray< IOdysseyLayer* > layers = TArray<IOdysseyLayer*>();
    mLayers->DepthFirstSearchTree( &layers, false );
    
    if( iIndex >= layers.Num() || iIndex < 0 )
        return NULL;
    
    return mLayers->FindNode( layers[iIndex] );
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

void
FOdysseyLayerStack::DeleteLayer( IOdysseyLayer* iLayerToDelete )
{
    FOdysseyNTree< IOdysseyLayer* >* node = mLayers->FindNode( iLayerToDelete );

    TArray< IOdysseyLayer* > layers = TArray<IOdysseyLayer*>();
    mLayers->DepthFirstSearchTree( &layers );
    
    int indexNewSelectedLayer = -1;
    for( int i = 0; i < layers.Num(); i++ )
    {
        if( iLayerToDelete == layers[i] )
            indexNewSelectedLayer = i;
    }
    
    mLayers->DeleteNodeIfExist( node );
    
    layers.Empty();
    mLayers->DepthFirstSearchTree( &layers );
        
    if( layers.Num() == 0)
        mCurrentLayer = NULL;
    
    if( indexNewSelectedLayer < layers.Num() - 1 )
        mCurrentLayer = mLayers->FindNode( layers[indexNewSelectedLayer] );
    else
        mCurrentLayer = mLayers->FindNode( layers.Last() );
}

void FOdysseyLayerStack::MergeDownLayer( IOdysseyLayer* iLayerToMergeDown )
{
    ::ULIS::FPerformanceOptions performanceOptions;
    performanceOptions.desired_workers = 1;
    TArray< IOdysseyLayer* > layers = TArray<IOdysseyLayer*>();
    mLayers->DepthFirstSearchTree( &layers, false );
    
    ::ULIS::FClearFillContext::Clear( mResultBlock->GetIBlock(), performanceOptions, false );
    for( int i = 0; i < layers.Num(); i++ )
    {
        if( layers[i] == iLayerToMergeDown && layers[i]->GetType() == IOdysseyLayer::eType::kImage && i != (layers.Num() - 1) && layers[i + 1]->GetType() == IOdysseyLayer::eType::kImage )
        {
            FOdysseyImageLayer* imageLayer1 = static_cast<FOdysseyImageLayer*>( layers[i] );
            FOdysseyImageLayer* imageLayer2 = static_cast<FOdysseyImageLayer*>( layers[i + 1] );

            if( imageLayer1 && imageLayer2 )
            {
                ::ULIS::FBlendingContext::Blend( imageLayer1->GetBlock()->GetIBlock(), imageLayer2->GetBlock()->GetIBlock(), ::ULIS::FRect( 0, 0, mWidth, mHeight ), imageLayer1->GetBlendingMode(), ::ULIS::eAlphaMode::kNormal, 1.f, performanceOptions, false );
            }
            DeleteLayer( layers[i] );
            break;
        }
    }
    ComputeResultBlock();
}

void FOdysseyLayerStack::FlattenLayer( IOdysseyLayer* iLayerToFlatten )
{
    if( iLayerToFlatten->GetType() != IOdysseyLayer::eType::kFolder )
        return;
    
    TArray< IOdysseyLayer* > layersToFlatten = TArray<IOdysseyLayer*>();
    FOdysseyNTree<IOdysseyLayer*>* folderNode = mLayers->FindNode( iLayerToFlatten );
    int indexFolder = folderNode->GetIndexInParent();
        
    folderNode->DepthFirstSearchTree( &layersToFlatten, false );
    
    AddImageLayerFromData( ComputeBlockOfLayers( folderNode ), folderNode->GetParent(), iLayerToFlatten->GetName(), indexFolder );
    DeleteLayer( iLayerToFlatten );
}

void FOdysseyLayerStack::DuplicateLayer( IOdysseyLayer* iLayerToDuplicate )
{
    TArray< IOdysseyLayer* > layers = TArray<IOdysseyLayer*>();
    mLayers->DepthFirstSearchTree( &layers, false );
    FOdysseyNTree<IOdysseyLayer*>* nodeToDuplicate = mLayers->FindNode( iLayerToDuplicate );
    
    if( nodeToDuplicate == NULL )
        return;
    
    ::ULIS::FClearFillContext::Clear( mResultBlock->GetIBlock() );

    if( nodeToDuplicate->GetNodeContent()->GetType() == IOdysseyLayer::eType::kImage )
    {
        FOdysseyImageLayer* imageLayer = static_cast<FOdysseyImageLayer*>( nodeToDuplicate->GetNodeContent() );

        FOdysseyImageLayer* copiedLayer = AddImageLayerFromData( imageLayer->GetBlock(), nodeToDuplicate->GetParent(), FName( *( imageLayer->GetName().ToString() + FString( "_Copy" ) ) ), nodeToDuplicate->GetIndexInParent() + 1 );

        copiedLayer->CopyPropertiesFrom( *imageLayer );
    }
    else if( nodeToDuplicate->GetNodeContent()->GetType() == IOdysseyLayer::eType::kFolder )
    {
        // we need to duplicate the content of the folder as well as the folder
        FOdysseyFolderLayer* folderLayer = static_cast<FOdysseyFolderLayer*>( nodeToDuplicate->GetNodeContent() );
        FOdysseyFolderLayer* currentFolderLayer = AddFolderLayer( nodeToDuplicate->GetParent(), FName( *( folderLayer->GetName().ToString() + FString( "_Copy" ) ) ), nodeToDuplicate->GetIndexInParent() + 1 );
        
        FOdysseyNTree<IOdysseyLayer*>* currentNode = mLayers->FindNode( currentFolderLayer );
        TArray<IOdysseyLayer*> layersInFolder = TArray<IOdysseyLayer*>();
        nodeToDuplicate->DepthFirstSearchTree( &layersInFolder, false );
        TArray<int> indexesFolder = TArray<int>();
        
        for( int i = 0; i < layersInFolder.Num(); i++ )
        {
            if( indexesFolder.Num() != 0 )
            {
                if( indexesFolder.Last() <= 0 )
                {
                    currentNode = currentNode->GetParent();
                    indexesFolder.Pop();
                }
                indexesFolder.Last()--;
            }
            
            if( layersInFolder[i]->GetType() == IOdysseyLayer::eType::kImage )
            {
                FOdysseyImageLayer* imageLayer = static_cast<FOdysseyImageLayer*>( layersInFolder[i] );

                FOdysseyImageLayer* copiedLayer = AddImageLayerFromData( imageLayer->GetBlock(), currentNode, FName( *( imageLayer->GetName().ToString() ) ) );
                copiedLayer->CopyPropertiesFrom( *imageLayer );
            }
            else if( layersInFolder[i]->GetType() == IOdysseyLayer::eType::kFolder )
            {
                indexesFolder.Add( mLayers->FindNode( layersInFolder[i] )->GetNodes()->Num() );
                folderLayer = AddFolderLayer( currentNode, layersInFolder[i]->GetName() );
                currentNode = mLayers->FindNode( folderLayer );
            }
        }
    }
    
    ComputeResultBlock();
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
    mLayers->DepthFirstSearchTree( &layers, false );
    
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


FOdysseyBlock*
FOdysseyLayerStack::ComputeBlockOfLayers( FOdysseyNTree< IOdysseyLayer* >* iLayers )
{
    checkf( iLayers != NULL, TEXT("Passed NullPtr to ComputeBlockOfLayers of FOdysseyLayerStack") );
    
    ::ULIS::FPerformanceOptions performanceOptions;
    performanceOptions.desired_workers = 1;
    
    FOdysseyBlock* resultBlock = new FOdysseyBlock( mWidth, mHeight, mTextureSourceFormat );
    ::ULIS::FClearFillContext::Clear( resultBlock->GetIBlock(), performanceOptions, false );
    
    if( !iLayers->GetNodeContent()->IsVisible() )
        return resultBlock;
    
    TArray< IOdysseyLayer* > layers = TArray<IOdysseyLayer*>();
    iLayers->DepthFirstSearchTree( &layers, false );
    
    for( int i = layers.Num() - 1; i >= 0 ; i-- )
    {
        IOdysseyLayer::eType type = layers[i]->GetType();
        if( type == IOdysseyLayer::eType::kImage && layers[i]->IsVisible() )
        {
            FOdysseyImageLayer* imageLayer = static_cast<FOdysseyImageLayer*>( layers[i] );
            
            ::ULIS::FBlendingContext::Blend( imageLayer->GetBlock()->GetIBlock(), resultBlock->GetIBlock(), ::ULIS::FRect( 0, 0, mWidth, mHeight ), imageLayer->GetBlendingMode(), ::ULIS::eAlphaMode::kNormal, imageLayer->GetOpacity(), performanceOptions, false );
        }
        else if( type == IOdysseyLayer::eType::kFolder )
        {
            FOdysseyNTree<IOdysseyLayer*>* folderNode = mLayers->FindNode( layers[i] );
            FOdysseyFolderLayer* folderLayer = static_cast<FOdysseyFolderLayer*>( layers[i] );
            
            TArray<IOdysseyLayer*> layersInFolder = TArray<IOdysseyLayer*>();
            folderNode->DepthFirstSearchTree( &layersInFolder, false );
            i-=layersInFolder.Num();
            
            if( folderLayer->IsVisible() )
                ::ULIS::FBlendingContext::Blend( ComputeBlockOfLayers( folderNode )->GetIBlock(), resultBlock->GetIBlock(), ::ULIS::FRect( 0, 0, mWidth, mHeight ), folderLayer->GetBlendingMode(), ::ULIS::eAlphaMode::kNormal, folderLayer->GetOpacity(), performanceOptions, false );
        }
    }
    
    return resultBlock;
}


//---











//FODysseyDrawingUndo ---------
FOdysseyDrawingUndo::FOdysseyDrawingUndo( FOdysseyLayerStack* iLayerStack )
{
    mLayerStackPtr = iLayerStack;

    mData = TArray<uint8>();
    
    //We reserve the maximum memory needed for a undo
    mData.Reserve( iLayerStack->GetResultBlock()->GetIBlock()->BytesTotal() );

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
    mLayerStackPtr->GetLayers()->DepthFirstSearchTree( &layers, false );
    
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
    if( mLayerStackPtr->GetCurrentLayer()->GetNodeContent()->GetType() != IOdysseyLayer::eType::kImage )
        return false;
    
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
        mLayerStackPtr->GetLayers()->DepthFirstSearchTree( &layers, false );
        
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
        mLayerStackPtr->GetLayers()->DepthFirstSearchTree( &layers, false );
        
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
