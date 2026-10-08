#pragma once

#include "core/application.h"

namespace Engine
{
    extern Application* create_application();
}

int main() {
    Engine::Application* app = Engine::create_application();
    app->run();
    delete app;
}
