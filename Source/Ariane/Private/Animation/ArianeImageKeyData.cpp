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
}

FArianeKeyedObject*
FArianeImageKeyData::GetKeyedObject( const FGuid& ObjectGuid )
{
    for( FInstancedStruct& InstancedKeyedObject : InstancedKeyedObjects )
    {
        FArianeKeyedObject* KeyedObject = InstancedKeyedObject.GetMutablePtr<FArianeKeyedObject>();

        if( KeyedObject->GetGuid() == ObjectGuid )
        {
            return KeyedObject;
        }
    }

    return nullptr;
}

void
FArianeImageKeyData::PostLoad()
{
    for( FInstancedStruct& InstancedKeyedObject : InstancedKeyedObjects )
    {
        FArianeKeyedObject* KeyedObject = InstancedKeyedObject.GetMutablePtr<FArianeKeyedObject>();

        KeyedObject->PostLoad();
    }
}

void
FArianeImageKeyData::PostEditUndo()
{
    for( FInstancedStruct& InstancedKeyedObject : InstancedKeyedObjects )
    {
        FArianeKeyedObject* KeyedObject = InstancedKeyedObject.GetMutablePtr<FArianeKeyedObject>();

        KeyedObject->PostEditUndo();
    }
}
