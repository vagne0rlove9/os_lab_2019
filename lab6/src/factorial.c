#include "factorial.h"

uint64_t MultModulo(uint64_t a, uint64_t b, uint64_t mod) {
  uint64_t result = 0;
  a = a % mod;
  while (b > 0) {
    if (b % 2 == 1)
      result = (result + a) % mod;
    a = (a * 2) % mod;
    b /= 2;
  }
  return result % mod;
}

struct sockaddr_in create_sockaddr(uint16_t port, uint32_t s_addr) {
  struct sockaddr_in sockaddr = {
    .sin_family = AF_INET, 
    .sin_port = htons(port), 
    .sin_addr.s_addr = s_addr, 
  };
  return sockaddr;
}