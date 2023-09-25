// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

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
