// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyEditorLayoutBuilder.h"

FOdysseyEditorLayoutBuilder::FOdysseyEditorLayoutBuilder(const FName& iName)
    : mLayout(FTabManager::NewLayout(iName))
{
}

TSharedRef<FTabManager::FLayout>
FOdysseyEditorLayoutBuilder::GetLayout() const
{
    return mLayout.ToSharedRef();
}

TSharedRef<FTabManager::FArea>
FOdysseyEditorLayoutBuilder::CreateArea(const FName& iName)
{
    TSharedRef<FTabManager::FArea> area = FTabManager::NewPrimaryArea();
    mAreas.Add(iName, area);
    return area;
}

TSharedRef<FTabManager::FArea>
FOdysseyEditorLayoutBuilder::GetArea(const FName& iName) const
{
    return mAreas[iName].ToSharedRef();
}

TSharedRef<FTabManager::FSplitter>
FOdysseyEditorLayoutBuilder::CreateSplitter(const FName& iName)
{
    TSharedRef<FTabManager::FSplitter> splitter = FTabManager::NewSplitter();
    mSplitters.Add(iName, splitter);
    return splitter;
}

TSharedRef<FTabManager::FSplitter>
FOdysseyEditorLayoutBuilder::GetSplitter(const FName& iName) const
{
    return mSplitters[iName].ToSharedRef();
}

TSharedRef<FTabManager::FStack>
FOdysseyEditorLayoutBuilder::CreateStack(const FName& iName)
{
    TSharedRef<FTabManager::FStack> stack = FTabManager::NewStack();
    mStacks.Add(iName, stack);
    return stack;
}

TSharedRef<FTabManager::FStack>
FOdysseyEditorLayoutBuilder::GetStack(const FName& iName) const
{
    return mStacks[iName].ToSharedRef();
}
