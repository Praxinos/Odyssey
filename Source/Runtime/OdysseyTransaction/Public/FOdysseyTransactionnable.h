// Copyright 2018-2019 Praxinos, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FOdysseyModification.h"
#include "FOdysseyUndoHistory.h"
#include "FOdysseyTransaction.h"

/**
 * Describes a transactionnable, which can be modified and put as a state inside a Undo History Widget
 */
class FOdysseyTransactionnable
{

public:
    FOdysseyTransactionnable(); // Historyless constructor, no transaction will be done in this transactionnable
    FOdysseyTransactionnable( FOdysseyUndoHistory* InUndoHistoryPtr);
    ~FOdysseyTransactionnable();

public:
    template <typename T>
    void ModifyAsState( T inData, T* inReferencedObject, void (*InCallbackPtr)() = 0 );
    void Record(FName InName);
    void EndRecord();
    FOdysseyUndoHistory* GetUndoHistoryPtr();


private:
    FOdysseyUndoHistory* UndoHistoryPtr;
};



inline FOdysseyTransactionnable::FOdysseyTransactionnable():
    UndoHistoryPtr(nullptr)
{
}

inline FOdysseyTransactionnable::FOdysseyTransactionnable(FOdysseyUndoHistory* InUndoHistoryPtr):
    UndoHistoryPtr(InUndoHistoryPtr)
{
}


inline FOdysseyTransactionnable::~FOdysseyTransactionnable()
{

}

inline
void
FOdysseyTransactionnable::Record(FName InName)
{
    if( UndoHistoryPtr != nullptr )
    {
        UndoHistoryPtr->Record(InName);
    }
}

inline
void
FOdysseyTransactionnable::EndRecord()
{
    if( UndoHistoryPtr != nullptr )
    {
        UndoHistoryPtr->EndRecord();
    }
}


inline
FOdysseyUndoHistory*
FOdysseyTransactionnable::GetUndoHistoryPtr()
{
    return UndoHistoryPtr;
}




template <typename T>
void FOdysseyTransactionnable::ModifyAsState(T inData, T* inReferencedObject, void (*InCallbackPtr)() )
{
    if( UndoHistoryPtr != nullptr )
    {
        if( UndoHistoryPtr->IsRecording() )
            UndoHistoryPtr->GetCurrentTransaction()->AppendState( new FOdysseyModificationState<T>(inData, inReferencedObject, InCallbackPtr ) );
        else
            UE_LOG(LogTemp, Display, TEXT("Transaction not recorded, did you forget to use Record() before Modify() ?"));
    }
}
