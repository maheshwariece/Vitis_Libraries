#ifndef _DSPLIB_UTILS_HPP_
#define _DSPLIB_UTILS_HPP_

/*
Testbench utilities.
This file contains sets of definitions and functions for use
by the test harness in L1 flow as well as tesr harness in L2 flow.
*/

#include <stdio.h>
#include <stdlib.h>
#include <adf.h>
#include <math.h>

#define RETURN_ERROR -1
#define RETURN_SUCCESS 0

// Define debug verbocity
#define DEBUG_LVL_ERROR 0   //
#define DEBUG_LVL_WARNING 1 //
#define DEBUG_LVL_NOTE 2    //
#define DEBUG_LVL_DBG 3     //

// Round up to y
#define CEIL(x, y) (((x + y - 1) / y) * y)

namespace xf {
namespace dsp {
namespace aie {
namespace testcase {
// Define Stimulus generation mode
typedef int T_STIM_MODE;
static const T_STIM_MODE STIM_GEN_SINE = 8;
static const T_STIM_MODE STIM_GEN_SYMHBINCONES = 7;
static const T_STIM_MODE STIM_GEN_SYMINCONES = 6;
static const T_STIM_MODE STIM_GEN_INCONES = 5;
static const T_STIM_MODE STIM_GEN_ALLONES = 4;
static const T_STIM_MODE STIM_GEN_IMPULSE = 3;
static const T_STIM_MODE STIM_GEN_PRBS = 2;
static const T_STIM_MODE STIM_GEN_FROM_FILE = 1;
static const T_STIM_MODE STIM_GEN_RANDOM = 0;

static const float kFloatTolerance = 0.001;

//----------------------------------------------------------------------
// read_array variants

// Read array of int16_t data from file. Return error if file read error
inline int read_array(int size, char* filename, int offset, int16_t* read_data) {
    FILE* pFile;
    int real = 0;

    pFile = fopen(filename, "r");
    if (pFile == NULL) {
        printf("failure opening file %s for reading\n", filename);
        return RETURN_ERROR;
    }
    for (int i = 0; i < size; i++) {
        fscanf(pFile, "%d \n", &real);
        read_data[i + offset] = real;
    }
    printf("Read Array:\n");
    for (int i = 0; i < (size + offset); i++) {
        if ((i % 16) == 0) printf("[%5d]", i);
        printf("%6d, ", read_data[i]);
        if ((i % 16) == 15) printf("\n");
    }
    fclose(pFile);
    pFile = NULL;
    return RETURN_SUCCESS;
}

// Read array of int32 data from file. Return error if file read error
inline int read_array(int size, char* filename, int offset, int32* read_data) {
    FILE* pFile;
    int real = 0;

    pFile = fopen(filename, "r");
    if (pFile == NULL) {
        printf("failure opening file %s for reading\n", filename);
        return RETURN_ERROR;
    }
    for (int i = 0; i < size; i++) {
        fscanf(pFile, "%d \n", &real);
        read_data[i + offset] = real;
    }
    fclose(pFile);
    pFile = NULL;
    return RETURN_SUCCESS;
}

// Read array of cint16_t data from file. Return error if file read error
inline int read_array(int size, char* filename, int offset, cint16_t* read_data) {
    FILE* pFile;
    int real = 0;
    int imag = 0;

    pFile = fopen(filename, "r");
    if (pFile == NULL) {
        printf("failure opening file %s for reading\n", filename);
        return RETURN_ERROR;
    }

    for (int i = 0; i < size; i++) {
        fscanf(pFile, "%d %d\n", &real, &imag);
        read_data[i + offset].real = real;
        read_data[i + offset].imag = imag;
    }
    fclose(pFile);
    pFile = NULL;
    return RETURN_SUCCESS;
}

// Read array of cint16_t data from file. Return error if file read error
inline int read_array(int size, char* filename, int offset, cint32* read_data) {
    FILE* pFile;
    int32 real = 0;
    int32 imag = 0;

    pFile = fopen(filename, "r");
    if (pFile == NULL) {
        printf("failure opening file %s for reading\n", filename);
        return RETURN_ERROR;
    }

    for (int i = 0; i < size; i++) {
        fscanf(pFile, "%d %d\n", &real, &imag);
        read_data[i + offset].real = real;
        read_data[i + offset].imag = imag;
    }
    fclose(pFile);
    pFile = NULL;
    return RETURN_SUCCESS;
}

// Read array of int32 data from file. Return error if file read error
inline int read_array(int size, char* filename, int offset, float* read_data) {
    FILE* pFile;
    int real = 0;

    pFile = fopen(filename, "r");
    if (pFile == NULL) {
        printf("failure opening file %s for reading\n", filename);
        return RETURN_ERROR;
    }
    for (int i = 0; i < size; i++) {
        fscanf(pFile, "%d \n", &real);
        read_data[i + offset] = (float)real;
    }
    fclose(pFile);
    pFile = NULL;
    return RETURN_SUCCESS;
}

// Read array of cint16_t data from file. Return error if file read error
inline int read_array(int size, char* filename, int offset, cfloat* read_data) {
    FILE* pFile;
    int32 real = 0;
    int32 imag = 0;

    pFile = fopen(filename, "r");
    if (pFile == NULL) {
        printf("failure opening file %s for reading\n", filename);
        return RETURN_ERROR;
    }

    for (int i = 0; i < size; i++) {
        fscanf(pFile, "%d %d\n", &real, &imag);
        read_data[i + offset].real = (float)real;
        read_data[i + offset].imag = (float)imag;
    }
    fclose(pFile);
    pFile = NULL;
    return RETURN_SUCCESS;
}

//----------------------------------------------------------------------
// isSame variants

// Compare 2 int16_ts
inline bool isSame(int16_t a, int16_t b) {
    return a == b;
}

// Compare 2 int32_ts
inline bool isSame(int32 a, int32 b) {
    return a == b;
}

// Compare 2 cint16_ts
inline bool isSame(cint16_t a, cint16_t b) {
    return ((a.real == b.real) && (a.imag == b.imag));
}

// Compare 2 cint32s
inline bool isSame(cint32 a, cint32 b) {
    return ((a.real == b.real) && (a.imag == b.imag));
}

// Compare 2 floats
inline bool isSame(float a, float b) {
    //    return a==b;

    if (a != 0.0) {
        return (fabs((a - b) / a) <= kFloatTolerance);
    } else {
        return a == b;
    }
}

// Compare 2 cfloats
inline bool isSame(cfloat a, cfloat b) {
    cfloat errRatio;

    if (isSame(a.real, b.real) && isSame(a.imag, b.imag)) {
        return true;
    } else {
        return false;
    }

    // divide by zero trap
    //  if (a.real == 0.0 && a.imag == 0.0 && b.real == 0.0 && b.imag == 0.0) return true;

    // errRatio.real = (a.real-b.real)/a.real ;
    // errRatio.imag = (a.imag-b.imag)/a.imag ;
    // if ((fabsf(errRatio.real) < kFloatTolerance) && (fabsf(errRatio.imag) < kFloatTolerance)) {
    //  return true;
    //} else {
    //  return false;
    // }
    // return ((a.real==b.real) && (a.imag == b.imag)); //for exact match
}

//----------------------------------------------------------------------
// null<type> (initialization) variants

// Null int16_t
inline int16_t nullInt16() {
    return 0;
}
// Null cint16_t
inline cint16_t nullCint16() {
    cint16_t d;
    d.real = 0;
    d.imag = 0;
    return d;
}

// Null int32
inline int32 nullInt32() {
    return 0;
}
// Null cint32
inline cint32 nullCint32() {
    cint32 d;
    d.real = 0;
    d.imag = 0;
    return d;
}

//----------------------------------------------------------------------
// rand<type> variants

static unsigned long int nextRand = 1;

// Random int16_t - based on stdlib.h
// stdlib's rand has a range of 0-32768
inline int16_t randInt16(void) {
    int myrand;
    myrand = rand();
    if (rand() % 2 == 1) {
        return (int16_t)myrand;
    } else {
        return (int16_t)0 - (int16_t)myrand;
    }
    //    nextRand = nextRand * 1103515245 + 12345;
    //  return (int16_t)(nextRand);
}

// Random int32
inline int32 randInt32(void) {
    // the standard rand() function returns a value from 0 to 2^15-1, so some manipulation is required to achieve the
    // range -2^31 to 2^31-1.
    int32_t myRand;

    myRand = rand();
    if ((rand() % 2) == 0) {
        myRand = -myRand;
    }
    myRand <<= 14; // TODO 16 is the correct figure, but this causes overflow of int64_t, so until a type exists beyond
                   // int64_t, this avoids test errors
    myRand |= ((rand() % 2) << 15);
    myRand |= rand();

    return myRand;
}

// Random cint16_t
inline cint16_t randCint16(void) {
    cint16_t d;
    d.real = randInt16();
    d.imag = randInt16();
    return d;
}

// Random cint32
inline cint32 randCint32(void) {
    cint32 d;
    d.real = randInt32();
    d.imag = randInt32();
    return d;
}

/*void srand(unsigned int seed)
{
    nextRand = seed;
    }*/

//----------------------------------------------------------------------
// randElem variants

// Random element template function
template <typename T_RDATA>
T_RDATA randElem() {
    return 0;
};

// Random int16_t element
template <>
inline int16_t randElem() {
    return randInt16();
};

// Random int32 element
template <>
inline int32 randElem() {
    return randInt32();
};

// Random cint16_t element
template <>
inline cint16_t randElem() {
    return randCint16();
};

// Random cint32 element
template <>
inline cint32 randElem() {
    return randCint32();
};

// Random float element
template <>
inline float randElem() {
    return (float)randInt32() / (float)randInt32();
};

// Random cint32 element
template <>
inline cfloat randElem() {
    cfloat retVal;

    retVal.real = (float)randInt32() / (float)randInt32();
    retVal.imag = (float)randInt32() / (float)randInt32();
    return retVal;
};

//----------------------------------------------------------------------
// randData variants

// Random int16_t array
inline void randData(int size, int offset, int16_t* d) {
    // initial zeros
    for (int i = 0; i < offset; i++) {
        d[i] = nullInt16();
    }
    for (int i = offset; i < size; i++) {
        d[i] = randInt16();
    }
}
// Random int32 array
inline void randData(int size, int offset, int32* d) {
    // initial zeros
    for (int i = 0; i < offset; i++) {
        d[i] = nullInt32();
    }
    for (int i = offset; i < size; i++) {
        d[i] = randInt32();
    }
}
// Random cint16_t array
inline void randData(int size, int offset, cint16_t* d) {
    // initial zeros
    for (int i = 0; i < offset; i++) {
        d[i].real = nullInt16();
        d[i].imag = nullInt16();
    }
    for (int i = offset; i < size; i++) {
        d[i].real = randInt16();
        d[i].imag = randInt16();
    }
}
// Random cint32 array
inline void randData(int size, int offset, cint32* d) {
    // initial zeros
    for (int i = 0; i < offset; i++) {
        d[i].real = nullInt32();
        d[i].imag = nullInt32();
    }
    for (int i = offset; i < size; i++) {
        d[i].real = randInt32();
        d[i].imag = randInt32();
    }
}
// Random float array
inline void randData(int size, int offset, float* d) {
    // initial zeros
    for (int i = 0; i < offset; i++) {
        d[i] = 0.0;
    }
    for (int i = offset; i < size; i++) {
        d[i] = (float)randInt32() / (float)randInt32(); // TODO - not a uniform random over range of float
    }
}
// Random cfloat array
inline void randData(int size, int offset, cfloat* d) {
    // initial zeros
    for (int i = 0; i < offset; i++) {
        d[i].real = 0.0;
        d[i].imag = 0.0;
    }
    for (int i = offset; i < size; i++) {
        d[i].real = (float)randInt32() / (float)randInt32();
        d[i].imag = (float)randInt32() / (float)randInt32();
    }
}

//----------------------------------------------------------------------
// impulseData variants

// Impulse int16_t
inline void impulseData(int size, int offset, int16_t* d) {
    // initial zeros
    for (int i = 0; i < offset; i++) {
        d[i] = 0;
    }
    d[offset] = 1;
    for (int i = offset + 1; i < size; i++) {
        d[i] = 0;
    }
}
// Impulse int32
inline void impulseData(int size, int offset, int32* d) {
    // initial zeros
    for (int i = 0; i < offset; i++) {
        d[i] = 0;
    }
    d[offset] = 1;
    for (int i = offset + 1; i < size; i++) {
        d[i] = 0;
    }
}
// Impulse cint16_t
inline void impulseData(int size, int offset, cint16_t* d) {
    // initial zeros
    for (int i = 0; i < offset; i++) {
        d[i].real = nullInt16();
        d[i].imag = nullInt16();
    }
    d[offset].real = 1;
    d[offset].imag = 1;
    for (int i = offset + 1; i < size; i++) {
        d[i].real = 0;
        d[i].imag = 0;
    }
}
// Impulse cint32
inline void impulseData(int size, int offset, cint32* d) {
    // initial zeros
    for (int i = 0; i < offset; i++) {
        d[i].real = nullInt32();
        d[i].imag = nullInt32();
    }
    d[offset].real = 1;
    d[offset].imag = 1;
    for (int i = offset + 1; i < size; i++) {
        d[i].real = 0;
        d[i].imag = 0;
    }
}
// Impulse float
inline void impulseData(int size, int offset, float* d) {
    // initial zeros
    for (int i = 0; i < offset; i++) {
        d[i] = 0.0;
    }
    d[offset] = 1.0;
    for (int i = offset + 1; i < size; i++) {
        d[i] = 0.0;
    }
}
// Impulse cfloat
inline void impulseData(int size, int offset, cfloat* d) {
    // initial zeros
    for (int i = 0; i < offset; i++) {
        d[i].real = 0.0;
        d[i].imag = 0.0;
    }
    d[offset].real = 1.0;
    d[offset].imag = 1.0;
    for (int i = offset + 1; i < size; i++) {
        d[i].real = 0.0;
        d[i].imag = 0.0;
    }
}

//----------------------------------------------------------------------
// allonesData variants

// Allones int16_t
inline void allonesData(int size, int offset, int16_t* d) {
    // initial zeros
    for (int i = 0; i < offset; i++) {
        d[i] = 0;
    }
    for (int i = offset; i < size; i++) {
        d[i] = 1;
    }
}
// Allones int32
inline void allonesData(int size, int offset, int32* d) {
    // initial zeros
    for (int i = 0; i < offset; i++) {
        d[i] = 0;
    }
    for (int i = offset; i < size; i++) {
        d[i] = 1;
    }
}
// Allones cint16_t
inline void allonesData(int size, int offset, cint16_t* d) {
    // initial zeros
    for (int i = 0; i < offset; i++) {
        d[i].real = nullInt16();
        d[i].imag = nullInt16();
    }
    for (int i = offset; i < size; i++) {
        d[i].real = 1;
        d[i].imag = 1;
    }
}
// Allones cint32
inline void allonesData(int size, int offset, cint32* d) {
    // initial zeros
    for (int i = 0; i < offset; i++) {
        d[i].real = nullInt32();
        d[i].imag = nullInt32();
    }
    for (int i = offset; i < size; i++) {
        d[i].real = 1;
        d[i].imag = 1;
    }
}
// Allones float
inline void allonesData(int size, int offset, float* d) {
    // initial zeros
    for (int i = 0; i < offset; i++) {
        d[i] = 0.0;
    }
    for (int i = offset; i < size; i++) {
        d[i] = 1.0;
    }
}
// Allones cfloat
inline void allonesData(int size, int offset, cfloat* d) {
    // initial zeros
    for (int i = 0; i < offset; i++) {
        d[i].real = 0.0;
        d[i].imag = 0.0;
    }
    for (int i = offset; i < size; i++) {
        d[i].real = 1.0;
        d[i].imag = 1.0;
    }
}

//----------------------------------------------------------------------
// inconesData variants

// Incones int16_t
inline void inconesData(int size, int offset, int16_t* d) {
    int16_t acc = 1;
    // initial zeros
    for (int i = 0; i < offset; i++) {
        d[i] = 0;
    }
    for (int i = offset; i < size; i++) {
        d[i] = acc++;
    }
}
// Incones int32
inline void inconesData(int size, int offset, int32* d) {
    int32 acc = 1;
    // initial zeros
    for (int i = 0; i < offset; i++) {
        d[i] = 0;
    }
    for (int i = offset; i < size; i++) {
        d[i] = acc++;
    }
}
// Incones cint16_t
inline void inconesData(int size, int offset, cint16_t* d) {
    int16_t acc = 1;
    // initial zeros
    for (int i = 0; i < offset; i++) {
        d[i].real = nullInt16();
        d[i].imag = nullInt16();
    }
    for (int i = offset; i < size; i++) {
        d[i].real = acc++;
        d[i].imag = acc++;
    }
}
// Incones cint32
inline void inconesData(int size, int offset, cint32* d) {
    int32 acc = 1;
    // initial zeros
    for (int i = 0; i < offset; i++) {
        d[i].real = nullInt32();
        d[i].imag = nullInt32();
    }
    for (int i = offset; i < size; i++) {
        d[i].real = acc++;
        d[i].imag = acc++;
    }
}
// Incones float
inline void inconesData(int size, int offset, float* d) {
    float acc = 1.0;
    // initial zeros
    for (int i = 0; i < offset; i++) {
        d[i] = 0.0;
    }
    for (int i = offset; i < size; i++) {
        d[i] = acc;
        acc += 1.0;
    }
}
// Incones cfloat
inline void inconesData(int size, int offset, cfloat* d) {
    float acc = 1;
    // initial zeros
    for (int i = 0; i < offset; i++) {
        d[i].real = 0.0;
        d[i].imag = 0.0;
    }
    for (int i = offset; i < size; i++) {
        d[i].real = acc;
        acc += 1.0;
        d[i].imag = acc;
        acc += 1.0;
    }
}

//----------------------------------------------------------------------
// symInconesData variants

// symIncones int16_t
inline void symIncOnesData(int size, int offset, int16_t* d) {
    int16_t acc = 1; // non-zero since zero is background.
    // initial zeros
    for (int i = 0; i < offset; i++) {
        d[i] = 0;
    }
    for (int i = offset; i < size / 2 + 1; i++) {
        d[i] = acc++;
        d[size - 1 - i] = d[i];
    }
}
// symIncones int32
inline void symIncOnesData(int size, int offset, int32* d) {
    int32 acc = 1; // non-zero since zero is background.
    // initial zeros
    for (int i = 0; i < offset; i++) {
        d[i] = 0;
    }
    for (int i = offset; i < size / 2 + 1; i++) {
        d[i] = acc++;
        d[size - 1 - i] = d[i];
    }
}
// symIncones cint16_t
inline void symIncOnesData(int size, int offset, cint16_t* d) {
    int16_t acc = 1; // non-zero since zero is background.
    // initial zeros
    for (int i = 0; i < offset; i++) {
        d[i].real = nullInt16();
        d[i].imag = nullInt16();
    }
    for (int i = offset; i < size / 2 + 1; i++) {
        d[i].real = acc++;
        d[i].imag = acc++;
        d[size - 1 - i].real = d[i].real;
        d[size - 1 - i].imag = d[i].imag;
    }
}
// symIncones cint32
inline void symIncOnesData(int size, int offset, cint32* d) {
    int32 acc = 1; // non-zero since zero is background.
    // initial zeros
    for (int i = 0; i < offset; i++) {
        d[i].real = nullInt32();
        d[i].imag = nullInt32();
    }
    for (int i = offset; i < size / 2 + 1; i++) {
        d[i].real = acc++;
        d[i].imag = acc++;
        d[size - 1 - i].real = d[i].real;
        d[size - 1 - i].imag = d[i].imag;
    }
}
// symIncones float
inline void symIncOnesData(int size, int offset, float* d) {
    float acc = 1.0; // non-zero since zero is background.
    // initial zeros
    for (int i = 0; i < offset; i++) {
        d[i] = 0.0;
    }
    for (int i = offset; i < size / 2 + 1; i++) {
        d[i] = acc;
        acc += 1.0;
        d[size - 1 - i] = d[i];
    }
}
// symIncones cfloat
inline void symIncOnesData(int size, int offset, cfloat* d) {
    float acc = 1; // non-zero since zero is background.
    // initial zeros
    for (int i = 0; i < offset; i++) {
        d[i].real = 0.0;
        d[i].imag = 0.0;
    }
    for (int i = offset; i < size / 2 + 1; i++) {
        d[i].real = acc;
        acc += 1.0;
        d[i].imag = acc;
        acc += 1.0;
        d[size - 1 - i].real = d[i].real;
        d[size - 1 - i].imag = d[i].imag;
    }
}

//----------------------------------------------------------------------
// sineData variants

// sine int16_t
inline void sineData(int size, int offset, int16_t* d) {
#define MY_PI 3.1415926
    float theta;

    for (int i = 0; i < size / 8; i++) {
        theta = (double)i * 2.0 * MY_PI / (double)size;
        d[i] = (int16_t)(sin(theta) * (double)16384.0);
        d[size / 4 - i] = (int16_t)(cos(theta) * (double)16384.0);
    }
    d[size / 8] = 11585; // 16384/sqrt(2)
    for (int i = 1; i < size / 4; i++) {
        d[size / 2 - i] = d[i];
    }
    for (int i = 1; i < size / 2; i++) {
        d[i + size / 2] = -d[i];
    }
}

inline void sineData(int size, int offset, cint16_t* d) {
#define MY_PI 3.1415926
    float theta;

    for (int i = 0; i < size / 8; i++) {
        theta = (double)i * 2.0 * MY_PI / (double)size;
        d[i].imag = (int16_t)(sin(theta) * (double)16384.0);
        d[size / 4 - i].imag = (int16_t)(cos(theta) * (double)16384.0);
    }
    d[size / 8].imag = 11585; // 16384/sqrt(2)
    for (int i = 1; i < size / 4; i++) {
        d[size / 2 - i].imag = d[i].imag;
    }
    for (int i = 1; i < size / 2; i++) {
        d[i + size / 2].imag = -d[i].imag;
    }
    for (int i = 0; i < size; i++) {
        d[i].real = d[(i + size / 4) % size].imag;
    }
}

inline void sineData(int size, int offset, int32_t* d) {
#define MY_PI 3.1415926
    float theta;

    for (int i = 0; i < size / 8; i++) {
        theta = (double)i * 2.0 * MY_PI / (double)size;
        d[i] = (int32_t)(sin(theta) * (double)16384.0);
        d[size / 4 - i] = (int32_t)(cos(theta) * (double)16384.0);
    }
    d[size / 8] = 11585; // 16384/sqrt(2)
    for (int i = 1; i < size / 4; i++) {
        d[size / 2 - i] = d[i];
    }
    for (int i = 1; i < size / 2; i++) {
        d[i + size / 2] = -d[i];
    }
}

inline void sineData(int size, int offset, cint32_t* d) {
#define MY_PI 3.1415926
    float theta;

    for (int i = 0; i < size / 8; i++) {
        theta = (double)i * 2.0 * MY_PI / (double)size;
        d[i].imag = (int32_t)(sin(theta) * (double)16384.0);
        d[size / 4 - i].imag = (int32_t)(cos(theta) * (double)16384.0);
    }
    d[size / 8].imag = 11585; // 16384/sqrt(2)
    for (int i = 1; i < size / 4; i++) {
        d[size / 2 - i].imag = d[i].imag;
    }
    for (int i = 1; i < size / 2; i++) {
        d[i + size / 2].imag = -d[i].imag;
    }
    for (int i = 0; i < size; i++) {
        d[i].real = d[(i + size / 4) % size].imag;
    }
}

inline void sineData(int size, int offset, float* d) {
#define MY_PI 3.1415926
    float theta;

    for (int i = 0; i < size / 8; i++) {
        theta = (double)i * 2.0 * MY_PI / (double)size;
        d[i] = (float)(sin(theta) * (double)16384.0);
        d[size / 4 - i] = (float)(cos(theta) * (double)16384.0);
    }
    d[size / 8] = 11585; // 16384/sqrt(2)
    for (int i = 1; i < size / 4; i++) {
        d[size / 2 - i] = d[i];
    }
    for (int i = 1; i < size / 2; i++) {
        d[i + size / 2] = -d[i];
    }
}

inline void sineData(int size, int offset, cfloat* d) {
#define MY_PI 3.1415926
    float theta;

    for (int i = 0; i < size / 8; i++) {
        theta = (double)i * 2.0 * MY_PI / (double)size;
        d[i].real = (float)(cos(theta) * (double)16384.0);
        d[i].imag = (float)(sin(theta) * (double)16384.0);
        d[size / 4 - i].real = d[i].imag;
        d[size / 4 - i].imag = d[i].real;
    }
    d[size / 8].imag = 11585; // 16384/sqrt(2)
    for (int i = 1; i < size / 4; i++) {
        d[size / 2 - i].imag = d[i].imag;
    }
    for (int i = 1; i < size / 2; i++) {
        d[i + size / 2].imag = -d[i].imag;
    }
    for (int i = 0; i < size; i++) {
        d[i].real = d[(i + size / 4) % size].imag;
    }
}

// print array
inline void printf_array(char* info, int16_t* array, size_t fir_len) {
    printf("%s: \n", info);
    for (unsigned int i = 0; i < fir_len; i++) {
        if (i % 16 == 0) {
            printf("[%d]:", i);
        }
        printf("%6d ", array[i]);
        if (i % 16 == 15) {
            printf("\n");
        }
    }
    printf("\n ");
}
} // namespace testcase
} // namespace aie
} // namespace dsp
} // namespace xf
#endif // _DSPLIB_UTILS_HPP_

/*  (c) Copyright 2019 Xilinx, Inc. All rights reserved.

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
