#pragma once
#include "graphics\tac3acceleration.h"
#include "graphics\TacRenderer.h"
#include "core\tac3systemApplication.h"
#include "graphics\tac3light.h"
#include "tac3util\tac3utilities.h"
#include "tac3math\tac3boundingvolume.h"

namespace Tac
{
  class Entity;
  class Editor;
  class TweakBar;

  class SelectedEntity
  {
  public:
    SelectedEntity( Entity * gameObj );
    ~SelectedEntity();
    void LoadArchetype();
    void SaveArchetype();
    void SaveArchetypeAs();
    void AddComponent();
    void RemComponent();
    Entity * mGameObj;
  private:
    TweakBar * mTweakbar;

    std::vector<std::string> mComponentTypes;
    int mSelectedComponentIndex;
  };

  class Light;
  class Model;
  class TweakBar;
  class NormalMapGenerator;
  class CoolModelConverter;

  class Editor : public SystemApplication
  {
  public:
    Editor();
    virtual ~Editor();
    virtual std::string Init() override;
    virtual void Update(float dt) override;
    virtual void UnInit() override;

  private:
    TweakBar * mTweakbar;
    void OpenNormalMapGenerator();
    NormalMapGenerator * mNormalMapGen;
    void OpenModelConverter();
    CoolModelConverter * mModelConverter;

    std::string mLevelDisplayName;
    void OpenLevel();
    void SaveLevel();
    void SaveLevelAs();

    unsigned mEntityIndexToSelect;

    void DropSmallLight();


    void SelectEntity();
    void CreateEntity();
    void DeleteSelectedEntity();
    SelectedEntity * mSelectedEntity;

    void SplashEnter(); void SplashUpdate(float dt); void SplashExit();
    void MenuEnter(); void MenuUpdate(float dt); void MenuExit();
    void GameEnter(); void GameUpdate(float dt); void GameExit();
    void LobbyEnter(); void LobbyUpdate(float dt); void LobbyExit();
    enum State{ eSplash, eMenu, eGame, eLobby, eCount };
    StateMachine< Editor > mStatemachine;
  };


} // Tac

