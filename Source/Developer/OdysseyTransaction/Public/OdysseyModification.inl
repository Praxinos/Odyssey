// Copyright 2018-2019 Praxinos, Inc. All Rights Reserved.

inline
IOdysseyModification::~IOdysseyModification()
{
};

//---

template<typename T>
FOdysseyModificationState<T>::FOdysseyModificationState( T iValue, T* iReferencedObject, void( *iCallbackPtr )() )
{
    mStateBefore = iValue;
    mReferencedObject = iReferencedObject;
    mCallbackPtr = iCallbackPtr;
}

template<typename T>
FOdysseyModificationState<T>::~FOdysseyModificationState()
{
}

//---

template<typename T>
void
FOdysseyModificationState<T>::Undo()
{
    UE_LOG( LogTemp, Display, TEXT( "Undo" ) );

    *mReferencedObject = mStateBefore;

    if( mCallbackPtr )
        mCallbackPtr();
}

template<typename T>
void
FOdysseyModificationState<T>::Redo()
{
    UE_LOG( LogTemp, Display, TEXT( "Redo" ) );

    *mReferencedObject = mStateAfter;

    if( mCallbackPtr )
        mCallbackPtr();
}

template<typename T>
void
FOdysseyModificationState<T>::SealModification()
{
    mStateAfter = *mReferencedObject;
}

template<typename T>
EModificationType
FOdysseyModificationState<T>::GetModificationType()
{
    return EModificationType::kState;
}

//---
//---
//---

template<typename T>
FOdysseyModificationManipulation<T>::FOdysseyModificationManipulation( T* iReferencedObject, void ( T::*iUndoPtr )(), void ( T::*iRedoPtr )(), void ( T::*iCallbackPtr )() )
{
    mReferencedObject = iReferencedObject;
    mUndoPtr = iUndoPtr;
    mRedoPtr = iRedoPtr;
    mCallbackPtr = iCallbackPtr;
}

template<typename T>
FOdysseyModificationManipulation<T>::~FOdysseyModificationManipulation()
{
}

//---

template<typename T>
void
FOdysseyModificationManipulation<T>::Undo()
{
}

template<typename T>
void
FOdysseyModificationManipulation<T>::Redo()
{
}

template<typename T>
void
FOdysseyModificationManipulation<T>::SealModification()
{
}

template<typename T>
EModificationType
FOdysseyModificationManipulation<T>::GetModificationType()
{
    return EModificationType::kManipulation;
}
