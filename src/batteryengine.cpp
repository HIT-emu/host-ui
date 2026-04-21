#include "batteryengine.h"

// Константы из твоего battery_fixed.c
#define LN2_Q16  0x0000B172
#define INV_LN2  0x00017154
#define CORDIC_ITER 16

static const int32_t atanh_table[CORDIC_ITER] = {
    0, 0x00008C9F, 0x00004163, 0x0000202B, 0x00001005, 0x00000801, 0x00000400, 0x00000200,
    0x00000100, 0x00000080, 0x00000040, 0x00000020, 0x00000010, 0x00000008, 0x00000004, 0x00000002
};

// Базовые операции из твоего кода
int32_t BatteryEngine::f_mul(int32_t a, int32_t b) {
    return (int32_t)(((int64_t)a * b) >> 16);
}

int32_t BatteryEngine::f_div(int32_t a, int32_t b) {
    if (b == 0) return 0;
    return (int32_t)(((int64_t)a << 16) / b);
}

// Реализация натурального логарифма (CORDIC)
int32_t BatteryEngine::f_ln(int32_t x) {
    if (x <= 0) return 0;
    int32_t shift_correction = 0;
    while (x < (1 << 15)) {
        x <<= 1;
        shift_correction += LN2_Q16;
    }
    int32_t x_cur = x + (1 << 16);
    int32_t y_cur = x - (1 << 16);
    int32_t z_cur = 0;

    for (int i = 1; i < CORDIC_ITER; i++) {
        int32_t dx = x_cur >> i;
        int32_t dy = y_cur >> i;
        if (y_cur < 0) {
            x_cur += dy; y_cur += dx; z_cur -= atanh_table[i];
        } else {
            x_cur -= dy; y_cur -= dx; z_cur += atanh_table[i];
        }
        if (i == 4) { // Специфика CORDIC для гиперболических функций
            dx = x_cur >> 4; dy = y_cur >> 4;
            if (y_cur < 0) { x_cur += dy; y_cur += dx; z_cur -= atanh_table[4]; }
            else { x_cur -= dy; y_cur -= dx; z_cur += atanh_table[4]; }
        }
    }
    return (z_cur << 1) - shift_correction;
}

// Реализация экспоненты
int32_t BatteryEngine::f_exp(int32_t x) {
    if (x < -0x00100000) return 0;
    int32_t k_f = f_mul(x, INV_LN2);
    int16_t k = (int16_t)((k_f + (1 << 15)) >> 16);
    int32_t r = x - (k * LN2_Q16);

    // Схема Горнера для приближения Тейлора
    int32_t poly = f_mul(r, 0x00000AAB); // P4
    poly += 0x00002AAB;                  // P3
    poly = f_mul(r, poly);
    poly += 0x00008000;                  // P2
    poly = f_mul(r, poly);
    poly += (1 << 16);                   // P1
    poly = f_mul(r, poly);
    poly += (1 << 16);                   // P0

    return (k >= 0) ? (poly << k) : (poly >> (-k));
}

double BatteryEngine::calculateVoltage(double I, double C) {
    // Входные double конвертируем в fixed-point Q16.16
    // Используем capacity_ah напрямую
    int32_t f_Q = (int32_t)(m_params.capacity_ah * 65536.0);
    int32_t f_I = (int32_t)(I * 65536.0);
    int32_t f_C = (int32_t)(C * 65536.0);

    int32_t f_E0 = (int32_t)(m_params.E0 * 65536.0);
    int32_t f_k1 = (int32_t)(m_params.k1 * 65536.0);
    int32_t f_k2 = (int32_t)(m_params.k2 * 65536.0);
    int32_t f_A  = (int32_t)(m_params.A * 65536.0);
    int32_t f_B  = (int32_t)(m_params.B * 65536.0);
    const int32_t f_R = 7144; // 0.109 Ohm в Q16.16

    // Модель: V = E0 - k1*ln(1-C/Q) + k2*ln(C/Q) - R*I + A*exp(-B*C)
    int32_t ratio = f_div(f_C, f_Q);
    int32_t ln1 = f_ln((1 << 16) - ratio);
    int32_t ln2 = f_ln(ratio);
    int32_t exp_term = f_mul(f_A, f_exp(-f_mul(f_B, f_C)));

    int32_t res = f_E0 + f_mul(f_k1, ln1) + f_mul(f_k2, ln2) - f_mul(f_R, f_I) + exp_term;

    return (double)res / 65536.0;
}





