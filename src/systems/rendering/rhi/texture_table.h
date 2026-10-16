#include "systems/rendering/rhi/command_list.h"
#include "systems/rendering/rhi/texture_2d.h"
#include <memory>

namespace Primitive {
    using TextureIndex = uint32_t;

    class ITextureTable {
    public:
        virtual ~ITextureTable() = default;

        virtual TextureIndex register_texture(
            std::shared_ptr<Texture2D> texture
        ) = 0;

        virtual void unregister_texture(TextureIndex index) = 0;

        virtual void bind(ICommandList& command_list) = 0;
    };
}
