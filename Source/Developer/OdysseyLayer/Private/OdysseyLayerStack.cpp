// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc
#include "OdysseyLayerStack.h"
#include "HAL/PlatformFilemanager.h"
#include "Misc/FileHelper.h"
#include "UObject/UObjectGlobals.h"
#include "OdysseyImageLayer.h"
#include "OdysseyFolderLayer.h"
#include "OdysseyRootLayer.h"
#include "ULISLoaderModule.h"

#define LOCTEXT_NAMESPACE "OdysseyLayerStack"

::ULIS::eFormat
ComputationFormatForResultFormat(::ULIS::eFormat iFormat)
{
    return  static_cast< ::ULIS::eFormat >( iFormat | ULIS_W_ALPHA( 1 ) );
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyLayerStack::~FOdysseyLayerStack()
{
    mLayerRoot->ImageResultChangedDelegate().RemoveAll(this);
    mLayerRoot->NodeAdded().RemoveAll(this);
    mLayerRoot->NodeRemoved().RemoveAll(this);
    delete  mDrawingUndo;
}

FOdysseyLayerStack::FOdysseyLayerStack()
    : mWidth( -1 )
    , mHeight( -1 )
    , mFormat( static_cast< ::ULIS::eFormat >( 0 ) )
    , mOutputFormat( static_cast< ::ULIS::eFormat >( 0 ) )
    , mLayerRoot(MakeShareable(new  FOdysseyRootLayer()))
    , mCurrentLayer( mLayerRoot )
    , mDrawingUndo( new  FOdysseyDrawingUndo(this) )
{
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------- Public API

void
FOdysseyLayerStack::Init(int iWidth,int iHeight, ::ULIS::eFormat iFormat)
{
    mWidth = iWidth;
    mHeight = iHeight;
    mOutputFormat = iFormat;
    mFormat = ComputationFormatForResultFormat(iFormat);

    mLayerRoot->ImageResultChangedDelegate().RemoveAll(this); //in case Init is walled twice, which should not happen.... but actually happened once
    mLayerRoot->NodeAdded().RemoveAll(this); //in case Init is walled twice, which should not happen.... but actually happened once
    mLayerRoot->NodeRemoved().RemoveAll(this); //in case Init is walled twice, which should not happen.... but actually happened once

    mLayerRoot->ImageResultChangedDelegate().AddRaw(this, &FOdysseyLayerStack::OnLayerRootImageResultChanged);
    mLayerRoot->NodeAdded().AddRaw(this, &FOdysseyLayerStack::OnLayerAdded);
    mLayerRoot->NodeRemoved().AddRaw(this, &FOdysseyLayerStack::OnLayerRemoved);
}

void
FOdysseyLayerStack::ComputeResultInBlock( ::ULIS::FBlock* ioBlock )
{
    ::ULIS::FRectI canvasRect = ::ULIS::FRectI(0, 0, Width(), Height());
    ComputeResultInBlock( ioBlock, &canvasRect, 1 );
}

void
FOdysseyLayerStack::ComputeResultInBlock( ::ULIS::FBlock* ioBlock, const ::ULIS::FRectI* iRects, const uint32 iNumRects )
{
    if (iNumRects == 0)
        return;

    TArray< ::ULIS::FVec2I > pos;
    TArray< ::ULIS::FVec2I > convPos;
    TArray< ::ULIS::FVec2I > outputPos;
    TArray< ::ULIS::FBlock* > output;
    TArray< ::ULIS::FBlock* > conv;
    pos.Reserve( iNumRects );
    convPos.Reserve( iNumRects );
    outputPos.Reserve( iNumRects );
    output.Reserve( iNumRects );
    conv.Reserve( iNumRects );
    for( uint32 i = 0; i < iNumRects; ++i )
    {
        const ::ULIS::FVec2I rectPos = iRects[i].Position();
        const ::ULIS::FVec2I rectSize = iRects[i].Size();
        const bool predA = ioBlock->Format() != mOutputFormat;
        const bool predB = mFormat != mOutputFormat;
        pos.Emplace( rectPos );
        convPos.Emplace( predA ? ::ULIS::FVec2I( 0 ) : predB ? ::ULIS::FVec2I( 0 ) : rectPos );
        outputPos.Emplace( predA ?::ULIS:: FVec2I( 0 ) : rectPos );
        output.Emplace( predA ? new ::ULIS::FBlock( rectSize.x, rectSize.y, mOutputFormat ) : ioBlock );
        conv.Emplace( predB ? new ::ULIS::FBlock( rectSize.x, rectSize.y, mOutputFormat ) : output[i] );
    }

    TArray<::ULIS::FEvent> eventRender = mLayerRoot->RenderImage( conv.GetData(), iRects, convPos.GetData(), iNumRects );

    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext( ::ULIS::Format_RGBA8 );
    
    TArray<::ULIS::FEvent> eventA;
    TArray<::ULIS::FEvent> eventB;
    eventA.SetNum(iNumRects);
    eventB.SetNum(iNumRects);
    for( uint32 i = 0; i < iNumRects; ++i )
    {
        const bool predA = conv[i] != output[i];
        const bool predB = output[i] != ioBlock;
        if( predA ) {
            eventA[i] = ::ULIS::FEvent( ::ULIS::FOnEventComplete( [ &conv, i ]( const ::ULIS::FRectI& ) {
                delete  conv[i];
            } ) );
            ctx.ConvertFormat(
                  *conv[i]
                , *output[i]
                , ::ULIS::FRectI::Auto, outputPos[i]
                , ::ULIS::FSchedulePolicy::MonoScanlines
                , 1
                , &eventRender[i]
                , &eventA[i]
            );
            ctx.Flush();
        } else {
            ctx.Dummy_OP(1, &eventRender[i], &eventA[i]);
            ctx.Flush();
            //::ULIS::FContext::FinishEventNo_OP( &eventA, ULIS_NO_ERROR );
        }

        if( predB )
        {
            eventB[i] = ::ULIS::FEvent( ::ULIS::FOnEventComplete( [ &output, i ]( const ::ULIS::FRectI& ) {
                delete  output[i];
            } ) );
            ctx.ConvertFormat(
                  *output[i]
                , *ioBlock
                , ::ULIS::FRectI::Auto
                , pos[i]
                , ::ULIS::FSchedulePolicy::MonoScanlines
                , 1
                , &eventA[i]
                , &eventB[i]
            );
            ctx.Flush();
        }
    }
    ctx.Fence();
}

void
FOdysseyLayerStack::ComputeResultInBlockWithBlockAsCurrentLayer(::ULIS::FBlock* ioBlock, FOdysseyBlock* iTempBlock, const ::ULIS::FRectI* iRects, const uint32 iNumRects )
{
    if( !mCurrentLayer )
        return;

    IOdysseyLayer::eType type = mCurrentLayer->GetType();
    if( type != IOdysseyLayer::eType::kImage || !mCurrentLayer->IsVisible() ) {
        ComputeResultInBlock( ioBlock, iRects, iNumRects );
        return;
    }

    TSharedPtr< FOdysseyImageLayer > imageLayer = StaticCastSharedPtr< FOdysseyImageLayer >( mCurrentLayer );
    FOdysseyBlock* block = imageLayer->GetBlock();
    imageLayer->SetBlock( iTempBlock, false, false );
    ComputeResultInBlock( ioBlock, iRects, iNumRects );
    imageLayer->SetBlock( block, false, false );
}

int
FOdysseyLayerStack::Width() const
{
    //TODO: one day we will have infinite size and resizable layers
    // So this day, compute the size from all layers
    return mWidth;
}

int
FOdysseyLayerStack::Height() const
{
    //TODO: one day we will have infinite size and resizable layers
    // So this day, compute the size from all layers
    return mHeight;
}

::ULIS::eFormat
FOdysseyLayerStack::Format() const
{
    return mFormat;
}

::ULIS::eFormat
FOdysseyLayerStack::OutputFormat() const
{
    return mOutputFormat;
}

FVector2D
FOdysseyLayerStack::Size() const
{
    return FVector2D(Width(),Height());
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------- Public Array Tampon Methods

void
FOdysseyLayerStack::AddLayer( TSharedPtr<IOdysseyLayer> iLayer, TSharedPtr<IOdysseyLayer> iParent, int iIndex )
{
    //TODO: Adding a layer should only add a layer, not set the currentlayer aswell
    if (iParent)
    {
        iParent->AddNode(iLayer, iIndex);
    }
    else
    {
        mLayerRoot->AddNode(iLayer, iIndex);
    }
    SetCurrentLayer(iLayer);
}

void
FOdysseyLayerStack::AddLayer( TSharedPtr<IOdysseyLayer> iLayer, int iIndex )
{
    //TODO: Adding a layer should only add a layer, not set the currentlayer aswell
    mLayerRoot->AddNode(iLayer, iIndex);
    SetCurrentLayer(iLayer);
}

TSharedPtr<FOdysseyRootLayer>
FOdysseyLayerStack::GetLayerRoot() const
{
    return mLayerRoot;
}

void
FOdysseyLayerStack::SetLayerRoot(TSharedPtr<FOdysseyRootLayer> iLayerRoot)
{
    TSharedPtr<FOdysseyRootLayer> oldValue = mLayerRoot;

    mLayerRoot->ImageResultChangedDelegate().RemoveAll(this);
    mLayerRoot->NodeAdded().RemoveAll(this);
    mLayerRoot->NodeRemoved().RemoveAll(this);

    mLayerRoot = iLayerRoot;

    mLayerRoot->ImageResultChangedDelegate().AddRaw(this, &FOdysseyLayerStack::OnLayerRootImageResultChanged);
    mLayerRoot->NodeAdded().AddRaw(this, &FOdysseyLayerStack::OnLayerAdded);
    mLayerRoot->NodeRemoved().AddRaw(this, &FOdysseyLayerStack::OnLayerRemoved);

    mOnRootLayerChanged.Broadcast(oldValue);
    
    SetCurrentLayer(mLayerRoot);
}

TSharedPtr<IOdysseyLayer>
FOdysseyLayerStack::GetCurrentLayer() const
{
    return mCurrentLayer;
}

int
FOdysseyLayerStack::GetCurrentLayerAsIndex() const
{
    TArray< TSharedPtr<IOdysseyLayer> > layers;
    mLayerRoot->DepthFirstSearchTree(&layers, false);

    for(int i = 0; i < layers.Num(); i++)
    {
        if(layers[i] == mCurrentLayer)
            return i;
    }
    return -1;
}

TSharedPtr<IOdysseyLayer>
FOdysseyLayerStack::GetLayerFromIndex(int iIndex) const
{
    TArray< TSharedPtr<IOdysseyLayer> > layers;
    mLayerRoot->DepthFirstSearchTree(&layers,false);

    if(iIndex >= layers.Num() || iIndex < 0)
        return NULL;

    return layers[iIndex];
}

void
FOdysseyLayerStack::SetCurrentLayer(TSharedPtr<IOdysseyLayer> iLayer)
{
    TSharedPtr<IOdysseyLayer> oldValue = mCurrentLayer;
    mCurrentLayer = iLayer;
    mOnCurrentLayerChanged.Broadcast(oldValue);
    //mOnLayerStackDirty.Broadcast();
}

void
FOdysseyLayerStack::DeleteLayer(TSharedPtr<IOdysseyLayer> iLayer)
{
    TSharedPtr<IOdysseyLayer> parent = iLayer->GetParent();
    int index = iLayer->GetIndexInParent();

    if (mCurrentLayer == iLayer || mCurrentLayer->HasForParent(iLayer))
    {
        TArray<TSharedPtr<IOdysseyLayer>> children = parent->GetNodes();
        int childrenCount = children.Num();
        if(childrenCount <= 1)
        {
            SetCurrentLayer(parent);
        }
        else if (index == childrenCount - 1)
        {
            SetCurrentLayer(children[index - 1]);
        }
        else
        {
            SetCurrentLayer(children[index + 1]);
        }
    }
    parent->DeleteNode(index);
}

void FOdysseyLayerStack::MergeDownLayer(TSharedPtr<IOdysseyLayer> iLayer)
{
    if (iLayer->GetType() != IOdysseyLayer::eType::kImage)
        return;

    TSharedPtr<IOdysseyLayer> parent = iLayer->GetParent();
    int index = iLayer->GetIndexInParent();
    TArray<TSharedPtr<IOdysseyLayer>> children = parent->GetNodes();
    if (index >= children.Num() - 1)
        return;

    TSharedPtr<IOdysseyLayer> dstLayer = children[index + 1];
    if (!dstLayer || dstLayer->GetType() != IOdysseyLayer::eType::kImage)
        return;

    TSharedPtr<FOdysseyImageLayer> srcLayerImage = StaticCastSharedPtr<FOdysseyImageLayer>(iLayer);
    TSharedPtr<FOdysseyImageLayer> dstLayerImage = StaticCastSharedPtr<FOdysseyImageLayer>(dstLayer);

    ::ULIS::FRectI canvasRect = ::ULIS::FRectI( 0, 0, Width(), Height());
    ::ULIS::FVec2I pos( 0, 0 );

    auto lvalue = dstLayerImage->GetBlock()->GetBlock();

    ::ULIS::FEvent eventBlend = ::ULIS::FEvent::NoOP();
    srcLayerImage->Blend( &lvalue, &canvasRect, &pos, 1, &eventBlend);
    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(lvalue->Format());
    ctx.Finish();

    dstLayerImage->ImageResultChangedDelegate().Broadcast( nullptr, 0 );
    DeleteLayer( iLayer );
}

void FOdysseyLayerStack::FlattenLayer(TSharedPtr<IOdysseyLayer> iLayer)
{
    if(iLayer->GetType() != IOdysseyLayer::eType::kFolder)
        return;

    TSharedPtr<FOdysseyFolderLayer> folderLayer = StaticCastSharedPtr<FOdysseyFolderLayer>(iLayer);
    TSharedPtr<IOdysseyLayer> parent = folderLayer->GetParent();
    int index = folderLayer->GetIndexInParent();
    TSharedPtr<FOdysseyImageLayer> imageLayer = MakeShareable(new FOdysseyImageLayer(iLayer->GetName(), FVector2D(Width(), Height()), Format()) );

    ::ULIS::FRectI canvasRect = ::ULIS::FRectI( 0, 0, Width(), Height());
    ::ULIS::FVec2I pos( 0, 0 );
    auto lvalue = imageLayer->GetBlock()->GetBlock();

    ::ULIS::FEvent eventBlend = ::ULIS::FEvent::NoOP();
    folderLayer->Blend(&lvalue, &canvasRect, &pos, 1, &eventBlend);
    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(lvalue->Format());
    ctx.Finish();

    DeleteLayer(folderLayer);
    AddLayer(imageLayer, parent, index);
}

void FOdysseyLayerStack::DuplicateLayer(TSharedPtr<IOdysseyLayer> iLayer)
{
    TSharedPtr<IOdysseyLayer> parent = iLayer->GetParent();
    int index = iLayer->GetIndexInParent();

    TSharedPtr<IOdysseyLayer> clone = MakeShareable(iLayer->Clone());
    clone->SetName(FName(*(iLayer->GetName().ToString() + FString("_Copy"))));
    AddLayer(clone, parent, index + 1);

    //Clone at the end so that every callbacks are called when changing current layer before duplicating
    IOdysseyLayer::CloneChildren(iLayer, clone);
}

void
FOdysseyLayerStack::ClearCurrentLayer()
{
    if(mCurrentLayer->GetType() == FOdysseyImageLayer::eType::kImage)
    {
        TSharedPtr<FOdysseyImageLayer> imageLayer = StaticCastSharedPtr<FOdysseyImageLayer>(mCurrentLayer);

        ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(imageLayer->GetBlock()->Format());
        ctx.Clear( *(imageLayer->GetBlock()->GetBlock()) );
        ctx.Finish();

        imageLayer->ImageResultChangedDelegate().Broadcast( nullptr, 0 );
    }
}

void
FOdysseyLayerStack::FillCurrentLayerWithColor(const ::ULIS::ISample& iColor)
{
    if(mCurrentLayer->GetType() == FOdysseyImageLayer::eType::kImage)
    {
        TSharedPtr<FOdysseyImageLayer> imageLayer = StaticCastSharedPtr<FOdysseyImageLayer>(mCurrentLayer);

        ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(imageLayer->GetBlock()->Format());
        ctx.Fill( *(imageLayer->GetBlock()->GetBlock()), iColor );
        ctx.Finish();

        imageLayer->ImageResultChangedDelegate().Broadcast( nullptr, 0 );
    }
}

//--------------------------------------------------------------------------------------
//-------------------------------------------------------------------------- Private API

void
FOdysseyLayerStack::OnLayerRootImageResultChanged( const ::ULIS::FRectI* iRects, const uint32 iNumRects )
{
    if( iRects && iNumRects ) {
        mOnImageResultChanged.Broadcast( iRects, iNumRects );
    } else {
        ::ULIS::FRectI rect( 0, 0, Width(), Height() );
        mOnImageResultChanged.Broadcast( &rect, 1 );
    }
}

void
FOdysseyLayerStack::OnLayerAdded(TSharedPtr<IOdysseyLayer> iNode)
{
    mOnStructureChanged.Broadcast();
}

void
FOdysseyLayerStack::OnLayerRemoved(TSharedPtr<IOdysseyLayer> iNode, TSharedPtr<IOdysseyLayer> iOldParent, int iOldIndex)
{
    mOnStructureChanged.Broadcast();
}

// Custom serialization version for FOdysseyLayerStack
struct FOdysseyLayerStackObjectVersion
{
    enum Type
    {
        // Before any version changes were made
        SavePixelFormat,
        
        VersionPlusOne,
        LatestVersion = VersionPlusOne - 1
    };

    // The GUID for this custom version number
    const static FGuid GUID;

private:
    FOdysseyLayerStackObjectVersion() {}
};

const FGuid FOdysseyLayerStackObjectVersion::GUID(0x09295A41, 0x4809652E, 0xBE684D8E, 0x5BBAC5E0);
//FCustomVersionRegistration FOdysseyLayerStackObjectVersionRegistration(FOdysseyLayerStackObjectVersion::GUID, FOdysseyLayerStackObjectVersion::LatestVersion, TEXT("FOdysseyLayerStackObjectVersion::SavePixelFormat"));

FArchive& 
operator<<(FArchive &Ar, FOdysseyLayerStack* ioSaveLayerStack )
{
    if( !ioSaveLayerStack )
        return Ar;

    //Set the Object Version
    //Ar.UsingCustomVersion(FOdysseyLayerStackObjectVersion::GUID);

    Ar << ioSaveLayerStack->mWidth;
    Ar << ioSaveLayerStack->mHeight;

    //WARNING: Here loading the root layer is not creating a new pointer for it in operator <<
    // This is a special behaviour only for the root layer
    TSharedPtr<IOdysseyLayer> layer = ioSaveLayerStack->mLayerRoot;
    Ar << (layer);

    //if( Ar.CustomVer(FOdysseyLayerStackObjectVersion::GUID) >= FOdysseyLayerStackObjectVersion::SavePixelFormat )
    //{
        uint32 fmt = static_cast< uint32 >( ioSaveLayerStack->mOutputFormat );
        Ar << fmt;

        // Hotfix to replace profile code in older texture files
        if( ULIS_R_PROFILE( fmt ) == ULIS_None )
            fmt = (fmt & ULIS_E_PROFILE) | ULIS_W_PROFILE( ::ULIS::FFormatMetrics::DefaultProfileCodeForColorModel( static_cast< ::ULIS::eColorModel >( ULIS_R_MODEL( fmt ) ) ) );

        ioSaveLayerStack->mOutputFormat = static_cast< ULIS::eFormat >( fmt );
    //}
    /*else
    {
        ioSaveLayerStack->mOutputFormat = ::ULIS::Format_BGRA8;
    }*/

    if (Ar.IsLoading())
    {
        ioSaveLayerStack->Init(ioSaveLayerStack->mWidth, ioSaveLayerStack->mHeight, ioSaveLayerStack->mOutputFormat);
        //Set the current layer to the first image layer
        TArray< TSharedPtr<IOdysseyLayer> > layers;
        layer->DepthFirstSearchTree(&layers, false);
        for (int i = 0; i < layers.Num(); i++)
        {
            if (layers[i]->GetType() == IOdysseyLayer::eType::kImage)
            {
                ioSaveLayerStack->SetCurrentLayer(layers[i]);
                break;
            }
        }
    }
    
    return Ar;
}

//---

//FODysseyDrawingUndo ---------
FOdysseyDrawingUndo::FOdysseyDrawingUndo(FOdysseyLayerStack* iLayerStack)
{
    mLayerStackPtr = iLayerStack;

    // mData = TArray<uint8>();
    // FOdysseyBlock* tmp = new FOdysseyBlock(iLayerStack->Width(), iLayerStack->Height(), iLayerStack->Format());
    mCurrentIndex = 0;

    //We reserve the maximum memory needed for a undo

    //mData.Reserve(tmp->GetBlock()->BytesTotal());
    //delete tmp;

    static int numberUndoStack = 1;

    mUndoPath = FPaths::Combine(FPaths::EngineSavedDir(),*FString( FString::FromInt(numberUndoStack) + "undos.save"));
    mRedoPath = FPaths::Combine(FPaths::EngineSavedDir(),*FString( FString::FromInt(numberUndoStack) + "redos.save"));
    numberUndoStack++;

    Clear();
}

FOdysseyDrawingUndo::~FOdysseyDrawingUndo()
{
    //delete mData;
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
    }
    else if(mCurrentIndex < mUndosPositions.Num() - 1)
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
    TArray< TSharedPtr<IOdysseyLayer> > layers;
    mLayerStackPtr->GetLayerRoot()->DepthFirstSearchTree(&layers,false);

    TSharedPtr<FOdysseyImageLayer> imageLayer = nullptr;
    for(int j = 0; j < layers.Num(); j++)
    {
        if(iAddress == (UPTRINT)(layers[j].Get()))
        {
            imageLayer = StaticCastSharedPtr<FOdysseyImageLayer> (layers[j]);
            break;
        }
    }

    if(imageLayer == nullptr)
        return false;

    UPTRINT address = (UPTRINT)imageLayer.Get();
    mToBinary << address;
    mToBinary << iXTile;
    mToBinary << iYTile;
    mToBinary << iSizeX;
    mToBinary << iSizeY;

    if(iXTile >= 0 && iYTile >= 0 && iSizeX > 0 && iSizeY > 0)
    {
        FOdysseyBlock* tileBlock = new FOdysseyBlock(iSizeX, iSizeY, imageLayer->GetBlock()->Format());

        ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext( imageLayer->GetBlock()->Format() );
        ctx.Copy( *(imageLayer->GetBlock()->GetBlock()), *(tileBlock->GetBlock()), ::ULIS::FRectI(iXTile, iYTile, iSizeX, iSizeY) );
        ctx.Finish();

        mToBinary << tileBlock->GetArray();

        delete tileBlock;
    }

    mNumberBlocksRedo[mCurrentIndex]++;

    return true;
}

bool
FOdysseyDrawingUndo::SaveData(const TArray<::ULIS::FRectI>& iRects)
{
    if (mLayerStackPtr->GetCurrentLayer()->GetType() != IOdysseyLayer::eType::kImage)
        return false;

    TSharedPtr<FOdysseyImageLayer> imageLayer = StaticCastSharedPtr<FOdysseyImageLayer>(mLayerStackPtr->GetCurrentLayer());
    UPTRINT address = (UPTRINT)imageLayer.Get();
    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(imageLayer->GetBlock()->Format());

    TArray<FOdysseyBlock*> tileBlocks;
    for (int i = 0; i < iRects.Num(); i++)
    {
        ::ULIS::FRectI rect = iRects[i];
        FOdysseyBlock* tileBlock = new FOdysseyBlock(rect.w, rect.h, imageLayer->GetBlock()->Format());
        tileBlocks.Add(tileBlock);

        if (rect.x >= 0 && rect.y >= 0 && rect.w > 0 && rect.h > 0)
        {
            ctx.Copy(*(imageLayer->GetBlock()->GetBlock()), *(tileBlock->GetBlock()), rect, ::ULIS::FVec2I(0,0), ::ULIS::FSchedulePolicy::MonoScanlines );
            ctx.Flush();
        }
    }

    ctx.Finish();

    for (int i = 0; i < iRects.Num(); i++)
    {
        ::ULIS::FRectI rect = iRects[i];
        mToBinary << address;
        mToBinary << rect.x;
        mToBinary << rect.y;
        mToBinary << rect.w;
        mToBinary << rect.h;

        if (rect.x >= 0 && rect.y >= 0 && rect.w > 0 && rect.h > 0)
        {
            mToBinary << tileBlocks[i]->GetArray();
            delete tileBlocks[i];
        }

        mNumberBlocksUndo[mCurrentIndex]++;

    }

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

    TSharedPtr<FOdysseyImageLayer> imageLayer = nullptr;
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
        
        if (tileX >= 0 && tileY >= 0 && sizeX > 0 && sizeY > 0)
        {
            TArray< TSharedPtr<IOdysseyLayer> > layers;
            mLayerStackPtr->GetLayerRoot()->DepthFirstSearchTree(&layers, false);

            for (int j = 0; j < layers.Num(); j++)
            {
                if (address == (UPTRINT)(layers[j].Get()))
                {
                    imageLayer = StaticCastSharedPtr<FOdysseyImageLayer>(layers[j]);
                    break;
                }
            }

            if (imageLayer == nullptr)
                return false;

            FOdysseyBlock* tileBlock = new FOdysseyBlock(sizeX, sizeY, imageLayer->GetBlock()->Format());
            Ar << tileBlock->GetArray();

            ::ULIS::FRectI tileRect(tileX, tileY, sizeX, sizeY);

            ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(imageLayer->GetBlock()->Format());
            ctx.Copy(*(tileBlock->GetBlock()), *(imageLayer->GetBlock()->GetBlock()), ::ULIS::FRectI(0, 0, sizeX, sizeY), ::ULIS::FVec2I(tileX, tileY) );
            ctx.Finish();

            imageLayer->GetBlock()->GetBlock()->Dirty(tileRect);
            delete tileBlock;
        }
    }

    if(bSaveForRedo)
    {
        EndRecordRedo();
    }

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

    TSharedPtr<FOdysseyImageLayer> imageLayer = nullptr;
    for(int i = 0; i < mNumberBlocksRedo[mCurrentIndex]; i++)
    {
        Ar << address;
        Ar << tileX;
        Ar << tileY;
        Ar << sizeX;
        Ar << sizeY;

        if(tileX >= 0 && tileY >= 0 && sizeX > 0 && sizeY > 0)
        {
            TArray< TSharedPtr<IOdysseyLayer> > layers;
            mLayerStackPtr->GetLayerRoot()->DepthFirstSearchTree(&layers,false);

            for(int j = 0; j < layers.Num(); j++)
            {
                if(address == (UPTRINT)(layers[j].Get()))
                {
                    imageLayer = StaticCastSharedPtr<FOdysseyImageLayer> (layers[j]);
                    break;
                }
            }

            if(imageLayer == nullptr)
                return false;

            FOdysseyBlock* tileBlock = new FOdysseyBlock(sizeX, sizeY, imageLayer->GetBlock()->Format());
            Ar << tileBlock->GetArray();

            ::ULIS::FRectI tileRect(tileX, tileY, sizeX, sizeY);

            ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(imageLayer->GetBlock()->Format());
            ctx.Copy(*(tileBlock->GetBlock()), *(imageLayer->GetBlock()->GetBlock()), ::ULIS::FRectI(0, 0, sizeX, sizeY), ::ULIS::FVec2I(tileX, tileY));
            ctx.Finish();

            imageLayer->GetBlock()->GetBlock()->Dirty(tileRect);
            delete tileBlock;
        }
    }

    if(mCurrentIndex < (mUndosPositions.Num() - 1))
        mCurrentIndex++;

    return true;
}

bool
FOdysseyDrawingUndo::HasUndo()
{
    return mCurrentIndex > 0;
}

bool
FOdysseyDrawingUndo::HasRedo()
{
    return mCurrentIndex < mUndosPositions.Num() - 1;
}

#undef LOCTEXT_NAMESPACE

