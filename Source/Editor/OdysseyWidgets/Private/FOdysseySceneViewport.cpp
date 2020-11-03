// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "FOdysseySceneViewport.h"

#include "Widgets/SViewport.h"

FOdysseySceneViewport::FOdysseySceneViewport( FViewportClient* InViewportClient, TSharedPtr<SViewport> InViewportWidget )
    : FSceneViewport(InViewportClient, InViewportWidget)
{
    
}

FReply
FOdysseySceneViewport::OnMouseButtonDown( const FGeometry& InGeometry, const FPointerEvent& MouseEvent )
{
    FReply reply = FSceneViewport::OnMouseButtonDown(InGeometry, MouseEvent);
    if (reply.IsEventHandled())
    {
        TSharedRef<SViewport> ViewportWidgetRef = GetViewportWidget().Pin().ToSharedRef();
        reply.CaptureMouse(ViewportWidgetRef);
    }
    return reply;
}

FReply
FOdysseySceneViewport::OnMouseButtonUp( const FGeometry& InGeometry, const FPointerEvent& MouseEvent )
{
    FReply reply = FSceneViewport::OnMouseButtonUp(InGeometry, MouseEvent);
    if (reply.IsEventHandled())
    {
        reply.ReleaseMouseCapture();
    }
    return reply;
}

FNavigationReply
FOdysseySceneViewport::OnNavigation(const FGeometry& MyGeometry, const FNavigationEvent& InNavigationEvent)
{
	return FNavigationReply::Escape();
}
