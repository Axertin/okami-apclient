#pragma once

/** Sourced from https://github.com/allogic/Nippon/tree/main

MIT License

Copyright (c) 2023 0x616c

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <cstdint>
#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace Nippon
{

inline constexpr std::string_view kCipherKey = "YaKiNiKuM2rrVrPJpGMkfe3EK4RbpbHw";

class BlowFish
{
  public:
    static void Create(std::string const &Key);

    static void Encrypt(uint32_t *XL, uint32_t *XR);
    static void Decrypt(uint32_t *XL, uint32_t *XR);

    /// Encrypt/Decrypt a mutable byte vector in-place (ECB mode, 8-byte blocks).
    static void Encrypt(std::vector<uint8_t> &Bytes);
    static void Decrypt(std::vector<uint8_t> &Bytes);

    /// Encrypt/Decrypt a mutable span in-place (ECB mode, 8-byte blocks).
    static void Encrypt(std::span<uint8_t> Bytes);
    static void Decrypt(std::span<uint8_t> Bytes);

  private:
    [[nodiscard]] static uint32_t Feistel(uint32_t X);
};

} // namespace Nippon
