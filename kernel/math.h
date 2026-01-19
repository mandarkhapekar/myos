/*
 * ============================================================================
 * Math Library Header
 * ============================================================================
 * Floating-point math functions for MyOS
 * Used by LLM inference engine
 * ============================================================================
 */

#ifndef MYOS_MATH_H
#define MYOS_MATH_H

#include "kernel.h"

/* Constants */
#define M_PI        3.14159265358979323846f
#define M_E         2.71828182845904523536f
#define M_LN2       0.69314718055994530942f
#define M_LN10      2.30258509299404568402f
#define M_SQRT2     1.41421356237309504880f

/* Floating point limits */
#define FLT_MAX     3.402823466e+38f
#define FLT_MIN     1.175494351e-38f
#define FLT_EPSILON 1.192092896e-07f

/* Basic math functions */
float fabsf(float x);
float fmodf(float x, float y);
float floorf(float x);
float ceilf(float x);
float roundf(float x);

/* Exponential and logarithmic */
float expf(float x);
float logf(float x);
float log10f(float x);
float powf(float x, float y);

/* Trigonometric (basic) */
float sinf(float x);
float cosf(float x);
float tanhf(float x);

/* Square root */
float sqrtf(float x);
float rsqrtf(float x);  /* Fast reciprocal sqrt */

/* Min/max */
static inline float fminf(float a, float b) { return a < b ? a : b; }
static inline float fmaxf(float a, float b) { return a > b ? a : b; }

/* Clamping */
static inline float clampf(float x, float lo, float hi) {
    return fminf(fmaxf(x, lo), hi);
}

#endif /* MYOS_MATH_H */
