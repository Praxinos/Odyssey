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
    FArianeKeyedObject** FoundObject = nullptr;

    FoundObject = KeyedObjectLookup.Find( ObjectGuid );


    return FoundObject ? *FoundObject : nullptr;
}

void
FArianeDrawingKeyData::PostLoad()
{
    BuildLookup();

    for( FInstancedStruct& InstancedKeyedObject : InstancedKeyedObjects )
    {
        FArianeKeyedObject* KeyedObject = InstancedKeyedObject.GetMutablePtr<FArianeKeyedObject>();

        KeyedObject->PostLoad();
    }
}

void
FArianeDrawingKeyData::PostEditUndo()
{
    BuildLookup();

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

    for( FInstancedStruct& InstancedKeyedObject : InstancedKeyedObjects )
    {
        FArianeKeyedObject* KeyedObject = InstancedKeyedObject.GetMutablePtr<FArianeKeyedObject>();

        KeyedObjectLookup.Add( KeyedObject->GetGuid(), KeyedObject );
    }
}
