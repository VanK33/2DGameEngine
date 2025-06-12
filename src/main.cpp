#include "engine/GameApp.hpp"

int main() {
    engine::GameApp app;

    if (!app.Init()) return 1;

    app.Run();
    app.Shutdown();

    return 0;
}
