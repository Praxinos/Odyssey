// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "ModeToolbar/FOdysseyViewportDrawingEditorToolbar.h"

#define LOCTEXT_NAMESPACE "OdysseyViewportDrawingEditorModeToolbar"


/////////////////////////////////////////////////////
// FOdysseyViewportDrawingEditorModeToolbar
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyViewportDrawingEditorModeToolbar::FOdysseyViewportDrawingEditorModeToolbar() :
    CurrentMesh(nullptr),
    CurrentLOD(-1),
    CurrentUV(-1),
    MeshColor( FLinearColor( 0.8f, 0.8f, 0.8f, 0.25f ))
{
}


FOdysseyViewportDrawingEditorModeToolbar::~FOdysseyViewportDrawingEditorModeToolbar()
{
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------- Getter / Setter

UStaticMesh* FOdysseyViewportDrawingEditorModeToolbar::GetCurrentMesh() const
{
    return CurrentMesh;
}

void FOdysseyViewportDrawingEditorModeToolbar::SetCurrentMesh( UStaticMesh* InCurrentMesh )
{
    CurrentMesh = InCurrentMesh;
}

int FOdysseyViewportDrawingEditorModeToolbar::GetCurrentLOD() const
{
    return CurrentLOD;
}


int FOdysseyViewportDrawingEditorModeToolbar::GetCurrentUVChannel() const
{
    return CurrentUV;
}

int FOdysseyViewportDrawingEditorModeToolbar::GetMaxLOD() const
{
    if( !CurrentMesh )
        return -1;

    if( !CurrentMesh->RenderData )
        return -1;

    return CurrentMesh->RenderData->LODResources.Num();
}

int FOdysseyViewportDrawingEditorModeToolbar::GetMaxUVChannelForCurrentLOD()
{
    int NumLODLevels = CurrentMesh->RenderData->LODResources.Num();

    if( CurrentLOD < 0 || CurrentLOD > NumLODLevels )
        return -1;


    return CurrentMesh->RenderData->LODResources[CurrentLOD].VertexBuffers.StaticMeshVertexBuffer.GetNumTexCoords();
}

FLinearColor FOdysseyViewportDrawingEditorModeToolbar::GetMeshColor() const
{
    return MeshColor;
}

//CALLBACKS -------------------------------------------


void FOdysseyViewportDrawingEditorModeToolbar::SetMeshColor( FLinearColor InNewColor )
{
    MeshColor = InNewColor;
}

void FOdysseyViewportDrawingEditorModeToolbar::SetCurrentLOD(int InNewLOD)
{
    CurrentLOD = FMath::Clamp(InNewLOD, -1, GetMaxLOD());
}

void FOdysseyViewportDrawingEditorModeToolbar::SetCurrentUVChannel(int InNewUV)
{
    CurrentUV = FMath::Clamp(InNewUV, -1, GetMaxUVChannelForCurrentLOD());
}

ECheckBoxState FOdysseyViewportDrawingEditorModeToolbar::GetLODCheckState(int InLOD)
{
    return (CurrentLOD == InLOD ? ECheckBoxState::Checked : ECheckBoxState::Unchecked);
}

ECheckBoxState FOdysseyViewportDrawingEditorModeToolbar::GetUVChannelCheckState(int InUVChannel)
{
    return (CurrentUV == InUVChannel ? ECheckBoxState::Checked : ECheckBoxState::Unchecked);
}

#undef LOCTEXT_NAMESPACE
