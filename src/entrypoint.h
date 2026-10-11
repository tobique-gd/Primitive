#pragma once

#include "core/application.h"

namespace Primitive {
    extern Application* create_application();
}

int main() {
    Primitive::Application* app = Primitive::create_application();
    app->run();
    delete app;
}
