// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "UObject/GCObject.h"

#include "OdysseyMediaProvider.h"

namespace ULIS { class FBlock; }
class UOdysseyLayerStack;
class FOdysseyPainterEditor;
class UTexture;

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
    /*
     * The blocks of the clipboard functions are canvas-sized, in any format: they are converted to the format of the owner.
     * Each function is undoable (it opens its own transaction).
     * TODO(GPU-tools): CPU blocks, to replace with render targets once the selection mask is on the GPU.
     */

    /** Clears from the current layer the pixels the block holds: a blend whose alpha is subtracted. */
    virtual void ClearFromCopyBlock(TSharedPtr<::ULIS::FBlock> iBlock) = 0;
    /** Blends the block over the current layer, or the cell of the current frame. */
    virtual void PasteBlockToCurrentLayer(TSharedPtr<::ULIS::FBlock> iBlock) = 0;
    /** Adds a raster layer holding the block (UOdysseyLayerStack::AddLayer) and makes it current. */
    virtual void PasteBlockToNewLayer( TSharedPtr<::ULIS::FBlock> iCopyBlock ) = 0;
    virtual bool IsValid() const = 0;

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
