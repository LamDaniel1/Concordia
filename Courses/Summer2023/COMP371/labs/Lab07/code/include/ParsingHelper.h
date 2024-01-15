//
// COMP 371 Assignment Framework
//
// Created by Nicolas Bergeron on 8/7/14.
// Updated by Gary Chang on 14/1/15
//
// Copyright (c) 2014-2019 Concordia University. All rights reserved.
//

#pragma once

#include <algorithm>
#include <cassert>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>

// Case insensitive strings (eg: Hello == HELLo)
// This is used for parsing scene files
// Stolen from: Herb Suter - http://www.gotw.ca/gotw/029.htm

struct ci_char_traits : public std::char_traits<char>
// just inherit all the other functions
//  that we don't need to override
{
  static bool eq(char c1, char c2) { return toupper(c1) == toupper(c2); }
  static bool ne(char c1, char c2) { return toupper(c1) != toupper(c2); }
  static bool lt(char c1, char c2) { return toupper(c1) < toupper(c2); }

  static int compare(const char* s1, const char* s2, size_t n) {

        // Taken from: https://stackoverflow.com/revisions/2886589/2
        while( n-- != 0 ) {
            if( toupper(*s1) < toupper(*s2) ) return -1;
            if( toupper(*s1) > toupper(*s2) ) return 1;
            ++s1; ++s2;
        }
        return 0;
  }

  static const char* find(const char* s, int n, char a) {
    while (n-- > 0 && toupper(*s) != toupper(a)) {
      ++s;
    }
    return s;
  }
};

// Add to the list if you need to...
typedef std::basic_string<char, ci_char_traits> ci_string;
typedef std::basic_ifstream<char, ci_char_traits> ci_ifstream;
typedef std::basic_istringstream<char, ci_char_traits> ci_istringstream;
typedef std::basic_stringstream<char, ci_char_traits> ci_stringstream;
typedef std::basic_istream<char, ci_char_traits> ci_istream;
