/* Included twice with a complete ordinary C definition at each genuine entry.
 * OUTPUT runs after unlinking and before the fresh pool load. */
u32 REGISTRY_REMOVE_NAME(GeorgeGenericMap *map,u32 key REGISTRY_REMOVE_ARGUMENT)
{
    u32 bucket=key%map->bucket_count;
    GeorgeGenericMapNode *node=map->buckets[bucket],*previous=0;
    while (node!=0) {
        if (node->key==key) {
            GeorgeRegistryNodePool *pool;
            u32 index;
            GeorgeGenericMapNode **slots;
            if (previous!=0) previous->next=node->next;
            else map->buckets[bucket]=node->next;
            REGISTRY_REMOVE_OUTPUT(node);
            pool=((GeorgeRegistryPooledMap *)map)->pool;
            index=pool->index;
            slots=pool->slots;
            index=index-1U;
            pool->index=index;
            slots[index]=node;
            map->count=map->count-1U;
            return 1;
        }
        previous=node;
        node=node->next;
    }
    return 0;
}
