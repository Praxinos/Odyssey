// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#pragma once

#include "CoreMinimal.h"
#include "Serialization/BufferArchive.h"
#include "Serialization/MemoryReader.h"


#include "OdysseyBlockUndoable.generated.h"

UCLASS()
class ODYSSEYIMAGING_API UOdysseyBlockUndoable : public UObject
{
    GENERATED_BODY()
    
public:
    // Construction / Destruction
    ~UOdysseyBlockUndoable();
    UOdysseyBlockUndoable();

public:
    // Public API
    TArray< uint8 >&                      GetArray();
    void                                  SetArray( uint8* iStart, int iSize );
    static UOdysseyBlockUndoable*         CreateTransient();

private:
    TArray<uint8>         mArray;
    
    int testSave;
    int testLoad;
    
private:
    FBufferArchive      mToBinary;

};

