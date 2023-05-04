// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "Mutations/OdysseyMutation.h"

class FOdysseyRootMutation;

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
    
    void Commit();
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
    FSimpleDelegate& OnMutated();

public:
    static const FGuid& StaticID();

    //A unique identifier for the mutation type
    virtual const FGuid& GetID() override;

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
    FSimpleDelegate mOnMutated;
};
