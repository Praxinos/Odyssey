// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "OdysseyMediaProvider.h"

namespace ULIS { class FBlock; }
class UOdysseyLayerStack;

/**
 * Base class for an Editor Source
 */
class ODYSSEYPAINTEREDITOR_API FOdysseyPainterEditorSource
    : public FGCObject //Allows us to register External UObject in Garbage Collector
{
public:
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnAddEditedObject, UObject*);
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnRemoveEditedObject, UObject*);
    
public:
    // Construction / Destruction
    virtual ~FOdysseyPainterEditorSource();
    FOdysseyPainterEditorSource();

public:
    virtual void Activate();
    virtual void Inactivate();
    bool IsActivated() const;

public:
    // Getters
    virtual const FGuid& Id() const = 0;
    virtual int Width() const = 0;
    virtual int Height() const  = 0;
    virtual UTexture* DisplayTexture() const = 0;
    virtual TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> GetDisplayBlock() = 0;
    virtual FOdysseyMediaProvider GetCurrentMediaProvider() = 0;
    virtual UOdysseyLayerStack* GetLayerStack() const = 0;

    // Utility functions
    virtual void Clear() = 0;
    virtual void PasteBlockToNewLayer( TSharedPtr<::ULIS::FBlock> iBlock ) = 0;

    FOnAddEditedObject& OnAddEditedObjectDelegate();
    FOnRemoveEditedObject& OnRemoveEditedObjectDelegate();
    const TArray<UObject*>& GetEditedObjects() const;
    
    virtual void AddReferencedObjects(FReferenceCollector& Collector);
    virtual FString GetReferencerName() const;

    virtual void RecordCurrentFrameUndo() const;

protected:
    // Interface
    void AddEditedObject(UObject* iObject);
    void RemoveEditedObject(UObject* iObject);

public:
    bool mIsActivated;
    TArray<UObject*> mEditedObjects;

    FOnAddEditedObject mOnAddEditedObject;
    FOnRemoveEditedObject mOnRemoveEditedObject;
};
