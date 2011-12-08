/*
 * policy.hpp
 *
 *  Created on: Dec 1, 2011
 *      Author: petergoodman
 *     Version: $Id$
 */

#ifndef XY_POLICY_HPP_
#define XY_POLICY_HPP_

#include <pthread.h>
#include <cstdlib>

#define XY_SHARED_DATA_COMMON                           \
    private:                                            \
    T data_;                                            \
    mutable pthread_mutex_t lock_;                      \
    public:                                             \
    shared_data(T &&default_val) throw()                \
        : data_(default_val)                            \
    {                                                   \
        if(0 != pthread_mutex_init(&lock_, nullptr)) {  \
            ::exit(EXIT_FAILURE);                       \
        }                                               \
    }                                                   \
    ~shared_data(void) throw() {                        \
        pthread_mutex_destroy(&lock_);                  \
    }

#define XY_SHARED_DATA_WRITE(L)                         \
    self_type &operator=(T &&new_val) throw() {         \
        L(pthread_mutex_lock(&lock_);)                  \
        data_ = new_val;                                \
        L(pthread_mutex_unlock(&lock_);)                \
        return *this;                                   \
    }

#define XY_SHARED_DATA_READ(L)                          \
    operator T(void) const throw() {                    \
        L(pthread_mutex_lock(&lock_);)                  \
        const T ret_val(data_);                         \
        L(pthread_mutex_unlock(&lock_);)                \
        return ret_val;                                 \
    }

#define XY_LOCKED(x) x
#define XY_UNLOCKED(x)

namespace xy { namespace threading {

    /// forward declaration
    template <typename policy, typename T>
    class shared_data;

    /// locking policies
    class read_write_locked { };
    class read_locked { };
    class write_locked { };

    /// reads and writes are locked
    template <typename T>
    class shared_data<read_write_locked, T> {
        typedef shared_data<read_write_locked, T> self_type;

        XY_SHARED_DATA_COMMON
        XY_SHARED_DATA_WRITE(XY_LOCKED)
        XY_SHARED_DATA_READ(XY_LOCKED)
    };

    template <typename T>
    pthread_mutex_t shared_data<read_write_locked, T>::lock_ = PTHREAD_MUTEX_INITIALIZER;

    /// reads are locked, writes are not
    template <typename T>
    class shared_data<read_locked, T> {
        typedef shared_data<read_locked, T> self_type;

        XY_SHARED_DATA_COMMON
        XY_SHARED_DATA_WRITE(XY_UNLOCKED)
        XY_SHARED_DATA_READ(XY_LOCKED)
    };

    template <typename T>
    pthread_mutex_t shared_data<read_locked, T>::lock_ = PTHREAD_MUTEX_INITIALIZER;

    /// writes are locked, reads are not
    template <typename T>
    class shared_data<write_locked, T> {
        typedef shared_data<write_locked, T> self_type;

        XY_SHARED_DATA_COMMON
        XY_SHARED_DATA_WRITE(XY_LOCKED)
        XY_SHARED_DATA_READ(XY_UNLOCKED)
    };

    template <typename T>
    pthread_mutex_t shared_data<write_locked, T>::lock_ = PTHREAD_MUTEX_INITIALIZER;
}}


#endif /* XY_POLICY_HPP_ */
