/*
 * ============================================================================
 * Math Library Implementation
 * ============================================================================
 * Software floating-point math for MyOS (no FPU instructions required).
 * These are not the fastest implementations, but they work!
 * 
 * Algorithms used:
 * - expf: Taylor series
 * - logf: Newton-Raphson iteration
 * - sqrtf: Babylonian method
 * - sinf/cosf: Taylor series with range reduction
 * - tanhf: Definition using expf
 * ============================================================================
 */

#include "math.h"

/* ============================================================================
 * Basic Functions
 * ============================================================================ */

/* Absolute value */
float fabsf(float x) {
    return x < 0 ? -x : x;
}

/* Floor - round toward negative infinity */
float floorf(float x) {
    int i = (int)x;
    return (float)(x < i ? i - 1 : i);
}

/* Ceiling - round toward positive infinity */
float ceilf(float x) {
    int i = (int)x;
    return (float)(x > i ? i + 1 : i);
}

/* Round to nearest */
float roundf(float x) {
    return floorf(x + 0.5f);
}

/* Floating modulo */
float fmodf(float x, float y) {
    if (y == 0.0f) return 0.0f;
    return x - floorf(x / y) * y;
}

/* ============================================================================
 * Exponential: expf(x) = e^x
 * Uses Taylor series: e^x = 1 + x + x²/2! + x³/3! + ...
 * ============================================================================ */

float expf(float x) {
    /* Handle edge cases */
    if (x > 88.0f) return FLT_MAX;      /* Overflow */
    if (x < -88.0f) return 0.0f;        /* Underflow */
    
    /* Range reduction: e^x = 2^k * e^r where r = x - k*ln(2) */
    /* This improves accuracy for large x */
    int k = (int)(x / M_LN2);
    float r = x - k * M_LN2;
    
    /* Taylor series for e^r (|r| <= ln(2)/2) */
    float result = 1.0f;
    float term = 1.0f;
    
    for (int i = 1; i <= 20; i++) {
        term *= r / i;
        result += term;
        if (fabsf(term) < FLT_EPSILON) break;
    }
    
    /* Multiply by 2^k */
    /* 2^k = (1 << k) for integer k */
    if (k > 0) {
        for (int i = 0; i < k; i++) result *= 2.0f;
    } else if (k < 0) {
        for (int i = 0; i < -k; i++) result *= 0.5f;
    }
    
    return result;
}

/* ============================================================================
 * Logarithm: logf(x) = ln(x)
 * Uses Newton-Raphson: y = ln(x) => x = e^y
 * We iterate: y_{n+1} = y_n + 2*(x - e^{y_n}) / (x + e^{y_n})
 * ============================================================================ */

float logf(float x) {
    /* Handle edge cases */
    if (x <= 0.0f) return -FLT_MAX;     /* Log of non-positive */
    if (x == 1.0f) return 0.0f;
    
    /* Range reduction: log(x) = log(m * 2^e) = log(m) + e*log(2) */
    /* where 1 <= m < 2 */
    int e = 0;
    float m = x;
    
    while (m >= 2.0f) { m *= 0.5f; e++; }
    while (m < 1.0f) { m *= 2.0f; e--; }
    
    /* Now 1 <= m < 2, use series for log(m) */
    /* log(m) = log(1 + (m-1)) using series */
    float t = (m - 1.0f) / (m + 1.0f);
    float t2 = t * t;
    
    /* log((1+t)/(1-t)) = 2*(t + t³/3 + t⁵/5 + ...) */
    float result = 0.0f;
    float term = t;
    
    for (int i = 1; i <= 21; i += 2) {
        result += term / i;
        term *= t2;
    }
    result *= 2.0f;
    
    /* Add back the exponent part */
    result += e * M_LN2;
    
    return result;
}

/* Log base 10 */
float log10f(float x) {
    return logf(x) / M_LN10;
}

/* ============================================================================
 * Power: powf(x, y) = x^y = e^(y * ln(x))
 * ============================================================================ */

float powf(float x, float y) {
    if (x == 0.0f) return 0.0f;
    if (y == 0.0f) return 1.0f;
    if (x == 1.0f) return 1.0f;
    
    /* Handle negative base with integer exponent */
    if (x < 0.0f) {
        int yi = (int)y;
        if ((float)yi != y) return 0.0f;  /* Non-integer power of negative */
        float result = expf(y * logf(-x));
        return (yi & 1) ? -result : result;
    }
    
    return expf(y * logf(x));
}

/* ============================================================================
 * Square Root: sqrtf(x)
 * Uses Babylonian method (Newton-Raphson for x² = n)
 * ============================================================================ */

float sqrtf(float x) {
    if (x < 0.0f) return 0.0f;
    if (x == 0.0f) return 0.0f;
    if (x == 1.0f) return 1.0f;
    
    /* Initial guess */
    float guess = x * 0.5f;
    if (x > 1.0f) guess = x;
    
    /* Newton-Raphson: y = (y + x/y) / 2 */
    for (int i = 0; i < 20; i++) {
        float new_guess = 0.5f * (guess + x / guess);
        if (fabsf(new_guess - guess) < FLT_EPSILON * guess) break;
        guess = new_guess;
    }
    
    return guess;
}

/* Fast reciprocal square root (1/sqrt(x)) */
/* Uses the famous Quake III "fast inverse square root" trick */
float rsqrtf(float x) {
    if (x <= 0.0f) return 0.0f;
    
    /* Convert float bits to int for bit manipulation */
    union { float f; uint32_t i; } conv;
    conv.f = x;
    conv.i = 0x5f3759df - (conv.i >> 1);  /* Magic! */
    
    /* One iteration of Newton-Raphson refinement */
    float y = conv.f;
    y = y * (1.5f - 0.5f * x * y * y);
    
    return y;
}

/* ============================================================================
 * Trigonometric Functions
 * ============================================================================ */

/* Helper: reduce angle to [-π, π] */
static float reduce_angle(float x) {
    while (x > M_PI) x -= 2.0f * M_PI;
    while (x < -M_PI) x += 2.0f * M_PI;
    return x;
}

/* Sine using Taylor series */
float sinf(float x) {
    x = reduce_angle(x);
    
    float x2 = x * x;
    float result = x;
    float term = x;
    
    for (int i = 1; i <= 10; i++) {
        term *= -x2 / ((2*i) * (2*i + 1));
        result += term;
    }
    
    return result;
}

/* Cosine using Taylor series */
float cosf(float x) {
    x = reduce_angle(x);
    
    float x2 = x * x;
    float result = 1.0f;
    float term = 1.0f;
    
    for (int i = 1; i <= 10; i++) {
        term *= -x2 / ((2*i - 1) * (2*i));
        result += term;
    }
    
    return result;
}

/* Hyperbolic tangent: tanh(x) = (e^x - e^-x) / (e^x + e^-x) */
float tanhf(float x) {
    /* For large |x|, tanh approaches ±1 */
    if (x > 10.0f) return 1.0f;
    if (x < -10.0f) return -1.0f;
    
    float ep = expf(x);
    float em = expf(-x);
    
    return (ep - em) / (ep + em);
}
