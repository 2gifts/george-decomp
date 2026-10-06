/* Included with RESOURCE_ACCOUNT_FUNCTION and RESOURCE_ACCOUNT_CHANGE.
 * The complete add/subtract originals differ only at six arithmetic sites.
 * Every intervening global/record reload remains in the shared ordinary C. */
void RESOURCE_ACCOUNT_FUNCTION(u32 first,u32 second,GeorgeResourceRecord *record)
{
    GeorgeResourceBasePrefix *base=(GeorgeResourceBasePrefix *)record;
    GeorgeResourceManagerState *manager;
    GeorgeResourceAllocator *allocator;
    u32 size,kind,address;
    u32 *slot;
    if (first!=0) {
        size=base->size;
        manager=(GeorgeResourceManagerState *)D_003F960C;
        manager->bytes30=RESOURCE_ACCOUNT_CHANGE(manager->bytes30,size);
        kind=base->kind;
        manager=(GeorgeResourceManagerState *)D_003F960C;
        size=base->size;
        slot=(u32 *)((u32)manager+0x38U+(kind<<2));
        *slot=RESOURCE_ACCOUNT_CHANGE(*slot,size);
        manager=(GeorgeResourceManagerState *)D_003F960C;
        kind=base->kind;
        slot=(u32 *)((u32)manager+0x58U+(kind<<2));
        *slot=RESOURCE_ACCOUNT_CHANGE(*slot,1U);
        manager=(GeorgeResourceManagerState *)D_003F960C;
        address=(u32)base->payload;
        allocator=manager->allocator;
        if (address>=allocator->first && address<=allocator->last)
            manager->count28=RESOURCE_ACCOUNT_CHANGE(manager->count28,1U);
        else
            manager->count24=RESOURCE_ACCOUNT_CHANGE(manager->count24,1U);
    }
    if (second!=0) {
        size=base->size;
        manager=(GeorgeResourceManagerState *)D_003F960C;
        manager->bytes34=RESOURCE_ACCOUNT_CHANGE(manager->bytes34,size);
    }
}
