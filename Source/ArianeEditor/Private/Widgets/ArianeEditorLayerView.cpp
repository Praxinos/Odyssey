// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

// Ariane Headers
#include "ArianeEditorLayerView.h"
#include "ArianeEditor.h"
#include "ArianePainting3DComponent.h"
#include "ArianeLayerStack.h"
#include "ArianeLayerDrawing.h"
// Unreal Headers

#define LOCTEXT_NAMESPACE "ArianeEditor"

void
UArianeEditorLayerView::SetEditor( FArianeEditor* InEditor )
{
    Editor = InEditor;
}

void
UArianeEditorLayerView::ImportLayerProperties( UArianeLayer* Layer )
{
    Transform = Layer->GetRelativeTransform();
}

void
UArianeEditorLayerView::PostEditChangeLayerProperty( UArianeLayer* Layer
                                                   , FPropertyChangedEvent& PropertyChangedEvent )
{
    FName PropertyName = PropertyChangedEvent.GetPropertyName();
    FName MemberPropertyName = PropertyChangedEvent.GetMemberPropertyName();

    if( PropertyName == GET_MEMBER_NAME_CHECKED( UArianeEditorLayerView, Transform ) )
    {
        Layer->SetRelativeTransform( Transform );
    }
}

void
UArianeEditorLayerView::PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent)
{
    UArianePainting3DComponent* Painting3DComponent = Editor->GetCurrentPainting3DComponent();
    UArianeLayerStack* LayerStack = Painting3DComponent->GetLayerStack();
    const TArray<UArianeLayer*>& SelectedLayers = LayerStack->GetSelectedLayers();

    for( UArianeLayer* Layer : SelectedLayers )
    {
        PostEditChangeLayerProperty( Layer, PropertyChangedEvent );
    }

    // Note: this will trigger OnPreUpdateDelegate and OnPostUpdateDelegate.
    // Any widget that has registrerd to these delegates can update itself.
    // for example the LayerTransformTool will update its gizmo.
    Painting3DComponent->Update( PropertyChangedEvent.ChangeType & EPropertyChangeType::Interactive );
}

#undef LOCTEXT_NAMESPACE
