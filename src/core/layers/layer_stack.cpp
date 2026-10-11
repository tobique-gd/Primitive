#include "core/layers/layer_stack.h"
#include "core/layers/ilayer.h"
#include <cstddef>
#include <memory>
#include <algorithm>

namespace Primitive {
    void LayerStack::on_update() {
        for (const auto& layer : m_layers) {
            layer->on_update();
        }
    }

    void LayerStack::push_layer(std::shared_ptr<ILayer> layer)
    {
        layer->set_event_dispatcher(this);

        m_layers.push_back(layer);

        layer->on_attach();
    }

    void LayerStack::pop_layer(std::shared_ptr<ILayer> layer) {
        auto it = std::find(m_layers.begin(), m_layers.end(), layer);

        if (it != m_layers.end()) {
            (*it)->on_detach();
            m_layers.erase(it);
        }
    }

    void LayerStack::dispatch_event(std::shared_ptr<IEvent> event) {
        for (auto it = m_layers.rbegin(); it != m_layers.rend(); ++it)
        {
            (*it)->on_event(event);
            if (event->is_consumed()) {
                break;
            }
        }
    }

}
