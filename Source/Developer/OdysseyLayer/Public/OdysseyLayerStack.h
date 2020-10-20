// Copyright © 2018-2020 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#pragma once

#include "CoreMinimal.h"
#include "Serialization/BufferArchive.h"
#include "Serialization/MemoryReader.h"
#include "IOdysseyLayer.h"
#include "OdysseyImageLayer.h"
#include "OdysseyFolderLayer.h"
#include "OdysseyRootLayer.h"
#include "IOdysseySerializable.h"
#include "OdysseyBlock.h"
#include <ULIS3>

class FOdysseyDrawingUndo;

class ODYSSEYLAYER_API FOdysseyLayerStack
{
public:
    // Current Layer Changed Event
    // Params :
    // - Previous current layer
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnLayerStackCurrentLayerChanged, TSharedPtr<IOdysseyLayer>);
    
    // Root Layer Changed Event
    // Params :
    // - Previous Root layer
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnLayerStackRootLayerChanged, TSharedPtr<FOdysseyRootLayer>);

    // Image Result Changed Event
    // Params: None
	DECLARE_MULTICAST_DELEGATE(FOnLayerStackImageResultChanged);

    // Sent when children are changed (added, removed, moved)
    // Params: None
	DECLARE_MULTICAST_DELEGATE(FOnLayerStackStructureChanged);

public:
    // Construction / Destruction
    ~FOdysseyLayerStack();
    FOdysseyLayerStack();

    void Init(int iWidth,int iHeight, ::ul3::tFormat iOutputFormat);

public:
    // Public API / Result Computation

    // Computes the result of all layers into one given block
    // The given block is cleared at start
    void ComputeResultInBlock( ::ul3::FBlock* ioBlock );

    // Computes the result of one part defined by iRect of all layers into one given block
    // The given block is cleared at start
    void ComputeResultInBlock( ::ul3::FBlock* ioBlock, const ::ul3::FRect& iRect );

    // Computes the result of one part defined by iRect of all layers into one given block
    // Adds a Temporary buffer above the current layer
	void ComputeResultInBlockWithBlockAsCurrentLayer(::ul3::FBlock* ioBlock, FOdysseyBlock* iTempBlock, const ::ul3::FRect& iRect);

public:
    // Public API / Getters

    // Return layer stack width
    int         Width() const;

    // Return layer stack height
    int         Height() const;

	// Return layer stack format
	int         Format() const;

	// Return layer stack output format
	int         OutputFormat() const;

    // Return layer stack width and height
    FVector2D   Size() const;

public:
    // Public API / Layers Management
    // Adds a Layer to the stack at iIndex in the specified Parent node
    void                                AddLayer( TSharedPtr<IOdysseyLayer> iLayer, TSharedPtr<IOdysseyLayer> iParent, int iAtIndex = -1 );

    // Adds a Layer to the stack at iIndex in the root node
    void                                AddLayer( TSharedPtr<IOdysseyLayer> iLayer, int iAtIndex = -1 );

    // Get the Current Layer
    TSharedPtr<FOdysseyRootLayer>       GetLayerRoot() const;

    // Set the Current Layer
    void                                SetLayerRoot(TSharedPtr<FOdysseyRootLayer> iLayerRoot);

    // Get the Current Layer
    TSharedPtr<IOdysseyLayer>           GetCurrentLayer() const;

    // Get the Current Layer Index
    int                                 GetCurrentLayerAsIndex() const;

    // Get the layer at Index
    TSharedPtr<IOdysseyLayer>           GetLayerFromIndex( int iIndex ) const;

    // Sets the Current Layer
    void                                SetCurrentLayer( TSharedPtr<IOdysseyLayer> iLayer );

    // Deletes a Layer
    void                                DeleteLayer( TSharedPtr<IOdysseyLayer> iLayerToDelete );

    // Merges the given image layer with the layer under it
    void                                MergeDownLayer( TSharedPtr<IOdysseyLayer> iLayerToMergeDown );

    // Flatten the foler layer into a single image layer
    void                                FlattenLayer( TSharedPtr<IOdysseyLayer> iLayerToFlatten );

    //Duplicates the given layer
    void                                DuplicateLayer( TSharedPtr<IOdysseyLayer> iLayerToDuplicate );

    // Clears the content of the current Layer
    void                                ClearCurrentLayer();

    // Fills the content of the current layer with iColor
    void                                FillCurrentLayerWithColor( const ::ul3::IPixel& iColor );

public:
    //Public API / Callbacks

    FOnLayerStackCurrentLayerChanged&	OnCurrentLayerChanged() { return mOnCurrentLayerChanged; }
    FOnLayerStackRootLayerChanged&	    OnRootLayerChanged() { return mOnRootLayerChanged; }
    FOnLayerStackImageResultChanged&    OnImageResultChanged() { return mOnImageResultChanged; }
    FOnLayerStackStructureChanged&      OnStructureChanged() { return mOnStructureChanged; }

public:
    //Public API / Saving And Loading

    // Overloads for save in archive
    friend ODYSSEYLAYER_API FArchive& operator<<( FArchive &Ar,FOdysseyLayerStack* ioSaveLayerStack );

private:
    // Private API
    void                                OnLayerRootImageResultChanged();
    void                                OnLayerAdded(TSharedPtr<IOdysseyLayer> iNode);
    void                                OnLayerRemoved(TSharedPtr<IOdysseyLayer> iNode, TSharedPtr<IOdysseyLayer> iOldParent, int iOldIndex);

private:
    // Private Data Members
    int                                 mWidth;
    int                                 mHeight;
    ::ul3::tFormat                      mFormat;
	::ul3::tFormat                      mOutputFormat;
    TSharedPtr<FOdysseyRootLayer>       mLayerRoot;
    TSharedPtr<IOdysseyLayer>           mCurrentLayer;
    bool                                mIsInitialized;

	FOnLayerStackCurrentLayerChanged	mOnCurrentLayerChanged;
    FOnLayerStackRootLayerChanged	    mOnRootLayerChanged;
    FOnLayerStackImageResultChanged     mOnImageResultChanged;
    FOnLayerStackStructureChanged       mOnStructureChanged;

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
    TArray< uint8 > mData;

	//FOdysseyBlock* mData;
};
