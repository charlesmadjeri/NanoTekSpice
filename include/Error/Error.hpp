#pragma once

#include <string>

#include <exception>

namespace nts {
    class CustomError : public std::exception {
        private:
            std::string _message;

        public:
            CustomError(const std::string &message) : _message(message) {};
            const char *what() const noexcept override {
                return _message.c_str();
            }
    };
}
