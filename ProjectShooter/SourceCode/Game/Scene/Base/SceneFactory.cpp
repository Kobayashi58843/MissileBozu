#include "SceneFactory.h"

SceneFactory::SceneFactory()
{
}

SceneFactory::~SceneFactory()
{
}

BaseScene* SceneFactory::Create(const enSwitchToNextScene enNextScene, const SCENE_NEED_POINTER ScenePointerGroup)
{
	assert(enNextScene >= enSwitchToNextScene::Starting);
	assert(enNextScene < enSwitchToNextScene::Nothing);

	switch (enNextScene)
	{
	case enSwitchToNextScene::Starting:
		return new StartingScene(ScenePointerGroup);

		break;
	case enSwitchToNextScene::Title:
		return new TitleScene(ScenePointerGroup);

		break;
	case enSwitchToNextScene::StartEvent:
		return new StartEventScene(ScenePointerGroup);

		break;
	case enSwitchToNextScene::Action:
		return new ActionScene(ScenePointerGroup);

		break;
	case enSwitchToNextScene::Winning:
		return new WinningScene(ScenePointerGroup);

		break;
	case enSwitchToNextScene::Clear:
		return new ClearScene(ScenePointerGroup);

		break;
	case enSwitchToNextScene::Loser:
		return new LoserScene(ScenePointerGroup);

		break;
	case enSwitchToNextScene::Continue:
		return new ContinueScene(ScenePointerGroup);

		break;
	case enSwitchToNextScene::Over:
		return new OverScene(ScenePointerGroup);

		break;
	}

	return nullptr;
}