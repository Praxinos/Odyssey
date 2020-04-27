// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#pragma once

#include "CoreMinimal.h"
#include <ULIS3>

#include "Serialization/BufferArchive.h"
#include "Serialization/MemoryReader.h"
#include "IOdysseyLayer.h"
#include "IOdysseySerializable.h"
#include "OdysseyTree.h"

#include "OdysseyBlock.h"

class FOdysseyDrawingUndo;

class ODYSSEYLAYER_API FOdysseyLayerStack //: public IOdysseySerializable
{
public:
    // Construction / Destruction
    ~FOdysseyLayerStack();
    FOdysseyLayerStack();
    FOdysseyLayerStack(int iWidth,int iHeight);

public:
    // Public API
    void               Init(int iWidth,int iHeight);
    void               InitFromData(FOdysseyBlock* iData);
    FOdysseyBlock*     GetResultBlock();
    void               ComputeResultBlock();
    void               ComputeResultBlock(const ::ul3::FRect& iRect);
    void               ComputeResultBlockWithTempBuffer(const ::ul3::FRect& iRect,FOdysseyBlock* iTempBuffer,float iOpacity = 1.f,::ul3::eBlendingMode iMode = ::ul3::eBlendingMode::kNormal,::ul3::eAlphaMode iAlphaMode = ::ul3::eAlphaMode::kNormal);
    void               BlendTempBufferOnCurrentBlock(const ::ul3::FRect& iRect,FOdysseyBlock* iTempBuffer,float iOpacity = 1.f,::ul3::eBlendingMode iMode = ::ul3::eBlendingMode::kNormal,::ul3::eAlphaMode iAlphaMode = ::ul3::eAlphaMode::kNormal);
    int                Width() const;
    int                Height() const;
    FVector2D          Size() const;

public:
    // Public Array Tampon Methods
    FOdysseyImageLayer*                             AddImageLayer(FOdysseyNTree< IOdysseyLayer* >* iPosition,int iAtIndex = -1);
    FOdysseyImageLayer*                             AddImageLayer(int iAtIndex = -1);
    FOdysseyImageLayer*                             AddImageLayerFromData(FOdysseyBlock* iData,FOdysseyNTree< IOdysseyLayer* >* iPosition,FName iName = FName(),int iAtIndex = -1);
    FOdysseyImageLayer*                             AddImageLayerFromData(FOdysseyBlock* iData,FName iName = FName(),int iAtIndex = -1);

    FOdysseyFolderLayer*                            AddFolderLayer(FOdysseyNTree< IOdysseyLayer* >* iPosition,FName iName = FName(),int iAtIndex = -1);
    FOdysseyFolderLayer*                            AddFolderLayer(FName iName = FName(),int iAtIndex = -1);

    FOdysseyNTree< IOdysseyLayer* >*                GetLayers();
    FOdysseyNTree< IOdysseyLayer* >*                GetCurrentLayer() const;
    int                                             GetCurrentLayerAsIndex() const;
    FOdysseyNTree< IOdysseyLayer* >*                GetCurrentLayerFromIndex(int iIndex) const;

    void                                            SetCurrentLayer(IOdysseyLayer* iLayer);
    void                                            SetCurrentLayer(FOdysseyNTree< IOdysseyLayer* >* iLayer);

    void                                            DeleteLayer(IOdysseyLayer* iLayerToDelete);

    void                                            MergeDownLayer(IOdysseyLayer* iLayerToMergeDown);
    void                                            FlattenLayer(IOdysseyLayer* iLayerToFlatten);
    void                                            DuplicateLayer(IOdysseyLayer* iLayerToDuplicate);
    void                                            ClearCurrentLayer();
    void                                            FillCurrentLayerWithColor(const ::ul3::CColor& iColor);

    TArray< TSharedPtr< FText > >                   GetBlendingModesAsText();

    ETextureSourceFormat                            GetTextureSourceFormat();

    // Overloads for save in archive
    friend FArchive& operator<<(FArchive &Ar,FOdysseyLayerStack* ioSaveLayerStack);

private:
    // Private API
    FName                                           GetNextLayerName();
    void                                            InitResultAndTempBlock();
    FOdysseyBlock*                                  ComputeBlockOfLayers(FOdysseyNTree< IOdysseyLayer* >* iLayers);

public:
    // Overloads for save in archive
    friend FArchive& operator<<(FArchive &Ar,FOdysseyLayerStack* SaveLayerStack);

private:
    // Private Data Members
    FOdysseyBlock*                                  mResultBlock;
    FOdysseyBlock*                                  mTempBlock; // Temporary block for the blend of the tempBuffer into the image
    FOdysseyNTree< IOdysseyLayer* >*                mLayers;
    FOdysseyNTree< IOdysseyLayer* >*                mCurrentLayer;
    int                                             mWidth;
    int                                             mHeight;
    ETextureSourceFormat                            mTextureSourceFormat;
    bool                                            mIsInitialized;

public:
    FOdysseyDrawingUndo*                            mDrawingUndo;
};

//Serialization of item
#include "OdysseyLayerStack.inl"

class ODYSSEYLAYER_API FOdysseyDrawingUndo
{
public:
    FOdysseyDrawingUndo(FOdysseyLayerStack* iLayerStack);
    ~FOdysseyDrawingUndo();

public:
    void StartRecord();
    void EndRecord();

private:
    void StartRecordRedo();
    void EndRecordRedo();
    bool SaveDataRedo(UPTRINT iAddress,uint8 iXTile,uint8 iYTile,unsigned int iSizeX,unsigned int iSizeY);

public:
    bool Clear();
    bool SaveData(uint8 iXTile,uint8 iYTile,unsigned int iSizeX,unsigned int iSizeY);
    bool LoadData();
    bool Redo();
    void Check();

private:
    FOdysseyLayerStack* mLayerStackPtr;

private:
    int mCurrentIndex;
    TArray<int64> mUndosPositions;
    TArray<int> mNumberBlocksUndo;
    TArray<int> mNumberBlocksRedo;
    FBufferArchive mToBinary;

    //Content is: X of the tile, Y of the tile, X size of the tile, Y size of the tile, PixelData, these 5 for each tile.
    // Why don't we use a struct or a dedicated class for that ?
    TArray< uint8 > mData;
    ::ul3::FBlock* mTileData;
    FString mUndoPath;
    FString mRedoPath;
};
