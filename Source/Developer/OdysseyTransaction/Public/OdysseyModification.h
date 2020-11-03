// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

enum class EModificationType : uint8
{
    kState,
    kCache,
    kManipulation
};

//---

/**
 * Describes a state of an item, used to revert back to that state at any point using a Undo History Widget, abstract version useful for storage
 */
class ODYSSEYTRANSACTION_API IOdysseyModification
{
public:
    virtual ~IOdysseyModification();

public:
    /** Reload the state of the object stored in FOdysseyModification to change ReferencedObject */
    virtual void Undo() = 0;
    virtual void Redo() = 0;

    virtual void SealModification() = 0;

    virtual EModificationType GetModificationType() = 0;
};

//---

/**
 * Describes a state of an item, used to revert back to that state at any point using a Undo History Widget
 */
template <typename T>
class FOdysseyModificationState : public IOdysseyModification
{
public:
    FOdysseyModificationState( T iValue, T* iReferencedObject, void( *iCallbackPtr )() = 0 );

    virtual ~FOdysseyModificationState();

public:
    virtual void Undo();
    virtual void Redo();
    virtual void SealModification();
    virtual EModificationType GetModificationType();

public:
    T mStateBefore;
    T mStateAfter;//We need to store this at the end of the record
    T* mReferencedObject;
    void( *mCallbackPtr )();
};

//---

/**
 * Describes a manipulation of an item, used to revert back by using those manipulations at any point using a Undo History Widget
 */
template <typename T>
class FOdysseyModificationManipulation : public IOdysseyModification
{
public:
    FOdysseyModificationManipulation( T* iReferencedObject, void ( T::*iUndoPtr )(), void ( T::*iRedoPtr )(), void ( T::*iCallbackPtr )() = 0 );

    virtual ~FOdysseyModificationManipulation();

public:
    virtual void Undo();
    virtual void Redo();
    virtual void SealModification();
    virtual EModificationType GetModificationType();

public:
    T* mReferencedObject;
    void ( T::*mUndoPtr )();
    void ( T::*mRedoPtr )();
    void ( T::*mCallbackPtr )();
};

#include "OdysseyModification.inl"
