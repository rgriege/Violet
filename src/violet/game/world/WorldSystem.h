#ifndef WORLD_SYSTEM_H
#define WORLD_SYSTEM_H

#include "violet/core/system/System.h"
#include "violet/core/time/Time.h"
#include "violet/game/Config.h"

namespace Violet
{
	class Component;
	class Deserializer;
	class Engine;
	class Entity;
	class SystemFactory;
}

class VIOLET_GAME_API WorldSystem : public Violet::System
{
public:

	static void install(Violet::SystemFactory & factory);
	static std::unique_ptr<System> init(Violet::Deserializer & deserializer);
	static const char * getStaticLabel();

public:

	WorldSystem(float timeScale);

	virtual bool owns(const char * label) const override;
	virtual bool has(const char * label, const Violet::Entity & entity) const override;
	virtual Violet::Component & fetch(const char * label, const Violet::Entity & entity) override;
	virtual void update(float dt, Violet::Engine & engine) override;
	virtual void clear() override;

	const Violet::Time & getTime() const;
	float getTimeScale() const;
	void setTimeScale(float timeScale);

private:

	Violet::Time m_time;
	float m_timeScale;
};

#endif