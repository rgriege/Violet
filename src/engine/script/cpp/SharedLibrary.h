#ifndef VIOLET_SharedLibrary_H
#define VIOLET_SharedLibrary_H

#include "engine/utility/unique_val.h"

#include <memory>

namespace Violet
{
    class SharedLibrary
    {
    private:

        class Implementation;

    public:

        static std::shared_ptr<SharedLibrary> load(const char * filename);

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
