// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Mutations/OdysseyMutator.h"

#include "Editor.h"
#include "Misc/Change.h"
#include "Misc/ITransaction.h"

class FOdysseyMutationsUndo : public FCommandChange
{
public:
    FOdysseyMutationsUndo()
        : mRootMutation(nullptr)
    {
    }

    FOdysseyMutationsUndo(TSharedPtr<FOdysseyRootMutation> iRootMutation)
        : mRootMutation(iRootMutation)
    {
    }

	/** Makes the change to the object */
    //REDO
	virtual void Apply( UObject* Object ) override
    {
        mRootMutation->Apply();
        mRootMutation->OnChanged().ExecuteIfBound();
        mRootMutation->OnCommited().ExecuteIfBound();
    }

	/** Reverts change to the object */
    //UNDO
	virtual void Revert( UObject* Object ) override
    {
        mRootMutation->Revert();
        mRootMutation->OnChanged().ExecuteIfBound();
        mRootMutation->OnCommited().ExecuteIfBound();
    }

	/** Describes this change (for debugging) */
	virtual FString ToString() const override
    {
        return mRootMutation->GetName();
    }

private:
    TSharedPtr<FOdysseyRootMutation> mRootMutation;
};

//======================================================

FOdysseyMutator::~FOdysseyMutator()
{
    Commit();
}

FOdysseyMutator::FOdysseyMutator(UObject* iObject, const FString& iName, bool iGenerateUndo)
    : mObject(iObject)
    , mRootMutation(MakeShared<FOdysseyRootMutation>(iName))
    , mGenerateUndo(iGenerateUndo)
{
}

TSharedPtr<FOdysseyRootMutation>
FOdysseyMutator::GetRootMutation() const
{
    return mRootMutation;
}

bool
FOdysseyMutator::IsDirty() const
{
    return mRootMutation->IsDirty();
}

void
FOdysseyMutator::AddMutation(TSharedPtr<IOdysseyMutation> iMutation)
{
    mRootMutation->AddMutation(iMutation);
}

void
FOdysseyMutator::ApplyMutation(TSharedPtr<IOdysseyMutation> iMutation)
{
    iMutation->Apply();
}

void
FOdysseyMutator::AddAndApplyMutation(TSharedPtr<IOdysseyMutation> iMutation)
{
    AddMutation(iMutation);
    ApplyMutation(iMutation);
}

void
FOdysseyMutator::Change()
{
    if (mRootMutation->GetMutations().Num() <= 0)
        return;
    
    mRootMutation->OnChanged().ExecuteIfBound();
}

void
FOdysseyMutator::Commit()
{
    if (mRootMutation->GetMutations().Num() <= 0)
        return;
    
    mRootMutation->OnChanged().ExecuteIfBound();

    if ( mGenerateUndo && GEditor->IsTransactionActive() )
        GUndo->StoreUndo(mObject, MakeUnique<FOdysseyMutationsUndo>(mRootMutation));

    mRootMutation->OnCommited().ExecuteIfBound();
    Reset();
}

void
FOdysseyMutator::Revert()
{
    mRootMutation->Revert();
    Reset();
}

void
FOdysseyMutator::Reset()
{
    mRootMutation = MakeShared<FOdysseyRootMutation>(mRootMutation->GetName());
}

//=====================================

FOdysseyRootMutation::FOdysseyRootMutation(const FString& iName)
    : mName(iName)
{
    
}

FSimpleDelegate&
FOdysseyRootMutation::OnChanged()
{
    return mOnChanged;
}

FSimpleDelegate&
FOdysseyRootMutation::OnCommited()
{
    return mOnCommited;
}

bool
FOdysseyRootMutation::IsDirty() const
{
    if (mMutations.IsEmpty())
        return false;

    for (int i = 0; i < mMutations.Num(); i++)
    {
        if (mMutations[i]->IsDirty())
            return true;
    }

    return false;
}

void
FOdysseyRootMutation::Apply()
{
    for (int i = 0; i < mMutations.Num(); i++)
    {
        if (!mMutations[i]->IsDirty())
            continue;

        mMutations[i]->Apply();
    }
}

void
FOdysseyRootMutation::Revert()
{
    for (int i = mMutations.Num() - 1; i >= 0; i--)
    {
        if (!mMutations[i]->IsDirty())
            continue;
            
        mMutations[i]->Revert();
    }
}

const FString&
FOdysseyRootMutation::GetName() const
{
    return mName;
}

void
FOdysseyRootMutation::AddMutation(TSharedPtr<IOdysseyMutation> iMutation)
{
    mMutations.Add(iMutation);
}

const TArray<TSharedPtr<IOdysseyMutation>>&
FOdysseyRootMutation::GetMutations() const
{
    return mMutations;
}
