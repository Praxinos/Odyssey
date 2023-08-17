// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

/**
 * Base class for a Painting Editor
 */
class ODYSSEYEDITOR_API FOdysseyEditorLayoutBuilder
{
public:
    FOdysseyEditorLayoutBuilder(const FName& iName);

public:
    TSharedRef<FTabManager::FLayout> GetLayout() const;
    
    TSharedRef<FTabManager::FArea> CreateArea(const FName& iName);
    TSharedRef<FTabManager::FArea> GetArea(const FName& iName) const;

    TSharedRef<FTabManager::FSplitter> CreateSplitter(const FName& iName);
    TSharedRef<FTabManager::FSplitter> GetSplitter(const FName& iName) const;

    TSharedRef<FTabManager::FStack> CreateStack(const FName& iName);
    TSharedRef<FTabManager::FStack> GetStack(const FName& iName) const;

private:
    TSharedPtr<FTabManager::FLayout> mLayout;
    TMap<FName, TSharedPtr<FTabManager::FArea>> mAreas;
    TMap<FName, TSharedPtr<FTabManager::FSplitter>> mSplitters;
    TMap<FName, TSharedPtr<FTabManager::FStack>> mStacks;
};