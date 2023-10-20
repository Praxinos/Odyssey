// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "LayerStack/Cells/CellImageStagger/OdysseyAnimationCellImageStaggerMutator.h"

#include "LayerStack/Cells/CellImageStagger/OdysseyAnimationCellImageStagger.h"

class FOdysseySetBehaviourMutation
    : public IOdysseyMutation
{
public:
    FOdysseySetBehaviourMutation(TSharedPtr<FOdysseyAnimationCellImageStagger> iCell, FOdysseyAnimationCellImageStagger::eBehaviour iBehaviour)
        : mCell(iCell)
        , mOldBehaviour(iCell->mBehaviour)
        , mNewBehaviour(iBehaviour)
    {
    }

public:
    //Applies the mutation
    virtual void Apply() override
    {
        mCell->mBehaviour = mNewBehaviour;
    }

    //Reverts the mutation (Undo)
    virtual void Revert() override
    {
        mCell->mBehaviour = mOldBehaviour;
    }

public:
    TSharedPtr<FOdysseyAnimationCellImageStagger> mCell;
    FOdysseyAnimationCellImageStagger::eBehaviour mOldBehaviour;
    FOdysseyAnimationCellImageStagger::eBehaviour mNewBehaviour;
};

class FOdysseySetReachMutation
    : public IOdysseyMutation
{
public:
    FOdysseySetReachMutation(TSharedPtr<FOdysseyAnimationCellImageStagger> iCell, int iReach)
        : mCell(iCell)
        , mOldReach(iCell->mReach)
        , mNewReach(iReach)
    {
    }

public:
    //Applies the mutation
    virtual void Apply() override
    {
        mCell->mReach = mNewReach;
    }

    //Reverts the mutation (Undo)
    virtual void Revert() override
    {
        mCell->mReach = mNewReach;
    }

public:
    TSharedPtr<FOdysseyAnimationCellImageStagger> mCell;
    int mOldReach;
    int mNewReach;
};

FOdysseyAnimationCellImageStaggerMutator::FOdysseyAnimationCellImageStaggerMutator(UObject* iOwner, TSharedPtr<FOdysseyAnimationCellImageStagger> iCell)
    : FOdysseyMutator(iOwner, "FOdysseyAnimationCellImageStaggerMutator")
    , mCell(iCell)
{
    GetRootMutation()->OnCommited().BindLambda(
        [cell = mCell]()
        {
            cell->ImageRenderingCompositionChanged();
        }
    );
}

void
FOdysseyAnimationCellImageStaggerMutator::SetBehaviour(FOdysseyAnimationCellImageStagger::eBehaviour iBehaviour)
{
    TSharedPtr<FOdysseySetBehaviourMutation> mutation = MakeShared<FOdysseySetBehaviourMutation>(mCell, iBehaviour);
    AddAndApplyMutation(mutation);
}

void
FOdysseyAnimationCellImageStaggerMutator::SetReach(int iReach)
{
    TSharedPtr<FOdysseySetReachMutation> mutation = MakeShared<FOdysseySetReachMutation>(mCell, iReach);
    AddAndApplyMutation(mutation);
}
