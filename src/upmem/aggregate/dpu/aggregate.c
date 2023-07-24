#include <stdint.h>
#include <stdio.h>
#include <defs.h>
#include <mram.h>
#include <alloc.h>
#include <handshake.h>
#include <barrier.h>

#include "../common/common.h"

__host dpu_params dpu_parameters;
__host aggr_res dpu_results;

#if defined(COUNT) || defined(AVERAGE)
aggr_val_t dpu_cnt_res[NR_TASKLETS];
// __host aggr_val_t dpu_cnt_val;
#endif

#if defined(SUM) || defined(AVERAGE)
aggr_val_t dpu_sum_res[NR_TASKLETS];
// __host aggr_val_t dpu_sum_val;
#endif

#ifdef AVERAGE
double dpu_avg_res[NR_TASKLETS];
// __host double dpu_avg_val;
#endif

#ifdef MINIMUM
aggr_val_t dpu_min_res[NR_TASKLETS];
// __host aggr_val_t dpu_min_val;
#endif

#ifdef MAXIMUM
aggr_val_t dpu_max_res[NR_TASKLETS];
// __host aggr_val_t dpu_max_val;
#endif

BARRIER_INIT(aggr_barrier, NR_TASKLETS);

void compute_aggregates() {
    aggr_val_t cnt = 0;
    aggr_val_t sum = 0;
    double avg = 0.0;
    aggr_val_t min = 18446744073709551615ul;
    aggr_val_t max = 0;

    for (uint32_t i = 0; i < NR_TASKLETS; i++) {
        #if defined(COUNT) || defined(AVERAGE)
        cnt += dpu_cnt_res[i];
        #endif

        #if defined(SUM) || defined(AVERAGE)
        sum += dpu_sum_res[i];
        #endif

        #ifdef MINIMUM
        if (min > dpu_min_res[i]) {
            min = dpu_min_res[i];
        }
        #endif

        #ifdef MAXIMUM
        if (max < dpu_max_res[i]) {
            max = dpu_max_res[i];
        }
        #endif
    }

    #if defined(COUNT) || defined(AVERAGE)
    dpu_results.cnt = cnt;
    #endif

    #if defined(SUM) || defined(AVERAGE)
    dpu_results.sum = sum;
    #endif

    #ifdef AVERAGE
    if (dpu_results.cnt > 0) {
        avg = dpu_results.sum / (double)dpu_results.cnt;
    }
    dpu_results.avg = avg;
    #endif

    #ifdef MINIMUM
    dpu_results.min = min;
    #endif

    #ifdef MAXIMUM
    dpu_results.max = max;
    #endif
}

int aggregation() {
    unsigned int tasklet_id = me();
    // printf("Tasklet: %d\n", tasklet_id);

    if (tasklet_id == 0) {
        mem_reset();
    }
    barrier_wait(&aggr_barrier);

    /* initialize tasklet result buffers */
    #if defined(COUNT) || defined(AVERAGE)
    dpu_cnt_res[tasklet_id] = 0;
    #endif

    #if defined(SUM) || defined(AVERAGE)
    dpu_sum_res[tasklet_id] = 0;
    #endif

    #ifdef AVERAGE
    dpu_avg_res[tasklet_id] = 0.0;
    #endif

    #ifdef MINIMUM
    dpu_min_res[tasklet_id] = 18446744073709551615ul;
    #endif

    #ifdef MAXIMUM
    dpu_max_res[tasklet_id] = 0;
    #endif

    mrnode* mr_elems = (mrnode*) DPU_MRAM_HEAP_POINTER;
    mrnode* wr_elems = (mrnode*) mem_alloc(NUM_WR_ELEMS_PER_TASKLET * ELEM_SIZE);
    for (uint64_t i = tasklet_id * NUM_WR_ELEMS_PER_TASKLET; i < dpu_parameters.elems; i += NUM_WR_ELEMS_PER_TASKLET * NR_TASKLETS) {

        mram_read((__mram_ptr void const*) &mr_elems[i], wr_elems, NUM_WR_ELEMS_PER_TASKLET * ELEM_SIZE);

        uint32_t num_elems = ((i + NUM_WR_ELEMS_PER_TASKLET) < dpu_parameters.elems) ?
                             NUM_WR_ELEMS_PER_TASKLET :
                             dpu_parameters.elems - i;

        for (uint32_t j = 0; j < num_elems; j++) {
            #if defined(COUNT) || defined(AVERAGE)
            dpu_cnt_res[tasklet_id]++;
            #endif

            #if defined(SUM) || defined(AVERAGE)
            dpu_sum_res[tasklet_id] += wr_elems[j].id_;
            #endif

            #ifdef MINIMUM
            if (dpu_min_res[tasklet_id] > wr_elems[j].id_) {
                dpu_min_res[tasklet_id] = wr_elems[j].id_;
            }
            #endif

            #ifdef MAXIMUM
            if (dpu_max_res[tasklet_id] < wr_elems[j].id_) {
                dpu_max_res[tasklet_id] = wr_elems[j].id_;
            }
            #endif
        }
    }

    barrier_wait(&aggr_barrier);
    if (tasklet_id == 0) { /* first tasklet collects the DPU local results */
        compute_aggregates();
        // printf("Processed elements: %lu\n", dpu_parameters.elems);
    }

    return 0;
}

int (*kernels[NUM_KERNELS])() = {aggregation};

int main() {
    return kernels[0]();
}