#ifndef hash_h_
#define hash_h_

#include <stdint.h>


uint32_t aggr_hash(uint32_t key);
uint32_t global_partition_hash(uint32_t key);

#endif
