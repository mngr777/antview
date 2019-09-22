#include <iostream>
#include <string>
#include "../trail_parser.hpp"
#include "../ant.hpp"

int main() {
    using namespace std;

    std::string t1("((0 0) (1 2) (3 5))");

    TrailParser tp1;
    cout << "Parsing trail: " << t1 << endl;
    tp1.parse(t1);

    if (tp1.is_error())
        cerr << "Parse error: " << tp1.error_message() << endl;
    if (!tp1.is_done()) {
        cerr << t1 << endl;
        cerr << "Parsing is not finished"
             << "State: " << tp1.state_string() << endl
             << "Line: " << tp1.line_num()
             << ", Pos: " << tp1.char_num() << endl
             << ", Buffer: " << tp1.buffer() << endl;
        return -1;
    }

    cout << "Parsed trail:  " << tp1.result() << endl;

    return 0;
}
