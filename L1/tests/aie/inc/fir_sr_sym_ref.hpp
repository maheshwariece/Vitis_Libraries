#ifndef _DSPLIB_fir_sr_sym_REF_HPP_
#define _DSPLIB_fir_sr_sym_REF_HPP_

/*
Single Rate Symmetric FIR Kernel Reference model.
This file holds the declaration of the reference model class. The reference model
is functionally equivalent to the kernel class with intrinics. The reference model
does not use intrinsics or vector operations. The reference mode, once validated
acts as the golden reference to verify the AIE-targetting kernel class.
*/

#include <adf.h>
#include <limits>

namespace xf {
namespace dsp {
namespace aie {
namespace fir {
namespace sr_sym {
//-----------------------------------------------------------------------------------------------------
// Single Rate class - no coefficient reload, single output
template <typename TT_DATA,  // type of data input and output
          typename TT_COEFF, // type of coefficients           (e.g. int16, cint32)
          unsigned int TP_FIR_LEN,
          unsigned int TP_SHIFT,
          unsigned int TP_RND,
          unsigned int TP_INPUT_WINDOW_VSIZE,
          unsigned int TP_USE_COEFF_RELOAD = 0, // 1 = use coeff reload, 0 = don't use coeff reload
          unsigned int TP_NUM_OUTPUTS = 1>
class fir_sr_sym_ref {
   private:
    // This array holds the coefficient values for the reference model.
    // Strictly speaking the alignment is not required when targetting
    // the x86 or scalar processors.
    TT_COEFF chess_storage(% chess_alignof(v8cint16)) m_internalTaps[TP_FIR_LEN];

   public:
    // Constructor
    fir_sr_sym_ref(const TT_COEFF (&taps)[(TP_FIR_LEN + 1) / 2]) {
        for (int i = 0; i < (TP_FIR_LEN + 1) / 2; i++) {
            m_internalTaps[i] = taps[i];
            m_internalTaps[TP_FIR_LEN - 1 - i] = taps[i];
        }
    }

    // Register Kernel Class
    static void registerKernelClass() { REGISTER_FUNCTION(fir_sr_sym_ref::filter); }
    // FIR
    void filter(input_window<TT_DATA>* inWindow, output_window<TT_DATA>* outWindow);
};

// Specialization - no coefficient reload, dual outputs
template <typename TT_DATA,  // type of data input and output
          typename TT_COEFF, // type of coefficients           (e.g. int16, cint32)
          unsigned int TP_FIR_LEN,
          unsigned int TP_SHIFT,
          unsigned int TP_RND,
          unsigned int TP_INPUT_WINDOW_VSIZE>
class fir_sr_sym_ref<TT_DATA,
                     TT_COEFF,
                     TP_FIR_LEN,
                     TP_SHIFT,
                     TP_RND,
                     TP_INPUT_WINDOW_VSIZE,
                     0 /*USE_COEFF_RELOAD_FALSE*/,
                     2> {
   private:
    // This array holds the coefficient values for the reference model.
    // Strictly speaking the alignment is not required when targetting
    // the x86 or scalar processors.
    TT_COEFF chess_storage(% chess_alignof(v8cint16)) m_internalTaps[TP_FIR_LEN];

   public:
    // Constructor
    fir_sr_sym_ref(const TT_COEFF (&taps)[(TP_FIR_LEN + 1) / 2]) {
        for (int i = 0; i < (TP_FIR_LEN + 1) / 2; i++) {
            m_internalTaps[i] = taps[i];
            m_internalTaps[TP_FIR_LEN - 1 - i] = taps[i];
        }
    }

    // Register Kernel Class
    static void registerKernelClass() { REGISTER_FUNCTION(fir_sr_sym_ref::filter); }
    // FIR
    void filter(input_window<TT_DATA>* inWindow, output_window<TT_DATA>* outWindow, output_window<TT_DATA>* outWindow2);
};

//-----------------------------------------------------------------------------------------------------
// Specialization using coefficient reload and single output
template <typename TT_DATA,  // type of data input and output
          typename TT_COEFF, // type of coefficients           (e.g. int16, cint32)
          unsigned int TP_FIR_LEN,
          unsigned int TP_SHIFT,
          unsigned int TP_RND,
          unsigned int TP_INPUT_WINDOW_VSIZE>
class fir_sr_sym_ref<TT_DATA,
                     TT_COEFF,
                     TP_FIR_LEN,
                     TP_SHIFT,
                     TP_RND,
                     TP_INPUT_WINDOW_VSIZE,
                     1 /*USE_COEFF_RELOAD_TRUE*/,
                     1> {
   private:
    // This array holds the coefficient values for the reference model.
    // Strictly speaking the alignment is not required when targetting
    // the x86 or scalar processors.
    TT_COEFF chess_storage(% chess_alignof(v8cint16)) m_internalTaps[TP_FIR_LEN];

   public:
    // Constructor
    fir_sr_sym_ref() {}

    void firReload(const TT_COEFF (&taps)[(TP_FIR_LEN + 1) / 2]) {
        for (int i = 0; i < (TP_FIR_LEN + 1) / 2; i++) {
            m_internalTaps[i] = taps[i];
            m_internalTaps[TP_FIR_LEN - 1 - i] = taps[i];
        }
    }

    // Register Kernel Class
    static void registerKernelClass() { REGISTER_FUNCTION(fir_sr_sym_ref::filter); }

    // FIR
    void filter(input_window<TT_DATA>* inWindow,
                output_window<TT_DATA>* outWindow,
                const TT_COEFF (&inTaps)[(TP_FIR_LEN + 1) / 2]);
};

// Specialization using coefficient reload and dual output
template <typename TT_DATA,  // type of data input and output
          typename TT_COEFF, // type of coefficients           (e.g. int16, cint32)
          unsigned int TP_FIR_LEN,
          unsigned int TP_SHIFT,
          unsigned int TP_RND,
          unsigned int TP_INPUT_WINDOW_VSIZE>
class fir_sr_sym_ref<TT_DATA,
                     TT_COEFF,
                     TP_FIR_LEN,
                     TP_SHIFT,
                     TP_RND,
                     TP_INPUT_WINDOW_VSIZE,
                     1 /*USE_COEFF_RELOAD_TRUE*/,
                     2> {
   private:
    // This array holds the coefficient values for the reference model.
    // Strictly speaking the alignment is not required when targetting
    // the x86 or scalar processors.
    TT_COEFF chess_storage(% chess_alignof(v8cint16)) m_internalTaps[TP_FIR_LEN];

   public:
    // Constructor
    fir_sr_sym_ref() {}

    void firReload(const TT_COEFF (&taps)[(TP_FIR_LEN + 1) / 2]) {
        for (int i = 0; i < (TP_FIR_LEN + 1) / 2; i++) {
            m_internalTaps[i] = taps[i];
            m_internalTaps[TP_FIR_LEN - 1 - i] = taps[i];
        }
    }

    // Register Kernel Class
    static void registerKernelClass() { REGISTER_FUNCTION(fir_sr_sym_ref::filter); }

    // FIR
    void filter(input_window<TT_DATA>* inWindow,
                output_window<TT_DATA>* outWindow,
                output_window<TT_DATA>* outWindow2,
                const TT_COEFF (&inTaps)[(TP_FIR_LEN + 1) / 2]);
};
}
}
}
}
}
#endif // _DSPLIB_fir_sr_sym_REF_HPP_

/*  (c) Copyright 2020 Xilinx, Inc. All rights reserved.

    This file contains confidential and proprietary information
    of Xilinx, Inc. and is protected under U.S. and
    international copyright and other intellectual property
    laws.

    DISCLAIMER
    This disclaimer is not a license and does not grant any
    rights to the materials distributed herewith. Except as
    otherwise provided in a valid license issued to you by
    Xilinx, and to the maximum extent permitted by applicable
    law: (1) THESE MATERIALS ARE MADE AVAILABLE "AS IS" AND
    WITH ALL FAULTS, AND XILINX HEREBY DISCLAIMS ALL WARRANTIES
    AND CONDITIONS, EXPRESS, IMPLIED, OR STATUTORY, INCLUDING
    BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, NON-
    INFRINGEMENT, OR FITNESS FOR ANY PARTICULAR PURPOSE; and
    (2) Xilinx shall not be liable (whether in contract or tort,
    including negligence, or under any other theory of
    liability) for any loss or damage of any kind or nature
    related to, arising under or in connection with these
    materials, including for any direct, or any indirect,
    special, incidental, or consequential loss or damage
    (including loss of data, profits, goodwill, or any type of
    loss or damage suffered as a result of any action brought
    by a third party) even if such damage or loss was
    reasonably foreseeable or Xilinx had been advised of the
    possibility of the same.

    CRITICAL APPLICATIONS
    Xilinx products are not designed or intended to be fail-
    safe, or for use in any application requiring fail-safe
    performance, such as life-support or safety devices or
    systems, Class III medical devices, nuclear facilities,
    applications related to the deployment of airbags, or any
    other applications that could lead to death, personal
    injury, or severe property or environmental damage
    (individually and collectively, "Critical
    Applications"). Customer assumes the sole risk and
    liability of any use of Xilinx products in Critical
    Applications, subject only to applicable laws and
    regulations governing limitations on product liability.

    THIS COPYRIGHT NOTICE AND DISCLAIMER MUST BE RETAINED AS
    PART OF THIS FILE AT ALL TIMES.                       */
