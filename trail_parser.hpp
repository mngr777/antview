#ifndef ANTVIEW_TRAIL_PARSER_HPP_
#define ANTVIEW_TRAIL_PARSER_HPP_

#include <cstddef>
#include <string>
#include "ant.hpp"

class TrailParser {
public:
    enum State {
        StateReady,
        StateError,
        StateDone,
        StateExpectItemOrEnd,
        StateExpectNumber,
        StateNumber,
        StateExpectItemEnd
    };

    enum Error {
        ErrorOk,
        ErrorUnrecognizedSymbol,
        ErrorUnexpectedDigit,
        ErrorUnexpectedLeftParen,
        ErrorUnexpectedRightParen,
        ErrorNumberInvalid,
        ErrorNumberOutOfRange
    };

    TrailParser();

    void consume(const char c);

    void finish();

    Ant::Trail result() {
        return trail_;
    }

    bool is_done() {
        return state_ == StateDone;
    }

    Error error() {
        return error_;
    }

    std::string state_string() const;
    std::string error_message() const;

    std::size_t line_num() const {
        return line_num_;
    }

    std::size_t char_num() const {
        return char_num_;
    }

    const std::string& buffer() const {
        return buffer_;
    }

private:
    void reset();

    void space();
    void digit(const char c);
    void paren_left();
    void paren_right();

    void complete_number();
    void complete_pos();

    void count(char c);
    void set_error(Error error);

    Ant::Trail trail_;
    Ant::Pos pos_;
    unsigned coord_num_;
    State state_;
    Error error_;
    std::string buffer_;

    std::size_t line_num_;
    std::size_t char_num_;
};

#endif
