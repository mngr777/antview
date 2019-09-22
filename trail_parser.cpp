#include "trail_parser.hpp"
#include <cassert>
#include <cstring>
#include <stdexcept>

const char Space[] = " \t\r\n";
const char Digits[] = "0123456789";

static bool is_space(char c);
static bool is_digit(char c);
static bool is_paren_left(char c);
static bool is_paren_right(char c);

TrailParser::TrailParser() {
    reset();
}

void TrailParser::consume(const char c) {
    if (state_ == StateError) {
        return;
    } else if (state_ == StateDone) {
        reset();
    }

    count(c);

    if (is_space(c)) {
        space();

    } else if (is_digit(c)) {
        digit(c);

    } else if (is_paren_left(c)) {
        paren_left();

    } else if (is_paren_right(c)) {
        paren_right();

    } else {
        set_error(ErrorUnrecognizedSymbol);
    }
}

void TrailParser::finish() {

}

void TrailParser::reset() {
    trail_.clear();
    coord_num_ = 0;
    state_ = StateReady;
    error_ = ErrorOk;
    buffer_.clear();
    line_num_ = 0;
    char_num_ = 0;
}

void TrailParser::space() {
    switch (state_) {
        case StateNumber:
            complete_number();
            break;
        case StateReady:
        case StateError:
        case StateDone:
        case StateExpectItemOrEnd:
        case StateExpectNumber:
        case StateExpectItemEnd:
            // do nothing
            break;
    }
}

void TrailParser::digit(const char c) {
    switch (state_) {
        case StateExpectNumber:
            state_ = StateNumber;
            // fallthru
        case StateNumber:
            buffer_ += c;
            break;

        case StateReady:
        case StateExpectItemOrEnd:
        case StateExpectItemEnd:
            set_error(ErrorUnexpectedDigit);
            break;

        case StateError:
        case StateDone:
            // do nothing;
            break;
    }
}

void TrailParser::paren_left() {
    switch (state_) {
        case StateReady:
            state_ = StateExpectItemOrEnd;
            break;

        case StateExpectItemOrEnd:
            state_ = StateExpectNumber;
            break;

        case StateExpectNumber:
        case StateNumber:
        case StateExpectItemEnd:
            set_error(ErrorUnexpectedLeftParen);
            break;

        case StateError:
        case StateDone:
            // do nothing
            break;
    }
}

void TrailParser::paren_right() {
    switch (state_) {
        case StateExpectItemOrEnd:
            state_ = StateDone;
            break;

        case StateExpectItemEnd:
            state_ = StateExpectItemOrEnd;
            break;

        case StateReady:
        case StateExpectNumber:
            set_error(ErrorUnexpectedRightParen);
            break;

        case StateNumber:
            complete_number();
            if (state_ == StateExpectItemEnd) {
                complete_pos();
            } else {
                set_error(ErrorUnexpectedRightParen);
            }
            break;

        case StateError:
        case StateDone:
            // do nothing
            break;
    }
}

void TrailParser::count(char c) {
    if (c == '\n') {
        ++line_num_;
        char_num_ = 0;
    } else {
        ++char_num_;
    }
}

void TrailParser::complete_number() {
    // Parse number in buffer
    auto number = Ant::Coord{};
    try {
        number = std::stoi(buffer_);
    } catch (const std::invalid_argument&) {
        set_error(ErrorNumberInvalid);
    } catch (const std::out_of_range&) {
        set_error(ErrorNumberOutOfRange);
    }

    // Clear buffer
    buffer_.clear();

    // Update position coordinate
    switch (coord_num_) {
        case 0:
            pos_.first = number;
            break;
        case 1:
            pos_.second = number;
            break;
        default: assert(false && "Invalid coord_num_");
    }

    // Update coordinate number (0 or 1)
    coord_num_ = (coord_num_ + 1) % 2;

    // Update state
    state_ = (coord_num_ == 0)
        ? StateExpectItemEnd // both coordinates are set
        : StateExpectNumber; // expecting second coordinate

}

void TrailParser::complete_pos() {
    assert(state_ == StateExpectItemEnd);
    // Add position to trail
    trail_.insert(pos_);
    state_ = StateExpectItemOrEnd;
}

void TrailParser::set_error(TrailParser::Error error) {
    state_ = StateError;
    error_ = error;
}

bool is_space(char c) {
    return std::strchr(Space, c);
}

bool is_digit(char c) {
    return std::strchr(Digits, c);
}

bool is_paren_left(char c) {
    return c == '(';
}

bool is_paren_right(char c) {
    return c == ')';
}
