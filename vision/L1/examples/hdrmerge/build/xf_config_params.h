/*
 * Copyright 2020 Xilinx, Inc.
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
 */

#define NO 1 // Normal Operation
#define RO 0 // Resource Optimized

#define T_8U 0
#define T_16U 1
#define T_10U 0
#define T_12U 0

// Set the input and output pixel depth:
#define IN_TYPE XF_16UC1
#define OUT_TYPE XF_16UC1
#define SIN_CHANNEL_TYPE XF_8UC1
#define INPUT_PTR_WIDTH 32
#define OUTPUT_PTR_WIDTH 32

#define HEIGHT 1000
#define WIDTH 1000

#define NO_EXPS 2

#define NPIX XF_NPPC1
