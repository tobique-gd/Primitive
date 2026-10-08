#include "core/guid.h"

#include <random>

namespace Engine
{
    GUID::GUID()
    {
        static std::random_device rd;
        static std::mt19937_64 gen(rd());
        static std::uniform_int_distribution<uint64_t> dis;

        m_GUID = dis(gen);

        if (m_GUID == 0)
            m_GUID = 1;
    }

    GUID::GUID(uint64_t guid)
        : m_GUID(guid)
    {
    }

    const GUID GUID::INVALID = GUID(0);
}
