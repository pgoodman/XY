/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * libdatrie - Double-Array Trie Library
 * Copyright (C) 2006  Theppitak Karoonboonyanan <thep@linux.thai.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

/*
 * triedefs.h - General typedefs for trie
 * Created: 2006-08-11
 * Author:  Theppitak Karoonboonyanan <thep@linux.thai.net>
 */

#ifndef __TRIEDEFS_H
#define __TRIEDEFS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <string.h>

#include "xy/deps/libdatrie/typedefs.h"

/**
 * @file triedefs.h
 * @brief General typedefs for trie
 */

/**
 * @brief Trie character type for alphabet
 */
typedef char         AlphaChar;

/**
 * @brief Error value for alphabet character
 */
#define ALPHA_CHAR_ERROR   (~(AlphaChar)0)

/**
 * @brief Trie character type for key
 */
typedef unsigned char  TrieChar;
/**
 * @brief Trie terminator character
 */
#define TRIE_CHAR_TERM    '\0'
#define TRIE_CHAR_MAX     255

/**
 * @brief Type of Trie index
 */
typedef int32          TrieIndex;
/**
 * @brief Trie error index
 */
#define TRIE_INDEX_ERROR  0
/**
 * @brief Maximum trie index value
 */
#define TRIE_INDEX_MAX    0x7fffffff

/**
 * @brief Type of value associated to trie entries
 */
typedef uint32          TrieData;
/**
 * @brief Trie error data
 */
#define TRIE_DATA_ERROR  ((uint32) 0xFFFFFFFF)

#ifdef __cplusplus
}
#endif

#endif  /* __TRIEDEFS_H */

/*
vi:ts=4:ai:expandtab
*/
