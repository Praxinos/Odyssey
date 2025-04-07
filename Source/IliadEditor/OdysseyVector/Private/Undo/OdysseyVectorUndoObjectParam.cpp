// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Undo/OdysseyVectorUndoObjectParam.h"
#include "OdysseyVectorPath.h"
#include "OdysseyVectorGroup.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorLayer.h"

FOdysseyVectorUndoObjectParam::~FOdysseyVectorUndoObjectParam()
{
    // free memory in all cases (applied or not)
    for( FSnapshotObject* objectSnapshot : mObjectSnapshotArray )
    {
        delete objectSnapshot;
    }

    if( mApplied )
    {
        // nothing to do
    }
    else
    {
        // nothing to do
    }
}

FSnapshotObject*
FOdysseyVectorUndoObjectParam::CreateObjectSnapshot( FOdysseyVectorObject* iObject
                                                   , uint64 iObjectParamFlags )
{
    if( iObject->GetClass() == FOdysseyVectorPath::StaticClass() )
    {
        FOdysseyVectorPath* path = static_cast<FOdysseyVectorPath*>(iObject);

        return new FSnapshotPath( path, iObjectParamFlags );
    }

    if( iObject->GetClass() == FOdysseyVectorGroup::StaticClass() )
    {
        FOdysseyVectorGroup* group = static_cast<FOdysseyVectorGroup*>(iObject);

        return new FSnapshotGroup( group, iObjectParamFlags );
    }

    if( iObject->GetClass() == FOdysseyVectorGroupPaint::StaticClass() )
    {
        FOdysseyVectorGroupPaint* paintGroup = static_cast<FOdysseyVectorGroupPaint*>(iObject);

        return new FSnapshotGroupPaint( paintGroup, iObjectParamFlags );
    }

    return new FSnapshotObject( iObject, iObjectParamFlags );
}

FSnapshotObject*
FOdysseyVectorUndoObjectParam::CreateObjectSnapshot( FOdysseyVectorObject* iObject
                                                   , const FName& iCategoryName )
{
    uint64 objectParamFlags = 0;

    if( iObject->HasBaseClass( FOdysseyVectorObject::StaticClass() ) )
    {
        if( iCategoryName == "Identity" )
        {
            objectParamFlags |= FSnapshotFlags::Object::NAME;
        }

        if( iCategoryName == "Appearance" )
        {
            objectParamFlags |= FSnapshotFlags::Object::COLORING;
            objectParamFlags |= FSnapshotFlags::Object::VISIBILITY;
            objectParamFlags |= FSnapshotFlags::Object::OPACITY;
        }

        if( iCategoryName == "Transform" )
        {
            objectParamFlags |= FSnapshotFlags::Object::TRANSFORMATIONS;
        }
    }

    if( iObject->HasBaseClass( FOdysseyVectorPath::StaticClass() ) )
    {
        if( iCategoryName == "Path" )
        {
            objectParamFlags |= FSnapshotFlags::Object::Path::BRUSH;
            objectParamFlags |= FSnapshotFlags::Object::Path::JOINTTYPE;
            objectParamFlags |= FSnapshotFlags::Object::Path::MITERLIMIT;
        }
    }

    if( iObject->HasBaseClass( FOdysseyVectorGroup::StaticClass() ) )
    {
        if( iCategoryName == "Appearance" )
        {
            objectParamFlags |= FSnapshotFlags::Object::Group::HUDCOLOR;
        }
    }

    if( iObject->HasBaseClass( FOdysseyVectorGroupPaint::StaticClass() ) )
    {
        if( iCategoryName == "PaintGroup" )
        {
            objectParamFlags |= FSnapshotFlags::Object::Group::Paint::PAINTED;
            objectParamFlags |= FSnapshotFlags::Object::Group::Paint::MONOCHROME;
            objectParamFlags |= FSnapshotFlags::Object::Group::Paint::MONOCHROMECOLOR;
            objectParamFlags |= FSnapshotFlags::Object::Group::Paint::WIREFRAME;
            objectParamFlags |= FSnapshotFlags::Object::Group::Paint::WIREFRAMECOLOR;
            objectParamFlags |= FSnapshotFlags::Object::Group::Paint::INTERSECTSCANVAS;
        }

        if( iCategoryName == "GapDetection" )
        {
            objectParamFlags |= FSnapshotFlags::Object::Group::Paint::GAPTOLERANCE;
            objectParamFlags |= FSnapshotFlags::Object::Group::Paint::GAPDETECTIONSCHEME;
            objectParamFlags |= FSnapshotFlags::Object::Group::Paint::SEGMENTEXTENSIONSCHEME;
            objectParamFlags |= FSnapshotFlags::Object::Group::Paint::SEGMENTEXTENSIONSIMPLIFIED;
        }

        if( iCategoryName == "Advanced" )
        {
            objectParamFlags |= FSnapshotFlags::Object::Group::Paint::REALTIME;
        }
    }

    return CreateObjectSnapshot( iObject, objectParamFlags );
}

FOdysseyVectorUndoObjectParam::FOdysseyVectorUndoObjectParam( FOdysseyVectorGroupPaint* iScene
                                                            , FOdysseyVectorObject* iObject
                                                            , uint64 iReturnFlags )
    : FOdysseyVectorUndo( iScene->GetLayer(), iReturnFlags )
{
}

FOdysseyVectorUndoObjectParam::FOdysseyVectorUndoObjectParam( FOdysseyVectorGroupPaint* iScene
                                                            , FOdysseyVectorObject* iObject
                                                            , const FName& iCategoryName
                                                            , uint64 iReturnFlags )
    : FOdysseyVectorUndo( iScene->GetLayer(), iReturnFlags )
{
    mObjectSnapshotArray.push_back( CreateObjectSnapshot( iObject, iCategoryName ) );
}

FOdysseyVectorUndoObjectParam::FOdysseyVectorUndoObjectParam( FOdysseyVectorGroupPaint* iScene
                                                            , const std::vector<FOdysseyVectorObject*>& iObjectArray
                                                            , const FName& iCategoryName
                                                            , uint64 iReturnFlags )
    : FOdysseyVectorUndo( iScene->GetLayer(), iReturnFlags )
{
    mObjectSnapshotArray.reserve( iObjectArray.size() );

    for( FOdysseyVectorObject* vectorObject : iObjectArray )
    {
        // Note: setting the owner does not make sense per se, as the bucket is only
        // temporary, but is mandatory in the ctor
        mObjectSnapshotArray.push_back( CreateObjectSnapshot( vectorObject, iCategoryName ) );
    }
}

FOdysseyVectorUndoObjectParam::FOdysseyVectorUndoObjectParam( FOdysseyVectorGroupPaint* iScene
                                                            , const std::list<FOdysseyVectorObject*>& iObjectList
                                                            , const FName& iCategoryName
                                                            , uint64 iReturnFlags )
    : FOdysseyVectorUndo( iScene->GetLayer(), iReturnFlags )
{
    mObjectSnapshotArray.reserve( iObjectList.size() );

    for( FOdysseyVectorObject* vectorObject : iObjectList )
    {
        // Note: setting the owner does not make sense per se, as the bucket is only
        // temporary, but is mandatory in the ctor
        mObjectSnapshotArray.push_back( CreateObjectSnapshot( vectorObject, iCategoryName ) );
    }
}

void
FOdysseyVectorUndoObjectParam::Apply( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Apply( iIgnored );

    for( int i = 0; i < mObjectSnapshotArray.size(); i++ )
    {
        mObjectSnapshotArray[i]->Restore();
    }

    // Update vector scenes and call callbacks if any (for refreshing GUI e.g)
    Update();
}

void
FOdysseyVectorUndoObjectParam::Revert( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Revert( iIgnored );

    for( int i = 0; i < mObjectSnapshotArray.size(); i++ )
    {
        mObjectSnapshotArray[i]->Restore();
    }

    // Update vector scenes and call callbacks if any (for refreshing GUI e.g)
    Update();
}

FOdysseyVectorUndoObjectVisibility::~FOdysseyVectorUndoObjectVisibility( )
{
}

FOdysseyVectorUndoObjectVisibility::FOdysseyVectorUndoObjectVisibility( FOdysseyVectorGroupPaint* iScene
                                                                      , FOdysseyVectorObject* iObject
                                                                      , uint64 iReturnFlags )
    : FOdysseyVectorUndoObjectParam( iScene, iObject, iReturnFlags )
{
    mObjectSnapshotArray.push_back( CreateObjectSnapshot( iObject, FSnapshotFlags::Object::VISIBILITY ) );
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoObjectParam::ToString() const
{
    return FString("FOdysseyVectorUndoObjectParam");
}
