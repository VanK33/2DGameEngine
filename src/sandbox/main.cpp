#include "SandboxApp.hpp" 

int main() {
    sandbox::SandboxApp app;

    if (!app.Init()) return 1;

    app.Run();
    app.Shutdown();

    return 0;
}
