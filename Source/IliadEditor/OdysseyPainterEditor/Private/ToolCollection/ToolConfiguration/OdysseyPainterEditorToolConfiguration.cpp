// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyPainterEditorToolConfiguration.h"

/////////////////////////////////////////////////////
// FToolPropertySnapshot

bool FToolPropertySnapshot::operator==( const FToolPropertySnapshot& iOther ) const
{
    if( Values.Num() != iOther.Values.Num() )
    {
        return false;
    }

    TArray<uint8> bufferA;
    TArray<uint8> bufferB;

    FMemoryWriter writerA(bufferA, true);
    FMemoryWriter writerB(bufferB, true);

    FToolPropertySnapshot::StaticStruct()->SerializeItem( writerA, (void*)this, nullptr );
    FToolPropertySnapshot::StaticStruct()->SerializeItem( writerB, (void*)&iOther, nullptr );

    return bufferA == bufferB;
}
