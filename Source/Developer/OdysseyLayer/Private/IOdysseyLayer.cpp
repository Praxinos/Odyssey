// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#include "IOdysseyLayer.h"
#include "OdysseyFolderLayer.h"
#include "OdysseyImageLayer.h"

//---

IOdysseyLayer::~IOdysseyLayer()
{
}

IOdysseyLayer::IOdysseyLayer( const IOdysseyLayer& iLayer )
    : mName(iLayer.mName)
    , mIsLocked( iLayer.mIsLocked )
    , mIsVisible( iLayer.mIsVisible )
    , mType( iLayer.mType )
{
    TArray<TSharedPtr<IOdysseyLayer>> children = GetNodes();
    for(int i = 0; i < children.Num(); i++)
    {
        TSharedPtr<IOdysseyLayer> layer = MakeShareable(children[i]->Clone());
        AddNode(layer);
    }
}

IOdysseyLayer::IOdysseyLayer( const eType type )
    : mName()
    , mIsLocked( false )
    , mIsVisible( true )
    , mType( type )
{
}

IOdysseyLayer::IOdysseyLayer( const FName& iName, const eType type )
    : mName( iName )
    , mIsLocked( false )
    , mIsVisible( true )
    , mType( type )
{
}

//---

IOdysseyLayer::eType
IOdysseyLayer::GetType() const
{
    return mType;
}


FName
IOdysseyLayer::GetName() const
{
    return mName;
}

FText
IOdysseyLayer::GetNameAsText() const
{
    return FText::FromName( mName );
}

void
IOdysseyLayer::SetName( FName iName )
{
    FName oldValue = mName;
    mName = iName;
	mNameChangedDelegate.Broadcast(oldValue);
}

bool
IOdysseyLayer::IsLocked(bool iCheckParent) const
{
	if (!iCheckParent)
		return mIsLocked;

	TSharedPtr<IOdysseyLayer> layer = GetParent();
	while (layer)
	{
		if (!layer->IsLocked())
			return false;
	}
	return true;
}

void
IOdysseyLayer::SetIsLocked( bool iIsLocked )
{
    bool oldValue = mIsLocked;
    mIsLocked = iIsLocked;
    mLockChangedDelegate.Broadcast(oldValue);
}

bool
IOdysseyLayer::IsVisible(bool iCheckParent) const
{
	if (!iCheckParent)
		return mIsVisible;

	TSharedPtr<IOdysseyLayer> layer = GetParent();
	while (layer)
	{
		if (!layer->IsVisible())
			return false;
	}
	return true;
}

void
IOdysseyLayer::SetIsVisible( bool iIsVisible )
{
    bool oldValue = mIsLocked;
    mIsVisible = iIsVisible;
    mVisibilityChangedDelegate.Broadcast(oldValue);
}

FName
IOdysseyLayer::GetNextLayerName()
{
	TArray<TSharedPtr<IOdysseyLayer>> layers;
	DepthFirstSearchTree(&layers);
    return FName(*(FString("Layer ") + FString::FromInt(layers.Num())));
}

void
IOdysseyLayer::Serialize(FArchive &Ar)
{
    if (mType != IOdysseyLayer::eType::kRoot)
    {
        Ar << mName;
        Ar << mIsLocked;
        Ar << mIsVisible;
    }
}

void
IOdysseyLayer::SerializeWithChildren(FArchive &Ar)
{
    Serialize(Ar);

    if (Ar.IsSaving())
    {   
        TArray<TSharedPtr<IOdysseyLayer>> children = GetNodes();
        int numNodes = children.Num();
        Ar << numNodes;
        for (int i = 0; i < numNodes; i++)
        {
			IOdysseyLayer* child = children[i].Get();
            Ar << child;
        }
    }
    else if (Ar.IsLoading())
    {
        int numNodes = 0;
        Ar << numNodes;
        for (int i = 0; i < numNodes; i++)
        {
            IOdysseyLayer* layer = nullptr;
            Ar << layer;
            AddNode(MakeShareable(layer));
        }
    }
}

FArchive&
operator<<(FArchive &Ar, IOdysseyLayer*& ioLayer )
{
    //ES: For compatibility reasons
    if (ioLayer && ioLayer->mType == IOdysseyLayer::eType::kRoot )
    {
        ioLayer->SerializeWithChildren(Ar);
        return Ar;
    }

    if( Ar.IsSaving() )
    {
        Ar << ioLayer->mType;
        ioLayer->SerializeWithChildren(Ar);
    }
    else if( Ar.IsLoading() )
    {
        IOdysseyLayer::eType layerType;
        Ar << layerType;
        switch(layerType)
        {
            case IOdysseyLayer::eType::kImage :
                ioLayer = new FOdysseyImageLayer( FName(), NULL );
            break;

            case IOdysseyLayer::eType::kFolder :
                ioLayer = new FOdysseyFolderLayer( FName() );
            break;
        }
        ioLayer->SerializeWithChildren(Ar);
    }
    return Ar;
}

IOdysseyLayer::FOdysseyLayerNameChanged&
IOdysseyLayer::NameChangedDelegate()
{
    return mNameChangedDelegate;
}

IOdysseyLayer::FOdysseyLayerLockChanged&
IOdysseyLayer::LockChangedDelegate()
{
    return mLockChangedDelegate;
}

IOdysseyLayer::FOdysseyLayerVisibilityChanged&
IOdysseyLayer::VisibilityChangedDelegate()
{
    return mVisibilityChangedDelegate;
}
