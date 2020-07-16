// Copyright © 2018-2020 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000
#include "OdysseyLayerStack.h"
#include "HAL/PlatformFilemanager.h"
#include "Misc/FileHelper.h"
#include "UObject/UObjectGlobals.h"
#include "OdysseyImageLayer.h"
#include "OdysseyFolderLayer.h"
#include "ULISLoaderModule.h"

#define LOCTEXT_NAMESPACE "OdysseyLayerStack"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyLayerStack::~FOdysseyLayerStack()
{
    delete  mResultBlock;
    delete  mTempBlock;
    delete  mLayers;
    delete  mDrawingUndo;
}

FOdysseyLayerStack::FOdysseyLayerStack()
    : mResultBlock( nullptr )
    , mTempBlock( nullptr )
    , mLayers( new  FOdysseyNTree< IOdysseyLayer* >( nullptr ) )
    , mCurrentLayer( mLayers )
    , mWidth( 0 )
    , mHeight( 0 )
    , mTextureSourceFormat( ETextureSourceFormat::TSF_BGRA8 )
    , mIsInitialized( false )
{
}

FOdysseyLayerStack::FOdysseyLayerStack(int iWidth,int iHeight)
    : mResultBlock( nullptr )
    , mTempBlock( nullptr )
    , mLayers( new  FOdysseyNTree< IOdysseyLayer* >( nullptr ) )
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

    SetCurrentLayer( mLayers->GetNodes()->GetData()[0] );

    mDrawingUndo = new  FOdysseyDrawingUndo( this );
    ComputeResultBlock();
}

void
FOdysseyLayerStack::InitFromData( FOdysseyBlock* iData )
{
    if( mIsInitialized )
        return;

    checkf( iData, TEXT( "Cannot Initialize Layer Stack from NULL data" ) );

    mWidth = iData->Width();
    mHeight = iData->Height();
    mTextureSourceFormat = iData->GetUE4TextureSourceFormat();
    mIsInitialized = true;

    InitResultAndTempBlock();

    //Is not 0 if it comes from an existing OdysseyTexture
    if( mLayers->GetNodes()->Num() == 0 )
        AddImageLayerFromData( iData,mLayers );

    SetCurrentLayer(  mLayers->GetNodes()->GetData()[0] );

    mDrawingUndo = new  FOdysseyDrawingUndo( this );
    ComputeResultBlock();
}

FOdysseyBlock*
FOdysseyLayerStack::GetResultBlock()
{
    return  mResultBlock;
}

void
FOdysseyLayerStack::ComputeResultBlock()
{
    IULISLoaderModule& hULIS = IULISLoaderModule::Get();
    uint32 perfIntent = ULIS3_PERF_MT | ULIS3_PERF_SSE42 | ULIS3_PERF_AVX2;
    ::ul3::FRect canvasRect = ::ul3::FRect( 0, 0, mWidth, mHeight );
    ::ul3::Clear( hULIS.ThreadPool(), ULIS3_BLOCKING, perfIntent, hULIS.HostDeviceInfo(), ULIS3_NOCB, mResultBlock->GetBlock(), canvasRect );

    TArray< IOdysseyLayer* > layers = TArray< IOdysseyLayer* >();
    mLayers->DepthFirstSearchTree( &layers, false );

    TMap< FOdysseyNTree<IOdysseyLayer* >*, FOdysseyBlock* > folderBlocks = TMap< FOdysseyNTree< IOdysseyLayer* >*, FOdysseyBlock* >();

    for( int i = layers.Num() - 1; i >= 0 ; i-- ) 
    {
        IOdysseyLayer::eType type = layers[i]->GetType();
        if( type == IOdysseyLayer::eType::kImage && layers[i]->IsVisible() )
        {
            FOdysseyImageLayer* imageLayer = static_cast<FOdysseyImageLayer*>(layers[i]);
            FOdysseyNTree<IOdysseyLayer*>* imageNode = mLayers->FindNode(imageLayer);
            FOdysseyNTree<IOdysseyLayer*>* parentNode = imageNode->GetParent();

            if( parentNode->GetNodeContent() != NULL ) 
            {
                // This means the imageLayer is inside a folder
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
                    FOdysseyBlock* block = nullptr;

					if (folderBlocks.Contains(imageNode->GetParent()))  //FolderBlock exists
                    {
                        block = folderBlocks[imageNode->GetParent()];
                    }
                    else
                    {
                        block = new FOdysseyBlock(mWidth, mHeight, mTextureSourceFormat);
                        ::ul3::Clear(hULIS.ThreadPool(), ULIS3_BLOCKING, perfIntent, hULIS.HostDeviceInfo(), ULIS3_NOCB, block->GetBlock(), block->GetBlock()->Rect());
                        folderBlocks.Add(imageNode->GetParent(), block);
                    }

                    ::ul3::Blend( hULIS.ThreadPool()
                                , ULIS3_BLOCKING
                                , perfIntent
                                , hULIS.HostDeviceInfo()
                                , ULIS3_NOCB
                                , imageLayer->GetBlock()->GetBlock()
                                , folderBlocks[imageNode->GetParent()]->GetBlock()
                                , canvasRect
                                , ::ul3::FVec2F( 0, 0 )
                                , ULIS3_NOAA
                                , imageLayer->GetBlendingMode()
                                , ::ul3::AM_NORMAL
                                , imageLayer->GetOpacity() );
                }
            } 
            else //Image layer is at the root
            {
                ::ul3::Blend( hULIS.ThreadPool()
                            , ULIS3_BLOCKING
                            , perfIntent
                            , hULIS.HostDeviceInfo()
                            , ULIS3_NOCB
                            , imageLayer->GetBlock()->GetBlock()
                            , mResultBlock->GetBlock()
                            , canvasRect
                            , ::ul3::FVec2F( 0, 0 )
                            , ULIS3_NOAA
                            , imageLayer->GetBlendingMode()
                            , ::ul3::AM_NORMAL
                            , imageLayer->GetOpacity() );
            }
        } 
        else if( type == IOdysseyLayer::eType::kFolder && layers[i]->IsVisible() ) 
        {
            FOdysseyNTree< IOdysseyLayer* >* folderNode = mLayers->FindNode( layers[i] );
            FOdysseyFolderLayer* folderLayer = static_cast< FOdysseyFolderLayer* >( layers[i] );

            if( folderBlocks.Contains(folderNode) ) 
            {
                //Time to blend the folder unto the resultBlock
                ::ul3::Blend( hULIS.ThreadPool()
                            , ULIS3_BLOCKING
                            , perfIntent
                            , hULIS.HostDeviceInfo()
                            , ULIS3_NOCB
                            , folderBlocks[folderNode]->GetBlock()
                            , mResultBlock->GetBlock()
                            , canvasRect
                            , ::ul3::FVec2F( 0, 0 )
                            , ULIS3_NOAA
                            , folderLayer->GetBlendingMode()
                            , ::ul3::AM_NORMAL
                            , folderLayer->GetOpacity() );
            }
        }
    }

    mResultBlock->GetBlock()->Invalidate();
    for( auto& elem : folderBlocks )
        delete elem.Value;
}

void
FOdysseyLayerStack::ComputeResultBlock( const ::ul3::FRect& iRect ) 
{
    ::ul3::FVec2F pos( iRect.x, iRect.y );
    IULISLoaderModule& hULIS = IULISLoaderModule::Get();
    uint32 perfIntent = /*ULIS3_PERF_MT |*/ ULIS3_PERF_SSE42 | ULIS3_PERF_AVX2;
    ::ul3::Clear( hULIS.ThreadPool(), ULIS3_BLOCKING, perfIntent, hULIS.HostDeviceInfo(), ULIS3_NOCB, mResultBlock->GetBlock(), iRect );

    TArray< IOdysseyLayer* > layers = TArray< IOdysseyLayer* >();
    mLayers->DepthFirstSearchTree( &layers, false );

    TMap< FOdysseyNTree< IOdysseyLayer* >*, FOdysseyBlock* > folderBlocks = TMap< FOdysseyNTree<IOdysseyLayer* >*, FOdysseyBlock* >();

    for( int i = layers.Num() - 1; i >= 0 ; i-- ) 
    {
        IOdysseyLayer::eType type = layers[i]->GetType();
        if( type == IOdysseyLayer::eType::kImage && layers[i]->IsVisible() ) 
        {
            FOdysseyImageLayer* imageLayer = static_cast<FOdysseyImageLayer*>(layers[i]);
            FOdysseyNTree<IOdysseyLayer*>* imageNode = mLayers->FindNode(imageLayer);
            FOdysseyNTree<IOdysseyLayer*>* parentNode = imageNode->GetParent();

            if( parentNode->GetNodeContent() != NULL )  //This means the imageLayer is inside a folder
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
                    FOdysseyBlock* block = nullptr;

					if (folderBlocks.Contains(imageNode->GetParent()))  //FolderBlock exists
                    {
                        block = folderBlocks[imageNode->GetParent()];
                    }
                    else
                    {
                        block = new FOdysseyBlock(mWidth, mHeight, mTextureSourceFormat);
                        ::ul3::Clear(hULIS.ThreadPool(), ULIS3_BLOCKING, perfIntent, hULIS.HostDeviceInfo(), ULIS3_NOCB, block->GetBlock(), block->GetBlock()->Rect());
                        folderBlocks.Add(imageNode->GetParent(), block);
                    }

                    ::ul3::Blend( hULIS.ThreadPool()
                                , ULIS3_BLOCKING
                                , perfIntent
                                , hULIS.HostDeviceInfo()
                                , ULIS3_NOCB
                                , imageLayer->GetBlock()->GetBlock()
                                , folderBlocks[imageNode->GetParent()]->GetBlock()
                                , iRect
                                , pos
                                , ULIS3_NOAA
                                , imageLayer->GetBlendingMode()
                                , ::ul3::AM_NORMAL
                                , imageLayer->GetOpacity() );
                }
            } 
            else //Image layer is at the root
            {
                ::ul3::Blend( hULIS.ThreadPool()
                            , ULIS3_BLOCKING
                            , perfIntent
                            , hULIS.HostDeviceInfo()
                            , ULIS3_NOCB
                            , imageLayer->GetBlock()->GetBlock()
                            , mResultBlock->GetBlock()
                            , iRect
                            , pos
                            , ULIS3_NOAA
                            , imageLayer->GetBlendingMode()
                            , ::ul3::AM_NORMAL
                            , imageLayer->GetOpacity() );
            }
        }
        else if( type == IOdysseyLayer::eType::kFolder && layers[i]->IsVisible() ) 
        {
            FOdysseyNTree<IOdysseyLayer*>* folderNode = mLayers->FindNode(layers[i]);
            FOdysseyFolderLayer* folderLayer = static_cast<FOdysseyFolderLayer*>(layers[i]);

            if( folderBlocks.Contains(folderNode) ) 
            {
                //Time to blend the folder unto the resultBlock
                ::ul3::Blend( hULIS.ThreadPool()
                            , ULIS3_BLOCKING
                            , perfIntent
                            , hULIS.HostDeviceInfo()
                            , ULIS3_NOCB
                            , folderBlocks[folderNode]->GetBlock()
                            , mResultBlock->GetBlock()
                            , iRect
                            , pos
                            , ULIS3_NOAA
                            , folderLayer->GetBlendingMode()
                            , ::ul3::AM_NORMAL
                            , folderLayer->GetOpacity() );
            }
        }
    }

    mResultBlock->GetBlock()->Invalidate( iRect );

    for( auto& elem : folderBlocks )
        delete elem.Value;
}

void
FOdysseyLayerStack::ComputeResultBlockWithTempBuffer(const ::ul3::FRect& iRect, FOdysseyBlock* iTempBuffer, float iOpacity, ::ul3::eBlendingMode iMode ,::ul3::eAlphaMode iAlphaMode)
{
    //There is no current layer or the current layer is locked or not visible -> We don't need to redraw anything
    if( !mCurrentLayer || mCurrentLayer->GetNodeContent()->IsLocked() || !mCurrentLayer->GetNodeContent()->IsVisible() )
        return;

	::ul3::FVec2F pos(iRect.x, iRect.y);
	::ul3::FVec2I posi(pos.x, pos.y);

	// Clear result block
	IULISLoaderModule& hULIS = IULISLoaderModule::Get();
	uint32 perfIntent = /*ULIS3_PERF_MT |*/ ULIS3_PERF_SSE42 | ULIS3_PERF_AVX2;
	::ul3::Clear(hULIS.ThreadPool(), ULIS3_BLOCKING, perfIntent, hULIS.HostDeviceInfo(), ULIS3_NOCB, mResultBlock->GetBlock(), iRect);

	TArray< IOdysseyLayer* > layers = TArray<IOdysseyLayer*>();
	mLayers->DepthFirstSearchTree(&layers, false);

	TMap< FOdysseyNTree< IOdysseyLayer* >*, FOdysseyBlock* > folderBlocks = TMap< FOdysseyNTree< IOdysseyLayer* >*, FOdysseyBlock* >();

	for (int i = layers.Num() - 1; i >= 0; i--) 
    {
		IOdysseyLayer::eType type = layers[i]->GetType();
		if (type == IOdysseyLayer::eType::kImage && layers[i]->IsVisible()) 
        {
			FOdysseyImageLayer* imageLayer = static_cast<FOdysseyImageLayer*>(layers[i]);
			FOdysseyNTree< IOdysseyLayer* >* imageNode = mLayers->FindNode(imageLayer);
			FOdysseyNTree< IOdysseyLayer* >* parentNode = imageNode->GetParent();

			if (parentNode->GetNodeContent() != NULL) //This means the imageLayer is inside a folder
            {
				bool visible = true;
                bool locked = false;
				while (parentNode->GetNodeContent() != NULL) 
                {
					//If a folder above isn't visible, we don't add this image layer to the result block
					if (!parentNode->GetNodeContent()->IsVisible())
						visible = false;

                    //If a folder above is locked, we won't draw on the current layer
                    if(parentNode->GetNodeContent()->IsLocked())
                        locked = true;

					parentNode = parentNode->GetParent();
				}

				if (visible) 
                {
                    FOdysseyBlock* block = nullptr;

					if (folderBlocks.Contains(imageNode->GetParent()))  //FolderBlock exists
                    {
                        block = folderBlocks[imageNode->GetParent()];
                    }
                    else
                    {
                        block = new FOdysseyBlock(mWidth, mHeight, mTextureSourceFormat);
                        ::ul3::Clear(hULIS.ThreadPool(), ULIS3_BLOCKING, perfIntent, hULIS.HostDeviceInfo(), ULIS3_NOCB, block->GetBlock(), block->GetBlock()->Rect());
                        folderBlocks.Add(imageNode->GetParent(), block);
                    }

					if (imageLayer && imageLayer == mCurrentLayer->GetNodeContent() && !locked && iTempBuffer) //Current layer inside a folder
                    {
                        ::ul3::Copy( hULIS.ThreadPool()
                            , ULIS3_BLOCKING
                            , perfIntent
                            , hULIS.HostDeviceInfo()
                            , ULIS3_NOCB
                            , imageLayer->GetBlock()->GetBlock()
                            , mTempBlock->GetBlock()
                            , iRect
                            , ::ul3::FVec2I( iRect.x, iRect.y ) );

						::ul3::Blend(hULIS.ThreadPool()
							, ULIS3_BLOCKING
							, perfIntent
							, hULIS.HostDeviceInfo()
							, ULIS3_NOCB
							, iTempBuffer->GetBlock()
							, mTempBlock->GetBlock()
							, iRect
							, pos
							, ULIS3_NOAA
						    , iMode
							, imageLayer->IsAlphaLocked() ? ::ul3::AM_BACK : iAlphaMode
							, iOpacity);

                        ::ul3::Blend(hULIS.ThreadPool()
							, ULIS3_BLOCKING
							, perfIntent
							, hULIS.HostDeviceInfo()
							, ULIS3_NOCB
							, mTempBlock->GetBlock()
							, folderBlocks[imageNode->GetParent()]->GetBlock()
							, iRect
							, pos
							, ULIS3_NOAA
							, imageLayer->GetBlendingMode()
							, ::ul3::AM_NORMAL
							, imageLayer->GetOpacity());
					}
					else //Other layer inside a folder
                    {
						::ul3::Blend(hULIS.ThreadPool()
							, ULIS3_BLOCKING
							, perfIntent
							, hULIS.HostDeviceInfo()
							, ULIS3_NOCB
							, imageLayer->GetBlock()->GetBlock()
							, folderBlocks[imageNode->GetParent()]->GetBlock()
							, iRect
							, pos
							, ULIS3_NOAA
							, imageLayer->GetBlendingMode()
							, ::ul3::AM_NORMAL
							, imageLayer->GetOpacity());
					}
				}
			}
			else //Image layer is at the root
			{
				if (imageLayer && imageLayer == mCurrentLayer->GetNodeContent() && iTempBuffer) //Current layer
				{
                    ::ul3::Copy( hULIS.ThreadPool()
                        , ULIS3_BLOCKING
                        , perfIntent
                        , hULIS.HostDeviceInfo()
                        , ULIS3_NOCB
                        , imageLayer->GetBlock()->GetBlock()
                        , mTempBlock->GetBlock()
                        , iRect
                        , ::ul3::FVec2I( iRect.x, iRect.y ) );

					::ul3::Blend(hULIS.ThreadPool()
						, ULIS3_BLOCKING
						, perfIntent
						, hULIS.HostDeviceInfo()
						, ULIS3_NOCB
						, iTempBuffer->GetBlock()
						, mTempBlock->GetBlock()
						, iRect
						, pos
						, ULIS3_NOAA
						, iMode
						, imageLayer->IsAlphaLocked() ? ::ul3::AM_BACK : iAlphaMode
						, iOpacity);

                    ::ul3::Blend(hULIS.ThreadPool()
						, ULIS3_BLOCKING
						, perfIntent
						, hULIS.HostDeviceInfo()
						, ULIS3_NOCB
						, mTempBlock->GetBlock()
						, mResultBlock->GetBlock()
						, iRect
						, pos
						, ULIS3_NOAA
						, imageLayer->GetBlendingMode()
						, ::ul3::AM_NORMAL
						, imageLayer->GetOpacity());
				}
				else if (imageLayer) //Other layer
                {
					::ul3::Blend(hULIS.ThreadPool()
						, ULIS3_BLOCKING
						, perfIntent
						, hULIS.HostDeviceInfo()
						, ULIS3_NOCB
						, imageLayer->GetBlock()->GetBlock()
						, mResultBlock->GetBlock()
						, iRect
						, pos
						, ULIS3_NOAA
						, imageLayer->GetBlendingMode()
						, ::ul3::AM_NORMAL
						, imageLayer->GetOpacity());
				}
			}
		}
		else if (type == IOdysseyLayer::eType::kFolder && layers[i]->IsVisible()) 
        {
			FOdysseyNTree<IOdysseyLayer*>* folderNode = mLayers->FindNode(layers[i]);
			FOdysseyFolderLayer* folderLayer = static_cast<FOdysseyFolderLayer*>(layers[i]);

			if (folderBlocks.Contains(folderNode)) 
            {
				//Time to blend the folder unto the resultBlock
				::ul3::Blend(hULIS.ThreadPool()
					, ULIS3_BLOCKING
					, perfIntent
					, hULIS.HostDeviceInfo()
					, ULIS3_NOCB
					, folderBlocks[folderNode]->GetBlock()
					, mResultBlock->GetBlock()
					, iRect
					, pos
					, ULIS3_NOAA
					, folderLayer->GetBlendingMode()
					, ::ul3::AM_NORMAL
					, folderLayer->GetOpacity());
			}
		}
	}

	mResultBlock->GetBlock()->Invalidate(iRect);

    //Deletion of the created blocks for the folders
	for (auto& elem : folderBlocks)
		delete elem.Value;
}

void
FOdysseyLayerStack::BlendTempBufferOnCurrentBlock( const ::ul3::FRect& iRect, FOdysseyBlock* iTempBuffer, float iOpacity, ::ul3::eBlendingMode iMode ,::ul3::eAlphaMode iAlphaMode )
{
    //There is no current layer or the current layer is locked or not visible -> We don't need to redraw anything
    if( !mCurrentLayer || mCurrentLayer->GetNodeContent()->IsLocked() || !mCurrentLayer->GetNodeContent()->IsVisible() )
        return;

    //Checking for the eventual folders the current layer may be in
    FOdysseyNTree< IOdysseyLayer* >* parentNode = mCurrentLayer->GetParent();
    if ( parentNode->GetNodeContent() != NULL )
    {
		while (parentNode->GetNodeContent() != NULL) 
        {
			if (!parentNode->GetNodeContent()->IsVisible())
				return;

            if(parentNode->GetNodeContent()->IsLocked())
                return;

			parentNode = parentNode->GetParent();
		}
    }


    ::ul3::FVec2F pos( iRect.x, iRect.y );
    IULISLoaderModule& hULIS = IULISLoaderModule::Get();
    uint32 perfIntent = /*ULIS3_PERF_MT |*/ ULIS3_PERF_SSE42 | ULIS3_PERF_AVX2;
    ::ul3::Clear( hULIS.ThreadPool(), ULIS3_BLOCKING, perfIntent, hULIS.HostDeviceInfo(), ULIS3_NOCB, mResultBlock->GetBlock(), iRect );

    TArray< IOdysseyLayer* > layers = TArray<IOdysseyLayer*>();
    mLayers->DepthFirstSearchTree(&layers,false);

    IOdysseyLayer::eType type = mCurrentLayer->GetNodeContent()->GetType();
    if(type != IOdysseyLayer::eType::kImage || !mCurrentLayer->GetNodeContent()->IsVisible())
        return;

    FOdysseyImageLayer* imageLayer = static_cast<FOdysseyImageLayer*>(mCurrentLayer->GetNodeContent());

    if(imageLayer && iTempBuffer)
        ::ul3::Blend( hULIS.ThreadPool()
                    , ULIS3_BLOCKING
                    , perfIntent
                    , hULIS.HostDeviceInfo()
                    , ULIS3_NOCB
                    , iTempBuffer->GetBlock()
                    , imageLayer->GetBlock()->GetBlock()
                    , iRect
                    , pos
                    , ULIS3_NOAA
                    , iMode
                    , imageLayer->IsAlphaLocked() ? ::ul3::AM_BACK : iAlphaMode
                    , iOpacity );

    mOnLayerStackDirty.Broadcast();
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
    return FVector2D(mWidth,mHeight);
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------- Public Array Tampon Methods

FOdysseyImageLayer*
FOdysseyLayerStack::AddImageLayer(FOdysseyNTree< IOdysseyLayer* >* iPosition,int iAtIndex)
{
    FOdysseyImageLayer* layer = new FOdysseyImageLayer( GetNextLayerName(),FVector2D(mWidth,mHeight),mTextureSourceFormat);
    SetCurrentLayer(iPosition->AddNode(layer,iAtIndex));
    mOnLayerStackDirty.Broadcast();
    return layer;
}

FOdysseyImageLayer*
FOdysseyLayerStack::AddImageLayer(int iAtIndex)
{
    FOdysseyImageLayer* layer = new FOdysseyImageLayer( GetNextLayerName(),FVector2D(mWidth,mHeight),mTextureSourceFormat);
    SetCurrentLayer( mLayers->AddNode(layer,iAtIndex) );
    mOnLayerStackDirty.Broadcast();
    return layer;
}

FOdysseyImageLayer*
FOdysseyLayerStack::AddImageLayerFromData(FOdysseyBlock* iData,FOdysseyNTree< IOdysseyLayer* >* iPosition,FName iName,int iAtIndex)
{
    //assert(iData->GetUE4TextureSourceFormat() == mTextureSourceFormat);

    FOdysseyBlock* explicitCopyResized = new FOdysseyBlock( mWidth, mHeight, mTextureSourceFormat);
    IULISLoaderModule& hULIS = IULISLoaderModule::Get();
    uint32 perfIntent = ULIS3_PERF_MT | ULIS3_PERF_SSE42 | ULIS3_PERF_AVX2;
    ::ul3::Copy( hULIS.ThreadPool()
               , ULIS3_BLOCKING
               , perfIntent
               , hULIS.HostDeviceInfo()
               , ULIS3_NOCB
               , iData->GetBlock()
               , explicitCopyResized->GetBlock()
               , iData->GetBlock()->Rect()
               , ::ul3::FVec2I( 0, 0 ) );

    FOdysseyImageLayer* layer = new FOdysseyImageLayer( iName.IsNone() ? GetNextLayerName() : iName, explicitCopyResized );
    iPosition->AddNode( layer,iAtIndex );
    mOnLayerStackDirty.Broadcast();
    return  layer;
}

FOdysseyImageLayer*
FOdysseyLayerStack::AddImageLayerFromData(FOdysseyBlock* iData,FName iName,int iAtIndex)
{
    //assert(iData->GetUE4TextureSourceFormat() == mTextureSourceFormat);

    FOdysseyBlock* explicitCopyResized = new FOdysseyBlock(mWidth,mHeight,mTextureSourceFormat);

    IULISLoaderModule& hULIS = IULISLoaderModule::Get();
    uint32 perfIntent = ULIS3_PERF_MT | ULIS3_PERF_SSE42 | ULIS3_PERF_AVX2;
    ::ul3::Copy( hULIS.ThreadPool()
               , ULIS3_BLOCKING
               , perfIntent
               , hULIS.HostDeviceInfo()
               , ULIS3_NOCB
               , iData->GetBlock()
               , explicitCopyResized->GetBlock()
               , iData->GetBlock()->Rect()
               , ::ul3::FVec2I( 0, 0 ) );

    FOdysseyImageLayer* layer = new FOdysseyImageLayer( iName.IsNone() ? GetNextLayerName() : iName,explicitCopyResized);
    mCurrentLayer->AddNode(layer,iAtIndex);
    mOnLayerStackDirty.Broadcast();
    return layer;
}

FOdysseyFolderLayer*
FOdysseyLayerStack::AddFolderLayer(FOdysseyNTree< IOdysseyLayer* >* iPosition,FName iName,int iAtIndex)
{
    if(iName == FName())
        iName = GetNextLayerName();

    FOdysseyFolderLayer* layer = new FOdysseyFolderLayer( iName);
    SetCurrentLayer( iPosition->AddNode(layer,iAtIndex) );
    mOnLayerStackDirty.Broadcast();
    return layer;
}

FOdysseyFolderLayer*
FOdysseyLayerStack::AddFolderLayer(FName iName,int iAtIndex)
{
    if(iName == FName())
        iName = GetNextLayerName();

    FOdysseyFolderLayer* layer = new FOdysseyFolderLayer( iName);
    SetCurrentLayer( mLayers->AddNode(layer,iAtIndex) );
    mOnLayerStackDirty.Broadcast();
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
    mLayers->DepthFirstSearchTree(&layers,false);

    for(int i = 0; i < layers.Num(); i++)
    {
        if(layers[i] == mCurrentLayer->GetNodeContent())
            return i;
    }
    return -1;
}

FOdysseyNTree< IOdysseyLayer* >*
FOdysseyLayerStack::GetCurrentLayerFromIndex(int iIndex) const
{
    TArray< IOdysseyLayer* > layers = TArray<IOdysseyLayer*>();
    mLayers->DepthFirstSearchTree(&layers,false);

    if(iIndex >= layers.Num() || iIndex < 0)
        return NULL;

    return mLayers->FindNode(layers[iIndex]);
}

void
FOdysseyLayerStack::SetCurrentLayer(IOdysseyLayer* iLayer)
{
	mCurrentLayer = mLayers->FindNode(iLayer);
	mOnCurrentLayerChanged.Broadcast(mCurrentLayer);
    mOnLayerStackDirty.Broadcast();
}

void
FOdysseyLayerStack::SetCurrentLayer(FOdysseyNTree< IOdysseyLayer* >* iLayer)
{
    mCurrentLayer = iLayer;
	mOnCurrentLayerChanged.Broadcast(mCurrentLayer);
    mOnLayerStackDirty.Broadcast();
}

void
FOdysseyLayerStack::DeleteLayer(IOdysseyLayer* iLayerToDelete)
{
    FOdysseyNTree< IOdysseyLayer* >* node = mLayers->FindNode(iLayerToDelete);

    TArray< IOdysseyLayer* > layers = TArray<IOdysseyLayer*>();
    mLayers->DepthFirstSearchTree(&layers);

    int indexNewSelectedLayer = -1;
    for(int i = 0; i < layers.Num(); i++)
    {
        if(iLayerToDelete == layers[i])
            indexNewSelectedLayer = i;
    }

    mLayers->DeleteNodeIfExist(node);

    layers.Empty();
    mLayers->DepthFirstSearchTree(&layers);

    if(layers.Num() == 0)
    {
        IOdysseyLayer* nullLayer = NULL;
        SetCurrentLayer(nullLayer);
    }

    if(indexNewSelectedLayer < layers.Num() - 1)
        SetCurrentLayer( mLayers->FindNode(layers[indexNewSelectedLayer]) );
    else
        SetCurrentLayer( mLayers->FindNode(layers.Last()) );

    mOnLayerStackDirty.Broadcast();
}

void FOdysseyLayerStack::MergeDownLayer(IOdysseyLayer* iLayerToMergeDown)
{
    TArray< IOdysseyLayer* > layers = TArray<IOdysseyLayer*>();
    mLayers->DepthFirstSearchTree(&layers,false);

    ::ul3::FRect canvasRect = ::ul3::FRect( 0, 0, mWidth, mHeight );
    IULISLoaderModule& hULIS = IULISLoaderModule::Get();
    uint32 perfIntent = ULIS3_PERF_MT | ULIS3_PERF_SSE42 | ULIS3_PERF_AVX2;
    ::ul3::Clear( hULIS.ThreadPool(), ULIS3_BLOCKING, perfIntent, hULIS.HostDeviceInfo(), ULIS3_NOCB, mResultBlock->GetBlock(), canvasRect );

    for(int i = 0; i < layers.Num(); i++) {
        if( layers[i] == iLayerToMergeDown && layers[i]->GetType() == IOdysseyLayer::eType::kImage && i != (layers.Num() - 1) && layers[i + 1]->GetType() == IOdysseyLayer::eType::kImage ) {
            FOdysseyImageLayer* imageLayer1 = static_cast<FOdysseyImageLayer*>(layers[i]);
            FOdysseyImageLayer* imageLayer2 = static_cast<FOdysseyImageLayer*>(layers[i + 1]);

            if(imageLayer1 && imageLayer2) {
                ::ul3::Blend( hULIS.ThreadPool()
                            , ULIS3_BLOCKING
                            , perfIntent
                            , hULIS.HostDeviceInfo()
                            , ULIS3_NOCB
                            , imageLayer1->GetBlock()->GetBlock()
                            , imageLayer2->GetBlock()->GetBlock()
                            , canvasRect
                            , ::ul3::FVec2F( 0, 0 )
                            , ULIS3_NOAA
                            , imageLayer1->GetBlendingMode()
                            , ::ul3::AM_NORMAL
                            , imageLayer1->GetOpacity() );
            }
            DeleteLayer( layers[i] );
            break;
        }
    }
    ComputeResultBlock();
}

void FOdysseyLayerStack::FlattenLayer(IOdysseyLayer* iLayerToFlatten)
{
    if(iLayerToFlatten->GetType() != IOdysseyLayer::eType::kFolder)
        return;

    TArray< IOdysseyLayer* > layersToFlatten = TArray<IOdysseyLayer*>();
    FOdysseyNTree<IOdysseyLayer*>* folderNode = mLayers->FindNode(iLayerToFlatten);
    int indexFolder = folderNode->GetIndexInParent();

    folderNode->DepthFirstSearchTree(&layersToFlatten,false);

    AddImageLayerFromData(ComputeBlockOfLayers(folderNode),folderNode->GetParent(),iLayerToFlatten->GetName(),indexFolder);
    DeleteLayer(iLayerToFlatten);
}

void FOdysseyLayerStack::DuplicateLayer(IOdysseyLayer* iLayerToDuplicate)
{
    TArray< IOdysseyLayer* > layers = TArray<IOdysseyLayer*>();
    mLayers->DepthFirstSearchTree(&layers,false);
    FOdysseyNTree<IOdysseyLayer*>* nodeToDuplicate = mLayers->FindNode(iLayerToDuplicate);

    if(nodeToDuplicate == NULL)
        return;

    ::ul3::FRect canvasRect = ::ul3::FRect( 0, 0, mWidth, mHeight );
    IULISLoaderModule& hULIS = IULISLoaderModule::Get();
    uint32 perfIntent = ULIS3_PERF_MT | ULIS3_PERF_SSE42 | ULIS3_PERF_AVX2;
    ::ul3::Clear( hULIS.ThreadPool(), ULIS3_BLOCKING, perfIntent, hULIS.HostDeviceInfo(), ULIS3_NOCB, mResultBlock->GetBlock(), canvasRect );

    if(nodeToDuplicate->GetNodeContent()->GetType() == IOdysseyLayer::eType::kImage)
    {
        FOdysseyImageLayer* imageLayer = static_cast<FOdysseyImageLayer*>(nodeToDuplicate->GetNodeContent());

        FOdysseyImageLayer* copiedLayer = AddImageLayerFromData(imageLayer->GetBlock(),nodeToDuplicate->GetParent(),FName(*(imageLayer->GetName().ToString() + FString("_Copy"))),nodeToDuplicate->GetIndexInParent() + 1);

        copiedLayer->CopyPropertiesFrom(*imageLayer);
    } else if(nodeToDuplicate->GetNodeContent()->GetType() == IOdysseyLayer::eType::kFolder)
    {
        // we need to duplicate the content of the folder as well as the folder
        FOdysseyFolderLayer* folderLayer = static_cast<FOdysseyFolderLayer*>(nodeToDuplicate->GetNodeContent());
        FOdysseyFolderLayer* currentFolderLayer = AddFolderLayer(nodeToDuplicate->GetParent(),FName(*(folderLayer->GetName().ToString() + FString("_Copy"))),nodeToDuplicate->GetIndexInParent() + 1);

        FOdysseyNTree<IOdysseyLayer*>* currentNode = mLayers->FindNode(currentFolderLayer);
        TArray<IOdysseyLayer*> layersInFolder = TArray<IOdysseyLayer*>();
        nodeToDuplicate->DepthFirstSearchTree(&layersInFolder,false);
        TArray<int> indexesFolder = TArray<int>();

        for(int i = 0; i < layersInFolder.Num(); i++)
        {
            if(indexesFolder.Num() != 0)
            {
                if(indexesFolder.Last() <= 0)
                {
                    currentNode = currentNode->GetParent();
                    indexesFolder.Pop();
                }
                indexesFolder.Last()--;
            }

            if(layersInFolder[i]->GetType() == IOdysseyLayer::eType::kImage)
            {
                FOdysseyImageLayer* imageLayer = static_cast<FOdysseyImageLayer*>(layersInFolder[i]);

                FOdysseyImageLayer* copiedLayer = AddImageLayerFromData(imageLayer->GetBlock(),currentNode,FName(*(imageLayer->GetName().ToString())));
                copiedLayer->CopyPropertiesFrom(*imageLayer);
            } else if(layersInFolder[i]->GetType() == IOdysseyLayer::eType::kFolder)
            {
                indexesFolder.Add(mLayers->FindNode(layersInFolder[i])->GetNodes()->Num());
                folderLayer = AddFolderLayer(currentNode,layersInFolder[i]->GetName());
                currentNode = mLayers->FindNode(folderLayer);
            }
        }
    }

    ComputeResultBlock();
    mOnLayerStackDirty.Broadcast();
}

void
FOdysseyLayerStack::ClearCurrentLayer()
{
    if(mCurrentLayer->GetNodeContent()->GetType() == FOdysseyImageLayer::eType::kImage)
    {
        FOdysseyImageLayer* imageLayer = static_cast<FOdysseyImageLayer*>(mCurrentLayer->GetNodeContent());
        ::ul3::FRect canvasRect = ::ul3::FRect( 0, 0, mWidth, mHeight );
        IULISLoaderModule& hULIS = IULISLoaderModule::Get();
        uint32 perfIntent = ULIS3_PERF_MT | ULIS3_PERF_SSE42 | ULIS3_PERF_AVX2;
        ::ul3::Clear( hULIS.ThreadPool(), ULIS3_BLOCKING, perfIntent, hULIS.HostDeviceInfo(), ULIS3_NOCB, imageLayer->GetBlock()->GetBlock(), canvasRect );
        ComputeResultBlock();
        mOnLayerStackDirty.Broadcast();
    }
}

void
FOdysseyLayerStack::FillCurrentLayerWithColor(const ::ul3::IPixel& iColor)
{
    if(mCurrentLayer->GetNodeContent()->GetType() == FOdysseyImageLayer::eType::kImage)
    {
        FOdysseyImageLayer* imageLayer = static_cast<FOdysseyImageLayer*>(mCurrentLayer->GetNodeContent());
        ::ul3::FRect canvasRect = ::ul3::FRect( 0, 0, mWidth, mHeight );
        IULISLoaderModule& hULIS = IULISLoaderModule::Get();
        uint32 perfIntent = ULIS3_PERF_MT | ULIS3_PERF_SSE42 | ULIS3_PERF_AVX2;
        ::ul3::Fill( hULIS.ThreadPool(), ULIS3_BLOCKING, perfIntent, hULIS.HostDeviceInfo(), ULIS3_NOCB, imageLayer->GetBlock()->GetBlock(), iColor, canvasRect );
        ComputeResultBlock();
        mOnLayerStackDirty.Broadcast();
    }
}

TArray< TSharedPtr< FText > >
FOdysseyLayerStack::GetBlendingModesAsText()
{
    TArray< TSharedPtr< FText > > array;
    const int max = static_cast< int >( ::ul3::NUM_BLENDING_MODES );
    for( int i = 0; i < max; ++i )
        array.Add(MakeShared< FText >( FText::FromString( ANSI_TO_TCHAR( ::ul3::kwBlendingMode[i] ) ) ) );

    return  array;
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
    mLayers->DepthFirstSearchTree(&layers,false);

    return FName(*(FString("Layer ") + FString::FromInt(layers.Num())));
}

void
FOdysseyLayerStack::InitResultAndTempBlock()
{
    if(!mIsInitialized)
        return;

    mResultBlock = new FOdysseyBlock(mWidth,mHeight,mTextureSourceFormat);
    mTempBlock = new FOdysseyBlock(mWidth,mHeight,mTextureSourceFormat);

    ::ul3::FRect canvasRect = ::ul3::FRect( 0, 0, mWidth, mHeight );
    IULISLoaderModule& hULIS = IULISLoaderModule::Get();
    uint32 perfIntent = ULIS3_PERF_MT | ULIS3_PERF_SSE42 | ULIS3_PERF_AVX2;
    ::ul3::Clear( hULIS.ThreadPool(), ULIS3_BLOCKING, perfIntent, hULIS.HostDeviceInfo(), ULIS3_NOCB, mResultBlock->GetBlock(), canvasRect );
    ::ul3::Clear( hULIS.ThreadPool(), ULIS3_BLOCKING, perfIntent, hULIS.HostDeviceInfo(), ULIS3_NOCB, mTempBlock->GetBlock(), canvasRect );
}

FOdysseyBlock*
FOdysseyLayerStack::ComputeBlockOfLayers(FOdysseyNTree< IOdysseyLayer* >* iLayers)
{
    checkf(iLayers != NULL,TEXT("Passed NullPtr to ComputeBlockOfLayers of FOdysseyLayerStack"));

    IULISLoaderModule& hULIS = IULISLoaderModule::Get();
    uint32 perfIntent = ULIS3_PERF_MT | ULIS3_PERF_SSE42 | ULIS3_PERF_AVX2;
    ::ul3::FRect canvasRect = ::ul3::FRect( 0, 0, mWidth, mHeight );

    FOdysseyBlock* resultBlock = new FOdysseyBlock(mWidth,mHeight,mTextureSourceFormat);
    ::ul3::Clear( hULIS.ThreadPool(), ULIS3_BLOCKING, perfIntent, hULIS.HostDeviceInfo(), ULIS3_NOCB, resultBlock->GetBlock(), canvasRect );

    if(!iLayers->GetNodeContent()->IsVisible())
        return resultBlock;

    TArray< IOdysseyLayer* > layers = TArray<IOdysseyLayer*>();
    iLayers->DepthFirstSearchTree(&layers,false);

    for(int i = layers.Num() - 1; i >= 0 ; i--)
    {
        IOdysseyLayer::eType type = layers[i]->GetType();
        if(type == IOdysseyLayer::eType::kImage && layers[i]->IsVisible())
        {
            FOdysseyImageLayer* imageLayer = static_cast<FOdysseyImageLayer*>(layers[i]);
            ::ul3::Blend( hULIS.ThreadPool()
                        , ULIS3_BLOCKING
                        , perfIntent
                        , hULIS.HostDeviceInfo()
                        , ULIS3_NOCB
                        , imageLayer->GetBlock()->GetBlock()
                        , resultBlock->GetBlock()
                        , canvasRect
                        , ::ul3::FVec2F( 0, 0 )
                        , ULIS3_NOAA
                        , imageLayer->GetBlendingMode()
                        , ::ul3::AM_NORMAL
                        , imageLayer->GetOpacity() );


        } else if(type == IOdysseyLayer::eType::kFolder)
        {
            FOdysseyNTree<IOdysseyLayer*>* folderNode = mLayers->FindNode(layers[i]);
            FOdysseyFolderLayer* folderLayer = static_cast<FOdysseyFolderLayer*>(layers[i]);

            TArray<IOdysseyLayer*> layersInFolder = TArray<IOdysseyLayer*>();
            folderNode->DepthFirstSearchTree(&layersInFolder,false);
            i-=layersInFolder.Num();

            if(folderLayer->IsVisible())
                ::ul3::Blend( hULIS.ThreadPool()
                            , ULIS3_BLOCKING
                            , perfIntent
                            , hULIS.HostDeviceInfo()
                            , ULIS3_NOCB
                            , ComputeBlockOfLayers(folderNode)->GetBlock()
                            , resultBlock->GetBlock()
                            , canvasRect
                            , ::ul3::FVec2F( 0, 0 )
                            , ULIS3_NOAA
                            , folderLayer->GetBlendingMode()
                            , ::ul3::AM_NORMAL
                            , folderLayer->GetOpacity() );
        }
    }

    return resultBlock;
}

FArchive& 
operator<<(FArchive &Ar, FOdysseyLayerStack* ioSaveLayerStack )
{
    if( !ioSaveLayerStack )
        return Ar;

    Ar << ioSaveLayerStack->mWidth;
    Ar << ioSaveLayerStack->mHeight;

    Ar << *(ioSaveLayerStack->mLayers);

    if ( Ar.IsLoading() )
    {
        ioSaveLayerStack->mTextureSourceFormat = ETextureSourceFormat::TSF_BGRA8;
        ioSaveLayerStack->mIsInitialized = false;
        ioSaveLayerStack->Init( ioSaveLayerStack->mWidth, ioSaveLayerStack->mHeight );
        ioSaveLayerStack->ComputeResultBlock();
    }

    return Ar;
}

//---

//FODysseyDrawingUndo ---------
FOdysseyDrawingUndo::FOdysseyDrawingUndo(FOdysseyLayerStack* iLayerStack)
{
    mLayerStackPtr = iLayerStack;

    mData = TArray<uint8>();

    //We reserve the maximum memory needed for a undo
    mData.Reserve(iLayerStack->GetResultBlock()->GetBlock()->BytesTotal());

    mUndoPath = FPaths::Combine(FPaths::EngineSavedDir(),TEXT("undos.save"));
    mRedoPath = FPaths::Combine(FPaths::EngineSavedDir(),TEXT("redos.save"));

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
    if(mCurrentIndex == 0)
    {
        Clear();
    } else if(mCurrentIndex < mUndosPositions.Num() - 1)
    {
        mUndosPositions.SetNum(mCurrentIndex + 1);
        mNumberBlocksUndo.SetNum(mCurrentIndex + 1);
        mNumberBlocksUndo[mCurrentIndex] = 0;
        mNumberBlocksRedo.SetNum(mCurrentIndex + 1);
        mNumberBlocksRedo[mCurrentIndex] = 0;
    }
}

void
FOdysseyDrawingUndo::EndRecord()
{
    if(mNumberBlocksUndo[mCurrentIndex] != 0)
    {
        IPlatformFile& platformFile = FPlatformFileManager::Get().GetPlatformFile();
        IFileHandle* fileHandle = platformFile.OpenWrite(*mUndoPath,true);
        fileHandle->Seek(mUndosPositions[mCurrentIndex]);
        fileHandle->Write(mToBinary.GetData(),mToBinary.Num());
        fileHandle->Flush(true);

        mUndosPositions.Add(mUndosPositions[mCurrentIndex] + mToBinary.Num());
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
    if(mNumberBlocksRedo[mCurrentIndex] != 0)
    {
        IPlatformFile& platformFile = FPlatformFileManager::Get().GetPlatformFile();
        IFileHandle* fileHandle = platformFile.OpenWrite(*mRedoPath,true);
        fileHandle->Seek(mUndosPositions[mCurrentIndex]);
        fileHandle->Write(mToBinary.GetData(),mToBinary.Num());
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
    UE_LOG(LogTemp,Display,TEXT("Global:"));
    UE_LOG(LogTemp,Display,TEXT("mCurrentIndex: %d"),mCurrentIndex);
    for(int i = 0; i < mUndosPositions.Num(); i++)
    {
        UE_LOG(LogTemp,Display,TEXT("mUndosPositions[%d]: %lld"),i,mUndosPositions[i]);
    }

    UE_LOG(LogTemp,Display,TEXT("Undo:"));
    for(int i = 0; i < mNumberBlocksUndo.Num(); i++)
    {
        UE_LOG(LogTemp,Display,TEXT("mNumberBlocksUndo[%d]: %d"),i,mNumberBlocksUndo[i]);
    }

    UE_LOG(LogTemp,Display,TEXT("Redo:"));
    for(int i = 0; i < mNumberBlocksRedo.Num(); i++)
    {
        UE_LOG(LogTemp,Display,TEXT("mNumberBlocksRedo[%d]: %d"),i,mNumberBlocksRedo[i]);
    }
}

bool
FOdysseyDrawingUndo::SaveDataRedo(UPTRINT iAddress, unsigned int iXTile, unsigned int iYTile,unsigned int iSizeX,unsigned int iSizeY)
{
    TArray< IOdysseyLayer* > layers = TArray<IOdysseyLayer*>();
    mLayerStackPtr->GetLayers()->DepthFirstSearchTree(&layers,false);

    FOdysseyImageLayer* imageLayer = nullptr;
    for(int j = 0; j < layers.Num(); j++)
    {
        if(iAddress == (UPTRINT)layers[j])
        {
            imageLayer = static_cast<FOdysseyImageLayer*> (layers[j]);
            break;
        }
    }

    if(imageLayer == nullptr)
        return false;

    IULISLoaderModule& hULIS = IULISLoaderModule::Get();
    uint32 perfIntent = ULIS3_PERF_MT | ULIS3_PERF_SSE42 | ULIS3_PERF_AVX2;
    mTileData = ::ul3::XCopy( hULIS.ThreadPool()
                            , ULIS3_BLOCKING
                            , perfIntent
                            , hULIS.HostDeviceInfo()
                            , ULIS3_NOCB
                            , imageLayer->GetBlock()->GetBlock()
                            , ::ul3::FRect( iXTile, iYTile, iSizeX, iSizeY ) );

    TArray<uint8> array = TArray<uint8>();
    array.AddUninitialized(mTileData->BytesTotal());

    FMemory::Memcpy(array.GetData(),mTileData->DataPtr(),mTileData->BytesTotal());

    UPTRINT address = (UPTRINT)imageLayer;
    mToBinary << address;
    mToBinary << iXTile;
    mToBinary << iYTile;
    mToBinary << iSizeX;
    mToBinary << iSizeY;
    mToBinary << array;

    mNumberBlocksRedo[mCurrentIndex]++;

    // Why is mTileData not deleted here ??
    //delete  mTileData;

    return true;
}

bool
FOdysseyDrawingUndo::SaveData(unsigned int iXTile, unsigned int iYTile,unsigned int iSizeX,unsigned int iSizeY)
{
    if(mLayerStackPtr->GetCurrentLayer()->GetNodeContent()->GetType() != IOdysseyLayer::eType::kImage)
        return false;

    FOdysseyImageLayer* imageLayer = static_cast<FOdysseyImageLayer*>(mLayerStackPtr->GetCurrentLayer()->GetNodeContent());
    IULISLoaderModule& hULIS = IULISLoaderModule::Get();
    uint32 perfIntent = ULIS3_PERF_MT | ULIS3_PERF_SSE42 | ULIS3_PERF_AVX2;
    mTileData = ::ul3::XCopy( hULIS.ThreadPool()
                            , ULIS3_BLOCKING
                            , perfIntent
                            , hULIS.HostDeviceInfo()
                            , ULIS3_NOCB
                            , imageLayer->GetBlock()->GetBlock()
                            , ::ul3::FRect( iXTile, iYTile, iSizeX, iSizeY ) );

    TArray<uint8> array = TArray<uint8>();
    array.AddUninitialized(mTileData->BytesTotal());

    FMemory::Memcpy(array.GetData(),mTileData->DataPtr(),mTileData->BytesTotal());

    UPTRINT address = (UPTRINT)imageLayer;
    mToBinary << address;
    mToBinary << iXTile;
    mToBinary << iYTile;
    mToBinary << iSizeX;
    mToBinary << iSizeY;
    mToBinary << array;

    mNumberBlocksUndo[mCurrentIndex]++;

    delete mTileData;

    return true;
}

bool
FOdysseyDrawingUndo::LoadData()
{
    if(mCurrentIndex > 0)
        mCurrentIndex--;

    bool bSaveForRedo = mNumberBlocksRedo[mCurrentIndex] == 0;

    if(bSaveForRedo)
    {
        StartRecordRedo();
    }

    UPTRINT address;
	unsigned int tileX = 0;
	unsigned int tileY = 0;
    unsigned int sizeX;
    unsigned int sizeY;

    TArray<uint8> TheBinaryArray;
    FFileHelper::LoadFileToArray(TheBinaryArray,*mUndoPath);

    FMemoryReader Ar = FMemoryReader(TheBinaryArray);
    Ar.Seek(mUndosPositions[mCurrentIndex]);

    for(int i = 0; i < mNumberBlocksUndo[mCurrentIndex]; i++)
    {
        Ar << address;
        Ar << tileX;
        Ar << tileY;
        Ar << sizeX;
        Ar << sizeY;

        if(bSaveForRedo)
        {
            SaveDataRedo(address,tileX,tileY,sizeX,sizeY);
        }

        Ar << mData;

        //Should be out of this loop
        FOdysseyImageLayer* imageLayer = nullptr;
        TArray< IOdysseyLayer* > layers = TArray<IOdysseyLayer*>();
        mLayerStackPtr->GetLayers()->DepthFirstSearchTree(&layers,false);

        for(int j = 0; j < layers.Num(); j++)
        {
            if(address == (UPTRINT)layers[j])
            {
                imageLayer = static_cast<FOdysseyImageLayer*> (layers[j]);
                break;
            }
        }

        if(imageLayer == nullptr)
            return false;
        //---

        //Useless, I just want mTileData at the right size for the next undo, to change
        IULISLoaderModule& hULIS = IULISLoaderModule::Get();
        uint32 perfIntent = ULIS3_PERF_MT | ULIS3_PERF_SSE42 | ULIS3_PERF_AVX2;
        if(i == 0)
            mTileData = ::ul3::XCopy( hULIS.ThreadPool()
                                    , ULIS3_BLOCKING
                                    , perfIntent
                                    , hULIS.HostDeviceInfo()
                                    , ULIS3_NOCB
                                    , imageLayer->GetBlock()->GetBlock()
                                    , ::ul3::FRect( tileX, tileY, sizeX, sizeY ) );

        if(mData.Num() > 0 && tileX >= 0 && tileY >= 0 && sizeX > 0 && sizeY > 0)
        {
            for(int j = 0; j < mData.Num(); j++)
            {
                *(mTileData->DataPtr() + j) = mData[j];
            }

            ::ul3::Copy( hULIS.ThreadPool()
                       , ULIS3_BLOCKING
                       , perfIntent
                       , hULIS.HostDeviceInfo()
                       , ULIS3_NOCB
                       , mTileData
                       , imageLayer->GetBlock()->GetBlock()
                       , ::ul3::FRect( 0, 0, sizeX, sizeY )
                       , ::ul3::FVec2I( tileX, tileY ) );
            mLayerStackPtr->ComputeResultBlock(::ul3::FRect(tileX,tileY,sizeX,sizeY));
        }
    }

    if(bSaveForRedo)
    {
        EndRecordRedo();
    }

    if( mNumberBlocksUndo[mCurrentIndex] != 0)
        delete mTileData;

    return true;
}

bool
FOdysseyDrawingUndo::Redo()
{
    UPTRINT address;
	unsigned int tileX;
	unsigned int tileY;
    unsigned int sizeX;
    unsigned int sizeY;

    TArray<uint8> TheBinaryArray;
    FFileHelper::LoadFileToArray(TheBinaryArray,*mRedoPath);

    FMemoryReader Ar = FMemoryReader(TheBinaryArray);
    Ar.Seek(mUndosPositions[mCurrentIndex]);

    for(int i = 0; i < mNumberBlocksRedo[mCurrentIndex]; i++)
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
        mLayerStackPtr->GetLayers()->DepthFirstSearchTree(&layers,false);

        for(int j = 0; j < layers.Num(); j++)
        {
            if(address == (UPTRINT)layers[j])
            {
                imageLayer = static_cast<FOdysseyImageLayer*> (layers[j]);
                break;
            }
        }

        if(imageLayer == nullptr)
            return false;
        //---

        //Useless, I just want mTileData at the right size for the next undo, to change
        IULISLoaderModule& hULIS = IULISLoaderModule::Get();
        uint32 perfIntent = ULIS3_PERF_MT | ULIS3_PERF_SSE42 | ULIS3_PERF_AVX2;
        if( i == 0 ) {
            mTileData = ::ul3::XCopy( hULIS.ThreadPool()
                                    , ULIS3_BLOCKING
                                    , perfIntent
                                    , hULIS.HostDeviceInfo()
                                    , ULIS3_NOCB
                                    , imageLayer->GetBlock()->GetBlock()
                                    , ::ul3::FRect( tileX, tileY, sizeX, sizeY ) );
        }

        if(mData.Num() > 0 && tileX >= 0 && tileY >= 0 && sizeX > 0 && sizeY > 0)
        {
            for(int j = 0; j < mData.Num(); j++)
            {
                *(mTileData->DataPtr() + j) = mData[j];
            }

            ::ul3::Copy( hULIS.ThreadPool()
                       , ULIS3_BLOCKING
                       , perfIntent
                       , hULIS.HostDeviceInfo()
                       , ULIS3_NOCB
                       , mTileData
                       , imageLayer->GetBlock()->GetBlock()
                       , ::ul3::FRect( 0, 0, sizeX, sizeY )
                       , ::ul3::FVec2I( tileX, tileY ) );
            mLayerStackPtr->ComputeResultBlock(::ul3::FRect(tileX,tileY,sizeX,sizeY));
        }
    }

    if(mCurrentIndex < (mUndosPositions.Num() - 1))
        mCurrentIndex++;

    if( mNumberBlocksRedo[mCurrentIndex] != 0 )
        delete mTileData;

    return true;
}

#undef LOCTEXT_NAMESPACE

