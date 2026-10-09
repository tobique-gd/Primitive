#pragma once


namespace Engine::RHI
{
    class IRHITexture2D {
        public:
            virtual ~IRHITexture2D() = default;

            int get_width() const {
                return width;
            }

            int get_height() const {
                return height;
            }

            virtual void* get_native_handle() const = 0;
        protected:
            int width;
            int height;
    };
}
