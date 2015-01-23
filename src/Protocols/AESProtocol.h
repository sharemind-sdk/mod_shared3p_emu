/*
 * Copyright (C) 2015 Cybernetica
 *
 * Research/Commercial License Usage
 * Licensees holding a valid Research License or Commercial License
 * for the Software may use this file according to the written
 * agreement between you and Cybernetica.
 *
 * GNU General Public License Usage
 * Alternatively, this file may be used under the terms of the GNU
 * General Public License version 3.0 as published by the Free Software
 * Foundation and appearing in the file LICENSE.GPL included in the
 * packaging of this file.  Please review the following information to
 * ensure the GNU General Public License version 3.0 requirements will be
 * met: http://www.gnu.org/copyleft/gpl-3.0.html.
 *
 * For further information, please contact us at sharemind@cyber.ee.
 */

#ifndef MOD_SHARED3P_EMU_PROTOCOLS_AES_H
#define MOD_SHARED3P_EMU_PROTOCOLS_AES_H

#include <cryptopp/aes.h>
#include "../Shared3pValueTraits.h"
#include <algorithm>

namespace sharemind {

typedef s3p_xor_uint32_t::share_type AES_share_t;
typedef s3p_vec<s3p_xor_uint32_t> AES_share_vec_t;

/** Endianess swapping routine. */
static inline AES_share_t EndianessSwap(const AES_share_t v) {
    return (v >> 24) | (v >> 8 & 0x0000ff00) | (v << 8 & 0x00ff0000) | (v << 24 & 0xff000000) ;
}

template <size_t Nk_, size_t Nb_, size_t Nr_>
class __attribute__ ((visibility("internal"))) AesProtocol {

public: /* Types: */

    static constexpr size_t Nk = Nk_;
    static constexpr size_t Nb = Nb_;
    static constexpr size_t Nr = Nr_;

protected: /* Types: */

    /**
      The type for storing AES state. In the NIST specification it should have
      4x4 8-bit values, but we use the existing 32-bit data type to pack 4
      values into one, and we consider them as columns of the NIST state array.
     */

public: /* Methods: */

    /**
      \brief Encrypts or decrypts the given data with the given pre-expanded key.
      \param[in] plainText the input data.
      \param[in] preExpandedKey the pre-expanded encryption/decryption key.
      \param[out] cipherText the output data.

      \returns whether the encryption or decryption was successful.
    */
    void processWithExpandedKey(const AES_share_vec_t & plainText,
                                const AES_share_vec_t & preExpandedKey,
                                AES_share_vec_t & cipherText);

    /**
      Expands the given AES key.
      In emulator only copies the inKey to outKey.
      \param[in] inKey the input key.
      \param[out] outKey the output key.
    */
    void expandAesKey(const AES_share_vec_t & inKey,
                      AES_share_vec_t & outKey);

private: /* Fields: */


}; /* class AesProtocol { */

typedef AesProtocol<4u, 4u, 10u> Aes128Protocol;
typedef AesProtocol<6u, 4u, 12u> Aes192Protocol;
typedef AesProtocol<8u, 4u, 14u> Aes256Protocol;

template<size_t Nk_, size_t Nb_, size_t Nr_>
void AesProtocol<Nk_, Nb_, Nr_>::expandAesKey(const AES_share_vec_t & inKey,
                                              AES_share_vec_t & outKey)
{
    std::copy(inKey.begin(), inKey.end(), outKey.begin());
}

template<size_t Nk_, size_t Nb_, size_t Nr_>
void AesProtocol<Nk_, Nb_, Nr_>::processWithExpandedKey(const AES_share_vec_t & plainText,
                                                        const AES_share_vec_t & preExpandedKey,
                                                        AES_share_vec_t & cipherText)
{
    constexpr size_t KEYSIZE = Nk_ * 4;
    uint8_t key[KEYSIZE];
    std::memcpy(key, preExpandedKey.data(), KEYSIZE);
    std::transform(preExpandedKey.begin(), preExpandedKey.begin() + Nk_, (uint32_t*)key, EndianessSwap);

    uint8_t pt[Nb_ * 4];
    uint8_t ct[Nb_ * 4];
    CryptoPP::AESEncryption enc(key, KEYSIZE);
    auto i = plainText.begin();
    auto j = cipherText.begin();
    do {
        std::transform(i, i+Nb_, (uint32_t*)pt, EndianessSwap);
        enc.ProcessBlock(pt, ct);
        std::transform((uint32_t*)ct, ((uint32_t*)ct)+Nb_, j, EndianessSwap);
        i += Nb_;
        j += Nb_;
    } while (i != plainText.end() && j != cipherText.end());
}

} /* namespace sharemind { */

#endif /* MOD_SHARED3P_EMU_PROTOCOLS_AES_H */
