#ifndef MEMORIA_H
#define MEMORIA_H

#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define COLOR_GREEN   "\033[32m"
#define COLOR_RED     "\033[31m"
#define COLOR_RESET   "\033[0m"

void *memoria_malloc(size_t size);
void  memoria_free(void *ptr);
void *memoria_realloc(void *ptr, size_t new_size);
void *memoria_calloc(size_t nmemb, size_t size);

void *memoria_malloc_ex(size_t size, const char *file, int line, const char *func);
void  memoria_free_ex(void *ptr, const char *file, int line, const char *func);
void *memoria_realloc_ex(void *ptr, size_t new_size, const char *file, int line, const char *func);
void *memoria_calloc_ex(size_t nmemb, size_t size, const char *file, int line, const char *func);

void memoria_print_report(void);
void memoria_stats_reset(void);
bool memoria_stats_has_leaks(void);
size_t memoria_stats_alloc_count(void);
size_t memoria_stats_active_count(void);
size_t memoria_stats_double_free_count(void);
size_t memoria_stats_invalid_free_count(void);

#ifndef malloc
#define malloc(s)     memoria_malloc_ex(s, __FILE__, __LINE__, __func__)
#define free(p)       memoria_free_ex(p, __FILE__, __LINE__, __func__)
#define realloc(p,s)  memoria_realloc_ex(p, s, __FILE__, __LINE__, __func__)
#define calloc(n,s)   memoria_calloc_ex(n, s, __FILE__, __LINE__, __func__)
#endif

#ifdef MEMORIA_IMPLEMENTATION

typedef struct memoria_allocation_entry {
    void *ptr;
    size_t size;
    const char *file;
    int line;
    const char *func;
    struct memoria_allocation_entry *next;
} memoria_allocation_entry_t;

static struct {
    memoria_allocation_entry_t *head;
    size_t total_allocations;
    size_t total_frees;
    size_t active_allocations;
    size_t total_allocated_bytes;
    size_t total_freed_bytes;
    size_t double_free_count;
    size_t invalid_free_count;
} memoria_alloc_stats = {0};

static memoria_allocation_entry_t* memoria_find_allocation(void *ptr) {
    memoria_allocation_entry_t *current = memoria_alloc_stats.head;
    while (current) {
        if (current->ptr == ptr) return current;
        current = current->next;
    }
    return NULL;
}

static void memoria_add_allocation(void *ptr, size_t size, const char *file, int line, const char *func) {
    if (!ptr) return;
    memoria_allocation_entry_t *entry = (malloc)(sizeof(memoria_allocation_entry_t));
    if (!entry) return;
    entry->ptr = ptr;
    entry->size = size;
    entry->file = file;
    entry->line = line;
    entry->func = func;
    entry->next = memoria_alloc_stats.head;
    memoria_alloc_stats.head = entry;
    memoria_alloc_stats.total_allocations++;
    memoria_alloc_stats.active_allocations++;
    memoria_alloc_stats.total_allocated_bytes += size;
}

static bool memoria_remove_allocation(void *ptr, bool is_double_free_check,
                                      const char *file, int line, const char *func) {
    if (!ptr) return false;
    memoria_allocation_entry_t *current = memoria_alloc_stats.head;
    memoria_allocation_entry_t *prev = NULL;
    while (current) {
        if (current->ptr == ptr) {
            if (prev)
                prev->next = current->next;
            else
                memoria_alloc_stats.head = current->next;
            memoria_alloc_stats.total_frees++;
            memoria_alloc_stats.active_allocations--;
            memoria_alloc_stats.total_freed_bytes += current->size;
            (free)(current);
            return true;
        }
        prev = current;
        current = current->next;
    }
    if (is_double_free_check) {
        memoria_alloc_stats.double_free_count++;
        fprintf(stderr, "ERROR: Double free detected for pointer %p", ptr);
    } else {
        memoria_alloc_stats.invalid_free_count++;
        fprintf(stderr, "ERROR: Invalid free for pointer %p (not allocated or already freed)", ptr);
    }
    if (file) fprintf(stderr, " at %s:%d in %s", file, line, func ? func : "?");
    fprintf(stderr, "\n");
    return false;
}

void *memoria_malloc_ex(size_t size, const char *file, int line, const char *func) {
    void *ptr = (malloc)(size);
    if (ptr) memoria_add_allocation(ptr, size, file, line, func);
    return ptr;
}

void *memoria_malloc(size_t size) {
    return memoria_malloc_ex(size, NULL, 0, NULL);
}

void memoria_free_ex(void *ptr, const char *file, int line, const char *func) {
    if (!ptr) {
        fprintf(stderr, "\nWARNING: Free called with NULL pointer");
        if (file) fprintf(stderr, " at %s:%d in %s", file, line, func ? func : "?");
        fprintf(stderr, "\n");
        return;
    }
    if (memoria_remove_allocation(ptr, true, file, line, func))
        (free)(ptr);
}

void memoria_free(void *ptr) {
    memoria_free_ex(ptr, NULL, 0, NULL);
}

void *memoria_realloc_ex(void *ptr, size_t new_size, const char *file, int line, const char *func) {
    if (!ptr) return memoria_malloc_ex(new_size, file, line, func);
    if (new_size == 0) { memoria_free_ex(ptr, file, line, func); return NULL; }
    memoria_allocation_entry_t *old_entry = memoria_find_allocation(ptr);
    void *new_ptr = (realloc)(ptr, new_size);
    if (new_ptr) {
        if (old_entry) memoria_remove_allocation(ptr, false, file, line, func);
        memoria_add_allocation(new_ptr, new_size, file, line, func);
    } else if (old_entry) {
        fprintf(stderr, "WARNING: realloc failed for pointer %p", ptr);
        if (file) fprintf(stderr, " at %s:%d in %s", file, line, func ? func : "?");
        fprintf(stderr, "\n");
    }
    return new_ptr;
}

void *memoria_realloc(void *ptr, size_t new_size) {
    return memoria_realloc_ex(ptr, new_size, NULL, 0, NULL);
}

void *memoria_calloc_ex(size_t nmemb, size_t size, const char *file, int line, const char *func) {
    void *ptr = (calloc)(nmemb, size);
    if (ptr) memoria_add_allocation(ptr, nmemb * size, file, line, func);
    return ptr;
}

void *memoria_calloc(size_t nmemb, size_t size) {
    return memoria_calloc_ex(nmemb, size, NULL, 0, NULL);
}

void memoria_print_report(void) {
    int has_problems = (memoria_alloc_stats.active_allocations > 0)
                    || (memoria_alloc_stats.double_free_count > 0)
                    || (memoria_alloc_stats.invalid_free_count > 0);
    if (!has_problems) return;
    printf("\n====== Memory Statistics ======\n");
    printf("Total allocations:   %zu\n", memoria_alloc_stats.total_allocations);
    printf("Total frees:         %zu\n", memoria_alloc_stats.total_frees);
    printf("Active allocations:  %zu\n", memoria_alloc_stats.active_allocations);
    printf("Total allocated:     %zu bytes\n", memoria_alloc_stats.total_allocated_bytes);
    printf("Total freed:         %zu bytes\n", memoria_alloc_stats.total_freed_bytes);
    printf("Double free errors:  %zu\n", memoria_alloc_stats.double_free_count);
    printf("Invalid free errors: %zu\n", memoria_alloc_stats.invalid_free_count);
    if (memoria_alloc_stats.active_allocations > 0) {
        printf("\nLeaked allocations:\n" COLOR_RESET);
        memoria_allocation_entry_t *current = memoria_alloc_stats.head;
        while (current) {
            printf("  %p: %zu bytes", current->ptr, current->size);
            if (current->file)
                printf(" at %s:%d in %s", current->file, current->line, current->func ? current->func : "?");
            printf("\n");
            current = current->next;
        }
    }
    printf("===============================\n\n");
    fflush(stdout);
}

void memoria_stats_reset(void) {
    memoria_allocation_entry_t *current = memoria_alloc_stats.head;
    while (current) {
        memoria_allocation_entry_t *next = current->next;
        (free)(current->ptr);
        (free)(current);
        current = next;
    }
    memset(&memoria_alloc_stats, 0, sizeof(memoria_alloc_stats));
}

bool memoria_stats_has_leaks(void) {
    return (memoria_alloc_stats.active_allocations > 0);
}

size_t memoria_stats_alloc_count(void) {
    return memoria_alloc_stats.total_allocations;
}

size_t memoria_stats_active_count(void) {
    return memoria_alloc_stats.active_allocations;
}

size_t memoria_stats_double_free_count(void) {
    return memoria_alloc_stats.double_free_count;
}

size_t memoria_stats_invalid_free_count(void) {
    return memoria_alloc_stats.invalid_free_count;
}

#endif /* MEMORIA_IMPLEMENTATION */
#endif /* MEMORIA_H */
