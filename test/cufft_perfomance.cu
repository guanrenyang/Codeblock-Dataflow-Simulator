#include <cufft.h>
#include <cuda_runtime.h>
#include <iostream>
#include <vector>
#include <chrono>

int main() {
    int gpu_id = 1;
    cudaSetDevice(gpu_id);

    // Get GPU properties
    cudaDeviceProp prop;
    cudaGetDeviceProperties(&prop, gpu_id);
    std::cout << "Using GPU: " << prop.name << " (ID: " << gpu_id << ")" << std::endl;

    // Get GPU clock frequency (prop.clockRate is in kHz and needs to be converted to Hz)
    float clock_rate_hz = prop.clockRate * 1000.0;  // convert kHz to Hz
    std::cout << "GPU Clock Rate: " << clock_rate_hz / 1e9 << " GHz" << std::endl;

    // Define a list of FFT sizes
    std::vector<int> fft_sizes = {128, 512, 1024, 2048, 4096, 8192, 16384, 32768, 65536};

    // Define the number of iterations
    const int num_iterations = 1024;

    for (int N : fft_sizes) {
        std::cout << "Running FFT for N = " << N << std::endl;

        // Host data allocation
        cufftComplex* h_data = (cufftComplex*)malloc(N * sizeof(cufftComplex));

        // Initialize input data
        for (int i = 0; i < N; i++) {
            h_data[i].x = i;  // Real part
            h_data[i].y = 0.0;  // Imaginary part
        }

        // Device data allocation
        cufftComplex* d_data;
        cudaMalloc((void**)&d_data, N * sizeof(cufftComplex));

        // Copy data from host to device
        cudaMemcpy(d_data, h_data, N * sizeof(cufftComplex), cudaMemcpyHostToDevice);

        // Create cuFFT plan
        cufftHandle plan;
        if (cufftPlan1d(&plan, N, CUFFT_C2C, 1) != CUFFT_SUCCESS) {
            std::cerr << "CUFFT error: Plan creation failed" << std::endl;
            free(h_data);
            cudaFree(d_data);
            continue;
        }

        // Start timing
        auto start_time = std::chrono::high_resolution_clock::now();

        // Perform FFT multiple times for averaging
        for (int i = 0; i < num_iterations; i++) {
            if (cufftExecC2C(plan, d_data, d_data, CUFFT_FORWARD) != CUFFT_SUCCESS) {
                std::cerr << "CUFFT error: ExecC2C failed" << std::endl;
                cufftDestroy(plan);
                free(h_data);
                cudaFree(d_data);
                return -1;
            }
            cudaDeviceSynchronize();
        }

        // Stop timing
        auto end_time = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed_seconds = end_time - start_time;
        double average_time = elapsed_seconds.count() / num_iterations;

        // Print the average time for this FFT size
        std::cout << "Average time for FFT size " << N << ": " << average_time * 1e3 << " ms" << std::endl;

        // Clean up
        cufftDestroy(plan);
        free(h_data);
        cudaFree(d_data);
    }

    return 0;
}