#ifndef WORLD_SYSTEM_H
#define WORLD_SYSTEM_H

#include "violet/system/system.h"
#include "violet/time/time.h"
#include "game/config.h"

#include <Memory>

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

	static void install(Violet::SystemFactory & Factory);
	static void init(Violet::Deserializer & Deserializer);
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