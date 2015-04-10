#ifndef SUB_WINDOW_SYSTEM_H
#define SUB_WINDOW_SYSTEM_H

#include "violet/core/window/WindowSystem.h"

class SubWindowSystem : public Violet::WindowSystem
{
public:

	static void install(Violet::SystemFactory & factory);
	static std::unique_ptr<Violet::System> init(Violet::Deserializer & deserializer);

public:

	SubWindowSystem(int width, int height);

	virtual ~SubWindowSystem() override = default;

	virtual void render() override;
	virtual bool getEvent(EventType type, Event* event) override;
	virtual void addEvent(Event event) override;
	virtual int getWidth() const override;
	virtual int getHeight() const override;

private:

	const int m_width;
	const int m_height;
};

#endif