#ifndef ENGINE_SYSTEM_H
#define ENGINE_SYSTEM_H

#include "violet/core/Engine.h"
#include "violet/core/system/System.h"

class EngineSystem : public Violet::System
{
public:

	static void install(Violet::SystemFactory & factory);
	static std::unique_ptr<Violet::System> init(Violet::Deserializer & deserializer);
	static const char * getStaticLabel();

public:

	EngineSystem(std::unique_ptr<Violet::Engine> && engine);

	void loadScene(const char * filename);

	virtual void update(float dt, Violet::Engine & engine) override;

private:

	std::unique_ptr<Violet::Engine> m_engine;
};

#endif