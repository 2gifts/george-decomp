/* Shared ordinary C for independently verified complete wrapper pairs.
 * Each original symbol retains its own natural compile/link comparison. */
#define FILE_OPEN_WRAPPER(name, flags) \
u32 name(const char *path) \
{ \
    char resolved[0x400]; \
    s32 descriptor; \
    if(*(const signed char *)path==0)return 0; \
    func_002ABAE8(path,resolved); \
    func_00363AE0(0); \
    descriptor=func_003689B0(resolved,flags); \
    func_00363AE0(0); \
    if(descriptor<0)return 0; \
    return (u32)descriptor+1u; \
}

#define FILE_THREE_WORD_WRAPPER(name, second_type, callee) \
s32 name(u32 encoded_descriptor,second_type second,s32 third) \
{ \
    s32 result; \
    func_00363AE0(0); \
    result=callee((s32)(encoded_descriptor-1u),second,third); \
    func_00363AE0(0); \
    return result; \
}
