// IDDN FR.001.250001.004.S.X.2019.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "Widgets/SCompoundWidget.h"

//---

class FCinematicBoardSection;

class EPOSTRACKSEDITOR_API SCinematicBoardSectionPlane
    : public SCompoundWidget
{
public:
    static float GetHeight( TSharedRef<const FCinematicBoardSection> iBoardSection );

public:
    SLATE_BEGIN_ARGS( SCinematicBoardSectionPlane )
        : _Binding()
        {}
        SLATE_ARGUMENT( FMovieScenePossessable, Binding )
    SLATE_END_ARGS()

    // Construct the widget
    void Construct(const FArguments& InArgs, TSharedRef<FCinematicBoardSection> iBoardSection);

public:

private:
    TSharedPtr<FCinematicBoardSection> mBoardSection;

    FMovieScenePossessable mBinding;
};

//---

class EPOSTRACKSEDITOR_API SCinematicBoardSectionPlanes
    : public SCompoundWidget
{
public:
    static float GetHeight( TSharedRef<const FCinematicBoardSection> iBoardSection );

public:
    SLATE_BEGIN_ARGS( SCinematicBoardSectionPlanes )
        {}
    SLATE_END_ARGS()

    // Construct the widget
    void Construct( const FArguments& InArgs, TSharedRef<FCinematicBoardSection> iBoardSection );

private:
    TSharedPtr<FCinematicBoardSection> mBoardSection;
};
