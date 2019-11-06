// Copyright 2018-2019 Praxinos, Inc. All Rights Reserved.

inline
FOdysseyTransactionnable::FOdysseyTransactionnable() :
    mUndoHistoryPtr( nullptr )
{
}

inline
FOdysseyTransactionnable::FOdysseyTransactionnable( FOdysseyUndoHistory* iUndoHistoryPtr ) :
    mUndoHistoryPtr( iUndoHistoryPtr )
{
}

inline
FOdysseyTransactionnable::~FOdysseyTransactionnable()
{

}

//---

inline
void
FOdysseyTransactionnable::Record( FName iName )
{
    if( !mUndoHistoryPtr )
        return;

    mUndoHistoryPtr->Record( iName );
}

inline
void
FOdysseyTransactionnable::EndRecord()
{
    if( !mUndoHistoryPtr )
        return;

    mUndoHistoryPtr->EndRecord();
}

inline
FOdysseyUndoHistory*
FOdysseyTransactionnable::GetUndoHistoryPtr()
{
    return mUndoHistoryPtr;
}

//---

template <typename T>
void
FOdysseyTransactionnable::ModifyAsState( T iData, T* iReferencedObject, void( *iCallbackPtr )() )
{
    if( mUndoHistoryPtr != nullptr )
    {
        if( mUndoHistoryPtr->IsRecording() )
            mUndoHistoryPtr->GetCurrentTransaction()->AppendState( new FOdysseyModificationState<T>( iData, iReferencedObject, iCallbackPtr ) );
        else
            UE_LOG( LogTemp, Display, TEXT( "Transaction not recorded, did you forget to use Record() before Modify() ?" ) );
    }
}
