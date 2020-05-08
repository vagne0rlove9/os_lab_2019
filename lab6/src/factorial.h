#include <stdint.h>
#include <netinet/in.h>
#include <pthread.h>

struct FactorialArgs {
  uint64_t begin;
  uint64_t end;
  uint64_t mod;
};

uint64_t MultModulo(uint64_t a, uint64_t b, uint64_t mod);
struct sockaddr_in create_sockaddr(uint16_t port, uint32_t s_addr);