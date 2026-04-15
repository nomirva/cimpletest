#ifndef TALLOCATOR_H
#define TALLOCATOR_H

#include "../src/test_allocator.h"

#include <stddef.h>
#include <stdbool.h>

// extern const Allocator test_allocator;

void test_allocator_print_stats(void);

void test_allocator_reset_stats(void);

bool test_allocator_has_leaks(void);

size_t test_allocator_get_allocation_count(void);

size_t test_allocator_get_active_allocations(void);

#endif