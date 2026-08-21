#include <iostream>
#include <iomanip>
#include <vector>
#include <chrono>
#include <string>
#include <cmath>
#include <cstring>

#include <Arduino.h>
#include <Audio.h>
#include <AudioStream.h>
#include "interpolation.h"
#include "ResamplingArrayReader.h"
#include "ResamplingSdReader.h"
#include "IndexableSDFile.h"

using namespace newdigate;
using namespace std::chrono;

struct BenchmarkResult {
    std::string name;
    double duration_ms;
    uint64_t total_ops;
    double ops_per_sec;
    double ns_per_op;
    double m_samples_per_sec;
};

static void print_header(const std::string &title) {
    std::cout << "\n========================================================================================\n";
    std::cout << "  " << title << "\n";
    std::cout << "========================================================================================\n";
    std::cout << std::left 
              << std::setw(52) << "Benchmark Scenario"
              << std::right 
              << std::setw(12) << "Time (ms)"
              << std::setw(14) << "Ops / Sec"
              << std::setw(12) << "ns / op"
              << std::setw(14) << "MSamples/s"
              << "\n";
    std::cout << "----------------------------------------------------------------------------------------\n";
}

static void print_result(const BenchmarkResult &res) {
    std::cout << std::left 
              << std::setw(52) << res.name
              << std::right 
              << std::fixed << std::setprecision(2)
              << std::setw(12) << res.duration_ms
              << std::scientific << std::setprecision(2)
              << std::setw(14) << res.ops_per_sec
              << std::fixed << std::setprecision(2)
              << std::setw(12) << res.ns_per_op
              << std::setw(14) << res.m_samples_per_sec
              << "\n";
}

// -----------------------------------------------------------------------------------------
// 1. Benchmark: Raw Interpolation Math Kernels
// -----------------------------------------------------------------------------------------
void benchmark_interpolation_kernels(int iterations = 20000000) {
    print_header("1. Interpolation Math Kernels");

    // Pre-generate input test patterns
    int16_t d1 = 1200, d2 = 3400, d3 = -2100, d4 = -8500;
    std::vector<float> x_vals(1024);
    for (size_t i = 0; i < x_vals.size(); i++) {
        x_vals[i] = 1.0f + static_cast<float>(i) / 1024.0f;
    }

    // Benchmark fastinterpolate
    {
        volatile int32_t accumulator = 0;
        auto start = high_resolution_clock::now();
        for (int i = 0; i < iterations; i++) {
            float x = x_vals[i & 1023];
            int16_t val = fastinterpolate(d1, d2, d3, d4, x);
            accumulator += val;
        }
        auto end = high_resolution_clock::now();
        double dur_ms = duration_cast<nanoseconds>(end - start).count() / 1e6;

        BenchmarkResult res;
        res.name = "fastinterpolate (4-point Lagrange kernel)";
        res.duration_ms = dur_ms;
        res.total_ops = iterations;
        res.ops_per_sec = (iterations / (dur_ms / 1000.0));
        res.ns_per_op = (dur_ms * 1e6) / iterations;
        res.m_samples_per_sec = (iterations / 1e6) / (dur_ms / 1000.0);
        print_result(res);
    }

    // Benchmark generic interpolate (4 points)
    {
        InterpolationData pts[4] = {
            {0, d1},
            {1, d2},
            {2, d3},
            {3, d4}
        };
        int generic_iterations = iterations / 4; // fewer iterations since it's much slower
        volatile int32_t accumulator = 0;
        auto start = high_resolution_clock::now();
        for (int i = 0; i < generic_iterations; i++) {
            double x = x_vals[i & 1023];
            int16_t val = interpolate(pts, x, 4);
            accumulator += val;
        }
        auto end = high_resolution_clock::now();
        double dur_ms = duration_cast<nanoseconds>(end - start).count() / 1e6;

        BenchmarkResult res;
        res.name = "interpolate (generic N-point polynomial)";
        res.duration_ms = dur_ms;
        res.total_ops = generic_iterations;
        res.ops_per_sec = (generic_iterations / (dur_ms / 1000.0));
        res.ns_per_op = (dur_ms * 1e6) / generic_iterations;
        res.m_samples_per_sec = (generic_iterations / 1e6) / (dur_ms / 1000.0);
        print_result(res);
    }
}

// -----------------------------------------------------------------------------------------
// 2. Benchmark: In-Memory Resampling Reader (ResamplingArrayReader)
// -----------------------------------------------------------------------------------------
void benchmark_resampling_reader(int target_blocks = 20000) {
    print_header("2. ResamplingArrayReader Throughput");

    const size_t audio_length = 44100 * 2; // 2 seconds at 44.1kHz (stereo = 88200 samples)
    std::vector<int16_t> audio_data(audio_length);
    for (size_t i = 0; i < audio_length; i++) {
        audio_data[i] = static_cast<int16_t>(sin(i * 0.05) * 16000.0);
    }

    const uint16_t block_size = 128; // AUDIO_BLOCK_SAMPLES

    struct Scenario {
        std::string name;
        uint16_t channels;
        double playbackRate;
        loop_type loopType;
        bool crossfade;
    };

    std::vector<Scenario> scenarios = {
        // Mono Playback Rates
        {"Mono 1ch | Rate 1.00x | No Loop", 1, 1.0, looptype_none, false},
        {"Mono 1ch | Rate 0.75x | Repeat", 1, 0.75, looptype_repeat, false},
        {"Mono 1ch | Rate 1.33x | Repeat", 1, 1.3333, looptype_repeat, false},
        {"Mono 2.50x | Repeat", 1, 2.5, looptype_repeat, false},

        // Reverse Playback
        {"Mono 1ch | Rate -1.00x | Repeat", 1, -1.0, looptype_repeat, false},
        {"Mono 1ch | Rate -1.25x | Repeat", 1, -1.25, looptype_repeat, false},

        // Stereo & Multi-Channel
        {"Stereo 2ch | Rate 1.00x | Repeat", 2, 1.0, looptype_repeat, false},
        {"Stereo 2ch | Rate 0.85x | Repeat", 2, 0.85, looptype_repeat, false},
        {"Stereo 2ch | Rate 1.50x | Repeat", 2, 1.5, looptype_repeat, false},
        {"Quad 4ch | Rate 1.25x | Repeat", 4, 1.25, looptype_repeat, false},

        // Looping & Crossfade
        {"Stereo 2ch | Rate 1.00x | PingPong | No Crossfade", 2, 1.0, looptype_pingpong, false},
        {"Stereo 2ch | Rate 1.00x | Repeat | Crossfade 256", 2, 1.0, looptype_repeat, true},
        {"Stereo 2ch | Rate 0.90x | PingPong | Crossfade 256", 2, 0.9, looptype_pingpong, true},
    };

    int16_t out_buffers[8][128];
    void *out_ptrs[8];
    for (int i = 0; i < 8; i++) {
        out_ptrs[i] = out_buffers[i];
    }

    for (const auto &sc : scenarios) {
        ResamplingArrayReader reader;
        reader.begin();
        reader.setLoopType(sc.loopType);
        if (sc.crossfade) {
            reader.setCrossfadeDurationInSamples(256);
        }
        reader.setPlaybackRate(sc.playbackRate);

        uint32_t samples_per_channel = audio_length / sc.channels;
        reader.playRaw(audio_data.data(), samples_per_channel, sc.channels);
        reader.setLoopStart(0);
        reader.setLoopFinish(samples_per_channel);

        uint64_t total_samples_read = 0;
        auto start = high_resolution_clock::now();
        for (int b = 0; b < target_blocks; b++) {
            unsigned int read_count = reader.read(out_ptrs, block_size);
            total_samples_read += (read_count * sc.channels);
            if (read_count == 0 && sc.loopType == looptype_none) {
                // Rewind for non-looping benchmark
                reader.playRaw(audio_data.data(), samples_per_channel, sc.channels);
            }
        }
        auto end = high_resolution_clock::now();
        double dur_ms = duration_cast<nanoseconds>(end - start).count() / 1e6;

        BenchmarkResult res;
        res.name = sc.name;
        res.duration_ms = dur_ms;
        res.total_ops = total_samples_read;
        res.ops_per_sec = (total_samples_read / (dur_ms / 1000.0));
        res.ns_per_op = (dur_ms * 1e6) / total_samples_read;
        res.m_samples_per_sec = (total_samples_read / 1e6) / (dur_ms / 1000.0);
        print_result(res);
        reader.close();
    }
}

// -----------------------------------------------------------------------------------------
// 3. Benchmark: IndexableFile Buffer Lookup & Cache Access
// -----------------------------------------------------------------------------------------
void benchmark_indexable_file(int iterations = 10000000) {
    print_header("3. IndexableSDFile Buffer Lookup & Caching");

    // Setup simulated SD card data
    const size_t file_len_samples = 44100 * 4; // 4 seconds of 16-bit samples = 352800 bytes
    std::vector<int16_t> mock_file(file_len_samples);
    for (size_t i = 0; i < file_len_samples; i++) {
        mock_file[i] = static_cast<int16_t>(i & 0x7FFF);
    }
    SD.setSDCardFileData(reinterpret_cast<char*>(mock_file.data()), file_len_samples * sizeof(int16_t));

    File f = SD.open("test.raw");
    IndexableSDFile<512, 4> indexable_file("test.raw", SD, f);

    // Scenario A: Sequential Forward Access (Cache Hit in current buffer)
    {
        volatile int32_t accumulator = 0;
        auto start = high_resolution_clock::now();
        long pos = 0;
        for (int i = 0; i < iterations; i++) {
            accumulator += indexable_file[pos];
            pos++;
            if (pos >= static_cast<long>(file_len_samples)) pos = 0;
        }
        auto end = high_resolution_clock::now();
        double dur_ms = duration_cast<nanoseconds>(end - start).count() / 1e6;

        BenchmarkResult res;
        res.name = "IndexableFile Sequential Access (512x4 buffers)";
        res.duration_ms = dur_ms;
        res.total_ops = iterations;
        res.ops_per_sec = (iterations / (dur_ms / 1000.0));
        res.ns_per_op = (dur_ms * 1e6) / iterations;
        res.m_samples_per_sec = (iterations / 1e6) / (dur_ms / 1000.0);
        print_result(res);
    }

    // Scenario B: Reverse Sequential Access
    {
        volatile int32_t accumulator = 0;
        auto start = high_resolution_clock::now();
        long pos = file_len_samples - 1;
        for (int i = 0; i < iterations; i++) {
            accumulator += indexable_file[pos];
            pos--;
            if (pos < 0) pos = file_len_samples - 1;
        }
        auto end = high_resolution_clock::now();
        double dur_ms = duration_cast<nanoseconds>(end - start).count() / 1e6;

        BenchmarkResult res;
        res.name = "IndexableFile Reverse Access (512x4 buffers)";
        res.duration_ms = dur_ms;
        res.total_ops = iterations;
        res.ops_per_sec = (iterations / (dur_ms / 1000.0));
        res.ns_per_op = (dur_ms * 1e6) / iterations;
        res.m_samples_per_sec = (iterations / 1e6) / (dur_ms / 1000.0);
        print_result(res);
    }

    // Scenario C: Resampling Stride Access (e.g. 1.75x sample step)
    {
        volatile int32_t accumulator = 0;
        auto start = high_resolution_clock::now();
        double pos = 0.0;
        for (int i = 0; i < iterations; i++) {
            accumulator += indexable_file[static_cast<long>(pos)];
            pos += 1.75;
            if (pos >= file_len_samples) pos = 0.0;
        }
        auto end = high_resolution_clock::now();
        double dur_ms = duration_cast<nanoseconds>(end - start).count() / 1e6;

        BenchmarkResult res;
        res.name = "IndexableFile Stride 1.75x Access (512x4 buffers)";
        res.duration_ms = dur_ms;
        res.total_ops = iterations;
        res.ops_per_sec = (iterations / (dur_ms / 1000.0));
        res.ns_per_op = (dur_ms * 1e6) / iterations;
        res.m_samples_per_sec = (iterations / 1e6) / (dur_ms / 1000.0);
        print_result(res);
    }

    indexable_file.close();
    f.close();
}

int main(int argc, char **argv) {
    std::cout << "\n========================================================================================\n";
    std::cout << "                 TEENSY VARIABLE PLAYBACK - PERFORMANCE BENCHMARK SUITE                \n";
    std::cout << "========================================================================================\n";

    int iter_scale = 1;
    if (argc > 1) {
        iter_scale = std::max(1, std::atoi(argv[1]));
    }

    benchmark_interpolation_kernels(20000000 * iter_scale);
    benchmark_resampling_reader(20000 * iter_scale);
    benchmark_indexable_file(10000000 * iter_scale);

    std::cout << "\n========================================================================================\n";
    std::cout << "                               BENCHMARKS COMPLETED                                      \n";
    std::cout << "========================================================================================\n\n";
    return 0;
}
