#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <immintrin.h> // UNLOCKS THE HARDWARE REGISTERS

#define PORT 8080
#define BUFFER_SIZE 1024

// --- THE REAL SIMD KERNEL ---
void validate_packet_simd(char* data, size_t len) {
    if (len < 32) {
        printf("[SIMD KERNEL]: Packet (%zu bytes) too small for AVX2. Skipping vectorization.\n", len);
        return;
    }

    // 1. Fill a 256-bit register with 'F' (The Forge Signature)
    __m256i signature = _mm256_set1_epi8('F');

    // 2. Load the first 32 bytes of the stream into a register
    __m256i chunk = _mm256_loadu_si256((__m256i*)data);

    // 3. Compare all 32 bytes simultaneously
    __m256i result = _mm256_cmpeq_epi8(chunk, signature);

    // 4. Generate a bitmask of the matches
    int mask = _mm256_movemask_epi8(result);

    if (mask != 0) {
        printf("[SIMD KERNEL]: !! HARDWARE MATCH !! Signature detected (Mask: 0x%X)\n", mask);
    } else {
        printf("[SIMD KERNEL]: [GENERIC_FLOW] - No match in vector chunk.\n");
    }
}

// --- THE PERSISTENT TRANSPORT LAYER ---
void start_listener() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    listen(server_fd, 3);
    printf("[FORGE-STREAM] AVX2 Infrastructure active on port %d...\n", PORT);

    while(1) {
        printf("[WAITING]: For live stream connection...\n");
        new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
        
        if (new_socket < 0) continue;

        printf("[STREAM ESTABLISHED]: Persistent channel open.\n");

        while(1) {
            ssize_t valread = read(new_socket, buffer, BUFFER_SIZE - 1);
            
            if (valread <= 0) {
                printf("[STREAM CLOSED]: Client disconnected.\n");
                break;
            }

            buffer[valread] = '\0';
            printf("[LIVE DATA]: %s\n", buffer);
            
            // EXECUTE SIMD SCAN ON THE HOT BUFFER
            validate_packet_simd(buffer, (size_t)valread);
            
            memset(buffer, 0, BUFFER_SIZE);
        }
        close(new_socket);
    }
}
