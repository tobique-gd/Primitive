#pragma once


namespace Primitive::RHI
{
    class ITexture2D {
        public:
            virtual ~ITexture2D() = default;

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
