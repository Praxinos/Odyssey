// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

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

        return new FSnapshotPath( path, iObjectParamFlags, eSnapshotState::Initial );
    }

    if( iObject->GetClass() == FOdysseyVectorGroup::StaticClass() )
    {
        FOdysseyVectorGroup* group = static_cast<FOdysseyVectorGroup*>(iObject);

        return new FSnapshotGroup( group, iObjectParamFlags, eSnapshotState::Initial );
    }

    if( iObject->GetClass() == FOdysseyVectorGroupPaint::StaticClass() )
    {
        FOdysseyVectorGroupPaint* paintGroup = static_cast<FOdysseyVectorGroupPaint*>(iObject);

        return new FSnapshotGroupPaint( paintGroup, iObjectParamFlags, eSnapshotState::Initial );
    }

    return new FSnapshotObject( iObject, iObjectParamFlags, eSnapshotState::Initial );
}

FSnapshotObject*
FOdysseyVectorUndoObjectParam::CreateObjectSnapshot( FOdysseyVectorObject* iObject
                                                   , const FName& iCategoryName
                                                   , const FName& iMemberPropertyName
                                                   , const FName& iPropertyName )
{
    uint64 objectParamFlags = 0;

    if( iObject->HasBaseClass( FOdysseyVectorObject::StaticClass() ) )
    {
        if( ( iCategoryName == "Identity" ) || ( iCategoryName == "" ) )
        {
            objectParamFlags |= FSnapshotFlags::Object::NAME;
        }

        if( ( iCategoryName == "Appearance" ) || ( iCategoryName == "" ) )
        {
            objectParamFlags |= FSnapshotFlags::Object::COLORING;
            objectParamFlags |= FSnapshotFlags::Object::VISIBILITY;
            objectParamFlags |= FSnapshotFlags::Object::OPACITY;
        }

        if( ( iCategoryName == "Transform" ) || ( iCategoryName == "" ) )
        {
            objectParamFlags |= FSnapshotFlags::Object::TRANSFORMATIONS;
        }
    }

    if( iObject->HasBaseClass( FOdysseyVectorPath::StaticClass() ) )
    {
        if( ( iMemberPropertyName == "Brush" ) || ( iCategoryName == "" ) )
        {
            objectParamFlags |= FSnapshotFlags::Object::Path::BRUSH;
        }

        if( ( iCategoryName == "Path" ) || ( iCategoryName == "" ) )
        {
            objectParamFlags |= FSnapshotFlags::Object::Path::JOINTTYPE;
            objectParamFlags |= FSnapshotFlags::Object::Path::MITERLIMIT;
        }
    }

    if( iObject->HasBaseClass( FOdysseyVectorGroup::StaticClass() ) )
    {
        if( ( iCategoryName == "Appearance" ) || ( iCategoryName == "" ) )
        {
            objectParamFlags |= FSnapshotFlags::Object::Group::HUDCOLOR;
        }
    }

    if( iObject->HasBaseClass( FOdysseyVectorGroupPaint::StaticClass() ) )
    {
        if( ( iCategoryName == "PaintGroup" ) || ( iCategoryName == "" ) )
        {
            objectParamFlags |= FSnapshotFlags::Object::Group::Paint::PAINTED;
            objectParamFlags |= FSnapshotFlags::Object::Group::Paint::MONOCHROME;
            objectParamFlags |= FSnapshotFlags::Object::Group::Paint::MONOCHROMECOLOR;
            objectParamFlags |= FSnapshotFlags::Object::Group::Paint::WIREFRAME;
            objectParamFlags |= FSnapshotFlags::Object::Group::Paint::WIREFRAMECOLOR;
            objectParamFlags |= FSnapshotFlags::Object::Group::Paint::INTERSECTSCANVAS;
        }

        if( ( iCategoryName == "GapDetection" ) || ( iCategoryName == "" ) )
        {
            objectParamFlags |= FSnapshotFlags::Object::Group::Paint::GAPTOLERANCE;
            objectParamFlags |= FSnapshotFlags::Object::Group::Paint::GAPDETECTIONSCHEME;
            objectParamFlags |= FSnapshotFlags::Object::Group::Paint::SEGMENTEXTENSIONSCHEME;
            objectParamFlags |= FSnapshotFlags::Object::Group::Paint::SEGMENTEXTENSIONSIMPLIFIED;
        }

        if( ( iCategoryName == "Advanced" ) || ( iCategoryName == "" ) )
        {
            objectParamFlags |= FSnapshotFlags::Object::Group::Paint::REALTIME;
        }
    }

    return CreateObjectSnapshot( iObject, objectParamFlags );
}

FOdysseyVectorUndoObjectParam::FOdysseyVectorUndoObjectParam( FOdysseyVectorLayer* iLayer
                                                            , FOdysseyVectorObject* iObject )
    : FOdysseyVectorUndo( iLayer )
{
}

FOdysseyVectorUndoObjectParam::FOdysseyVectorUndoObjectParam( FOdysseyVectorLayer* iLayer
                                                            , FOdysseyVectorObject* iObject
                                                            , const FName& iCategoryName
                                                            , const FName& iMemberPropertyName
                                                            , const FName& iPropertyName )
    : FOdysseyVectorUndo( iLayer )
{
    mObjectSnapshotArray.push_back( CreateObjectSnapshot( iObject
                                                        , iCategoryName
                                                        , iMemberPropertyName
                                                        , iPropertyName ) );
}

FOdysseyVectorUndoObjectParam::FOdysseyVectorUndoObjectParam( FOdysseyVectorLayer* iLayer
                                                            , const std::vector<FOdysseyVectorObject*>& iObjectArray
                                                            , const FName& iCategoryName
                                                            , const FName& iMemberPropertyName
                                                            , const FName& iPropertyName )
    : FOdysseyVectorUndo( iLayer )
{
    mObjectSnapshotArray.reserve( iObjectArray.size() );

    for( FOdysseyVectorObject* vectorObject : iObjectArray )
    {
        // Note: setting the owner does not make sense per se, as the bucket is only
        // temporary, but is mandatory in the ctor
        mObjectSnapshotArray.push_back( CreateObjectSnapshot( vectorObject
                                                            , iCategoryName
                                                            , iMemberPropertyName
                                                            , iPropertyName ) );
    }
}

FOdysseyVectorUndoObjectParam::FOdysseyVectorUndoObjectParam( FOdysseyVectorLayer* iLayer
                                                            , const std::list<FOdysseyVectorObject*>& iObjectList
                                                            , const FName& iCategoryName
                                                            , const FName& iMemberPropertyName
                                                            , const FName& iPropertyName )
    : FOdysseyVectorUndo( iLayer )
{
    mObjectSnapshotArray.reserve( iObjectList.size() );

    for( FOdysseyVectorObject* vectorObject : iObjectList )
    {
        // Note: setting the owner does not make sense per se, as the bucket is only
        // temporary, but is mandatory in the ctor
        mObjectSnapshotArray.push_back( CreateObjectSnapshot( vectorObject
                                                            , iCategoryName
                                                            , iMemberPropertyName
                                                            , iPropertyName ) );
    }
}

void
FOdysseyVectorUndoObjectParam::Apply( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Apply( iIgnored );

    for( int i = 0; i < mObjectSnapshotArray.size(); i++ )
    {
        mObjectSnapshotArray[i]->LoadState( eSnapshotState::Altered );
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
        mObjectSnapshotArray[i]->RecordState( eSnapshotState::Altered );
    }


    for( int i = 0; i < mObjectSnapshotArray.size(); i++ )
    {
        mObjectSnapshotArray[i]->LoadState( eSnapshotState::Initial );
    }

    // Update vector scenes and call callbacks if any (for refreshing GUI e.g)
    Update();
}

FOdysseyVectorUndoObjectVisibility::~FOdysseyVectorUndoObjectVisibility( )
{
}

FOdysseyVectorUndoObjectVisibility::FOdysseyVectorUndoObjectVisibility( FOdysseyVectorLayer* iLayer
                                                                      , FOdysseyVectorObject* iObject )
    : FOdysseyVectorUndoObjectParam( iLayer, iObject )
{
    mObjectSnapshotArray.push_back( CreateObjectSnapshot( iObject, FSnapshotFlags::Object::VISIBILITY ) );
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoObjectParam::ToString() const
{
    return FString("FOdysseyVectorUndoObjectParam");
}
