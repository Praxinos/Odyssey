#include "Undo/OdysseyVectorUndoPropertyChanged.h"

FOdysseyVectorUndoPropertyChanged::~FOdysseyVectorUndoPropertyChanged()
{

}

FOdysseyVectorUndoPropertyChanged::FOdysseyVectorUndoPropertyChanged( FOdysseyVectorScene* iScene, FOdysseyVectorObject* iObject )
    : FOdysseyVectorUndo( iScene )
    , mObject( iObject )
{
    mObjectParam = mObject->mObjectParam;

    if( mObject->HasBaseClass( FOdysseyVectorPath::StaticClass() ) )
    {
        FOdysseyVectorPath* path = static_cast<FOdysseyVectorPath*>( mObject );

        mPathParam = path->mPathParam;
    }

    if( mObject->HasBaseClass( FOdysseyVectorGroupPaint::StaticClass() ) )
    {
        FOdysseyVectorGroupPaint* paintGroup = static_cast<FOdysseyVectorGroupPaint*>( mObject );

        mGroupPaintParam = paintGroup->mGroupPaintParam;
    }

    if( mObject->HasBaseClass( FOdysseyVectorEllipse::StaticClass() ) )
    {
        FOdysseyVectorEllipse* ellipse = static_cast<FOdysseyVectorEllipse*>( mObject );

        mEllipseParam = ellipse->mEllipseParam;
    }
}

void
FOdysseyVectorUndoPropertyChanged::SwapParam()
{
    FObjectParam objectParamSwap = mObject->mObjectParam;

    mObject->mObjectParam = mObjectParam;
    mObjectParam = objectParamSwap;

    if( mObject->HasBaseClass( FOdysseyVectorPath::StaticClass() ) )
    {
        FOdysseyVectorPath* path = static_cast<FOdysseyVectorPath*>( mObject );
        FPathParam pathParamSwap = path->mPathParam;

        path->mPathParam = mPathParam;
        mPathParam = pathParamSwap;
    }

    if( mObject->HasBaseClass( FOdysseyVectorGroupPaint::StaticClass() ) )
    {
        FOdysseyVectorGroupPaint* paintGroup = static_cast<FOdysseyVectorGroupPaint*>( mObject );
        FGroupPaintParam groupPaintParamSwap = paintGroup->mGroupPaintParam;

        paintGroup->mGroupPaintParam = mGroupPaintParam;
        mGroupPaintParam = groupPaintParamSwap;
    }

    if( mObject->HasBaseClass( FOdysseyVectorEllipse::StaticClass() ) )
    {
        FOdysseyVectorEllipse* ellipse = static_cast<FOdysseyVectorEllipse*>( mObject );
        FEllipseParam ellipseParamSwap = ellipse->mEllipseParam;

        ellipse->mEllipseParam = mEllipseParam;
        mEllipseParam = ellipseParamSwap;
    }
}

void
FOdysseyVectorUndoPropertyChanged::Apply( UObject* iIgnored )
{
    FOdysseyVectorUndo::Apply( iIgnored );

    FOdysseyVectorUndoPropertyChanged::SwapParam();

    mObject->UpdateMatrix();
    mObject->Invalidate();

    // update invalidated objects
    mScene->Update(0);
    // call callbacks if any (for refreshing GUI e.g)
    mScene->Signal( FOdysseyVectorScene::SIGNAL_SCENE_REDRAW
                  | FOdysseyVectorScene::SIGNAL_OBJECT_MODIFIED );
}

void
FOdysseyVectorUndoPropertyChanged::Revert( UObject* iIgnored )
{
    FOdysseyVectorUndo::Revert( iIgnored );

    FOdysseyVectorUndoPropertyChanged::SwapParam();

    mObject->UpdateMatrix();
    mObject->Invalidate();

    // update invalidated objects
    mScene->Update(0);
    // call callbacks if any (for refreshing GUI e.g)
    mScene->Signal( FOdysseyVectorScene::SIGNAL_SCENE_REDRAW
                  | FOdysseyVectorScene::SIGNAL_OBJECT_MODIFIED );
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoPropertyChanged::ToString() const
{
    return FString("FOdysseyVectorUndoPropertyChanged");
}
