/*
 * reader.hpp
 *
 *  Created on: Jun 14, 2011
 *      Author: Peter Goodman
 *
 * Copyright 2011 Peter Goodman, all rights reserved.
 */

#ifndef XY_FILE_HPP_
#define XY_FILE_HPP_

#include <fcntl.h>
#include <stdint.h>
#include <cstring>

#include "xy/include/array.hpp"

#ifndef O_BINARY
#   define O_BINARY 0x00
#endif

#ifndef O_RDONLY
#   define O_RDONLY 0x00
#endif

#ifndef O_WRONLY
#   define O_WRONLY 0x01
#endif

#ifndef O_RDWR
#   define O_RDWR 0x02
#endif

#ifndef O_CREAT
#   define O_CREAT 0x0200
#endif

namespace xy { namespace io {

    class read_tag {
    public:
        enum {
            FLAGS = O_RDONLY | O_BINARY
        };
    };
    class write_tag {
    public:
        enum {
            FLAGS = O_WRONLY | O_BINARY | O_CREAT
        };
    };
    class read_and_write_tag {
        enum {
            FLAGS = O_RDWR | O_BINARY | O_CREAT
        };
    };

    namespace {
        enum {
            INVALID_FILE_DESCRIPTOR = -1
        };
    }

    template <typename> class file;
    template <typename> class with_open_file;

    template<>
    class file<read_tag> {
    private:

        template <typename> friend class with_open_file;

        int fd;
        const char *file_name;

        file(void) throw();
        file(int) throw();

    public:

        ~file(void) throw();

        const char *name(void) const throw();

        template <const size_t BLOCK_SIZE, const size_t GIVEN_SIZE>
        size_t read_block(uint8_t (&block)[GIVEN_SIZE]) const throw() {
            static_assert(
                GIVEN_SIZE >= BLOCK_SIZE,
                "The array passed to file<read_tag>::read_block() is not "
                "large enough for the specified block size."
            );

            memset(block, 0, array::size(block));

            ssize_t amount{read(fd, block, array::size(block))};

            if(amount < 0) {
                // TODO report error
                return 0U;
            }

            return static_cast<size_t>(amount);
        }
    };

    template <typename access_tag>
    class with_open_file {
    private:

        file<access_tag> open_file;

        template <typename ...state_types>
        class use_file_func {
        public:
            typedef void (type)(file<access_tag> &, state_types&...);
        };

    public:


        template <typename ...state_types>
        with_open_file(
            const char *fname,
            typename use_file_func<state_types...>::type *func,
            state_types&... args
        ) throw() {
            open_file.fd = INVALID_FILE_DESCRIPTOR;

            if(nullptr == fname || '\0' == *fname) {
                return;
            }

            const int fd = open(fname, access_tag::FLAGS);
            if(0 > fd) {
                return;
            }

            open_file.fd = fd;
            open_file.file_name = fname;

            func(open_file, args...);
        }

        operator bool(void) const throw() {
            return INVALID_FILE_DESCRIPTOR != open_file.fd;
        }
    };
}}

#endif /* XY_FILE_READER_HPP_ */
