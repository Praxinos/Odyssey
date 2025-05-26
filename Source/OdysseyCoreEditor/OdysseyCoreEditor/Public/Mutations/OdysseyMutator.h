// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

class FOdysseyRootMutation;

class ODYSSEYCOREEDITOR_API IOdysseyMutation
{
public:
    virtual ~IOdysseyMutation() {}

public:
    //Returns true if the mutation actually changes something
    //Returns false if the mutation does nothing
    //example : if we create a mutation to change a cell's length, but adjust the mutation later so that the length does not change
    //          the mutation still exists even if it does nothing
    virtual bool IsDirty() const { return true; }

    //Applies the mutation
    virtual void Apply() = 0;

    //Reverts the mutation (Undo)
    virtual void Revert() = 0;
};

class ODYSSEYCOREEDITOR_API FOdysseyMutator
{
public:
    //Destructor
    virtual ~FOdysseyMutator();

    //Constructor
    FOdysseyMutator(UObject* iObject, const FString& iName, bool iGenerateUndo = true);

public:
    //Getters
    TSharedPtr<FOdysseyRootMutation> GetRootMutation() const;
    bool IsDirty() const;

public:
    //Setters
    void AddMutation(TSharedPtr<IOdysseyMutation> iMutation);
    void ApplyMutation(TSharedPtr<IOdysseyMutation> iMutation);
    void AddAndApplyMutation(TSharedPtr<IOdysseyMutation> iMutation);

    //Defines the current state as an intermediate state (non commited)
    //Allows for UI to change interactively while changing values
    virtual void Change();

    //Commits the current state as the definitive value
    virtual void Commit();

    //Reverts the all the mutations stored by the mutator and resets the mutator
    virtual void Revert();

    //Reset the mutator, removing all mutations
    virtual void Reset();

private:
    UObject* mObject;
    TSharedPtr<FOdysseyRootMutation> mRootMutation;
    bool mGenerateUndo;
};

class ODYSSEYCOREEDITOR_API FOdysseyRootMutation
    : public IOdysseyMutation
{
public:
    FOdysseyRootMutation(const FString& iName);

public:
    FSimpleDelegate& OnChanged();
    FSimpleDelegate& OnCommited();

public:
    //Applies the mutation
    virtual bool IsDirty() const override;

    //Applies the mutation
    virtual void Apply() override;

    //Reverts the mutation (Undo)
    virtual void Revert() override;

    const FString& GetName() const;

    void AddMutation(TSharedPtr<IOdysseyMutation> iMutation);

public:
    const TArray<TSharedPtr<IOdysseyMutation>>& GetMutations() const;

public:
    FString mName;
    TArray<TSharedPtr<IOdysseyMutation>> mMutations;
    FSimpleDelegate mOnChanged;
    FSimpleDelegate mOnCommited;
};

template <typename T>
class FOdysseyMutation
    : public IOdysseyMutation
{
public:
    DECLARE_DELEGATE_OneParam(FMutationDelegate, TSharedPtr<T>)

public:
    virtual ~FOdysseyMutation() {}
    FOdysseyMutation(TSharedPtr<T> iData, FMutationDelegate iApply, FMutationDelegate iRevert)
        : mData(iData)
        , mApply(iApply)
        , mRevert(iRevert)
    {
    }

public:
    //Applies the mutation
    virtual void Apply() override
    {
        mApply.ExecuteIfBound(mData);
    }

    //Reverts the mutation (Undo)
    virtual void Revert() override
    {
        mRevert.ExecuteIfBound(mData);
    }

private:
    TSharedPtr<T> mData;
    FMutationDelegate mApply;
    FMutationDelegate mRevert;
};
