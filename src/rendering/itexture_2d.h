#pragma once


namespace Engine
{
    class ITexture2D {
        public:
            virtual ~ITexture2D() = default;

            virtual void* get_native_handle() const = 0;
    };
}
