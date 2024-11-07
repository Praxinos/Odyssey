====================
**Getting Started**
====================

Minimal Requirements
====================

| Here you will find everything you need to retrieve and edit ILIAD's code.
| Please follow this guide in the presented order.
| In order to make everything work, you have to follow these minimal requierements:

- Install `Git <https://git-scm.com/downloads>`_
- Install `Python 3.9+ <https://www.python.org/downloads/>`_
- Install `CMake 3.19+ <https://cmake.org/download/>`_

Install Unreal Engine
=====================

To install Unreal Engine, you need to :

- Install `Epic Game's Launcher <https://www.epicgames.com/store/fr/download>`_
- Create an account with you e-mail address (if not yet created)
- Log in Epic Game's Launcher
- Click on the "Unreal Engine" Tab and then on "Library"

.. image:: images/UnrealEngineTab.png

- Click on the "+" icon near "Engine versions"

.. image:: images/UnrealEngineVersion.png

- In the created widget, select the version you need (usually the latest one) and click on "Install"

Install Unreal Engine's Debug Symbols
=====================================

| Installing Unreal Engine's Debug Symbols will help you navigate the Unreal Engine's code and retrieve some informations from it when debugging.
| As Unreal Engine is always compiled in some kind of Release mode, you will not get a 100% accurate debugging experience when navigating Unreal Engine's code.
| To install Unreal Engine's Debug Symbols for your current version of Unreal Engine, you need to :

- Open your Epic Games Launcher
- Go in "Unreal Engine" and then "Library"
  
.. image:: images/UnrealEngineTab.png

- Click the little arrow near the "Launch" button of the Unreal Engine version you want to install the Debug Symbols in.
- Select "Options"

.. image:: images/UnrealEngineOptions.png

- In the displayed list, check "Editor Symbols for debugging"

.. image:: images/UnrealEngineDebugSymbols.png

- Click "Apply" and wait for the symbols to be downloaded

Install Visual Studio
=====================

To edit and compile ILIAD, you need to install `Visual Studio <https://visualstudio.microsoft.com/fr/vs/community/>`_ (Community version is enough).

When installing Visual Studio, please check the following options :
- Desktop .NET Development
- Desktop C++ Development
- Game C++ Development

.. image:: images/VisualStudioInstallationOptions.png

Create an Unreal Engine Project
===============================

To create an Unreal Engine Project :

- Open Epic Game's Launcher
- Click on "Unreal Engine" and then "Library"
  
.. image:: images/UnrealEngineTab.png

- Click the "Launch" button of the Unreal Engine version you want to create a project in.

.. image:: images/UnrealEngineLaunch.png

- Select your new Project's Category (usually Games) and click "Next"

.. image:: images/UnrealEngineProjectCategory.png

- Select the template you want to use or Blank and click "Next"
- Select "C++" as the Project type, select any other option as you wish

.. image:: images/UnrealEngineCPP.png

- Define the location and name of you project and click "Next"

| Your project will then automatically open both in Unreal Engine and in Visual Studio.
| You can close both before continuing this guide.

.. important:: Be sure to launch the Unreal Engine version corresponding to the Git branch on which you will be working.
.. important:: Selecting "C++" as the project type is what will allow the project to be opened in Visual Studio Code and include ILIAD for development.

Add ILIAD to an Unreal Engine Project
=====================================

.. important:: Make sure your Unreal Engine project is a C++ project.

To include ILIAD in an Unreal Engine Project for development :

- Retrieve ILIAD from Github
- In your Unreal Engine Project directory and create a "Plugins" folder.
 
.. image:: images/UnrealEngineProjectFolder.png

- Copy you ILIAD folder or create a symbolic link to your ILIAD folder
- In your Unreal Engine Project directory, right-click on your project ".uproject" file and select "Generate Visual Studio Project Files"

.. image:: images/UnrealEngineGenerateProjectFiles.png

- In your Unreal Engine Project directory, open your ".sln" file with Visual Studio
- In Visual Studio you should now see ILIAD under "Games/YourProjectName/Plugins"
- In Visual Studio Compile your project in "DebugGame Editor" or "Development Editor" configuration.
  
.. note:: On Windows, creating a simple link does not work, you need to create a symbolic link in command line with ``mklink /D "Your/ILIAD/Folder" ILIAD``
