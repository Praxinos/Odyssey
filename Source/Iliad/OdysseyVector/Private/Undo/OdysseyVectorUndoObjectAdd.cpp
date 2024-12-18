// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Undo/OdysseyVectorUndoObjectAdd.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorSharedEnv.h"

FOdysseyVectorUndoObjectAdd::~FOdysseyVectorUndoObjectAdd()
{
    if( mApplied )
    {
        // nothing to do
    }
    else
    {
        for( int i = 0; i < mObjectArray.size(); i++ )
        {
            delete mObjectArray[i];
        }
    }
}

FOdysseyVectorUndoObjectAdd::FOdysseyVectorUndoObjectAdd( FOdysseyVectorGroupPaint* iScene
                                                        , FOdysseyVectorObject* iObject
                                                        , uint64 iReturnFlags )
    : FOdysseyVectorUndo( iScene->GetSharedEnv(), iReturnFlags )
{
    GetEngineListFromObjectList( { iScene }, mEngineList );

    mObjectArray.push_back( iObject );
}

FOdysseyVectorUndoObjectAdd::FOdysseyVectorUndoObjectAdd( FOdysseyVectorGroupPaint* iScene
                                                        , std::list<FOdysseyVectorObject*>& iObjectList
                                                        , uint64 iReturnFlags )
    : FOdysseyVectorUndo( iScene->GetSharedEnv(), iReturnFlags )
{
    GetEngineListFromObjectList( { iScene }, mEngineList );

    for( FOdysseyVectorObject* object : iObjectList )
    {
        mObjectArray.push_back( object );
    }
}

void
FOdysseyVectorUndoObjectAdd::Apply( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Apply( iIgnored );

    for( int i = 0; i < mObjectArray.size(); i++ )
    {
        mParentArray[i]->AppendChild( mObjectArray[i] );
    }

    // Update vector scenes and call callbacks if any (for refreshing GUI e.g)
    Update();
}

void
FOdysseyVectorUndoObjectAdd::Revert( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Revert( iIgnored );

    if( mObjectArray.size() )
    {
        mParentArray.resize( mObjectArray.size() );

        for( int i = 0; i < mObjectArray.size(); i++ )
        {
            mParentArray[i] = mObjectArray[i]->GetParent();

            mParentArray[i]->RemoveChild( mObjectArray[i] );
        }
    }

    mEngineList.front()->ClearObjectSelection();

    // Update vector scenes and call callbacks if any (for refreshing GUI e.g)
    Update();
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoObjectAdd::ToString() const
{
    return FString("FOdysseyVectorUndoObjectAdd");
}
