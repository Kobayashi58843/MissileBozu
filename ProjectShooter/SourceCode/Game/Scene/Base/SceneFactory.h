#pragma once

#include "..\\..\\Global.h"

#include "..\\StartingScene.h"
#include "..\\TitleScene.h"
#include "..\\StartEventScene.h"
#include "..\\ActionScene.h"
#include "..\\WinningScene.h"
#include "..\\ClearScene.h"
#include "..\\LoserScene.h"
#include "..\\ContinueScene.h"
#include "..\\OverScene.h"

class SceneFactory
{
public:
	SceneFactory();
	~SceneFactory();

	BaseScene* Create(const enSwitchToNextScene enNextScene, const SCENE_NEED_POINTER ScenePointerGroup);

private:

};