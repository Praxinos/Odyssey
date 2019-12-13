// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000
#pragma once

#include "CoreMinimal.h"
#include "Widgets/SWidget.h"
#include "OdysseyDiffUtils.h"

class FSCSEditorTreeNode;
class SKismetInspector;
class SSCSEditor;

class ODYSSEYBRUSHEDITOR_API FSCSDiff
{
public:
    FSCSDiff(const class UBlueprint* InOdysseyBrush);

    void HighlightProperty(FName VarName, FPropertySoftPath Property);
    TSharedRef< SWidget > TreeWidget();

    TArray< FSCSResolvedIdentifier > GetDisplayedHierarchy() const;

protected:
    void OnSCSEditorUpdateSelectionFromNodes(const TArray< TSharedPtr<class FSCSEditorTreeNode> >& SelectedNodes);
    void OnSCSEditorHighlightPropertyInDetailsView(const class FPropertyPath& InPropertyPath);

private:
    TSharedPtr< class SWidget > ContainerWidget;
    TSharedPtr< class SSCSEditor > SCSEditor;
    TSharedPtr< class SKismetInspector > Inspector;
};
