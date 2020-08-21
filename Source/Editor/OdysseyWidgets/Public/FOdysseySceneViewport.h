// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#pragma once

#include "CoreMinimal.h"
#include "Slate/SceneViewport.h"

/////////////////////////////////////////////////////
// FOdysseySceneViewport
class ODYSSEYWIDGETS_API FOdysseySceneViewport : public FSceneViewport
{
public:
    FOdysseySceneViewport( FViewportClient* InViewportClient, TSharedPtr<SViewport> InViewportWidget );
protected:
    virtual FReply OnMouseButtonDown( const FGeometry& InGeometry, const FPointerEvent& MouseEvent ) override;
    virtual FReply OnMouseButtonUp( const FGeometry& InGeometry, const FPointerEvent& MouseEvent ) override;
};
