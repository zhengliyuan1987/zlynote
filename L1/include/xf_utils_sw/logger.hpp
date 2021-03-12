/*
 * Copyright 2019 Xilinx, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef XF_UTILS_SW_LOGGER_HPP
#define XF_UTILS_SW_LOGGER_HPP

#include <iostream>
#include "CL/cl.h"

namespace xf {
namespace common {
namespace utils_sw {

// Translates error type to its corresponding string
const inline char* clErrorToString(int err) {
    // TODO complete list, referring to cl.h
    switch (err) {
        case CL_SUCCESS:
            return "CL_SUCCESS";
        case CL_INVALID_PROGRAM:
            return "CL_INVALID_PROGRAM";
        case CL_INVALID_KERNEL_NAME:
            return "CL_INVALID_KERNEL_NAME";
        case CL_INVALID_CONTEXT:
            return "CL_INVALID_CONTEXT";
        case CL_INVALID_VALUE:
            return "CL_INVALID_VALUE";
        case CL_INVALID_DEVICE:
            return "CL_INVALID_DEVICE";
        case CL_INVALID_BINARY:
            return "CL_INVALID_BINARY";
        case CL_OUT_OF_HOST_MEMORY:
            return "CL_OUT_OF_HOST_MEMORY";
        case CL_INVALID_QUEUE_PROPERTIES:
            return "CL_INVALID_QUEUE_PROPERTIES";
        case CL_INVALID_OPERATION:
            return "CL_INVALID_OPERATION";
    }
    return "unknown CL error code";
}

// Logger class for handling the unified Error/Warning/Info/Debug messages.
// Users can set the level of log that needed to be printed
class Logger {
   public:
    // Level of log, ERROR > WARNING > INFO > DEBUG
    enum class Level : unsigned { ERROR = 0, WARNING = 1, INFO = 2, DEBUG = 3 };

    // Standard messages list
    enum class Message : int {
        TEST_PASS = 0,
        TEST_FAIL,
        RUN_SUCC_CREATE_COMMANDQUEUE,
        RUN_FAIL_CREATE_COMMANDQUEUE,
        RUN_SUCC_CREATE_CONTEXT,
        RUN_FAIL_CREATE_CONTEXT,
        RUN_SUCC_CREATE_PROGRAM,
        RUN_FAIL_CREATE_PROGRAM,
        RUN_SUCC_CREATE_KERNEL,
        RUN_FAIL_CREATE_KERNEL,
        TIME_KERNEL_MS,
        TIME_H2D_MS,
        TIME_D2H_MS,
        TIME_E2E_MS,
        OCL_SUCC_CMD,
        OCL_FAIL_CMD
    };

    // Default constructor with std::cout/std::cerr to separate Info/Debug|Error/Warning into std::cout and std:cerr
    // and INFO level by default (not to print debug messages)
    Logger(std::ostream& s1 = std::cout, std::ostream& s2 = std::cerr)
        : os_debug_info(s1), os_warn_error(s2), level(Level::INFO) {}

    // set the level of messages that wants to be printed
    Logger& setLevel(Level l) {
        level = l;
        return *this;
    }

    // 2 overloaded functions for error messages
    Logger& error(Message msg) { return log(Level::ERROR, "Error: ", msg); }

    template <typename T>
    Logger& error(Message msg, T v) {
        return log(Level::ERROR, "Error: ", msg, v);
    }

    // 2 overloaded functions for warning messages
    Logger& warn(Message msg) { return log(Level::WARNING, "Warning: ", msg); }

    template <typename T>
    Logger& warn(Message msg, T v) {
        return log(Level::WARNING, "Warning: ", msg, v);
    }

    // 2 overloaded functions for info messages
    Logger& info(Message msg) { return log(Level::INFO, "Info: ", msg); }

    template <typename T>
    Logger& info(Message msg, T v) {
        return log(Level::INFO, "Info: ", msg, v);
    }

    // 2 overloaded functions for debug messages
    Logger& debug(Message msg) { return log(Level::DEBUG, "Debug: ", msg); }

    template <typename T>
    Logger& debug(Message msg, T v) {
        return log(Level::DEBUG, "Debug: ", msg, v);
    }

   private:
    std::ostream& os_debug_info;
    std::ostream& os_warn_error;
    Level level;

    // Log message without argument
    Logger& log(Level l, const char* prefix, Message msg, std::ostream& os) {
        if (level < l) return *this;
        os << prefix;
        switch (msg) {
            case Message::TEST_PASS:
                os << "Test passed" << std::endl;
                break;
            case Message::TEST_FAIL:
                os << "Test failed" << std::endl;
                break;
            case Message::RUN_SUCC_CREATE_PROGRAM:
                os << "Program created" << std::endl;
                break;
            case Message::RUN_SUCC_CREATE_KERNEL:
                os << "Kernel created" << std::endl;
                break;
            case Message::RUN_SUCC_CREATE_CONTEXT:
                os << "Context created" << std::endl;
                break;
            case Message::RUN_SUCC_CREATE_COMMANDQUEUE:
                os << "Command queue created" << std::endl;
                break;
            case Message::OCL_SUCC_CMD:
                os << "OCL check passed" << std::endl;
                break;
            case Message::OCL_FAIL_CMD:
                os << "OCL check failed" << std::endl;
                break;
            default:;
        }
        return *this;
    }
    Logger& log(Level l, const char* prefix, Message msg) {
        if (l == Level::INFO || l == Level::DEBUG)
            return log(l, prefix, msg, os_debug_info);
        else if (l == Level::WARNING || l == Level::ERROR)
            return log(l, prefix, msg, os_warn_error);
        return *this;
    }

    // Log message with one argument
    template <class T>
    Logger& log(Level l, const char* prefix, Message msg, T v, std::ostream& os) {
        if (level < l) return *this;
        os << prefix;
        switch (msg) {
            case Message::RUN_FAIL_CREATE_CONTEXT:
                os << "Failed to create context: " << clErrorToString(v) << std::endl;
                break;
            case Message::RUN_FAIL_CREATE_COMMANDQUEUE:
                os << "Failed to create command queue: " << clErrorToString(v) << std::endl;
                break;
            case Message::RUN_FAIL_CREATE_PROGRAM:
                os << "Failed to create program: " << clErrorToString(v) << std::endl;
                break;
            case Message::RUN_FAIL_CREATE_KERNEL:
                os << "Failed to create kernel: " << clErrorToString(v) << std::endl;
                break;
            case Message::TIME_KERNEL_MS:
                os << "Time in kernel: " << v << "ms" << std::endl;
                break;
            case Message::TIME_H2D_MS:
                os << "Time in host-to-device: " << v << "ms" << std::endl;
                break;
            case Message::TIME_D2H_MS:
                os << "Time in device-to-host: " << v << "ms" << std::endl;
                break;
            case Message::TIME_E2E_MS:
                os << "Time in end-to-end: " << v << "ms" << std::endl;
                break;
            default:;
        }
        return *this;
    }
    template <class T>
    Logger& log(Level l, const char* prefix, Message msg, T v) {
        if (l == Level::INFO || l == Level::DEBUG)
            return log(l, prefix, msg, v, os_debug_info);
        else if (l == Level::WARNING || l == Level::ERROR)
            return log(l, prefix, msg, v, os_warn_error);
        return *this;
    }
    // XXX: do we need 2-argument logger?

   public:
    // Gives Error/Info messages according to err and exit with 1 if error occurs plus exit_on_failure is on
    int logCreateProgram(cl_int err, bool exit_on_failure = false) {
        if (err == CL_SUCCESS) {
            info(Message::RUN_SUCC_CREATE_PROGRAM);
        } else {
            error(Message::RUN_FAIL_CREATE_PROGRAM, err);
            if (exit_on_failure) {
                exit(1);
            } else {
                return 1;
            }
        }
        return 0;
    }

    // Gives Error/Info messages according to err and exit with 1 if error occurs plus exit_on_failure is on
    int logCreateKernel(cl_int err, bool exit_on_failure = false) {
        if (err == CL_SUCCESS) {
            info(Message::RUN_SUCC_CREATE_KERNEL);
        } else {
            error(Message::RUN_FAIL_CREATE_KERNEL, err);
            if (exit_on_failure) {
                exit(1);
            } else {
                return 1;
            }
        }
        return 0;
    }

    // Gives Error/Info messages according to err and exit with 1 if error occurs plus exit_on_failure is on
    int logCreateContext(cl_int err, bool exit_on_failure = false) {
        if (err == CL_SUCCESS) {
            info(Message::RUN_SUCC_CREATE_CONTEXT);
        } else {
            error(Message::RUN_FAIL_CREATE_CONTEXT, err);
            if (exit_on_failure) {
                exit(1);
            } else {
                return 1;
            }
        }
        return 0;
    }

    // Gives Error/Info messages according to err and exit with 1 if error occurs plus exit_on_failure is on
    int logCreateCommandQueue(cl_int err, bool exit_on_failure = false) {
        if (err == CL_SUCCESS) {
            info(Message::RUN_SUCC_CREATE_COMMANDQUEUE);
        } else {
            error(Message::RUN_FAIL_CREATE_COMMANDQUEUE, err);
            if (exit_on_failure) {
                exit(1);
            } else {
                return 1;
            }
        }
        return 0;
    }

    // Gives Error/Info messages according to err and exit with 1 if error occurs plus exit_on_failure is on
    int logCommonCheck(cl_int err, bool exit_on_failure = false) {
        if (err == CL_SUCCESS) {
            info(Message::OCL_SUCC_CMD);
        } else {
            error(Message::OCL_FAIL_CMD);
            if (exit_on_failure) {
                exit(1);
            } else {
                return 1;
            }
        }
        return 0;
    }
};

} // namespace utils_sw
} // namespace common
} // namespace xf

#endif // XF_UTILS_SW_LOGGER_HPP
