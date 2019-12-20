// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#pragma once

#include "CoreMinimal.h"
#include <ULIS_CORE>
#include <ULIS_FDECL>
#include <ULIS_BLENDINGMODES>

#include "Serialization/BufferArchive.h"
#include "Serialization/MemoryReader.h"

class FOdysseyBlock;
class IOdysseyLayer;
class FOdysseyImageLayer;
class FOdysseyDrawingUndo;


class ODYSSEYLAYER_API FOdysseyLayerStack
{
public:
    // Construction / Destruction
    ~FOdysseyLayerStack();
    FOdysseyLayerStack();
    FOdysseyLayerStack( int iWidth, int iHeight );

public:
    // Public API
    void               Init( int iWidth, int iHeight );
    void               InitFromData( FOdysseyBlock* iData );
    FOdysseyBlock*     GetResultBlock();
    void               ComputeResultBlock();
    void               ComputeResultBlock( const ::ULIS::FRect& iRect );
    void               ComputeResultBlockWithTempBuffer( const ::ULIS::FRect& iRect, FOdysseyBlock* iTempBuffer, float iOpacity = 1.f, ::ULIS::eBlendingMode iMode = ::ULIS::eBlendingMode::kNormal, ::ULIS::eAlphaMode iAlphaMode = ::ULIS::eAlphaMode::kNormal );
    void               BlendTempBufferOnCurrentBlock( const ::ULIS::FRect& iRect, FOdysseyBlock* iTempBuffer, float iOpacity = 1.f, ::ULIS::eBlendingMode iMode = ::ULIS::eBlendingMode::kNormal, ::ULIS::eAlphaMode iAlphaMode = ::ULIS::eAlphaMode::kNormal );
    int                Width() const;
    int                Height() const;
    FVector2D          Size() const;

public:
    // Public Array Tampon Methods
    FOdysseyImageLayer*                     AddLayer( int iAtIndex = -1 );
    FOdysseyImageLayer*                     AddLayerFromData( FOdysseyBlock* iData, FName iName = FName(), int iAtIndex = -1 );
    TArray< TSharedPtr< IOdysseyLayer > >*  GetLayers();

    void                                    DeleteLayer( IOdysseyLayer* ILayerToDelete );
    void                                    DeleteLayer( int iIndexLayerToDelete );
    void                                    MergeDownLayer( IOdysseyLayer* ILayerToMergeDown );
    void                                    DuplicateLayer( IOdysseyLayer* ILayerToDuplicate );

    int                                     GetCurrentLayerIndex() const;
    void                                    SetCurrentLayerIndex( int iIndex );
    TSharedPtr< IOdysseyLayer >             GetCurrentLayer() const;
    void                                    ClearCurrentLayer();
    void                                    FillCurrentLayerWithColor( const ::ULIS::CColor& iColor );

    TArray< TSharedPtr< FText > >           GetBlendingModesAsText();

private:
    // Private API
    FName                                   GetNextLayerName();
    void                                    InitResultAndTempBlock();

private:
    // Private Data Members
    FOdysseyBlock*                          mResultBlock;
    FOdysseyBlock*                          mTempBlock; // Temporary block for the blend of the tempBuffer into the image
    TArray< TSharedPtr< IOdysseyLayer > >   mLayers;
    int                                     mCurrentIndex;
    int                                     mWidth;
    int                                     mHeight;
    ETextureSourceFormat                    mTextureSourceFormat;
    bool                                    mIsInitialized;
    
public:
    FOdysseyDrawingUndo*                    mDrawingUndo;
};







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
    bool SaveDataRedo( UPTRINT iAddress, uint8 iXTile, uint8 iYTile, unsigned int iSizeX, unsigned int iSizeY );


public:
    bool Clear();
    bool SaveData( uint8 iXTile, uint8 iYTile, unsigned int iSizeX, unsigned int iSizeY );
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
    TArray<uint8> mData;
    ::ULIS::IBlock* mTileData;
    FString mUndoPath;
    FString mRedoPath;
};
