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
    }

	/** Reverts change to the object */
    //UNDO
	virtual void Revert( UObject* Object ) override
    {
        mRootMutation->Revert();
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

void
FOdysseyMutator::AddAndApplyMutation(TSharedPtr<IOdysseyMutation> iMutation)
{
    iMutation->Apply();
    mRootMutation->AddMutation(iMutation);
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
    mRootMutation = MakeShared<FOdysseyRootMutation>(mRootMutation->GetName());
}

void
FOdysseyMutator::Abort()
{
    mRootMutation->Revert();
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

void
FOdysseyRootMutation::Apply()
{
    for (int i = 0; i < mMutations.Num(); i++)
    {
        mMutations[i]->Apply();
    }
    mOnChanged.ExecuteIfBound();
    mOnCommited.ExecuteIfBound();
}

void
FOdysseyRootMutation::Revert()
{
    for (int i = mMutations.Num() - 1; i >= 0; i--)
    {
        mMutations[i]->Revert();
    }
    mOnChanged.ExecuteIfBound();
    mOnCommited.ExecuteIfBound();
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
