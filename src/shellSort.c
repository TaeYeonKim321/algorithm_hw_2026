#include "sort.h"
#include "sortctx.h"

static void shellSortRange(SortCtx* ctx, size_t n){
    for(size_t i=n/2;i>0;i/=2){
        for(size_t j=i;j<n;j++){
            sortMove(ctx, ctx->tmp, sortElemAt(ctx, j));
            size_t k = j;
            while(k >= i && sortCompareTmp(ctx, k-i) > 0){
                sortMove(ctx, sortElemAt(ctx, k), sortElemAt(ctx, k-i));
                k-=i;
            }
            sortMove(ctx, sortElemAt(ctx, k), ctx->tmp);
        }
    }
}

void shellSort(void* base, size_t n, size_t size, SortCompare cmp, SortStats* stats){
    SortCtx ctx;
    if(!sortBegin(&ctx, base, n, size, cmp, stats)){ return; }
    shellSortRange(&ctx, n);
    sortEnd(&ctx);
}
