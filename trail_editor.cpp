#include "app/trail_editor.hpp"
#include "ant.hpp"

int main() {
    TrailEditorApp app;
    app.run("Trail Editor", Ant::MaxX * 32, Ant::MaxY * 32);
    return 0;
}
