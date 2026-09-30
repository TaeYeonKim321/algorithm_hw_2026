#include "sort.h"
#include "sortctx.h"
#include <stdlib.h>

static int cpr_NODE(SortCtx* ctx, char* items, size_t a, size_t b){
    int result=ctx->cmp(items+a*ctx->size, items+b*ctx->size);
    if(ctx->stats!=NULL){ ctx->stats->compares++; }
    if(result!=0){ return result; }
    if(a<b){ return -1; }
    if(a>b){ return 1; }
    return 0;
}

static void hSwap(size_t* heap, size_t i, size_t j){
    size_t tmp = heap[i];
    heap[i] = heap[j];
    heap[j] = tmp;

}

static void hUp(SortCtx* ctx, char* items, size_t* heap, size_t pos){
    while(pos>0){
        size_t parent = (pos-1)/2;
        if(cpr_NODE(ctx, items, heap[parent], heap[pos])<=0){ break; }
        hSwap(heap, parent, pos);
        pos=parent;
    }
}

static void hDown(SortCtx* ctx, char* items, size_t* heap, size_t count, size_t pos){
    while(1){
        size_t L = pos*2+1;
        size_t R = L+1;
        size_t min = pos;
        if(L<count && cpr_NODE(ctx, items, heap[L], heap[min])<0){ min=L; }
        if(R<count && cpr_NODE(ctx, items, heap[R], heap[min])<0){ min=R; }
        if(min==pos){ break; }
        hSwap(heap, pos, min);
        pos=min;
    }
}

static size_t findPile(SortCtx* ctx, char* items, size_t* top, size_t pile_cnt, size_t node){
    size_t lo = 0;
    size_t hi = pile_cnt;
    while(lo<hi){
        size_t mid = (lo+hi)/2;
        if(cpr_NODE(ctx, items, node, top[mid])<=0){ hi=mid; }
        else{ lo=mid+1; }
    }
    return lo;
}

void patienceSort(void* base, size_t n, size_t size, SortCompare cmp, SortStats* stats){
    SortCtx ctx;
    if(!sortBegin(&ctx, base, n, size, cmp, stats)){ return; }
    char* items = (char*)malloc(n*size);
    size_t* next = (size_t*)malloc(n*sizeof(size_t));
    size_t* top = (size_t*)malloc(n*sizeof(size_t));
    size_t* heap = (size_t*)malloc(n*sizeof(size_t));
    if(items==NULL || next==NULL || top==NULL || heap==NULL){
        free(items);
        free(next);
        free(top);
        free(heap);
        sortEnd(&ctx);
        return;
    }
    if(stats!=NULL){ stats->extraBytes = size+n*size+3*n*sizeof(size_t); }
    for(size_t i=0;i<n;i++){
        sortMove(&ctx, items+i*size, sortElemAt(&ctx, i));
        next[i]=n;
    }
    size_t pile_cnt = 0;
    for(size_t i=0;i<n;i++){
        size_t pile=findPile(&ctx, items, top, pile_cnt, i);
        if(pile==pile_cnt){
            top[pile]=i;
            pile_cnt++;
        }
        else{
            next[i]=top[pile];
            top[pile]=i;
        }

    }
    size_t heap_cnt = 0;
    for(size_t i=0;i<pile_cnt;i++){
        heap[heap_cnt]=top[i];
        hUp(&ctx, items, heap, heap_cnt);
        heap_cnt++;
    }
    for(size_t out=0;out<n;out++){
        size_t node = heap[0];
        sortMove(&ctx, sortElemAt(&ctx, out), items+node*size);
        if(next[node]!=n){
            heap[0]=next[node];
            hDown(&ctx, items, heap, heap_cnt, 0);
        }
        else{
            heap_cnt--;
            if(heap_cnt>0){
                heap[0]=heap[heap_cnt];
                hDown(&ctx, items, heap, heap_cnt, 0);
            }
        }
    }
    free(items);
    free(next);
    free(top);
    free(heap);
    sortEnd(&ctx);
}
