#include "editor/EditorSystem.h"
#include "violet/Engine.h"
#include "violet/log/Log.h"
#include "violet/log/LogTarget.h"
#include "violet/script/cpp/CppScript.h"
#include "violet/ui/UiList.h"
#include "violet/update/system/UpdateSystem.h"
#include "violet/utility/FormattedString.h"

#include "log.h"

#include <functional>

using namespace Violet;

class LocalTarget : public LogTarget
{
public:

    LocalTarget(std::deque<std::string> & logEntries) :
        LogTarget(),
        m_logEntries(logEntries)
    {
    }

    virtual ~LocalTarget() = default;

    virtual void log(const char * entry) override
    {
        m_logEntries.emplace_back(entry);

        while (m_logEntries.size() > 8)
            m_logEntries.pop_front();
    }

private:

    std::deque<std::string> & m_logEntries;
};

class Instance : public CppScript::Instance
{
public:

    Instance(CppScript & script, const char * elementFile, uint32 elementHeight) :
        CppScript::Instance(script),
        m_logEntries(),
        m_logTargetGuard(Log::installTarget(std::make_unique<LocalTarget>(m_logEntries))),
        m_list(elementFile, elementHeight)
    {
        using namespace std::placeholders;
        GetLogEntryMethod::assign(script, std::bind(&Instance::onGetLogEntry, this, _1));
        UpdateMethod::assign(script, std::bind(&Instance::onUpdate, this, _1));
    }

    virtual ~Instance() override
    {
        GetLogEntryMethod::remove(m_script);
        UpdateMethod::remove(m_script);
        m_list.clean(m_script);
    }

private:

    std::string const & onGetLogEntry(const uint32 index)
    {
        static const std::string s_empty;
        return index < m_logEntries.size() ? m_logEntries[index] : s_empty;
    }

    void onUpdate(const Entity & entity)
    {
        m_list.update(entity, m_logEntries.size());
    }

private:

    std::deque<std::string> m_logEntries;
    const LogTarget::Guard m_logTargetGuard;
    UiList m_list;
};

VIOLET_SCRIPT_EXPORT void init(CppScript & script, std::unique_ptr<CppScript::Instance> & instance)
{
    instance = std::make_unique<Instance>(script, "logEntry.json", 20);
}

