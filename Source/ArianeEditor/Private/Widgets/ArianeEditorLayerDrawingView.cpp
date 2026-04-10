// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

// Ariane Headers
#include "ArianeEditorLayerDrawingView.h"
#include "ArianeEditor.h"
#include "ArianePainting3DComponent.h"
#include "ArianeLayerStack.h"
#include "ArianeLayerDrawing.h"
// Unreal Headers

#define LOCTEXT_NAMESPACE "ArianeEditor"

void
UArianeEditorLayerDrawingView::ImportLayerProperties( UArianeLayer* Layer )
{
    UArianeLayerDrawing* DrawingLayer = Cast<UArianeLayerDrawing>(Layer);

    Super::ImportLayerProperties( Layer );

    if( DrawingLayer )
    {
        DrawingOrigin = DrawingLayer->GetDrawingOrigin();
        DrawingOrientation = DrawingLayer->GetDrawingOrientation();
    }
}

void
UArianeEditorLayerDrawingView::PostEditChangeLayerProperty( UArianeLayer* Layer
                                                          , FPropertyChangedEvent& PropertyChangedEvent )
{
    FName PropertyName = PropertyChangedEvent.GetPropertyName();
    FName MemberPropertyName = PropertyChangedEvent.GetMemberPropertyName();
    UArianeLayerDrawing* DrawingLayer = Cast<UArianeLayerDrawing>(Layer);

    Super::PostEditChangeLayerProperty( Layer, PropertyChangedEvent );

    if( DrawingLayer )
    {
        if( PropertyName == GET_MEMBER_NAME_CHECKED( UArianeEditorLayerDrawingView, DrawingOrigin ) )
        {
            DrawingLayer->SetDrawingOrigin( DrawingOrigin );
        }

        if( PropertyName == GET_MEMBER_NAME_CHECKED( UArianeEditorLayerDrawingView, DrawingOrientation ) )
        {
            DrawingLayer->SetDrawingOrientation( DrawingOrientation );
        }
    }
}

#undef LOCTEXT_NAMESPACE
