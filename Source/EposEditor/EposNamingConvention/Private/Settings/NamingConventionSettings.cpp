// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Settings/NamingConventionSettings.h"

#define LOCTEXT_NAMESPACE "NamingConventionSettings"

//---

static FPatternKeywordList sgAnimationPatternKeywordList;

const FPatternKeywordList& GetNamingConventionAnimationPatternKeywordList()
{
    if( !sgAnimationPatternKeywordList.mKeywordList.Num() )
    {
        sgAnimationPatternKeywordList.AddKeyword( ENamingConventionAnimationPatternKeyword::AnimationIndex   , "animation-index" , LOCTEXT( "animation-pattern-keyword.animation-index", "an incremental index" ) );
        //sgAnimationPatternKeywordList.AddKeyword( ENamingConventionAnimationPatternKeyword::CameraName   , "camera-name" , LOCTEXT( "animation-pattern-keyword.camera-name", "the name of the camera (won't update if camera name changes)" ) );
        //sgAnimationPatternKeywordList.AddKeyword( ENamingConventionAnimationPatternKeyword::ShotName     , "shot-name"   , LOCTEXT( "animation-pattern-keyword.shot-name", "the name of the shot (won't update if shot name changes)" ) );
    }

    return sgAnimationPatternKeywordList;
}

FNamingConventionAnimation::FNamingConventionAnimation()
{
    mPatternKeywordLists.AddKeywordList( &GetNamingConventionAnimationPatternKeywordList() );

    Pattern = FString::Printf( TEXT( "animation_%s" )
                               , *mPatternKeywordLists.GetKeyword( ENamingConventionAnimationPatternKeyword::AnimationIndex ).mKeywordWithBraces
    );
}

//---

static FPatternKeywordList sgCameraPatternKeywordList;

const FPatternKeywordList& GetNamingConventionCameraPatternKeywordList()
{
    if( !sgCameraPatternKeywordList.mKeywordList.Num() )
    {
        sgCameraPatternKeywordList.AddKeyword( ENamingConventionCameraPatternKeyword::CameraIndex , "camera-index", LOCTEXT( "camera-pattern-keyword.camera-index", "an incremental index" ) );
        //sgCameraPatternKeywordList.AddKeyword( ENamingConventionCameraPatternKeyword::ShotName    , "shot-name"   , LOCTEXT( "camera-pattern-keyword.shot-name", "the name of the shot (won't update if shot name changes)" ) );
    }

    return sgCameraPatternKeywordList;
}

FNamingConventionCamera::FNamingConventionCamera()
{
    mPatternKeywordLists.AddKeywordList( &GetNamingConventionCameraPatternKeywordList() );

    Pattern = FString::Printf( TEXT( "camera_%s" )
                               , *mPatternKeywordLists.GetKeyword( ENamingConventionCameraPatternKeyword::CameraIndex ).mKeywordWithBraces
    );
}

//---

static FPatternKeywordList sgCommonPatternKeywordList;

const FPatternKeywordList& GetNamingConventionCommonPatternKeywordList()
{
    if( !sgCommonPatternKeywordList.mKeywordList.Num() )
    {
        sgCommonPatternKeywordList.AddKeyword( ENamingConventionCommonPatternKeyword::StudioName        , "studio-name"         , LOCTEXT( "shot-pattern-keyword.studio-name", "the full studio name" ) );
        sgCommonPatternKeywordList.AddKeyword( ENamingConventionCommonPatternKeyword::StudioAcronym     , "studio-acronym"      , LOCTEXT( "shot-pattern-keyword.studio-acronym", "the studio name acronym" ) );
        sgCommonPatternKeywordList.AddKeyword( ENamingConventionCommonPatternKeyword::LicenseName       , "license-name"        , LOCTEXT( "shot-pattern-keyword.license-name", "the full license name" ) );
        sgCommonPatternKeywordList.AddKeyword( ENamingConventionCommonPatternKeyword::LicenseAcronym    , "license-acronym"     , LOCTEXT( "shot-pattern-keyword.license-acronym", "the license name acronym" ) );
        sgCommonPatternKeywordList.AddKeyword( ENamingConventionCommonPatternKeyword::ProductionName    , "production-name"     , LOCTEXT( "shot-pattern-keyword.production-name", "the full production title" ) );
        sgCommonPatternKeywordList.AddKeyword( ENamingConventionCommonPatternKeyword::ProductionAcronym , "production-acronym"  , LOCTEXT( "shot-pattern-keyword.production-acronym", "the production title acronym" ) );
        sgCommonPatternKeywordList.AddKeyword( ENamingConventionCommonPatternKeyword::Season            , "season"              , LOCTEXT( "shot-pattern-keyword.season", "the season number" ) );
        sgCommonPatternKeywordList.AddKeyword( ENamingConventionCommonPatternKeyword::Episode           , "episode"             , LOCTEXT( "shot-pattern-keyword.episode", "the episode number" ) );
        sgCommonPatternKeywordList.AddKeyword( ENamingConventionCommonPatternKeyword::Part              , "part"                , LOCTEXT( "shot-pattern-keyword.part", "the part (A, B, C, ...)" ) );
        //sgCommonPatternKeywordList.AddKeyword( ENamingConventionCommonPatternKeyword::DepartmentName    , "department-name"     , LOCTEXT( "shot-pattern-keyword.department-name", "the full department name" ) );
        //sgCommonPatternKeywordList.AddKeyword( ENamingConventionCommonPatternKeyword::DepartmentAcronym , "department-acronym"  , LOCTEXT( "shot-pattern-keyword.department-acronym", "the department name acronym" ) );
        sgCommonPatternKeywordList.AddKeyword( ENamingConventionCommonPatternKeyword::Initials          , "initials"            , LOCTEXT( "shot-pattern-keyword.initials", "some initials" ) );
    }

    return sgCommonPatternKeywordList;
}

//---

static FPatternKeywordList sgShotPatternKeywordList;

const FPatternKeywordList& GetNamingConventionShotPatternKeywordList()
{
    if( !sgShotPatternKeywordList.mKeywordList.Num() )
    {
        sgShotPatternKeywordList.AddKeyword( ENamingConventionShotPatternKeyword::ShotIndex         , "shot-index"          , LOCTEXT( "shot-pattern-keyword.shot-index", "an incremental index" ) );
        sgShotPatternKeywordList.AddKeyword( ENamingConventionShotPatternKeyword::TakeIndex         , "take-index"          , LOCTEXT( "shot-pattern-keyword.take-index", "an incremental index for take" ) );
    }

    return sgShotPatternKeywordList;
}

FNamingConventionShot::FNamingConventionShot()
{
    mPatternKeywordLists.AddKeywordList( &GetNamingConventionShotPatternKeywordList() );
    mPatternKeywordLists.AddKeywordList( &GetNamingConventionCommonPatternKeywordList() );

    Pattern = FString::Printf( TEXT( "shot_%s_%s" )
                               , *mPatternKeywordLists.GetKeyword( ENamingConventionShotPatternKeyword::ShotIndex ).mKeywordWithBraces
                               , *mPatternKeywordLists.GetKeyword( ENamingConventionShotPatternKeyword::TakeIndex ).mKeywordWithBraces
    );
}

//---

static FPatternKeywordList sgBoardPatternKeywordList;

const FPatternKeywordList& GetNamingConventionBoardPatternKeywordList()
{
    if( !sgBoardPatternKeywordList.mKeywordList.Num() )
    {
        sgBoardPatternKeywordList.AddKeyword( ENamingConventionBoardPatternKeyword::BoardIndex, "board-index", LOCTEXT( "board-pattern-keyword.board-index", "an incremental index" ) );
    }

    return sgBoardPatternKeywordList;
}

FNamingConventionBoard::FNamingConventionBoard()
{
    mPatternKeywordLists.AddKeywordList( &GetNamingConventionBoardPatternKeywordList() );
    mPatternKeywordLists.AddKeywordList( &GetNamingConventionCommonPatternKeywordList() );

    Pattern = FString::Printf( TEXT( "board_%s" )
                               , *mPatternKeywordLists.GetKeyword( ENamingConventionBoardPatternKeyword::BoardIndex ).mKeywordWithBraces
    );
}

//---

FName
UNamingConventionSettings::GetContainerName() const
{
    return TEXT( "Editor" );
}

FName
UNamingConventionSettings::GetCategoryName() const
{
    return TEXT( "Plugins" );
}

#undef LOCTEXT_NAMESPACE
