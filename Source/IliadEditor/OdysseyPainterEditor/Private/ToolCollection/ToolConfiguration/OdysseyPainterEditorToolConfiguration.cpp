// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyPainterEditorToolConfiguration.h"

#include "Serialization/ObjectAndNameAsStringProxyArchive.h"

/////////////////////////////////////////////////////
// FToolPropertySnapshot

bool FToolPropertySnapshot::operator==( const FToolPropertySnapshot& iOther ) const
{
    if (Values.Num() != iOther.Values.Num())
    {
        return false;
    }

    TArray<uint8> bufferA;
    TArray<uint8> bufferB;

    FMemoryWriter memWriterA( bufferA, true );
    FMemoryWriter memWriterB( bufferB, true );

    FObjectAndNameAsStringProxyArchive arA(memWriterA, false);
    FObjectAndNameAsStringProxyArchive arB(memWriterB, false);

    arA.SetIsSaving( true );
    arB.SetIsSaving( true );

    FToolPropertySnapshot::StaticStruct()->SerializeItem( arA, (void*)this, nullptr );
    FToolPropertySnapshot::StaticStruct()->SerializeItem( arB, (void*)&iOther, nullptr );

    return bufferA == bufferB;
}
