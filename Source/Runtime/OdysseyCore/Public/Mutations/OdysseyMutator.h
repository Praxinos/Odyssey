// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

class FOdysseyRootMutation;

class ODYSSEYCORE_API IOdysseyMutation
{
public:
    virtual ~IOdysseyMutation() {}

public:
    //Applies the mutation
    virtual void Apply() = 0;

    //Reverts the mutation (Undo)
    virtual void Revert() = 0;
};

class ODYSSEYCORE_API FOdysseyMutator
{
public:
    //Destructor
    ~FOdysseyMutator();

    //Constructor
    FOdysseyMutator(UObject* iObject, const FString& iName, bool iGenerateUndo = true);

public:
    //Getters
    TSharedPtr<FOdysseyRootMutation> GetRootMutation() const;

public:
    //Setters
    void AddAndApplyMutation(TSharedPtr<IOdysseyMutation> iMutation);
    
    //Defines the current state as an intermediate state (non commited)
    //Allows for UI to change interactively while changing values
    void Change();

    //Commits the current state as the definitive value
    void Commit();

    //Aborts any change to go back to the original state
    void Abort();

private:
    UObject* mObject;
    TSharedPtr<FOdysseyRootMutation> mRootMutation;
    bool mGenerateUndo;
};

class ODYSSEYCORE_API FOdysseyRootMutation
    : public IOdysseyMutation
{
public:
    FOdysseyRootMutation(const FString& iName);

public:
    FSimpleDelegate& OnChanged();
    FSimpleDelegate& OnCommited();

public:
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
