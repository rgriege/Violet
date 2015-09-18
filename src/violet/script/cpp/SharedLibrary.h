#ifndef VIOLET_SharedLibrary_H
#define VIOLET_SharedLibrary_H

#include "violet/Defines.h"
#include "violet/utility/unique_val.h"

#include <memory>
#include <string>

namespace Violet
{
    class VIOLET_API SharedLibrary
    {
    private:

        class Implementation;

    public:

        static std::shared_ptr<SharedLibrary> load(const char * filename);
        static const char * getSuffix();

    public:

        ~SharedLibrary();

        std::string getFilename() const;
        void * getMethodPtr(const char * name);

    private:

        SharedLibrary(unique_val<Implementation> && impl);

    private:

        unique_val<Implementation> m_impl;
    };
}

#endif
