#ifndef GEORGE_PROPERTY_UPDATES_TEMPLATE_H
#define GEORGE_PROPERTY_UPDATES_TEMPLATE_H

/* Common original inline bodies keep each original numeric call binding. */
#define PROPERTY_DEFER_CAPTURED(node, captured_flags) do { \
    if ((captured_flags)&0x10) (node)->flags=(u16)((captured_flags)|0x20); \
    else func_002B9660(node); \
} while (0)

/* Call sites retain the single-node/outer-body capture and time-store order. */
#define PROPERTY_UPDATE_AFTER_CAPTURE(node, time, callback) do { \
    GeorgePropertyOwnedNode *property_update_root=(node),*property_update_child,*property_update_next; \
    GeorgePropertyUpdateCallback property_update_callback=(callback); \
    float property_update_time=(time); \
    u16 property_update_flags; \
    if (property_update_callback!=NULL) { \
        property_update_root->flags|=0x10; \
        property_update_callback(property_update_root); \
        property_update_root->flags&=0xFFEF; \
    } \
    property_update_flags=property_update_root->flags; \
    if (property_update_flags&0x20) { \
        PROPERTY_DEFER_CAPTURED(property_update_root,property_update_flags); \
    } else { \
        func_002B9898(property_update_root); \
        property_update_child=(GeorgePropertyOwnedNode *)property_update_root->children.head; \
        property_update_next=(GeorgePropertyOwnedNode *)property_update_child->link.next; \
        while (property_update_next!=NULL) { \
            func_002B9748(property_update_child,property_update_time); \
            property_update_child=property_update_next; \
            property_update_next=(GeorgePropertyOwnedNode *)property_update_next->link.next; \
        } \
    } \
} while (0)

#endif
