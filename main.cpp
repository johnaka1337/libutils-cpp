#include "utils/platform.hpp"

#include <print>

#include <experimental/timer>
int main()
{
    using namespace std;
    using namespace utils::platform;
    const auto& f = cpu::GetFeatures();


    println( "Vendor: '{}'", f.vendor );
    println( "CPU name: {}", f.brand );
    println( "Ext. features level: {}", f.level );
    println( "SSE3: {}\nHTT: {}", f.sse3, f.htt );
    println( "AVX: {}\nAVX2: {}\nAVX512F: {}", f.avx, f.avx2, f.avx512f );
    std::println( "--- CPU Caches ---" );
    std::println( "L1 Data       : {} KB", f.l1d_kb );
    std::println( "L1 Instruction: {} KB", f.l1i_kb );
    std::println( "L2 Cache      : {} KB", f.l2_kb );
    std::println( "L3 Cache      : {} KB", f.l3_kb );
}
/*#include <iostream>
#include <vector>
#include <string_view>
#include <chrono>
#include <bit>         // Для std::popcount (C++20)
#include <immintrin.h> // Для интринсиков

// ---------------------------------------------------------
// 1. Обычный (Скалярный) режим
// ---------------------------------------------------------
size_t count_scalar(std::string_view s, char target)
{
    size_t count = 0;
    for (char c : s)
    {
        if (c == target) count++;
    }
    return count;
}

// ---------------------------------------------------------
// 2. Режим SSE2 (128 бит / 16 байт за такт)
// ---------------------------------------------------------
size_t count_sse2(std::string_view s, char target)
{
    size_t count = 0;
    const char* ptr = s.data();
    size_t len = s.size();

    // Создаем 128-битный вектор, заполненный нашим символом (16 раз подряд)
    __m128i v_target = _mm_set1_epi8(target);

    // Идем блоками по 16 байт
    while (len >= 16)
    {
        // Загружаем 16 байт из строки
        __m128i v_chunk = _mm_loadu_si128(reinterpret_cast<const __m128i*>(ptr));

        // Сравниваем каждый из 16 символов.
        // Если совпало, байт станет 0xFF, если нет - 0x00.
        __m128i v_cmp = _mm_cmpeq_epi8(v_chunk, v_target);

        // Магия! Сжимаем 16 байт по 0xFF/0x00 в один 16-битный int (каждый бит = результат сравнения)
        uint32_t mask = _mm_movemask_epi8(v_cmp);

        // Считаем единички в маске (аппаратная инструкция POPCNT)
        count += std::popcount(mask);

        ptr += 16;
        len -= 16;
    }

    // Добиваем "хвост" строки (менее 16 байт), если он остался
    for (size_t i = 0; i < len; ++i) {
        if (ptr[i] == target) count++;
    }
    return count;
}

// ---------------------------------------------------------
// 3. Режим AVX2 (256 бит / 32 байта за такт)
// ---------------------------------------------------------
// Важно: Эта функция вызовет краш (Illegal Instruction) на старых ПК без AVX2.
// В реальном проекте перед ее вызовом мы бы сделали `if (GetFeatures().avx2)`.
__attribute__((target("avx2"))) // Подсказка для GCC/Clang
size_t count_avx2(std::string_view s, char target)
{
    size_t count = 0;
    const char* ptr = s.data();
    size_t len = s.size();

    // Создаем 256-битный вектор (32 одинаковых символа)
    __m256i v_target = _mm256_set1_epi8(target);

    // Идем огромными блоками по 32 байта!
    while (len >= 32)
    {
        __m256i v_chunk = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(ptr));
        __m256i v_cmp   = _mm256_cmpeq_epi8(v_chunk, v_target);

        // Сжимаем 32 байта в один 32-битный int
        uint32_t mask   = _mm256_movemask_epi8(v_cmp);

        count += std::popcount(mask);

        ptr += 32;
        len -= 32;
    }

    // Добиваем хвост
    for (size_t i = 0; i < len; ++i) {
        if (ptr[i] == target) count++;
    }
    return count;
}

// ---------------------------------------------------------
// Тестирование
// ---------------------------------------------------------
int main()
{
    // Генерируем тестовую строку размером 1 ГИГАБАЙТ
    std::cout << "Generating 1GB text buffer...\n";
    size_t size = 1024 * 1024 * 1024;
    std::string data(size, 'a');

    // Вставляем наш искомый символ '\n' в случайные места
    for (size_t i = 0; i < size; i += 137) {
        data[i] = '\n';
    }

    std::string_view view = data;
    char target = '\n';

    auto run_test = [&](const char* name, auto func) {
        auto start = std::chrono::high_resolution_clock::now();
        size_t res = func(view, target);
        auto end = std::chrono::high_resolution_clock::now();

        std::chrono::duration<double, std::milli> ms = end - start;
        std::cout << name << ": \t" << ms.count() << " ms \t(Found: " << res << ")\n";
    };

    std::cout << "Starting benchmark...\n";
    run_test("Scalar", count_scalar);
    run_test("SSE2  ", count_sse2);
    run_test("AVX2  ", count_avx2);

    return 0;
}
*/
