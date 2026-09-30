#include "sort.h"
#include "sortctx.h"

static size_t partition(SortCtx* ctx, size_t lo, size_t hi){
    size_t pv_idx = hi-1;
    size_t bnd = lo;
    for(size_t i=lo;i<pv_idx;i++){
        if(sortCompareAt(ctx, i, pv_idx)<0){
            if(bnd!=i){ sortSwap(ctx, bnd, i); }
            bnd++;
        }
    }
    if(bnd!=pv_idx){ sortSwap(ctx, bnd, pv_idx); }
    return bnd;
}

static void quickSortRange(SortCtx* ctx, size_t lo, size_t hi, size_t lvl){
    if(hi-lo<2){ return; }
    if(ctx->stats!=NULL && lvl>ctx->stats->maxDepth){ ctx->stats->maxDepth = lvl; }
    size_t pv_idx = partition(ctx, lo, hi);
    quickSortRange(ctx, lo, pv_idx, lvl+1);
    quickSortRange(ctx, pv_idx+1, hi, lvl+1);
}

void quickSort(void* base, size_t n, size_t size, SortCompare cmp, SortStats* stats){
    SortCtx ctx;
    if(!sortBegin(&ctx, base, n, size, cmp, stats)){ return; }
    quickSortRange(&ctx, 0, n, 1);
    sortEnd(&ctx);
}
