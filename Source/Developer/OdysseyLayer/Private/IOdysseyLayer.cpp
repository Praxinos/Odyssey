// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

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

void
IOdysseyLayer::CloneChildren(TSharedPtr<IOdysseyLayer> iSrc, TSharedPtr<IOdysseyLayer> ioDst)
{
	TArray<TSharedPtr<IOdysseyLayer>> layers;
	TArray<TSharedPtr<IOdysseyLayer>> children = iSrc->GetNodes();
	for (int i = 0; i < children.Num(); i++)
	{
		TSharedPtr<IOdysseyLayer> layer = MakeShareable(children[i]->Clone());
		CloneChildren(children[i], layer);
		ioDst->AddNode(layer);
	}
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
	if (!iCheckParent || mIsLocked)
		return mIsLocked;

	TSharedPtr<IOdysseyLayer> layer = GetParent();
	while (layer)
	{
		if (layer->IsLocked(true))
			return true;

		layer = layer->GetParent();
	}
	return false;
}

void
IOdysseyLayer::SetIsLocked( bool iIsLocked )
{
    bool oldValue = mIsLocked;
    mIsLocked = iIsLocked;
    mLockChangedDelegate.Broadcast(oldValue);

    TSharedPtr<IOdysseyLayer> layer = GetParent();
	TSharedPtr<IOdysseyLayer> self = SharedThis(this);
	while (layer)
	{
		layer->ChildIsLockedChangedDelegate().Broadcast(self, oldValue);
		layer = layer->GetParent();
	}
}

bool
IOdysseyLayer::IsVisible(bool iCheckParent) const
{
	if (!iCheckParent || !mIsVisible)
		return mIsVisible;

	TSharedPtr<IOdysseyLayer> layer = GetParent();
	while (layer)
	{
		if (!layer->IsVisible(true))
			return false;

		layer = layer->GetParent();
	}
	return true;
}

void
IOdysseyLayer::SetIsVisible( bool iIsVisible )
{
    bool oldValue = mIsVisible;
    mIsVisible = iIsVisible;
    mVisibilityChangedDelegate.Broadcast(oldValue);

    TSharedPtr<IOdysseyLayer> layer = GetParent();
    TSharedPtr<IOdysseyLayer> self = SharedThis(this);
    while (layer)
    {
        layer->ChildIsVisibleChangedDelegate().Broadcast(self, oldValue);
        layer = layer->GetParent();
    }
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
            Ar << children[i];
        }
    }
    else if (Ar.IsLoading())
    {
        int numNodes = 0;
        Ar << numNodes;
        for (int i = 0; i < numNodes; i++)
        {
            TSharedPtr<IOdysseyLayer> layer;
            Ar << layer;
            AddNode(layer);
        }
    }
}

FArchive&
operator<<(FArchive &Ar, TSharedPtr<IOdysseyLayer>& ioLayer )
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
                ioLayer = MakeShareable(new FOdysseyImageLayer( FName(), NULL ));
            break;

            case IOdysseyLayer::eType::kFolder :
                ioLayer = MakeShareable(new FOdysseyFolderLayer( FName() ));
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

IOdysseyLayer::FOdysseyLayerChildIsLockedChanged&
IOdysseyLayer::ChildIsLockedChangedDelegate()
{
    return mChildIsLockedChangedDelegate;
}

IOdysseyLayer::FOdysseyLayerChildIsVisibleChanged&
IOdysseyLayer::ChildIsVisibleChangedDelegate()
{
    return mChildIsVisibleChangedDelegate;
}
