#ifndef WORLD_SYSTEM_H
#define WORLD_SYSTEM_H

#include "violet/system/System.h"
#include "violet/time/Time.h"
#include "game/Config.h"

#include <memory>

namespace Violet
{
	class Deserializer;
	class Engine;
	class Entity;
	class SystemFactory;
}

class VIOLET_GAME_API WorldSystem : public Violet::System
{
public:

	static void install(Violet::SystemFactory & factory);
	static void init(Violet::Deserializer & deserializer);
	static const char * getStaticLabel();

public:

	WorldSystem(float timeScale);

	virtual void update(float dt) override;

	const Violet::Time & getTime() const;
	float getTimeScale() const;
	void setTimeScale(float timeScale);

private:

	Violet::Time m_time;
	float m_timeScale;
};

#endif