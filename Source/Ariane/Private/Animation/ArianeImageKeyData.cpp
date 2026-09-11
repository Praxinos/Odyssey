// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "ArianeImageKeyData.h"
#include "ArianeImage.h"
#include "ArianeObject.h"
#include "ArianeGroup.h"
#include "ArianePath.h"
#include "ArianeKeyedPath.h"

void
FArianeImageKeyData::RecordGeometry( UArianeImage* RecordedImage )
{
    InstancedKeyedObjects.Empty();

    FArianeObject::Traverse( RecordedImage->GetRootGroup()
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
FArianeImageKeyData::GetKeyedObject( const FGuid& ObjectGuid )
{
    FArianeKeyedObject** FoundObject = nullptr;

    FoundObject = KeyedObjectLookup.Find( ObjectGuid );


    return FoundObject ? *FoundObject : nullptr;
}

void
FArianeImageKeyData::PostLoad()
{
    BuildLookup();

    for( FInstancedStruct& InstancedKeyedObject : InstancedKeyedObjects )
    {
        FArianeKeyedObject* KeyedObject = InstancedKeyedObject.GetMutablePtr<FArianeKeyedObject>();

        KeyedObject->PostLoad();
    }
}

void
FArianeImageKeyData::PostEditUndo()
{
    BuildLookup();

    for( FInstancedStruct& InstancedKeyedObject : InstancedKeyedObjects )
    {
        FArianeKeyedObject* KeyedObject = InstancedKeyedObject.GetMutablePtr<FArianeKeyedObject>();

        KeyedObject->PostEditUndo();
    }
}

void
FArianeImageKeyData::BuildLookup()
{
    KeyedObjectLookup.Empty();
    KeyedObjectLookup.Reserve( InstancedKeyedObjects.Num() );

    for( FInstancedStruct& InstancedKeyedObject : InstancedKeyedObjects )
    {
        FArianeKeyedObject* KeyedObject = InstancedKeyedObject.GetMutablePtr<FArianeKeyedObject>();

        KeyedObjectLookup.Add( KeyedObject->GetGuid(), KeyedObject );
    }
}
