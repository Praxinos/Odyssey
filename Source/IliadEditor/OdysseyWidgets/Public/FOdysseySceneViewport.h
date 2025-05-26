// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "Slate/SceneViewport.h"

/////////////////////////////////////////////////////
// FOdysseySceneViewport
class FOdysseySceneViewport : public FSceneViewport
{
public:
    ODYSSEYWIDGETS_API FOdysseySceneViewport( FViewportClient* InViewportClient, TSharedPtr<SViewport> InViewportWidget );
protected:
    ODYSSEYWIDGETS_API virtual FReply OnMouseButtonDown( const FGeometry& InGeometry, const FPointerEvent& MouseEvent ) override;
    ODYSSEYWIDGETS_API virtual FReply OnMouseButtonUp( const FGeometry& InGeometry, const FPointerEvent& MouseEvent ) override;
    ODYSSEYWIDGETS_API virtual FNavigationReply OnNavigation(const FGeometry& MyGeometry, const FNavigationEvent& InNavigationEvent) override;
};
