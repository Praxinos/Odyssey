// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyPainterEditorSource.h"

FOdysseyPainterEditorSource::~FOdysseyPainterEditorSource()
{

}

FOdysseyPainterEditorSource::FOdysseyPainterEditorSource()
    : mIsActivated(false)
{

}

void
FOdysseyPainterEditorSource::Activate(FOdysseyPainterEditor* iEditor)
{
    mIsActivated = true;
}

void
FOdysseyPainterEditorSource::Inactivate()
{
    mIsActivated = false;
}

bool
FOdysseyPainterEditorSource::IsActivated() const
{
    return mIsActivated;
}

FOdysseyPainterEditorSource::FOnAddEditedObject&
FOdysseyPainterEditorSource::OnAddEditedObjectDelegate()
{
    return mOnAddEditedObject;
}

FOdysseyPainterEditorSource::FOnRemoveEditedObject&
FOdysseyPainterEditorSource::OnRemoveEditedObjectDelegate()
{
    return mOnRemoveEditedObject;
}

const TArray<UObject*>&
FOdysseyPainterEditorSource::GetEditedObjects() const
{
    return mEditedObjects;
}

void
FOdysseyPainterEditorSource::AddEditedObject(UObject* iObject)
{
    mEditedObjects.Add(iObject);
    mOnAddEditedObject.Broadcast(iObject);
}

void
FOdysseyPainterEditorSource::RemoveEditedObject(UObject* iObject)
{
    mEditedObjects.Remove(iObject);
    mOnRemoveEditedObject.Broadcast(iObject);
}

void
FOdysseyPainterEditorSource::AddReferencedObjects(FReferenceCollector& Collector)
{

}

FString
FOdysseyPainterEditorSource::GetReferencerName() const
{
    return "FOdysseyPainterEditorSource";
}

void
FOdysseyPainterEditorSource::RecordCurrentFrameUndo() const
{
}
