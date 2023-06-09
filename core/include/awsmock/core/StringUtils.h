//
// Created by vogje01 on 22/08/2022.
//

#ifndef AWSMOCK_CORE_STRINGUTILS_H
#define AWSMOCK_CORE_STRINGUTILS_H

// C++ standard includes
#include <string>
#include <random>
#include <iterator>
#include <algorithm>
#include <sstream>
#include <vector>
#include <array>
#include <regex>
#include <iomanip>

// Poco includes
#include <Poco/String.h>
#include <Poco/URI.h>
#include <Poco/RegularExpression.h>

namespace AwsMock::Core {

    /**
     * String utilities.
     *
     * @author jens.vogt@opitz-consulting.com
     */
    class StringUtils {

    public:
      /**
       * Generates a random string with the given length.
       *
       * @param length length of the string
       * @return generated random string with the given length.
       */
      static std::string GenerateRandomString(int length);

      /**
       * Check whether the provided string is a number.
       *
       * @param value string value
       * @return true in case the provided string is numeric, otherwise false.
       */
      static bool IsNumeric(const std::string &value);

      /**
       * Check whether the provided string is a UUID.
       *
       * @param value string value
       * @return true in case the provided string is a UUID, otherwise false.
       */
      static bool IsUuid(const std::string &value);

      /**
       * Splits a string into pieces using the provided delimiter char.
       *
       * @param s string to split
       * @param delim delimiter
       * @return vector os strings.
       */
      static std::vector<std::string> Split(const std::string &s, char delim);

      /**
       * Strip whitespaces
       *
       * @param s string to split
       * @return string having all whiteplaces removed
       */
      static std::string StripWhiteSpaces(const std::string &s);

      /**
       * Return true if s1 and s2 are equal.
       *
       * @param s1 first input string
       * @param s2 second input string
       * @return true if s11 and s2 are equal
       */
      static bool Equals(const std::string &s1, const std::string &s2);

      /**
       * Return true if s1 and s2 are not case sensitive equal.
       *
       * @param s1 first input string
       * @param s2 second input string
       * @return true if s11 and s2 are equal
       */
      static bool EqualsIgnoreCase(const std::string &s1, const std::string &s2);

      /**
       * Returns true in case the string is either null or empty
       *
       * @param s1 pointer to the string to check
       * @return true if the string is null or empty.
       */
      static bool IsNullOrEmpty(const std::string *s1);

      /**
       * Returns true in case the string s1 contains string s1
       *
       * @param s1 string to check
       * @param s2 probe string
       * @return true if the string s1 contains string s2.
       */
      static bool Contains(const std::string &s1, const std::string &s2);

      /**
       * Check whether the given string start wit the given prefix
       *
       * @param s1 string to check
       * @param s2 probe string
       * @return true if the string s1 starts with s2.
       */
      static bool StartsWith(const std::string &s1, const std::string &s2);

      /**
       * Returns a substring by index.
       *
       * @param string string to process
       * @param beginIndex begin index
       * @param endIndex end index
       * @return substring
       */
      static std::string SubString(const std::string& string, int beginIndex, int endIndex);

      /**
       * Left trim a string.
       *
       * @param s input string
       * @return trimmed string
       */
      static inline std::string Ltrim(std::string s) {
          s.erase(s.find_last_not_of(' ') + 1);
          return s;
      }

      /**
       * Right trim a string.
       *
       * @param s input string
       * @return trimmed string
       */
      static inline std::string Rtrim(std::string s) {
          s.erase(0, s.find_first_not_of(' '));
          return s;
      }

      /**
       * Trim a string.
       *
       * @param s input string
       * @return trimmed string
       */
      static inline std::string Trim(std::string &s) {
          s = Rtrim(s);
          s = Ltrim(s);
          return s;
      }

      /**
       * Strip common line endings.
       *
       * @param input input string
       * @return string with stripped line endings.
       */
      static inline std::string StripLineEndings(const std::string &input) {
          std::string output;
          output.reserve(input.length());
          std::copy_if(input.begin(), input.end(), std::back_inserter(output), [](char c) { return c != '\r' && c != '\n'; });
          return output;
      }

      /**
       * Returns the string in URL encoding format.
       *
       * @param input input string
       * @return URL encoded string.
       */
      static std::string UrlEncode(const std::string &input);

      /**
       * Returns a decoded string as plain text.
       *
       * @param input input string
       * @return URL decoded string.
       */
      static std::string UrlDecode(const std::string &input);

      /**
       * Convert the given string to a hex encoded string.
       *
       * @param input input byte array
       * @param length length of the byte array
       * @return hex encoded string
       */
      static std::string ToHexString(unsigned char *input, size_t length);

    };

} // namespace AwsMock::Core

#endif // AWSMOCK_CORE_STRINGUTILS_H
