// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#pragma once

#include "CoreMinimal.h"

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
    TArray64< uint8 >&                    GetArray();
    void                                  SetArray( TArray64< uint8 >& iNewArray);
    static UOdysseyBlockUndoable*         CreateTransient();

private:
    //UPROPERTY()
    TArray64<uint8>         mArray;

};

