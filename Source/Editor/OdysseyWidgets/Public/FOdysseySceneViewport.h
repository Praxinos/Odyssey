// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

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
	virtual FNavigationReply OnNavigation(const FGeometry& MyGeometry, const FNavigationEvent& InNavigationEvent) override;
};
