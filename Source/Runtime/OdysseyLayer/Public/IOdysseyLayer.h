// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#pragma once

#include "CoreMinimal.h"

class FOdysseyBlock;
class FOdysseyLayerStack;

/**
 * Odyssey Layer
 * An abstract class for a layer, which can be of various types (drawing, sound, folder...)
 */
class ODYSSEYLAYER_API IOdysseyLayer
{
public:
    enum class eType : char
    {
        kInvalid,
        kImage,
        kFolder,
    };

public:
    // Construction / Destruction
    virtual ~IOdysseyLayer() = 0;
    IOdysseyLayer( const FOdysseyLayerStack* iParentStack, const eType type );
    IOdysseyLayer( const FOdysseyLayerStack* iParentStack, const FName& iName, const eType type );

public:
    virtual eType GetType() const;

    virtual FName GetName() const;
    virtual FText GetNameAsText() const;
    virtual void  SetName( FName iName );

    virtual bool  IsLocked() const;
    virtual void  SetIsLocked( bool iIsLocked );

    virtual bool  IsVisible() const;
    virtual void  SetIsVisible( bool iIsVisible );

    const FOdysseyLayerStack* GetParentStack() const;

    // Overloads for save in archive
    friend FArchive& operator<<(FArchive &Ar, IOdysseyLayer** ioSaveImageLayer );

protected:
    FName         mName;
    bool          mIsLocked;
    bool          mIsVisible;
    eType         mType;
    const FOdysseyLayerStack* mParentStack;
};


#include "OdysseyImageLayer.h"
#include "OdysseyFolderLayer.h"
inline FArchive& operator<<(FArchive &Ar, IOdysseyLayer** ioSaveLayer )
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
            (*ioSaveLayer) = new FOdysseyImageLayer(FName(), NULL );
            Ar << static_cast<FOdysseyImageLayer*>(*ioSaveLayer);
        }
        else if( layerType == IOdysseyLayer::eType::kFolder )
        {
            (*ioSaveLayer) = new FOdysseyFolderLayer( FName() );
            Ar << static_cast<FOdysseyFolderLayer*>(*ioSaveLayer);
        }
    }

    return Ar;
}
