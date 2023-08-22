#include "Undo/OdysseyVectorUndoPropertyChanged.h"

FOdysseyVectorUndoPropertyChanged::~FOdysseyVectorUndoPropertyChanged()
{
    mPropertiesRecordArray.clear();
}

FOdysseyVectorUndoPropertyChanged::FOdysseyVectorUndoPropertyChanged( FOdysseyVectorScene* iScene, std::list<FOdysseyVectorObject*>& iObjectList )
    : FOdysseyVectorUndo( iScene )
{
    std::list<FOdysseyVectorObject*>::iterator it;
    int i;

    mPropertiesRecordArray.resize( iObjectList.size() );

    for( i = 0, it = iObjectList.begin(); it != iObjectList.end(); i++, ++it )
    {
        FOdysseyVectorObject* object = (*it);

        mPropertiesRecordArray[i].mObject = object;
        mPropertiesRecordArray[i].mObjectParam = object->mObjectParam;

        if( object->HasBaseClass( FOdysseyVectorPath::StaticClass() ) )
        {
            FOdysseyVectorPath* path = static_cast<FOdysseyVectorPath*>( object );

            mPropertiesRecordArray[i].mPathParam = path->mPathParam;
        }

        if( object->HasBaseClass( FOdysseyVectorGroupPaint::StaticClass() ) )
        {
            FOdysseyVectorGroupPaint* paintGroup = static_cast<FOdysseyVectorGroupPaint*>( object );

            mPropertiesRecordArray[i].mGroupPaintParam = paintGroup->mGroupPaintParam;
        }
    }
}

void
FOdysseyVectorUndoPropertyChanged::SwapParam()
{
    for( int i = 0; i < mPropertiesRecordArray.size(); i++ )
    {
        FOdysseyVectorObject* object = mPropertiesRecordArray[i].mObject;
        FObjectParam objectParamSwap = mPropertiesRecordArray[i].mObjectParam;

        object->mObjectParam = mPropertiesRecordArray[i].mObjectParam;

        mPropertiesRecordArray[i].mObjectParam = objectParamSwap;

        if( object->HasBaseClass( FOdysseyVectorPath::StaticClass() ) )
        {
            FOdysseyVectorPath* path = static_cast<FOdysseyVectorPath*>( object );
            FPathParam pathParamSwap = path->mPathParam;

            path->mPathParam = mPropertiesRecordArray[i].mPathParam;

            mPropertiesRecordArray[i].mPathParam = pathParamSwap;
        }

        if( object->HasBaseClass( FOdysseyVectorGroupPaint::StaticClass() ) )
        {
            FOdysseyVectorGroupPaint* paintGroup = static_cast<FOdysseyVectorGroupPaint*>( object );
            FGroupPaintParam groupPaintParamSwap = paintGroup->mGroupPaintParam;

            paintGroup->mGroupPaintParam = mPropertiesRecordArray[i].mGroupPaintParam;

            mPropertiesRecordArray[i].mGroupPaintParam = groupPaintParamSwap;
        }

        object->UpdateMatrix();
        object->Invalidate();
    }
}

void
FOdysseyVectorUndoPropertyChanged::Apply( UObject* iIgnored )
{
    FOdysseyVectorUndo::Apply( iIgnored );

    FOdysseyVectorUndoPropertyChanged::SwapParam();

    // update invalidated objects
    mScene->Update( FOdysseyVectorObject::UPDATEPAINTGROUPS );

    mScene->GetEngine()->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    mScene->GetEngine()->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                               | FOdysseyVectorEngine::SIGNAL_OBJECT_MODIFIED );
}

void
FOdysseyVectorUndoPropertyChanged::Revert( UObject* iIgnored )
{
    FOdysseyVectorUndo::Revert( iIgnored );

    FOdysseyVectorUndoPropertyChanged::SwapParam();

    // update invalidated objects
    mScene->Update( FOdysseyVectorObject::UPDATEPAINTGROUPS );

    mScene->GetEngine()->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    mScene->GetEngine()->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                               | FOdysseyVectorEngine::SIGNAL_OBJECT_MODIFIED );
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoPropertyChanged::ToString() const
{
    return FString("FOdysseyVectorUndoPropertyChanged");
}
