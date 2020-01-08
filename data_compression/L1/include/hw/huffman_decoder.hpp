/*
 * (c) Copyright 2019 Xilinx, Inc. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
#ifndef _XFCOMPRESSION_HUFFMAN_DECODER_HPP_
#define _XFCOMPRESSION_HUFFMAN_DECODER_HPP_

/**
 * @file inflate_huffman.hpp
 * @brief Header for module used in ZLIB decompress kernel.
 *
 * This file is part of Vitis Data Compression Library.
 */

#define MAXBITS 15
#define TCODESIZE 2048

#define HEADER_STATE 1
#define TREE_PMBL_STATE 2
#define STORE_STATE 3
#define DYNAMIC_STATE 4
#define BYTEGEN_STATE 5
#define COMPLETE_STATE 6
#define STATIC_STATE 7

#define LITERAL_STAGE 1
#define MATCH_LENGTH_STAGE 2
#define MATCH_DIST_STAGE 3
#define MATCH_DIST_WRITE_STAGE 4

#define BIT 8
typedef ap_uint<32> compressd_dt;

#define READBITS(n) \
    while (bits_cntr < (uint32_t)(n)) NEXTBYTE();

#define NEXTBYTE()                                      \
    {                                                   \
        curInSize -= 2;                                 \
        uint16_t temp_lcl = (uint16_t)inStream.read();  \
        in_cntr += 2;                                   \
        bitbuffer += (uint64_t)(temp_lcl) << bits_cntr; \
        bits_cntr += 16;                                \
    }

#define BITS(n) ((uint32_t)bitbuffer & ((1 << (n)) - 1))

#define DUMPBITS(n)    \
    bitbuffer >>= (n); \
    bits_cntr -= (uint32_t)(n);

namespace xf {
namespace compression {

void code_generator_array(uint8_t curr_table,
                          uint16_t* lens,
                          uint32_t codes,
                          uint8_t* table_op,
                          uint8_t* table_bits,
                          uint16_t* table_val,
                          uint32_t* bits,
                          uint32_t* used) {
/**
 * @brief This module regenerates the code values based on bit length
 * information present in block preamble. Output generated by this module
 * presents operation, bits and value for each literal, match length and
 * distance.
 *
 * @param curr_table input current module to process i.e., literal or
 * distance table etc
 * @param lens input bit length information
 * @param codes input number of codes
 * @param table_op output operation per active symbol (literal or distance)
 * @param table_bits output bits to process per symbol (literal or distance)
 * @param table_val output value per symbol (literal or distance)
 * @param bits represents the start of the table
 * @param used presents next valid entry in table
 */
#pragma HLS INLINE REGION
    uint32_t sym = 0;
    uint32_t min, max;
    uint32_t root = *bits;
    uint32_t curr;
    uint32_t drop;
    uint32_t huff = 0;
    uint32_t incr;
    uint32_t fill;
    uint32_t low;
    uint32_t mask;

    uint8_t code_data_op = 0;
    uint8_t code_data_bits = 0;
    uint16_t code_data_val = 0;

    uint8_t* nptr_op;
    uint8_t* nptr_bits;
    uint16_t* nptr_val;

    const uint16_t* base;
    const uint16_t* extra;
    uint32_t match;
    uint16_t count[MAXBITS + 1] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
#pragma HLS ARRAY_PARTITION variable = count

    uint16_t offs[MAXBITS + 1];
#pragma HLS ARRAY_PARTITION variable = offs

    uint16_t codeBuffer[512];

    const uint16_t lbase[32] = {3,  4,  5,  6,  7,  8,  9,  10,  11,  13,  15,  17,  19,  23, 27, 31,
                                35, 43, 51, 59, 67, 83, 99, 115, 131, 163, 195, 227, 258, 0,  0,  0};
    const uint16_t lext[32] = {16, 16, 16, 16, 16, 16, 16, 16, 17, 17, 17, 17, 18, 18, 18,  18,
                               19, 19, 19, 19, 20, 20, 20, 20, 21, 21, 21, 21, 16, 77, 202, 0};
    const uint16_t dbase[32] = {1,    2,    3,    4,    5,    7,     9,     13,    17,  25,   33,
                                49,   65,   97,   129,  193,  257,   385,   513,   769, 1025, 1537,
                                2049, 3073, 4097, 6145, 8193, 12289, 16385, 24577, 0,   0};
    const uint16_t dext[32] = {16, 16, 16, 16, 17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22,
                               23, 23, 24, 24, 25, 25, 26, 26, 27, 27, 28, 28, 29, 29, 64, 64};

cnt_lens:
    for (uint16_t i = 0; i < codes; i++)
#pragma HLS PIPELINE II = 1
        count[lens[i]]++;

max_loop:
    for (max = MAXBITS; max >= 1; max--)
#pragma HLS PIPELINE II = 1
        if (count[max] != 0) break;

    if (root > max) root = max;

min_loop:
    for (min = 1; min < max; min++) {
#pragma HLS PIPELINE II = 1
        if (count[min] != 0) break;
    }

    if (root < min) root = min;

    int left = 1;
left_loop:
    for (uint16_t i = 1; i <= MAXBITS; i++) {
#pragma HLS PIPELINE II = 1
        left <<= 1;
        left -= count[i];
    }

    offs[1] = 0;
offs_loop:
    for (uint16_t i = 1; i < MAXBITS; i++)
#pragma HLS PIPELINE II = 1
        offs[i + 1] = offs[i] + count[i];

codes_loop:
    for (uint16_t i = 0; i < codes; i++) {
#pragma HLS PIPELINE II = 1
        if (lens[i] != 0) codeBuffer[offs[lens[i]]++] = (uint16_t)i;
    }

    switch (curr_table) {
        case 1:
            base = extra = codeBuffer;
            match = 20;
            break;
        case 2:
            base = lbase;
            extra = lext;
            match = 257;
            break;
        case 3:
            base = dbase;
            extra = dext;
            match = 0;
    }

    uint16_t len = min;

    nptr_op = table_op;
    nptr_bits = table_bits;
    nptr_val = table_val;

    curr = root;
    drop = 0;
    low = (uint32_t)(-1);
    *used = 1 << root;
    mask = *used - 1;

code_gen:
    for (;;) {
#pragma HLS PIPELINE II = 1
        code_data_bits = (uint8_t)(len - drop);

        if (codeBuffer[sym] + 1 < match) {
            code_data_op = (uint8_t)0;
            code_data_val = codeBuffer[sym];
        } else if (codeBuffer[sym] >= match) {
            code_data_op = (uint8_t)(extra[codeBuffer[sym] - match]);
            code_data_val = base[codeBuffer[sym] - match];
        } else {
            code_data_op = (uint8_t)(96);
            code_data_val = 0;
        }

        incr = 1 << (len - drop);
        fill = 1 << curr;
        min = fill;

        do {
            fill -= incr;
            nptr_op[(huff >> drop) + fill] = code_data_op;
            nptr_bits[(huff >> drop) + fill] = code_data_bits;
            nptr_val[(huff >> drop) + fill] = code_data_val;
        } while (fill != 0);

        incr = 1 << (len - 1);

        while (huff & incr) incr >>= 1;

        if (incr != 0) {
            huff &= incr - 1;
            huff += incr;
        } else
            huff = 0;

        sym++;

        if (--(count[len]) == 0) {
            if (len == max) break;
            len = lens[codeBuffer[sym]];
        }

        if (len > root && (huff & mask) != low) {
            if (drop == 0) drop = root;

            nptr_op += min;
            nptr_bits += min;
            nptr_val += min;

            curr = len - drop;
            left = (int)(1 << curr);

            for (int i = curr; i + drop < max; i++, curr++) {
                left -= count[curr + drop];
                if (left <= 0) break;
                left <<= 1;
            }

            *used += 1 << curr;

            low = huff & mask;
            table_op[low] = (uint8_t)curr;
            table_bits[low] = (uint8_t)root;
            table_val[low] = (uint16_t)(nptr_val - table_val);
        }
    }

    *bits = root;
}

/**
 * @brief This module is zlib/gzip huffman decoder it generates a LZ77 byte compressed
 * data and trasfer to lz_decompress_eos module for further byte unpacking
 *
 * @param inStream input bit packed data
 * @param outStream output lz77 compressed output in the form of 32bit packets
 * (Literals, Match Length, Distances)
 * @param endOfStream output completion of execution
 * @param input_size input data size
 */
void huffmanDecoder(hls::stream<ap_uint<2 * BIT> >& inStream,
                    hls::stream<compressd_dt>& outStream,
                    hls::stream<bool>& endOfStream,
                    uint32_t input_size) {
    uint64_t bitbuffer = 0;
    uint32_t curInSize = input_size;
    uint8_t bits_cntr = 0;

    uint8_t current_op = 0;
    uint8_t current_bits = 0;
    uint16_t current_val = 0;

    uint8_t len = 0;

    const uint16_t order[19] = {16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15};

    uint8_t dynamic_last = 0;
    uint32_t dynamic_nlen = 0;
    uint32_t dynamic_ndist = 0;
    uint32_t dynamic_ncode = 0;
    uint32_t dynamic_curInSize = 0;
    uint16_t dynamic_lens[512];

    uint32_t in_cntr = 0;

    uint32_t dynamic_lenbits = 0;
    uint32_t dynamic_distbits = 0;
    uint8_t copy = 0;

    bool done = true;

    uint8_t array_codes_op[TCODESIZE];
    uint8_t array_codes_bits[TCODESIZE];
    uint16_t array_codes_val[TCODESIZE];

    uint8_t block_mode;
    int cntr = 0;
    uint32_t used = 0;
    uint8_t next_state = HEADER_STATE;

    while (done) {
        done = false;

        if (next_state == HEADER_STATE) {
            done = true;
            READBITS(16);
            DUMPBITS(4);

            next_state = TREE_PMBL_STATE;

            bitbuffer = 0;
            bits_cntr = 0;
        } else if (next_state == TREE_PMBL_STATE) {
            done = true;

            if (bitbuffer == 0) READBITS(3);

            dynamic_last = BITS(1);
            DUMPBITS(1);

            switch (BITS(2)) {
                case 0:
                    next_state = STORE_STATE;
                    break;
                case 1:
                    done = false;
                    break;
                case 2:
                    next_state = DYNAMIC_STATE;
                    break;
                case 3:
                    done = false;
                    break;
            }
            DUMPBITS(2);

        } else if (next_state == STORE_STATE) {
            done = true;
            bitbuffer >>= bits_cntr & 7;
            bits_cntr -= bits_cntr & 7;

            READBITS(32);
            if (bits_cntr > 32) {
                bitbuffer >>= 32;
                bits_cntr = bits_cntr - 32;
            } else {
                bitbuffer = 0;
                bits_cntr = 0;
            }

            if (dynamic_last) {
                next_state = COMPLETE_STATE;
            } else {
                next_state = TREE_PMBL_STATE;
            }

        } else if (next_state == DYNAMIC_STATE) {
            done = true;
            READBITS(14);
            dynamic_nlen = BITS(5) + 257; // Max 288
            DUMPBITS(5);

            dynamic_ndist = BITS(5) + 1; // Max 30
            DUMPBITS(5);

            dynamic_ncode = BITS(4) + 4; // Max 19
            DUMPBITS(4);

            dynamic_curInSize = 0;

            while (dynamic_curInSize < dynamic_ncode) {
                READBITS(3);
                dynamic_lens[order[dynamic_curInSize++]] = (uint16_t)BITS(3);
                DUMPBITS(3);
            }

            while (dynamic_curInSize < 19) dynamic_lens[order[dynamic_curInSize++]] = 0;

            dynamic_lenbits = 7;

            xf::compression::code_generator_array(1, dynamic_lens, 19, array_codes_op, array_codes_bits,
                                                  array_codes_val, &dynamic_lenbits, &used);

            dynamic_curInSize = 0;

            // Figure out codes for LIT/ML and DIST
            while (dynamic_curInSize < dynamic_nlen + dynamic_ndist) {
                for (;;) {
                    current_op = array_codes_op[BITS(dynamic_lenbits)];
                    current_bits = array_codes_bits[BITS(dynamic_lenbits)];
                    current_val = array_codes_val[BITS(dynamic_lenbits)];
                    if ((uint32_t)(current_bits) <= bits_cntr) break;
                    NEXTBYTE();
                }

                if (current_val < 16) {
                    DUMPBITS(current_bits);
                    dynamic_lens[dynamic_curInSize++] = current_val;
                } else {
                    if (current_val == 16) {
                        READBITS(current_bits + 2);
                        DUMPBITS(current_bits);

                        if (dynamic_curInSize == 0) done = false;

                        len = dynamic_lens[dynamic_curInSize - 1];
                        copy = 3 + BITS(2);
                        DUMPBITS(2);

                    } else if (current_val == 17) {
                        READBITS(current_bits + 3);
                        DUMPBITS(current_bits);
                        len = 0;
                        copy = 3 + BITS(3);
                        DUMPBITS(3);
                    } else {
                        READBITS(current_bits + 7);
                        DUMPBITS(current_bits);
                        len = 0;
                        copy = 11 + BITS(7);
                        DUMPBITS(7);
                    }

                    while (copy--) dynamic_lens[dynamic_curInSize++] = (uint16_t)len;
                }
            } // End of while
            dynamic_lenbits = 9;
            xf::compression::code_generator_array(2, dynamic_lens, dynamic_nlen, array_codes_op, array_codes_bits,
                                                  array_codes_val, &dynamic_lenbits, &used);

            dynamic_distbits = 6;
            uint32_t dused = 0;
            xf::compression::code_generator_array(3, dynamic_lens + dynamic_nlen, dynamic_ndist, &array_codes_op[used],
                                                  &array_codes_bits[used], &array_codes_val[used], &dynamic_distbits,
                                                  &dused);
            next_state = BYTEGEN_STATE;
        } else if (next_state == BYTEGEN_STATE) {
            done = true;
            if (curInSize >= 6) {
                // mask length codes 1st level
                uint32_t lit_mask = (1 << dynamic_lenbits) - 1;
                // mask length codes 2nd level
                uint32_t dist_mask = (1 << dynamic_distbits) - 1;

                // Read from the table
                uint8_t current_op = 0;
                uint8_t current_bits = 0;
                uint16_t current_val = 0;

                uint8_t* mltable_op = array_codes_op;
                uint8_t* mltable_bits = array_codes_bits;
                uint16_t* mltable_val = array_codes_val;

                uint8_t* disttable_op = &array_codes_op[used];
                uint8_t* disttable_bits = &array_codes_bits[used];
                uint16_t* disttable_val = &array_codes_val[used];

                uint32_t op;
                uint16_t len;
                uint16_t dist;

                uint32_t ml_op = 0;
                uint32_t dist_op = 0;

                // ********************************
                //  Create Packets Below
                //  [LIT|ML|DIST|DIST] --> 32 Bit
                //  Read data from inStream - 8bits
                //  at a time. Decode the literals,
                //  ML, Distances based on tables
                // ********************************
                uint8_t curr_stage = LITERAL_STAGE;
                compressd_dt tmpVal;
                bool done = false;
                uint32_t cntr = 0;

                // Read from inStream
                if (bits_cntr < 15) {
                    uint16_t temp = inStream.read();
                    in_cntr += 2;
                    bitbuffer += (uint64_t)(temp) << bits_cntr;
                    bits_cntr += 16;
                }

                uint32_t lidx = bitbuffer & lit_mask;
                current_op = mltable_op[lidx];
                current_bits = mltable_bits[lidx];
                current_val = mltable_val[lidx];
                bool read_ml_bram = false;
                bool read_dist_bram = false;
                uint32_t didx = 0;

            ByteGen:
                for (; !done;) {
#pragma HLS PIPELINE II = 2
                    // In this stage read literals
                    // if length is read then push out
                    // if its single length if its extra
                    // move to match length stage
                    // This stage generates output
                    // INSTREAM: 1
                    // OUTSTREAM: 1
                    if (curr_stage == LITERAL_STAGE) {
                        uint32_t temp = (uint32_t)(current_bits);
                        bitbuffer >>= temp;
                        bits_cntr -= temp;
                        ml_op = (uint32_t)(current_op);

                        // Push Literal
                        // Otherwise fill length
                        if (ml_op == 0) {
                            tmpVal.range(7, 0) = (uint16_t)(current_val);
                            tmpVal.range(31, 8) = 0;
                            // printf("%c", current_val);
                            outStream << tmpVal;
                            endOfStream << 0;
                            curr_stage = LITERAL_STAGE;

                            if (bits_cntr < 15) {
                                uint16_t temp = inStream.read();
                                in_cntr += 2;
                                bitbuffer += (uint64_t)(temp) << bits_cntr;
                                bits_cntr += 16;
                            }

                            lidx = bitbuffer & lit_mask;
                            read_ml_bram = true;
                        } else {
                            curr_stage = MATCH_LENGTH_STAGE;
                        }
                    } else if (curr_stage == MATCH_LENGTH_STAGE) {
                        // In this stage read the extra match length
                        // update the tmpVal
                        // This stage reads but doesnt write
                        // INSTREAM: 1
                        // OUTSTREAM: NIL
                        if (ml_op & 16) {
                            len = (uint16_t)(current_val);
                            ml_op &= 15;
                            if (ml_op) {
                                len += (uint32_t)bitbuffer & ((1 << ml_op) - 1);
                                bitbuffer >>= ml_op;
                                bits_cntr -= ml_op;
                            }
                            tmpVal.range(31, 16) = len;
                            tmpVal.range(7, 0) = 0;
                            curr_stage = MATCH_DIST_STAGE;

                            if (bits_cntr < 15) {
                                uint16_t temp = inStream.read();
                                in_cntr += 2;
                                bitbuffer += (uint64_t)(temp) << bits_cntr;
                                bits_cntr += (2 * BIT);
                            }

                            didx = bitbuffer & dist_mask;
                            read_dist_bram = true;
                        } else if ((ml_op & 64) == 0) {
                            curr_stage = LITERAL_STAGE;
                            lidx = current_val + (bitbuffer & ((1 << ml_op) - 1));
                            read_ml_bram = true;
                        } else if (ml_op & 32) {
                            // Termination Condition
                            next_state = 2;
                            done = 1;
                        }

                    } else if (curr_stage == MATCH_DIST_STAGE) {
                        // In this stage read tmpVal - filled with match length
                        // data, work on match dist, read once or twice based
                        // on extra match dist
                        // This stage generates output
                        // INSTREAM: 1
                        // OUTSTREAM: 1
                        uint32_t temp = (uint32_t)(current_bits);
                        bitbuffer >>= temp;
                        bits_cntr -= temp;
                        dist_op = (uint32_t)(current_op);

                        if (dist_op & 16) {
                            curr_stage = MATCH_DIST_WRITE_STAGE;
                        } else if ((dist_op & 64) == 0) {
                            didx = current_val + (bitbuffer & ((1 << dist_op) - 1));
                            read_dist_bram = true;
                            curr_stage = MATCH_DIST_STAGE;
                        } else {
                            next_state = 77;
                            done = 1;
                        }
                    } else if (curr_stage == MATCH_DIST_WRITE_STAGE) {
                        dist = (uint32_t)(current_val);
                        dist_op &= 15;
                        dist += (uint32_t)bitbuffer & ((1 << dist_op) - 1);
                        bitbuffer >>= dist_op;
                        bits_cntr -= dist_op;

                        tmpVal.range(15, 0) = dist;
                        outStream << tmpVal;
                        endOfStream << 0;
                        curr_stage = LITERAL_STAGE;

                        if (bits_cntr < 15) {
                            uint16_t temp = inStream.read();
                            in_cntr += 2;
                            bitbuffer += (uint64_t)(temp) << bits_cntr;
                            bits_cntr += (2 * BIT);
                        }

                        lidx = bitbuffer & lit_mask;
                        read_ml_bram = true;
                        // printf("dist %d \n", dist);
                    }
                    if (read_ml_bram) {
                        read_ml_bram = false;
                        current_op = mltable_op[lidx];
                        current_bits = mltable_bits[lidx];
                        current_val = mltable_val[lidx];
                    }

                    if (read_dist_bram) {
                        read_dist_bram = false;
                        current_op = disttable_op[didx];
                        current_bits = disttable_bits[didx];
                        current_val = disttable_val[didx];
                    }

                    // Read inStream
                    if (bits_cntr < 15) {
                        uint16_t temp = inStream.read();
                        in_cntr += 2;
                        bitbuffer += (uint64_t)(temp) << bits_cntr;
                        bits_cntr += (2 * BIT);
                    }

                } // Top for-loop ends hre

                if (next_state == 77) done = false;
            } else
                done = false;
        } else if (next_state == COMPLETE_STATE) {
            done = false;
            break;
        }

    } // While end

    uint32_t leftover = input_size - in_cntr;
    if (leftover) {
        for (int i = 0; i < leftover; i += 2) {
            uint16_t c = inStream.read();
        }
    }

    outStream << 0; // Adding Dummy Data for last end of stream case
    endOfStream << 1;
}
} // Compression
} // XF
#endif // _XFCOMPRESSION_INFLATE_HUFFMAN_HPP_