// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#include "IOdysseyLayer.h"
#include "OdysseyFolderLayer.h"
#include "OdysseyImageLayer.h"

//---

IOdysseyLayer::~IOdysseyLayer()
{
}

IOdysseyLayer::IOdysseyLayer( const FOdysseyLayerStack* iParentStack, const eType type )
    : mName()
    , mIsLocked( false )
    , mIsVisible( true )
    , mType( type )
    , mParentStack( iParentStack )
{
}

IOdysseyLayer::IOdysseyLayer( const FOdysseyLayerStack* iParentStack, const FName& iName, const eType type )
    : mName( iName )
    , mIsLocked( false )
    , mIsVisible( true )
    , mType( type )
    , mParentStack( iParentStack )
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
    mName = iName;
}

bool
IOdysseyLayer::IsLocked() const
{
    return mIsLocked;
}

void
IOdysseyLayer::SetIsLocked( bool iIsLocked )
{
    mIsLocked = iIsLocked;
}

bool
IOdysseyLayer::IsVisible() const
{
    return mIsVisible;
}

void
IOdysseyLayer::SetIsVisible( bool iIsVisible )
{
    mIsVisible = iIsVisible;
}

const FOdysseyLayerStack*
IOdysseyLayer::GetParentStack() const {
    return  mParentStack;
}

FArchive& 
operator<<(FArchive &Ar, IOdysseyLayer** ioSaveLayer )
{
    if(!ioSaveLayer) return Ar;

    if( Ar.IsSaving() )
    {
        if(!(*ioSaveLayer)) return Ar; //We ignore the root, which is always NULL

        Ar << (*ioSaveLayer)->mType;

        if( (*ioSaveLayer)->GetType() == IOdysseyLayer::eType::kImage )
        {
            FOdysseyImageLayer* imageLayer = static_cast<FOdysseyImageLayer*> (*ioSaveLayer);
            Ar << imageLayer;
        }
        else if( (*ioSaveLayer)->GetType() == IOdysseyLayer::eType::kFolder )
        {
            FOdysseyFolderLayer* folderLayer = static_cast<FOdysseyFolderLayer*> (*ioSaveLayer);
            Ar << folderLayer;
        }
    }
    else if( Ar.IsLoading() )
    {
        IOdysseyLayer::eType layerType;
        Ar << layerType;
        
        if( layerType == IOdysseyLayer::eType::kImage )
        {
            (*ioSaveLayer) = new FOdysseyImageLayer( (*ioSaveLayer)->GetParentStack(), FName(), NULL );
            Ar << static_cast<FOdysseyImageLayer*>(*ioSaveLayer);
        }
        else if( layerType == IOdysseyLayer::eType::kFolder )
        {
            (*ioSaveLayer) = new FOdysseyFolderLayer( (*ioSaveLayer)->GetParentStack(), FName() );
            Ar << static_cast<FOdysseyFolderLayer*>(*ioSaveLayer);
        }
    }

    return Ar;
}
