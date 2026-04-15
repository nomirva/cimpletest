#include "test_allocator.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct allocation_entry {
    void *ptr;
    size_t size;
    struct allocation_entry *next;
} allocation_entry_t;

static struct {
    allocation_entry_t *head;
    size_t total_allocations;
    size_t total_frees;
    size_t active_allocations;
    size_t total_allocated_bytes;
    size_t total_freed_bytes;
    size_t double_free_count;
    size_t invalid_free_count;
} stats = {0};

static allocation_entry_t* find_allocation(void *ptr) {
    allocation_entry_t *current = stats.head;
    while (current) {
        if (current->ptr == ptr) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

static void add_allocation(void *ptr, size_t size) {
    if (!ptr) return;
    
    allocation_entry_t *entry = malloc(sizeof(allocation_entry_t));
    if (!entry) return;
    
    entry->ptr = ptr;
    entry->size = size;
    entry->next = stats.head;
    stats.head = entry;
    
    stats.total_allocations++;
    stats.active_allocations++;
    stats.total_allocated_bytes += size;
}

static bool remove_allocation(void *ptr, bool is_double_free_check) {
    if (!ptr) return false;
    
    allocation_entry_t *current = stats.head;
    allocation_entry_t *prev = NULL;
    
    while (current) {
        if (current->ptr == ptr) {
            if (prev) {
                prev->next = current->next;
            } else {
                stats.head = current->next;
            }
            
            stats.total_frees++;
            stats.active_allocations--;
            stats.total_freed_bytes += current->size;
            
            free(current);
            return true;
        }
        prev = current;
        current = current->next;
    }
    
    if (is_double_free_check) {
        stats.double_free_count++;
        fprintf(stderr, "ERROR: Double free detected for pointer %p\n", ptr);
    } else {
        stats.invalid_free_count++;
        fprintf(stderr, "ERROR: Invalid free detected for pointer %p (not allocated or already freed)\n", ptr);
    }
    
    return false;
}

static void* tracked_malloc(size_t size) {
    void *ptr = malloc(size);
    if (ptr) {
        add_allocation(ptr, size);
    }
    return ptr;
}

static void tracked_free(void *ptr) {
    if (!ptr) {
        fprintf(stderr, "\nWARNING: Free called with NULL pointer\n");
        return;
    }
    
    if (remove_allocation(ptr, true)) {
        free(ptr);
    }
}

static void* tracked_realloc(void *ptr, size_t new_size) {
    if (!ptr) {
        return tracked_malloc(new_size);
    }
    
    if (new_size == 0) {
        tracked_free(ptr);
        return NULL;
    }
    
    allocation_entry_t *old_entry = find_allocation(ptr);
    size_t old_size = old_entry ? old_entry->size : 0;

    void *new_ptr = realloc(ptr, new_size);
    
    if (new_ptr) {
        if (old_entry) {
            remove_allocation(ptr, false);
        }
        add_allocation(new_ptr, new_size);
        
        if (new_ptr != ptr && old_entry) {
            // realloc переместил память
        }
    } else if (old_entry) {
        fprintf(stderr, "WARNING: realloc failed for pointer %p, old size %zu, new size %zu\n", 
                ptr, old_size, new_size);
    }
    
    return new_ptr;
}

static void* tracked_calloc(size_t nmemb, size_t size) {
    void *ptr = calloc(nmemb, size);
    if (ptr) {
        add_allocation(ptr, nmemb * size);
    }
    return ptr;
}

const Allocator test_allocator = {
    .malloc = tracked_malloc,
    .free = tracked_free,
    .realloc = tracked_realloc,
    .calloc = tracked_calloc
};

void test_allocator_print_stats(void) {
    printf("\n=== Test Allocator Statistics ===\n");
    printf("Total allocations:  %zu\n", stats.total_allocations);
    printf("Total frees:        %zu\n", stats.total_frees);
    printf("Active allocations: %zu\n", stats.active_allocations);
    printf("Total allocated:    %zu bytes\n", stats.total_allocated_bytes);
    printf("Total freed:        %zu bytes\n", stats.total_freed_bytes);
    printf("Double free errors: %zu\n", stats.double_free_count);
    printf("Invalid free errors: %zu\n", stats.invalid_free_count);
    
    if (stats.active_allocations > 0) {
        printf("\nLeaked allocations:\n");
        allocation_entry_t *current = stats.head;
        while (current) {
            printf("  %p: %zu bytes\n", current->ptr, current->size);
            current = current->next;
        }
    } else {
        printf("\nNo memory leaks detected.\n");
    }
    
    printf("================================\n\n");
}

void test_allocator_reset_stats(void) {
    allocation_entry_t *current = stats.head;
    while (current) {
        allocation_entry_t *next = current->next;
        free(current->ptr);
        free(current);
        current = next;
    }
    
    memset(&stats, 0, sizeof(stats));
}

bool test_allocator_has_leaks(void) {
    return (stats.active_allocations > 0);
}

size_t test_allocator_get_allocation_count(void) {
    return stats.total_allocations;
}

size_t test_allocator_get_active_allocations(void) {
    return stats.active_allocations;
}