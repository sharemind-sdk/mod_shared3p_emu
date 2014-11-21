/*
 * This file is a part of the Sharemind framework.
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#ifndef MOD_SHARED3P_EMU_PROTOCOLS_UNARY_H
#define MOD_SHARED3P_EMU_PROTOCOLS_UNARY_H

#include "../Shared3pValueTraits.h"

namespace sharemind {

class __attribute__ ((visibility("internal"))) Conversion {
public:
  Conversion (Shared3pPDPI &pdpi) {}
public:
  template <typename DestT, typename SourceT>
  bool invoke (const s3p_vec<SourceT>& param, s3p_vec<DestT>& result,
        any_value_tag)
  {
    if (param.size() != result.size())
        return false;
    for (size_t i = 0u; i < param.size(); ++i)
        result[i] = static_cast<typename value_traits<DestT>::public_type> (param[i]);
    return true;
  }

  template <typename DestT, typename SourceT>
  bool invoke (const s3p_vec<SourceT>& param, s3p_vec<s3p_bool_t>& result,
                                    bool_value_tag)
  {
    if (param.size() != result.size())
      return false;
    for (size_t i = 0u; i < param.size(); ++i)
      result[i] = param[i];
    return true;
  }
};

class __attribute__ ((visibility("internal"))) SumProtocol {
public:
  SumProtocol (Shared3pPDPI &pdpi) {}
public:
  template <typename DestT, typename SourceT>
  bool invoke (const s3p_vec<SourceT>& param, s3p_vec<DestT>& result,
        any_value_tag)
  {
    const size_t param_size = param.size ();
    const size_t result_size = result.size ();
    if (result_size == 0)
        return false;

    if ((s3p_vec<DestT>*)&param == &result)
        return true;

    if (param_size == 0) {
        if (result_size != 1)
            return false;
        result[0] = 0;
        return true;
    }

    if (param_size % result_size != 0)
        return false;

    for (size_t i = 0u; i < result_size; ++i) {
        result[i] = 0;
    }
    const size_t subarr_len = param_size / result_size;
    for (size_t i = 0u; i < param_size; ++i) {
      result[i / subarr_len] += param[i];
    }

    return true;
  }

/*
  template <typename DestT, typename SourceT>
  bool invoke (const s3p_vec<SourceT>& param, s3p_vec<s3p_bool_t>& result,
                                    bool_value_tag)
  {
    if (param.size() != result.size())
      return false;
    for (size_t i = 0u; i < param.size(); ++i)
      result[i] = param[i];
    return true;
  }
  */
};

}

#endif /* MOD_SHARED3P_EMU_PROTOCOLS_UNARY_H */
