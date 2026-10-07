/* Authored typed initialized observations. Genuine query/getter spans are
 * compiled in separate translation units, with a controlled context provider.
 * Host layout is measured, not claimed equal to the EE ABI. */
#include "query_types.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include "gnu_exception_queries_golden.h"

extern "C" void *george_native_cp_exception_info(void);
extern "C" void query_native_provider(eh_context *(*)());
extern "C" unsigned query_native_provider_is(eh_context *(*)());
extern "C" void query_native_old_info(void *);

static eh_context contexts[2];
static cp_eh_info records[2];
static unsigned payload[16];
static const QueryGolden *current;
static unsigned calls, event_words[4], checks;

static void require(bool value, unsigned fixture, unsigned field)
{
    ++checks;
    if (!value) {
        fprintf(stderr, "gnu_exception_queries fixture %u field %u failed\n", fixture, field);
        exit(1);
    }
}

static cp_eh_info *record(unsigned i) { return i == 2 ? 0 : &records[i]; }

static unsigned pointer_token(const void *p)
{
    if (p == 0) return 0;
    for (unsigned i=0; i<2; ++i) {
        if (p == &records[i]) return 0x20300u+0x40u*i;
        if (p == &records[i].value) return 0x20308u+0x40u*i;
        if (p == &contexts[i]) return 0x20200u+0x20u*i;
        if (p == &payload[4*i]) return 0x21000u+0x10u*i;
        if (p == &payload[8+4*i]) return 0x21100u+0x10u*i;
    }
    if (p == &payload[3]) return 0x21030u;
    /* original_value is deliberately a distinct live initialized object. */
    if (p == &payload[1]) return 0x21200u;
    if (p == &payload[2]) return 0x21210u;
    fprintf(stderr,"undesignated native query pointer\n");exit(2);return 0;
}

static eh_context *alternate_provider()
{
    fprintf(stderr,"unexpected second provider invocation\n");exit(3);return 0;
}

static eh_context *controlled_provider()
{
    const unsigned *p=current->p;
    eh_context *ctx=&contexts[p[3]];
    cp_eh_info *before=(cp_eh_info *)ctx->info;
    ++calls;
    if (p[5]&1) ctx->info=record(p[4]);
    cp_eh_info *after=(cp_eh_info *)ctx->info;
    if ((p[5]&2) && after) after->caught=!after->caught;
    if (p[5]&4) {
        if (after) after->value=&payload[3];
        query_native_provider(alternate_provider);
    }
    event_words[0]=pointer_token(before);event_words[1]=pointer_token(after);
    event_words[2]=pointer_token(ctx);event_words[3]=p[5];
    return ctx;
}

int main()
{
    require(sizeof(void *)==4,0,900);
    /* These are actual host observations, separate from target bool4/long8. */
    printf("native layout: bool=%u long=%u record=%u value=%u caught=%u next=%u handlers=%u original=%u context=%u info=%u\n",
        (unsigned)sizeof(bool),(unsigned)sizeof(long),(unsigned)sizeof(cp_eh_info),
        (unsigned)offsetof(cp_eh_info,value),(unsigned)offsetof(cp_eh_info,caught),
        (unsigned)offsetof(cp_eh_info,next),(unsigned)offsetof(cp_eh_info,handlers),
        (unsigned)offsetof(cp_eh_info,original_value),(unsigned)sizeof(eh_context),
        (unsigned)offsetof(eh_context,info));
    for (unsigned fixture=0; fixture<sizeof(query_golden)/sizeof(query_golden[0]); ++fixture) {
        current=&query_golden[fixture];calls=0;
        const unsigned *p=current->p;
        for (unsigned i=0;i<16;++i) payload[i]=0xBEA00000u+i;
        for (unsigned i=0;i<2;++i) {
            contexts[i].handler_label=&payload[5];
            contexts[i].dynamic_handler_chain=(void **)&contexts[1-i].info;
            contexts[i].info=record(p[1+i]);contexts[i].table_index=&payload[6];
            records[i].eh_info.match_function=0;
            records[i].eh_info.language=(short)(0x10+i);records[i].eh_info.version=(short)(0x20+i);
            records[i].value=&payload[4*i];records[i].type=&payload[8+4*i];
            records[i].cleanup=0;records[i].caught=(p[6]>>i&1)!=0;
            records[i].next=&records[1-i];records[i].handlers=0x12340+i;
            records[i].original_value=&payload[1+i];
        }
        query_native_provider(controlled_provider);
        /* Native diagnostic snapshot is unread by the genuine helper. Only
         * the deliberate stale-slot negative uses it after the provider call. */
        query_native_old_info(contexts[p[3]].info);
        unsigned actual[23],n=0;
        if (p[0]==0) {
            void *address=george_native_cp_exception_info();
            cp_eh_info *fresh=(cp_eh_info *)contexts[p[3]].info;
            require(fresh!=0 && address==&fresh->value,fixture,901);
            actual[n++]=pointer_token(address);
            actual[n++]=pointer_token(*(void **)address);
        } else {
            actual[n++]=std::uncaught_exception()?1u:0u;actual[n++]=0;
        }
        actual[n++]=calls;actual[n++]=p[3];
        require(query_native_provider_is((p[5]&4)?alternate_provider:controlled_provider)!=0,fixture,909);
        actual[n++]=query_native_provider_is(controlled_provider)?0x0F000004u:0x0F000008u;
        for (unsigned i=0;i<2;++i) actual[n++]=pointer_token(contexts[i].info);
        for (unsigned i=0;i<2;++i) {
            actual[n++]=(unsigned short)records[i].eh_info.language | ((unsigned)(unsigned short)records[i].eh_info.version<<16);
            actual[n++]=pointer_token(records[i].value);actual[n++]=pointer_token(records[i].type);
            actual[n++]=records[i].cleanup==0?0u:0xFFFFFFFFu;
            actual[n++]=records[i].caught?1u:0u;actual[n++]=pointer_token(records[i].next);
            actual[n++]=(unsigned)records[i].handlers;actual[n++]=pointer_token(records[i].original_value);
            require(records[i].eh_info.match_function==0,fixture,902+i);
            require(contexts[i].handler_label==&payload[5] && contexts[i].table_index==&payload[6],fixture,904+i);
            require(contexts[i].dynamic_handler_chain==(void **)&contexts[1-i].info,fixture,906+i);
        }
        require(n==23 && calls==1,fixture,908);
        for (unsigned i=0;i<n;++i) require(actual[i]==current->expected[i],fixture,i);
        for (unsigned i=0;i<4;++i) require(event_words[i]==current->event[i],fixture,100+i);
        for (unsigned i=0;i<16;++i) require(payload[i]==0xBEA00000u+i,fixture,200+i);
    }
    printf("gnu_exception_queries: %u checks\n",checks);return 0;
}
