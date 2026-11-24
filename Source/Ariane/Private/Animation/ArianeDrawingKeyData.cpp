// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "ArianeDrawingKeyData.h"
#include "ArianeDrawing.h"
#include "ArianeObject.h"
#include "ArianeGroup.h"
#include "ArianePath.h"
#include "ArianeKeyedPath.h"

void
FArianeDrawingKeyData::RecordGeometry( UArianeDrawing* RecordedDrawing )
{
    InstancedKeyedObjects.Empty();

    FArianeObject::Traverse( RecordedDrawing->GetRootGroup()
                           , FArianeObject::ETraversalPolicy::AllChildren
                           , [this]( FArianeObject* Object ) -> FArianeObject::ETraversalReturnValue
        {
            if( Object->GetClass() == FArianePath::StaticClass() )
            {
                FArianePath* Path = static_cast<FArianePath*>(Object);

                InstancedKeyedObjects.Push( FInstancedStruct::Make<FArianeKeyedPath>( Path ) );
            }

            return FArianeObject::ETraversalReturnValue::Continue;
        } );

    BuildLookup();
}

FArianeKeyedObject*
FArianeDrawingKeyData::GetKeyedObject( const FGuid& ObjectGuid )
{
    // The cache is not serialized: it is empty after a load, a paste or an undo
    if( KeyedObjectLookup.Num() != InstancedKeyedObjects.Num() )
    {
        BuildLookup();
    }

    int32* FoundIndex = KeyedObjectLookup.Find( ObjectGuid );

    return FoundIndex ? InstancedKeyedObjects[*FoundIndex].GetMutablePtr<FArianeKeyedObject>()
                      : nullptr;
}

void
FArianeDrawingKeyData::PostLoad()
{
    for( FInstancedStruct& InstancedKeyedObject : InstancedKeyedObjects )
    {
        FArianeKeyedObject* KeyedObject = InstancedKeyedObject.GetMutablePtr<FArianeKeyedObject>();

        KeyedObject->PostLoad();
    }
}

void
FArianeDrawingKeyData::PostEditUndo()
{
    // The undone array may have the same size but other objects: let GetKeyedObject() rebuild the cache
    KeyedObjectLookup.Empty();

    for( FInstancedStruct& InstancedKeyedObject : InstancedKeyedObjects )
    {
        FArianeKeyedObject* KeyedObject = InstancedKeyedObject.GetMutablePtr<FArianeKeyedObject>();

        KeyedObject->PostEditUndo();
    }
}

void
FArianeDrawingKeyData::BuildLookup()
{
    KeyedObjectLookup.Empty();
    KeyedObjectLookup.Reserve( InstancedKeyedObjects.Num() );

    for( int32 i = 0; i < InstancedKeyedObjects.Num(); i++ )
    {
        const FArianeKeyedObject* KeyedObject = InstancedKeyedObjects[i].GetPtr<FArianeKeyedObject>();

        KeyedObjectLookup.Add( KeyedObject->GetGuid(), i );
    }
}
