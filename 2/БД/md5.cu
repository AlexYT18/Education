#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include <locale.h>
#include <time.h>
#include <string.h>
#include <malloc.h>
#include <stdio.h>
#include <stdint.h>
#include <intrin.h>
#pragma intrinsic(__rdtsc)

#define MAX_PASSWORDS 1000001
#define PASSWORD_LENGTH 256
#define LEFTROTATE(x, c) (((x) << (c)) | ((x) >> (32 - (c))))

__device__ void to_bytes(uint32_t val, uint8_t* bytes) {
    bytes[0] = (uint8_t)val;
    bytes[1] = (uint8_t)(val >> 8);
    bytes[2] = (uint8_t)(val >> 16);
    bytes[3] = (uint8_t)(val >> 24);
}

__device__ uint32_t to_int32(const uint8_t* bytes) {
    return (uint32_t)bytes[0] | ((uint32_t)bytes[1] << 8) | ((uint32_t)bytes[2] << 16) | ((uint32_t)bytes[3] << 24);
}
__device__ size_t cuda_strlen(const uint8_t* str) 
{
    size_t length = 0;
    while (str[length] != '\0') {
        length++;
    }
    return length;
}

__device__ void md5_single(uint8_t* initial_msg, size_t initial_len, uint8_t* digest) 
{
    if (initial_msg[cuda_strlen(initial_msg) - 1] == '\n') initial_msg[cuda_strlen(initial_msg) - 1] = '\0';
    if (initial_msg[cuda_strlen(initial_msg) - 2] == '\r') initial_msg[cuda_strlen(initial_msg) - 2] = '\0';
    initial_len = cuda_strlen(initial_msg);
    uint32_t r[] = { 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22,
                      5,  9, 14, 20, 5,  9, 14, 20, 5,  9, 14, 20, 5,  9, 14, 20,
                      4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23,
                      6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21 };

    uint32_t k[] = {
0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee ,
0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501 ,
0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be ,
0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821 ,
0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa ,
0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8 ,
0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed ,
0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a ,
0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c ,
0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70 ,
0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05 ,
0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665 ,
0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039 ,
0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1 ,
0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1 ,
0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391 
};

    uint32_t h0 = 0x67452301;
    uint32_t h1 = 0xefcdab89;
    uint32_t h2 = 0x98badcfe;
    uint32_t h3 = 0x10325476;

    size_t new_len = ((((initial_len + 8) / 64) + 1) * 64) - 8;
    uint8_t* msg = (uint8_t*)malloc(new_len + 64);
    if (!msg) return;

    memcpy(msg, initial_msg, initial_len);
    msg[initial_len] = 0x80;
    memset(msg + initial_len + 1, 0, new_len - initial_len - 1);

    to_bytes(initial_len * 8, msg + new_len);
    to_bytes(initial_len >> 29, msg + new_len + 4);

    for (size_t offset = 0; offset < new_len; offset += 64) {
        uint32_t w[16];
        for (int i = 0; i < 16; i++)
            w[i] = to_int32(msg + offset + i * 4);

        uint32_t a = h0, b = h1, c = h2, d = h3, f, g;

        for (int i = 0; i < 64; i++) {
            if (i < 16) {
                f = (b & c) | ((~b) & d);
                g = i;
            }
            else if (i < 32) {
                f = (d & b) | ((~d) & c);
                g = (5 * i + 1) % 16;
            }
            else if (i < 48) {
                f = b ^ c ^ d;
                g = (3 * i + 5) % 16;
            }
            else {
                f = c ^ (b | (~d));
                g = (7 * i) % 16;
            }

            uint32_t temp = d;
            d = c;
            c = b;
            b += LEFTROTATE((a + f + k[i] + w[g]), r[i]);
            a = temp;
        }

        h0 += a;
        h1 += b;
        h2 += c;
        h3 += d;
    }

    to_bytes(h0, digest);
    to_bytes(h1, digest + 4);
    to_bytes(h2, digest + 8);
    to_bytes(h3, digest + 12);
    free(msg);
}

__global__ void md5_kernel(uint8_t* input, uint8_t* output, size_t len, size_t num_passwords) 
{
    int idx = threadIdx.x + blockIdx.x * blockDim.x;
    if (idx >= num_passwords) return;
    //if (input[idx] == '\r' || input[idx] == '\n') input[idx] == '\0';
    uint8_t digest[16];
    md5_single(input + idx * len, len, digest);
    memcpy(output + idx * 16, digest, 16);
}

int main() 
{
    unsigned __int64 time = 0;
    FILE* input_file;
    //input_file = fopen("string.txt", "rb");
    input_file = fopen("Chinese-common-password-list-top-1000.txt", "rb");
    //input_file = fopen("Chinese-common-password-list-top-10000.txt", "rb");
    //input_file = fopen("Chinese-common-password-list-top-100000.txt", "rb");
    //input_file = fopen("Chinese-common-password-list-top-1000000.txt", "rb");
    //input_file = fopen("German_common-password-list-top-1000.txt", "rb");
    //input_file = fopen("German_common-password-list-top-10000.txt", "rb");
    //input_file = fopen("German_common-password-list-top-100000.txt", "rb");
    //input_file = fopen("German_common-password-list-top-1000000.txt", "rb");
    FILE* output_file = fopen("hashed_passwords.txt", "wb");
    if (!input_file || !output_file) return 1;

    char (*passwords)[PASSWORD_LENGTH] = (char(*)[PASSWORD_LENGTH])malloc(MAX_PASSWORDS * PASSWORD_LENGTH);//Поменять
    if (!passwords) {
        printf("Memory allocation failed!\n");
        return 1;
    }

    size_t num_passwords = 0;
    while (num_passwords < MAX_PASSWORDS && fgets(passwords[num_passwords], PASSWORD_LENGTH, input_file))//Поменять на длину паролей
    {
        num_passwords++;
    }
    fclose(input_file);

    uint8_t* d_input, * d_output;
    cudaMalloc((void**)&d_input, num_passwords * PASSWORD_LENGTH);//Поменять на длину паролей
    cudaMalloc((void**)&d_output, num_passwords * 16);

    cudaMemcpy(d_input, passwords, num_passwords * PASSWORD_LENGTH, cudaMemcpyHostToDevice);//Поменять на длину паролей

    int threadsPerBlock = 256;
    int blocksPerGrid = (num_passwords + threadsPerBlock - 1) / threadsPerBlock;
    time = __rdtsc();
    md5_kernel << <blocksPerGrid, threadsPerBlock >> > (d_input, d_output, PASSWORD_LENGTH, num_passwords);//Поменять на длину паролей
    cudaDeviceSynchronize();

    uint8_t* h_output = (uint8_t*)malloc(num_passwords * 16);
    cudaMemcpy(h_output, d_output, num_passwords * 16, cudaMemcpyDeviceToHost);
    time = __rdtsc() - time;
    for (size_t i = 0; i < num_passwords; i++) {
        for (int j = 0; j < 16; j++) {
            fprintf(output_file, "%02x", h_output[i * 16 + j]);
        }
        fprintf(output_file, "\n");
    }
	printf("\nTime: %llu\n", time);

    fclose(output_file);
    cudaFree(d_input);
    cudaFree(d_output);
    free(h_output);
    free(passwords);
    return 0;
}
