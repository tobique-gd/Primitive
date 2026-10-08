#pragma once

#include <cstdint>
#include <string>

namespace Engine
{
    class GUID {
        public:
            static const GUID INVALID;

            GUID();
            explicit GUID(uint64_t guid);
            GUID(const GUID&) = default;
            GUID& operator=(const GUID&) = default;
            ~GUID() = default;

            bool operator==(const GUID& other) const {
                return m_GUID == other.m_GUID;
            }

            bool operator!=(const GUID& other) const {
                return m_GUID != other.m_GUID;
            }

            uint64_t value() const {
                return m_GUID;
            }

            std::string string() const {
                return std::to_string(m_GUID);
            }

        private:
            uint64_t m_GUID;
    };
}
