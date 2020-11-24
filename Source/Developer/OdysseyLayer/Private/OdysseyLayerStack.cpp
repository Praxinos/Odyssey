// Copyright © 2018-2020 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000
#include "OdysseyLayerStack.h"
#include "HAL/PlatformFilemanager.h"
#include "Misc/FileHelper.h"
#include "UObject/UObjectGlobals.h"
#include "OdysseyImageLayer.h"
#include "OdysseyFolderLayer.h"
#include "OdysseyRootLayer.h"
#include "ULISLoaderModule.h"

#define LOCTEXT_NAMESPACE "OdysseyLayerStack"

::ul3::tFormat
ComputationFormatForResultFormat(::ul3::tFormat iFormat)
{
    return iFormat | ULIS3_W_ALPHA( 1 );
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
    , mFormat(0)
	, mOutputFormat(0)
    , mLayerRoot(MakeShareable(new  FOdysseyRootLayer()))
    , mCurrentLayer( mLayerRoot )
{
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------- Public API

void
FOdysseyLayerStack::Init(int iWidth,int iHeight, ::ul3::tFormat iFormat)
{
    mWidth = iWidth;
    mHeight = iHeight;
    mOutputFormat = iFormat;
	mFormat = ComputationFormatForResultFormat(iFormat);

    //TODO: Move outside one day
    mDrawingUndo = new  FOdysseyDrawingUndo( this );

	mLayerRoot->ImageResultChangedDelegate().RemoveAll(this); //in case Init is walled twice, which should not happen.... but actually happened once
	mLayerRoot->NodeAdded().RemoveAll(this); //in case Init is walled twice, which should not happen.... but actually happened once
	mLayerRoot->NodeRemoved().RemoveAll(this); //in case Init is walled twice, which should not happen.... but actually happened once

    mLayerRoot->ImageResultChangedDelegate().AddRaw(this, &FOdysseyLayerStack::OnLayerRootImageResultChanged);
    mLayerRoot->NodeAdded().AddRaw(this, &FOdysseyLayerStack::OnLayerAdded);
    mLayerRoot->NodeRemoved().AddRaw(this, &FOdysseyLayerStack::OnLayerRemoved);
}

void
FOdysseyLayerStack::ComputeResultInBlock( ::ul3::FBlock* ioBlock )
{
    ::ul3::FRect canvasRect = ::ul3::FRect( 0, 0, Width(), Height() );
    ComputeResultInBlock(ioBlock, canvasRect);
}

void ConvToPos(::ul3::FBlock* iSrc, ::ul3::FBlock* ioDst, const ::ul3::FVec2F& iPos)
{
	IULISLoaderModule& hULIS = IULISLoaderModule::Get();
	uint32 perfIntent = ULIS3_PERF_SSE42 | ULIS3_PERF_AVX2;

    ::ul3::FBlock* convDstBlock = ioDst;
    ::ul3::FBlock* convSrcBlock = iSrc;
    if (iSrc->Width() != ioDst->Width() || iSrc->Height() != ioDst->Height())
    {
	    convDstBlock = new ::ul3::FBlock(iSrc->Width(), iSrc->Height(), ioDst->Format());
    }

    if (iSrc->HasAlpha() && !ioDst->HasAlpha())
    {
        convSrcBlock = new ::ul3::FBlock(iSrc->Width(), iSrc->Height(), iSrc->Format());
        // ::ul3::Fill( hULIS.ThreadPool(), ULIS3_BLOCKING, perfIntent, hULIS.HostDeviceInfo(), ULIS3_NOCB, convSrcBlock, ::ul3::FPixelValue::FromGreyA8(0), convSrcBlock->Rect());
        

        //Use a copy and a Premultiply to accomplish the same behaviour as a Fill background with black if we keep only non alpha components
        ::ul3::Copy(hULIS.ThreadPool(), ULIS3_BLOCKING, perfIntent, hULIS.HostDeviceInfo(), ULIS3_NOCB, iSrc, convSrcBlock, iSrc->Rect(), ::ul3::FVec2F(0, 0));
        ::ul3::Premultiply(hULIS.ThreadPool(), ULIS3_BLOCKING, perfIntent, hULIS.HostDeviceInfo(), ULIS3_NOCB, convSrcBlock);
    }

	::ul3::Conv(hULIS.ThreadPool()
		, ULIS3_BLOCKING
		, perfIntent
		, hULIS.HostDeviceInfo()
		, ULIS3_NOCB
		, convSrcBlock
		, convDstBlock);

	::ul3::FRect rect = ::ul3::FRect::FromXYWH(0, 0, iSrc->Width(), iSrc->Height());
	::ul3::Copy(hULIS.ThreadPool()
		, ULIS3_BLOCKING
		, perfIntent
		, hULIS.HostDeviceInfo()
		, ULIS3_NOCB
		, convDstBlock
		, ioDst
		, rect
		, iPos);

    if (convDstBlock != ioDst)
        delete convDstBlock;
    
    if (convSrcBlock != iSrc)
        delete convSrcBlock;
}

void
FOdysseyLayerStack::ComputeResultInBlock( ::ul3::FBlock* ioBlock, const ::ul3::FRect& iRect ) 
{
	::ul3::FVec2F pos(iRect.x, iRect.y);
	::ul3::FVec2F convPos(iRect.x, iRect.y);
	::ul3::FVec2F outputPos(iRect.x, iRect.y);

	//Set the output block
	::ul3::FBlock* output = ioBlock;
	if (ioBlock->Format() != mOutputFormat)
	{
		output = new ::ul3::FBlock(iRect.w, iRect.h, mOutputFormat);
		outputPos.x = 0;
		outputPos.y = 0;
		convPos.x = 0;
		convPos.y = 0;
	}

	//convBlock
	::ul3::FBlock* convBlock = output;
	if (mFormat != mOutputFormat)
	{
		convBlock = new ::ul3::FBlock(iRect.w, iRect.h, mFormat);
		convPos.x = 0;
		convPos.y = 0;
	}

	mLayerRoot->RenderImage(convBlock, iRect, convPos);

	if (convBlock != output)
	{
		//Copy convblock (0, 0) to output (outputPos)
		ConvToPos(convBlock, output, outputPos);
		delete convBlock;
	}

	if (output != ioBlock)
	{
		//Copy output (0, 0) to ioBlock (pos)
		ConvToPos(output, ioBlock, pos);
		delete output;
	}
}

void
FOdysseyLayerStack::ComputeResultInBlockWithBlockAsCurrentLayer(::ul3::FBlock* ioBlock, FOdysseyBlock* iTempBlock, const ::ul3::FRect& iRect)
{
    if (!mCurrentLayer)
        return;

    IOdysseyLayer::eType type = mCurrentLayer->GetType();
    if (type != IOdysseyLayer::eType::kImage || !mCurrentLayer->IsVisible())
    {
        ComputeResultInBlock(ioBlock, iRect);
        return;
    }

    TSharedPtr<FOdysseyImageLayer> imageLayer = StaticCastSharedPtr<FOdysseyImageLayer>(mCurrentLayer);
    FOdysseyBlock* block = imageLayer->GetBlock();
    imageLayer->SetBlock(iTempBlock, false, false);
    ComputeResultInBlock(ioBlock, iRect);
    imageLayer->SetBlock(block, false, false);
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

int
FOdysseyLayerStack::Format() const
{
	return mFormat;
}

int
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

    ::ul3::FRect canvasRect = ::ul3::FRect( 0, 0, Width(), Height());
    ::ul3::FVec2F pos( 0, 0 );

    srcLayerImage->Blend(dstLayerImage->GetBlock()->GetBlock(), canvasRect, pos);
    dstLayerImage->ImageResultChangedDelegate().Broadcast();
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

    ::ul3::FRect canvasRect = ::ul3::FRect( 0, 0, Width(), Height());
    ::ul3::FVec2F pos( 0, 0 );
    folderLayer->Blend(imageLayer->GetBlock()->GetBlock(), canvasRect, pos);

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
        ::ul3::FRect canvasRect = ::ul3::FRect( 0, 0, Width(), Height());
        IULISLoaderModule& hULIS = IULISLoaderModule::Get();
        uint32 perfIntent = ULIS3_PERF_MT | ULIS3_PERF_SSE42 | ULIS3_PERF_AVX2;
        ::ul3::Clear( hULIS.ThreadPool(), ULIS3_BLOCKING, perfIntent, hULIS.HostDeviceInfo(), ULIS3_NOCB, imageLayer->GetBlock()->GetBlock(), canvasRect );
        imageLayer->ImageResultChangedDelegate().Broadcast();
    }
}

void
FOdysseyLayerStack::FillCurrentLayerWithColor(const ::ul3::IPixel& iColor)
{
    if(mCurrentLayer->GetType() == FOdysseyImageLayer::eType::kImage)
    {
        TSharedPtr<FOdysseyImageLayer> imageLayer = StaticCastSharedPtr<FOdysseyImageLayer>(mCurrentLayer);
        ::ul3::FRect canvasRect = ::ul3::FRect( 0, 0, Width(), Height());
        IULISLoaderModule& hULIS = IULISLoaderModule::Get();
        uint32 perfIntent = ULIS3_PERF_MT | ULIS3_PERF_SSE42 | ULIS3_PERF_AVX2;
        ::ul3::Fill( hULIS.ThreadPool(), ULIS3_BLOCKING, perfIntent, hULIS.HostDeviceInfo(), ULIS3_NOCB, imageLayer->GetBlock()->GetBlock(), iColor, canvasRect );
        imageLayer->ImageResultChangedDelegate().Broadcast();
    }
}

//--------------------------------------------------------------------------------------
//-------------------------------------------------------------------------- Private API

void
FOdysseyLayerStack::OnLayerRootImageResultChanged()
{
    mOnImageResultChanged.Broadcast();
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
FCustomVersionRegistration FOdysseyLayerStackObjectVersionRegistration(FOdysseyLayerStackObjectVersion::GUID, FOdysseyLayerStackObjectVersion::LatestVersion, TEXT("FOdysseyLayerStackObjectVersion::SavePixelFormat"));

FArchive& 
operator<<(FArchive &Ar, FOdysseyLayerStack* ioSaveLayerStack )
{
    if( !ioSaveLayerStack )
        return Ar;

	//Set the Object Version
	Ar.UsingCustomVersion(FOdysseyLayerStackObjectVersion::GUID);

    Ar << ioSaveLayerStack->mWidth;
    Ar << ioSaveLayerStack->mHeight;

    //WARNING: Here loading the root layer is not creating a new pointer for it in operator <<
	// This is a special behaviour only for the root layer
	TSharedPtr<IOdysseyLayer> layer = ioSaveLayerStack->mLayerRoot;
	Ar << (layer);
    
    if (Ar.CustomVer(FOdysseyLayerStackObjectVersion::GUID) >= FOdysseyLayerStackObjectVersion::SavePixelFormat)
    {
        Ar << ioSaveLayerStack->mOutputFormat;
    }
    else
    {
		ioSaveLayerStack->mOutputFormat = ULIS3_FORMAT_BGRA8;
    }

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

    mData = TArray<uint8>();
	FOdysseyBlock* tmp = new FOdysseyBlock(iLayerStack->Width(), iLayerStack->Height(), iLayerStack->Format());
    mCurrentIndex = 0;

    //We reserve the maximum memory needed for a undo

    mData.Reserve(tmp->GetBlock()->BytesTotal());
	delete tmp;

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

    UPTRINT address = (UPTRINT)imageLayer.Get();
    mToBinary << address;
    mToBinary << iXTile;
    mToBinary << iYTile;
    mToBinary << iSizeX;
    mToBinary << iSizeY;
    mToBinary << array;

    mNumberBlocksRedo[mCurrentIndex]++;

    // Why is mTileData not deleted here ??
    delete  mTileData;

    return true;
}

bool
FOdysseyDrawingUndo::SaveData(unsigned int iXTile, unsigned int iYTile,unsigned int iSizeX,unsigned int iSizeY)
{
    if(mLayerStackPtr->GetCurrentLayer()->GetType() != IOdysseyLayer::eType::kImage)
        return false;

    TSharedPtr<FOdysseyImageLayer> imageLayer = StaticCastSharedPtr<FOdysseyImageLayer>(mLayerStackPtr->GetCurrentLayer());
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

    UPTRINT address = (UPTRINT)imageLayer.Get();
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

        Ar << mData;

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
        //---

        //Useless, I just want mTileData at the right size for the next undo, to change
        IULISLoaderModule& hULIS = IULISLoaderModule::Get();
        uint32 perfIntent = ULIS3_PERF_MT | ULIS3_PERF_SSE42 | ULIS3_PERF_AVX2;
		mTileData = ::ul3::XCopy(hULIS.ThreadPool()
			, ULIS3_BLOCKING
			, perfIntent
			, hULIS.HostDeviceInfo()
			, ULIS3_NOCB
			, imageLayer->GetBlock()->GetBlock()
			, ::ul3::FRect(tileX, tileY, sizeX, sizeY));

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
            // mLayerStackPtr->ComputeResultBlock(::ul3::FRect(tileX,tileY,sizeX,sizeY));
        }

		delete mTileData;
    }

	if (imageLayer)
		imageLayer->ImageResultChangedDelegate().Broadcast();

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
        Ar << mData;

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
        }
    }

	if (imageLayer)
		imageLayer->ImageResultChangedDelegate().Broadcast();

    if(mCurrentIndex < (mUndosPositions.Num() - 1))
        mCurrentIndex++;

    if( mNumberBlocksRedo[mCurrentIndex] != 0 )
        delete mTileData;

    return true;
}

#undef LOCTEXT_NAMESPACE

