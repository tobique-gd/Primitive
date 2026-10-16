#pragma once


namespace Primitive
{
    enum class TextureFormat {
        RGBA8,

    };

    class Texture2D {
        public:
            virtual ~Texture2D() = default;

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
