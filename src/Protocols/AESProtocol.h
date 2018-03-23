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

#ifndef MOD_SHARED3P_EMU_PROTOCOLS_AESPROTOCOL_H
#define MOD_SHARED3P_EMU_PROTOCOLS_AESPROTOCOL_H

#include <algorithm>
#include <array>
#include <cryptopp/aes.h>
#include "../Shared3pValueTraits.h"
#include "../Shared3pVector.h"


namespace sharemind {

typedef s3p_xor_uint32_t::share_type AES_share_t;
typedef ShareVec<s3p_xor_uint32_t> AES_share_vec_t;


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
      \brief Encrypts or decrypts the given data with the multiple given
      pre-expanded keys.
      \param[in] plainText the input data.
      \param[in] preExpandedKey the pre-expanded encryption/decryption keys.
      \param[out] cipherText the output data.
    */
    void processWithExpandedKey(const AES_share_vec_t & plainText,
                                const AES_share_vec_t & preExpandedKey,
                                AES_share_vec_t & cipherText);

    /**
      \brief Encrypts or decrypts the given data with the given pre-expanded key.
      \param[in] plainText the input data.
      \param[in] preExpandedKey the pre-expanded encryption/decryption key.
      \param[out] cipherText the output data.
    */
    void processWithSingleExpandedKey(const AES_share_vec_t & plainText,
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

}; /* class AesProtocol { */

extern template class AesProtocol<4u, 4u, 10u>;
extern template class AesProtocol<6u, 4u, 12u>;
extern template class AesProtocol<8u, 4u, 14u>;

typedef AesProtocol<4u, 4u, 10u> Aes128Protocol;
typedef AesProtocol<6u, 4u, 12u> Aes192Protocol;
typedef AesProtocol<8u, 4u, 14u> Aes256Protocol;


} /* namespace sharemind { */

#endif /* MOD_SHARED3P_EMU_PROTOCOLS_AESPROTOCOL_H */
