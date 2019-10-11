#include <cassert>
#include <iostream>
#include "app/trail_editor.hpp"
#include "ant.hpp"
#include "data.hpp"

static Trail load_trail_or_exit(const std::string& filename);

int main(int argc, char** argv) {
    TrailEditorApp app;
    if (argc > 1)
        app.set_trail(load_trail_or_exit(argv[1]));
    app.run("Trail Editor", Ant::MaxX * 32, Ant::MaxY * 32);
    return 0;
}

Trail load_trail_or_exit(const std::string& filename) {
    try {
        return load_trail(filename);
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
        std::exit(-1);
    }
    assert(false);
}
