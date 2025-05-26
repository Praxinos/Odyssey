// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "OdysseyMediaProvider.h"

namespace ULIS { class FBlock; }
class UOdysseyLayerStack;

class FOdysseyPainterEditor;

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
    virtual void Activate(FOdysseyPainterEditor* iEditor);
    virtual void Inactivate();
    bool IsActivated() const;

public:
    // Getters
    virtual const FGuid& Id() const = 0;
    virtual int Width() const = 0;
    virtual int Height() const  = 0;
    virtual UTexture* DisplayTexture() const = 0;
    virtual FOdysseyMediaProvider GetCurrentMediaProvider() = 0;
    virtual UOdysseyLayerStack* GetLayerStack() const = 0;

    // Utility functions
    virtual void Clear() = 0;
    virtual void ClearFromCopyBlock(TSharedPtr<::ULIS::FBlock> iBlock) = 0; //Clears the current layer of all the pixels contained in iBlock: basically a blend with alpha = sub
    virtual void PasteBlockToCurrentLayer(TSharedPtr<::ULIS::FBlock> iBlock) = 0;
    virtual void PasteBlockToNewLayer( TSharedPtr<::ULIS::FBlock> iCopyBlock ) = 0;

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
