// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"
#include "Serialization/BufferArchive.h"
#include "Serialization/MemoryReader.h"
#include "IOdysseyLayer.h"
#include "OdysseyImageLayer.h"
#include "OdysseyFolderLayer.h"
#include "OdysseyRootLayer.h"
#include <ULIS>

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
    // Params:
    // - Rects List
    // - Num Rects
    DECLARE_MULTICAST_DELEGATE_TwoParams( FOnLayerStackImageResultChanged, const ::ULIS::FRectI*, const uint32 );

    // Sent when children are changed (added, removed, moved)
    // Params: None
    DECLARE_MULTICAST_DELEGATE(FOnLayerStackStructureChanged);

public:
    // Construction / Destruction
    ~FOdysseyLayerStack();
    FOdysseyLayerStack();

    void Init(int iWidth,int iHeight, ::ULIS::eFormat iOutputFormat);

public:
    // Public API / Result Computation

    // Computes the result of all layers into one given block
    // The given block is cleared at start
    void ComputeResultInBlock( ::ULIS::FBlock* ioBlock );

    // Computes the result of one part defined by iRect of all layers into one given block
    // The given block is cleared at start
    void ComputeResultInBlock( ::ULIS::FBlock* ioBlock, const ::ULIS::FRectI* iRects, const uint32 iNumRects );

    // Computes the result of one part defined by iRect of all layers into one given block
    // Adds a Temporary buffer above the current layer
    void ComputeResultInBlockWithBlockAsCurrentLayer( ::ULIS::FBlock* ioBlock, ::ULIS::FBlock* iTempBlock, const ::ULIS::FRectI* iRects, const uint32 iNumRects );

public:
    // Public API / Getters

    // Return layer stack width
    int         Width() const;

    // Return layer stack height
    int         Height() const;

    // Return layer stack format
    ::ULIS::eFormat Format() const;

    // Return layer stack output format
    ::ULIS::eFormat OutputFormat() const;

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
    void                                FillCurrentLayerWithColor( const ::ULIS::ISample& iColor );

public:
    //Public API / Callbacks

    FOnLayerStackCurrentLayerChanged&   OnCurrentLayerChanged() { return mOnCurrentLayerChanged; }
    FOnLayerStackRootLayerChanged&      OnRootLayerChanged() { return mOnRootLayerChanged; }
    FOnLayerStackImageResultChanged&    OnImageResultChanged() { return mOnImageResultChanged; }
    FOnLayerStackStructureChanged&      OnStructureChanged() { return mOnStructureChanged; }

public:
    //Public API / Saving And Loading

    // Overloads for save in archive
    friend ODYSSEYLAYER_API FArchive& operator<<( FArchive &Ar,FOdysseyLayerStack* ioSaveLayerStack );

private:
    // Private API
    void                                OnLayerRootImageResultChanged( const ::ULIS::FRectI* iRects, const uint32 iNumRects );
    void                                OnLayerAdded(TSharedPtr<IOdysseyLayer> iNode);
    void                                OnLayerRemoved(TSharedPtr<IOdysseyLayer> iNode, TSharedPtr<IOdysseyLayer> iOldParent, int iOldIndex);

private:
    // Private Data Members
    int                                 mWidth;
    int                                 mHeight;
    ::ULIS::eFormat                     mFormat;
    ::ULIS::eFormat                     mOutputFormat;
    TSharedPtr<FOdysseyRootLayer>       mLayerRoot;
    TSharedPtr<IOdysseyLayer>           mCurrentLayer;
    bool                                mIsInitialized;

    FOnLayerStackCurrentLayerChanged    mOnCurrentLayerChanged;
    FOnLayerStackRootLayerChanged        mOnRootLayerChanged;
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
    bool SaveData(const TArray<::ULIS::FRectI>& iRects);
    bool LoadData();
    bool Redo();
    void Check();

public:
    bool HasUndo();
    bool HasRedo();

private:
    FOdysseyLayerStack* mLayerStackPtr;
    int                 mCurrentIndex;
    TArray<int64>       mUndosPositions;
    TArray<int>         mNumberBlocksUndo;
    TArray<int>         mNumberBlocksRedo;
    FBufferArchive      mToBinary;
    // ::ULIS::FBlock*      mTileData;
    FString mUndoPath;
    FString mRedoPath;
    // TArray64< uint8 > mData;

    //::ULIS::FBlock* mData;
};
