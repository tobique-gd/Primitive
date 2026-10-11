#pragma once

#include "core/events/ieventdispatcher.h"
#include "core/layers/ilayer.h"
#include <memory>
#include <vector>
namespace Primitive {
    class LayerStack : public IEventDispatcher {
        public:
            LayerStack() = default;
            ~LayerStack() = default;

            void push_layer(std::shared_ptr<ILayer> layer);
            void pop_layer(std::shared_ptr<ILayer> layer);

            void on_update();
            void on_detach();
            virtual void dispatch_event(std::shared_ptr<IEvent> event) override;

        private:
            std::vector<std::shared_ptr<ILayer>> m_layers;

    };
}
