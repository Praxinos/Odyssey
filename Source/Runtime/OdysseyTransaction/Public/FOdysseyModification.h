// Copyright 2018-2019 Praxinos, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"


enum class EModificationType : uint8
{
    State,
    Cache,
    Manipulation
};

/**
 * Describes a state of an item, used to revert back to that state at any point using a Undo History Widget, abstract version useful for storage
 */
class ODYSSEYTRANSACTION_API IOdysseyModification
{
public:
    virtual ~IOdysseyModification() {};
    virtual EModificationType GetModificationType() = 0;

public:
    /** Reload the state of the object stored in FOdysseyModification to change ReferencedObject */
    virtual void Undo() = 0;

    virtual void Redo() = 0;

    virtual void SealModification() = 0;

};


/**
 * Describes a state of an item, used to revert back to that state at any point using a Undo History Widget
 */
template <typename T>
class FOdysseyModificationState : public IOdysseyModification
{

public:
    FOdysseyModificationState(T Value, T* InReferencedObject, void (*InCallbackPtr)() = 0 );

    ~FOdysseyModificationState();

public:
    void Undo();
    void Redo();
    void SealModification();
    EModificationType GetModificationType();

public:
    T StateBefore;
    T StateAfter;//We need to store this at the end of the record
    T* ReferencedObject;
    void (*CallbackPtr)();
};

template<typename T>
FOdysseyModificationState<T>::FOdysseyModificationState(T Value, T* InReferencedObject, void (*InCallbackPtr)() )
{
    StateBefore = Value;
    ReferencedObject = InReferencedObject;
    CallbackPtr = InCallbackPtr;
}


template<typename T>
FOdysseyModificationState<T>::~FOdysseyModificationState()
{
}


template<typename T>
void FOdysseyModificationState<T>::Undo()
{
    *ReferencedObject = StateBefore;
    UE_LOG(LogTemp, Display, TEXT("Undo"));

    if( CallbackPtr )
        CallbackPtr();
}

template<typename T>
void FOdysseyModificationState<T>::Redo()
{
    *ReferencedObject = StateAfter;
    UE_LOG(LogTemp, Display, TEXT("Redo"));

    if( CallbackPtr )
        CallbackPtr();
}

template<typename T>
void FOdysseyModificationState<T>::SealModification()
{
    StateAfter = *ReferencedObject;
}

template<typename T>
EModificationType FOdysseyModificationState<T>::GetModificationType()
{
    return EModificationType::State;
}





/**
 * Describes a manipulation of an item, used to revert back by using those manipulations at any point using a Undo History Widget
 */
template <typename T>
class FOdysseyModificationManipulation : public IOdysseyModification
{

public:
    FOdysseyModificationManipulation( T* InReferencedObject, void (T::*InUndoPtr)(), void (T::*InRedoPtr)(), void (T::*InCallbackPtr)() = 0 );

    ~FOdysseyModificationManipulation();

public:
    void Undo();
    void Redo();
    void SealModification();
    EModificationType GetModificationType();

public:
    T* ReferencedObject;
    void (T::*UndoPtr)();
    void (T::*RedoPtr)();
    void (T::*CallbackPtr)();
};


template<typename T>
FOdysseyModificationManipulation<T>::FOdysseyModificationManipulation( T* InReferencedObject, void (T::*InUndoPtr)(), void (T::*InRedoPtr)(), void (T::*InCallbackPtr)() )
{
    ReferencedObject = InReferencedObject;
    UndoPtr = InUndoPtr;
    RedoPtr = InRedoPtr;
    CallbackPtr = InCallbackPtr;
}


template<typename T>
FOdysseyModificationManipulation<T>::~FOdysseyModificationManipulation()
{
}


template<typename T>
void FOdysseyModificationManipulation<T>::Undo()
{
}

template<typename T>
void FOdysseyModificationManipulation<T>::Redo()
{
}

template<typename T>
void FOdysseyModificationManipulation<T>::SealModification()
{
}

template<typename T>
EModificationType FOdysseyModificationManipulation<T>::GetModificationType()
{
    return EModificationType::Manipulation;
}
