// Copyright © 2018-2020 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#pragma once

#include "CoreMinimal.h"
#include "Serialization/BufferArchive.h"
#include "Serialization/MemoryReader.h"
#include "IOdysseyLayer.h"
#include "OdysseyImageLayer.h"
#include "OdysseyFolderLayer.h"
#include "IOdysseySerializable.h"
#include "OdysseyTree.h"
#include "OdysseyBlock.h"
#include <ULIS3>

class FOdysseyDrawingUndo;

class ODYSSEYLAYER_API FOdysseyLayerStack
{
public:
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnCurrentLayerChanged, FOdysseyNTree< IOdysseyLayer* >*);
	DECLARE_MULTICAST_DELEGATE(FOnLayerStackDirty);

public:
    // Construction / Destruction
    ~FOdysseyLayerStack();
    FOdysseyLayerStack();
    FOdysseyLayerStack( int iWidth,int iHeight );

public:
    // Public API
    void                                Init( int iWidth,int iHeight );
    void                                InitFromData( FOdysseyBlock* iData );
    FOdysseyBlock*                      GetResultBlock();
    void                                ComputeResultBlock();
    void                                ComputeResultBlock( const ::ul3::FRect& iRect );
	void                                ComputeResultBlockWithTempBuffer(const ::ul3::FRect&    iRect,
                                                                         FOdysseyBlock*         iTempBuffer, 
                                                                         float                  iOpacity    = 1.f, 
                                                                         ::ul3::eBlendingMode   iMode       = ::ul3::BM_NORMAL,
                                                                         ::ul3::eAlphaMode      iAlphaMode  = ::ul3::AM_NORMAL);

    void                                BlendTempBufferOnCurrentBlock( const ::ul3::FRect&  iRect
                                                                     , FOdysseyBlock*       iTempBuffer
                                                                     , float                iOpacity    = 1.f
                                                                     , ::ul3::eBlendingMode iMode       = ::ul3::BM_NORMAL
                                                                     , ::ul3::eAlphaMode    iAlphaMode  = ::ul3::AM_NORMAL );
    int                                 Width() const;
    int                                 Height() const;
    FVector2D                           Size() const;

public:
    // Public Array Tampon Methods
    FOdysseyImageLayer*                 AddImageLayer( FOdysseyNTree< IOdysseyLayer* >* iPosition, int iAtIndex = -1 );
    FOdysseyImageLayer*                 AddImageLayer( int iAtIndex = -1 );
    FOdysseyImageLayer*                 AddImageLayerFromData( FOdysseyBlock* iData, FOdysseyNTree< IOdysseyLayer* >* iPosition, FName iName = FName(), int iAtIndex = -1 );
    FOdysseyImageLayer*                 AddImageLayerFromData( FOdysseyBlock* iData, FName iName = FName(), int iAtIndex = -1 );
    FOdysseyFolderLayer*                AddFolderLayer( FOdysseyNTree< IOdysseyLayer* >* iPosition, FName iName = FName() ,int iAtIndex = -1 );
    FOdysseyFolderLayer*                AddFolderLayer( FName iName = FName(), int iAtIndex = -1 );
    FOdysseyNTree< IOdysseyLayer* >*    GetLayers();
    FOdysseyNTree< IOdysseyLayer* >*    GetCurrentLayer() const;
    int                                 GetCurrentLayerAsIndex() const;
    FOdysseyNTree< IOdysseyLayer* >*    GetCurrentLayerFromIndex( int iIndex ) const;
    void                                SetCurrentLayer( IOdysseyLayer* iLayer );
    void                                SetCurrentLayer( FOdysseyNTree< IOdysseyLayer* >* iLayer );
    void                                DeleteLayer( IOdysseyLayer* iLayerToDelete );
    void                                MergeDownLayer( IOdysseyLayer* iLayerToMergeDown );
    void                                FlattenLayer( IOdysseyLayer* iLayerToFlatten );
    void                                DuplicateLayer( IOdysseyLayer* iLayerToDuplicate );
    void                                ClearCurrentLayer();
    void                                FillCurrentLayerWithColor( const ::ul3::IPixel& iColor );
    TArray< TSharedPtr< FText > >       GetBlendingModesAsText();
    ETextureSourceFormat                GetTextureSourceFormat();

    // Overloads for save in archive
    friend ODYSSEYLAYER_API FArchive& operator<<( FArchive &Ar,FOdysseyLayerStack* ioSaveLayerStack );

	FOnCurrentLayerChanged&	OnCurrentLayerChanged() { return mOnCurrentLayerChanged; }
    FOnLayerStackDirty& OnLayerStackDirty() { return mOnLayerStackDirty; }

private:
    // Private API
    FName                               GetNextLayerName();
    void                                InitResultAndTempBlock();
    FOdysseyBlock*                      ComputeBlockOfLayers( FOdysseyNTree< IOdysseyLayer* >* iLayers );

private:
    // Private Data Members
    FOdysseyBlock*                      mResultBlock;
    FOdysseyBlock*                      mTempBlock;
    FOdysseyNTree< IOdysseyLayer* >*    mLayers;
    FOdysseyNTree< IOdysseyLayer* >*    mCurrentLayer;
    int                                 mWidth;
    int                                 mHeight;
    ETextureSourceFormat                mTextureSourceFormat;
    bool                                mIsInitialized;

	FOnCurrentLayerChanged				mOnCurrentLayerChanged;
    FOnLayerStackDirty                  mOnLayerStackDirty;

public:
    FOdysseyDrawingUndo*                mDrawingUndo;
};

ODYSSEYLAYER_API FArchive& operator<<( FArchive &Ar, FOdysseyLayerStack* ioSaveLayerStack );

class ODYSSEYLAYER_API FOdysseyDrawingUndo
{
public:
    FOdysseyDrawingUndo( FOdysseyLayerStack* iLayerStack );
    ~FOdysseyDrawingUndo();

public:
    void StartRecord();
    void EndRecord();

private:
    void StartRecordRedo();
    void EndRecordRedo();
    bool SaveDataRedo( UPTRINT iAddress, unsigned int iXTile, unsigned int iYTile, unsigned int iSizeX, unsigned int iSizeY );

public:
    bool Clear();
    bool SaveData(unsigned int iXTile, unsigned int iYTile, unsigned int iSizeX, unsigned int iSizeY );
    bool LoadData();
    bool Redo();
    void Check();

private:
    FOdysseyLayerStack* mLayerStackPtr;
    int                 mCurrentIndex;
    TArray<int64>       mUndosPositions;
    TArray<int>         mNumberBlocksUndo;
    TArray<int>         mNumberBlocksRedo;
    FBufferArchive      mToBinary;
    ::ul3::FBlock* mTileData;
    FString mUndoPath;
    FString mRedoPath;
    TArray< uint8 > mData; // Format: [ X, Y, W, H, DATA ] [ ... ] for each tile
};
