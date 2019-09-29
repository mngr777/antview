#include "trail_parser.hpp"
#include <cassert>
#include <cstring>

const char Space[] = " \t\r\n";
const char Digits[] = "0123456789";

static bool is_space(char c);
static bool is_digit(char c);
static bool is_paren_left(char c);
static bool is_paren_right(char c);

static std::string state_message(const TrailParser& parser) {
    return std::string("")
        + "Parser state: " + parser.state_string() + "\n"
        + "Parser error: " + parser.error_message() + "\n"
        + "Position: line " + std::to_string(parser.line_num())
        + " char "  + std::to_string(parser.char_num());
}

TrailParserException::TrailParserException(const TrailParser& parser)
    : std::logic_error(state_message(parser)) {}


TrailParser::TrailParser() {
    reset();
}

std::size_t TrailParser::parse(const std::string& s) {
    std::size_t pos = 0;
    for (; pos < s.size(); ++pos) {
        consume(s[pos]);
        if (is_done() || is_error())
            break;
    }
    finish();
    return pos;
}

std::size_t TrailParser::parse(std::istream& s) {
    std::size_t pos = 0;
    char c = '\0';
    while (!is_done() && !is_error() && s.get(c))
        consume(c);
    return pos;
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

std::string TrailParser::state_string() const {
    switch (state_) {
        case StateReady:
            return "Ready";
        case StateError:
            return "Error";
        case StateDone:
            return "Done";
        case StateExpectItemOrTrailEnd:
            return "Expecting next position item or end of trail";
        case StateExpectNumber:
            return "Expecting coordinate number";
        case StateNumber:
            return "Reading coordinate number";
        case StateExpectItemEnd:
            return "Expecting end of position item";
        default:
            assert(false && "Undefined state");
    }
}

std::string TrailParser::error_message() const {
    switch (error_) {
        case ErrorOk:
            return "Ok";
        case ErrorUnrecognizedSymbol:
            return "Unrecognized symbol";
        case ErrorUnexpectedDigit:
            return "Unexpected digit";
        case ErrorUnexpectedLeftParen:
            return "Unexpected `('";
        case ErrorUnexpectedRightParen:
            return "Unexpected `)'";
        case ErrorNumberInvalid:
            return "Invalid number";
        case ErrorNumberOutOfRange:
            return "Number is out of range";
        default:
            assert(false && "Undefined error");
    }
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
        case StateExpectItemOrTrailEnd:
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
        case StateExpectItemOrTrailEnd:
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
            state_ = StateExpectItemOrTrailEnd;
            break;

        case StateExpectItemOrTrailEnd:
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
        case StateExpectItemOrTrailEnd:
            state_ = StateDone;
            break;

        case StateExpectItemEnd:
            state_ = StateExpectItemOrTrailEnd;
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
    auto number = Coord{};
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
    state_ = StateExpectItemOrTrailEnd;
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
