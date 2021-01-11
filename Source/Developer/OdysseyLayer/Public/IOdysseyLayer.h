// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"
#include "OdysseyTreeShared.h"

class FOdysseyBlock;

/**
 * Odyssey Layer
 * An abstract class for a layer, which can be of various types (drawing, sound, folder...)
 */
class ODYSSEYLAYER_API IOdysseyLayer : public FOdysseyNTreeShared<IOdysseyLayer>
{
public:
    // Layer Name Changed Event
    // FName is for the previous value
    DECLARE_MULTICAST_DELEGATE_OneParam(FOdysseyLayerNameChanged, FName);

    // Layer Locked Changed Event
    // Bool is for the previous value
    DECLARE_MULTICAST_DELEGATE_OneParam(FOdysseyLayerLockChanged, bool);

    // Layer Is Visible Changed Event
    // Bool is for the previous value
    DECLARE_MULTICAST_DELEGATE_OneParam(FOdysseyLayerVisibilityChanged, bool);

    // Layer Child Is Locked Changed Event
    // TSharedPtr<IOdysseyLayer> is the child that changed
    // Bool is for the previous value
    DECLARE_MULTICAST_DELEGATE_TwoParams(FOdysseyLayerChildIsLockedChanged, TSharedPtr<IOdysseyLayer>, bool);

    // Layer Child Is Visible Changed Event
    // TSharedPtr<IOdysseyLayer> is the child that changed
    // Bool is for the previous value
    DECLARE_MULTICAST_DELEGATE_TwoParams(FOdysseyLayerChildIsVisibleChanged, TSharedPtr<IOdysseyLayer>, bool);

public:
    enum class eType : char
    {
        kInvalid,
        kImage,
        kFolder,
        kRoot,
    };

public:
    // Construction / Destruction
    virtual ~IOdysseyLayer() = 0;
    IOdysseyLayer( const eType type );
    IOdysseyLayer( const FName& iName, const eType type );

    virtual IOdysseyLayer* Clone() const = 0;
	static void CloneChildren(TSharedPtr<IOdysseyLayer> iSrc, TSharedPtr<IOdysseyLayer> ioDst);

protected:
    IOdysseyLayer( const IOdysseyLayer& iLayer );

public:
    // Public Getters / Setters
    virtual eType GetType() const;

    //Name
    virtual FName GetName() const;
    virtual FText GetNameAsText() const;
    virtual void  SetName( FName iName );

    //IsLocked
    virtual bool  IsLocked(bool iCheckParent = false) const;
    virtual void  SetIsLocked( bool iIsLocked );

    //IsVisible
    virtual bool  IsVisible(bool iCheckParent = false) const;
    virtual void  SetIsVisible( bool iIsVisible );

    virtual bool  ImplementsCapability(FGuid iGuid) const = 0;

    template< typename T > T* GetCapability()
    {
        void* value = GetCapabilityPtrFromGuid(T::GetGuid());
        return reinterpret_cast<T*>(value);
    }
    virtual void* GetCapabilityPtrFromGuid(FGuid iGuid) = 0;

protected:
    // Overloads for save in archive
    virtual void Serialize(FArchive &Ar) = 0;

private:
    void SerializeWithChildren(FArchive &Ar);

    friend ODYSSEYLAYER_API FArchive& operator<<(FArchive &Ar, TSharedPtr<IOdysseyLayer>& ioLayer );

public:
    FOdysseyLayerNameChanged& NameChangedDelegate();
    FOdysseyLayerLockChanged& LockChangedDelegate();
    FOdysseyLayerVisibilityChanged& VisibilityChangedDelegate();
    FOdysseyLayerChildIsLockedChanged& ChildIsLockedChangedDelegate();
    FOdysseyLayerChildIsVisibleChanged& ChildIsVisibleChangedDelegate();

public:
    FName GetNextLayerName();

protected:
    FName         mName;
    bool          mIsLocked;
    bool          mIsVisible;
    eType         mType;

	FOdysseyLayerNameChanged mNameChangedDelegate;
	FOdysseyLayerLockChanged mLockChangedDelegate;
	FOdysseyLayerVisibilityChanged mVisibilityChangedDelegate;
	FOdysseyLayerChildIsLockedChanged mChildIsLockedChangedDelegate;
    FOdysseyLayerChildIsVisibleChanged mChildIsVisibleChangedDelegate;
};

ODYSSEYLAYER_API FArchive& operator<<(FArchive &Ar, TSharedPtr<IOdysseyLayer>& ioSaveImageLayer );