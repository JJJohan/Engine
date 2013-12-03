#include "Application/Application.h"

class App : public Engine::Application
{
public:
	App();
	virtual void Config();
	virtual void Update(const float a_delta);
	virtual void Draw();
	virtual void Input();
	virtual void Shutdown();
};