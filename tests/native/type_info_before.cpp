/* Authored typed host fixture; the genuine GNU method is a separate TU. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <typeinfo>
#include "type_info_before_golden.h"

/* The stack-only fixture never invokes a deleting destructor. This is a
 * link-only trap, not an allocation/deallocation implementation. */
static unsigned int delete_control_hits, what_control_hits;
void operator delete(void *) throw() { ++delete_control_hits; abort(); }
const char *std::exception::what() const { ++what_control_hits; abort(); }

struct NameProbe : public std::type_info {
    explicit NameProbe(const char *name) : std::type_info(name) {}
    unsigned int name_offset() const {
        return (unsigned int)((const char *)&_name-(const char *)this);
    }
};

static unsigned int checks;
static void check(bool value, unsigned int case_id) {
    ++checks;
    if (!value) { fprintf(stderr,"before case%u check%u failed\n",case_id,checks); exit(1); }
}
int main() {
    check(sizeof(void *)==4,0);
    for (unsigned int i=0; i<sizeof(before_fixtures)/sizeof(before_fixtures[0]); ++i) {
        const BeforeFixture &f=before_fixtures[i];
        unsigned char left[128] __attribute__((aligned(16)));
        unsigned char right[128] __attribute__((aligned(16)));
        memset(left,0xA5,sizeof left); memset(right,0xA5,sizeof right);
        unsigned char *lp=left+f.left_offset, *rp=right+f.right_offset;
        memcpy(lp,f.left,f.left_size+1);memcpy(rp,f.right,f.right_size+1);
        if(f.alias) rp=lp;
        NameProbe a((const char *)lp), b((const char *)rp);
        const NameProbe *arg=f.alias==2?&a:&b;
        check(a.name()==(const char *)lp,i);check(arg->name()==(const char *)rp,i);
        check((unsigned int)a.before(*arg)==f.expected,i);
        for(unsigned int k=0;k<=f.left_size;++k)check(lp[k]==f.left[k],i);
        for(unsigned int k=0;k<=f.right_size;++k)check(rp[k]==f.right[k],i);
        check(a.name_offset()==4,i);
        check(delete_control_hits==0 && what_control_hits==0,i);
    }
    NameProbe measured("layout");
    void *native_vptr=0;
    memcpy(&native_vptr,&measured,sizeof native_vptr);
    check(native_vptr!=0,0);
    check(delete_control_hits==0 && what_control_hits==0,0);
    printf("before_native_abi: pointer%u long%u size%u bool%u type_info%u name%u vptr0\n",
        (unsigned int)sizeof(void *),(unsigned int)sizeof(long),(unsigned int)sizeof(size_t),
        (unsigned int)sizeof(bool),(unsigned int)sizeof(std::type_info),measured.name_offset());
    printf("type_info_before: %u checks passed\n",checks);
    return 0;
}
