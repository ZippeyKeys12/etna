tasks: dict[str, dict[str, list[str]]] = {
    'BST': {
        'insert_1_spec': ['setNodeKey'],
        'insert_2_spec': ['setNodeKey'],
        'insert_3_spec': ['setNodeKey'],
        'delete_4_spec': ['deleteKey'],
        'delete_5_spec': ['deleteKey'],
        'forget_last_node_cur': [
            'findParent',
            'setNodeKey',
            'deleteKey'
        ],
        'insert_1_impl': ['setNodeKey'],
        'insert_2_impl': ['setNodeKey'],
        'insert_3_impl': ['setNodeKey'],
        'forget_to_update_root': ['deleteSmallest', 'deleteKey'],
        'always_update_root_instead_of_parent': ['deleteKey'],
        'always_assign_smaller': ['deleteKey']
    },
    'DLL': {
        'forget_to_set_next_prev': ['dll_insert_at']
    },
    'ImperativeQueue': {
        'forget_change_front': ['pop_queue']
    },
    'Runway': {
        'valid_a': ['tick'],
        'valid_exclusive_a_d': ['switch_modes', 'tick'],
        'valid_time': ['tick'],
        'valid_no_mode_active': ['tick'],
        'dont_reset_time': ['tick']
    },
    'SortedList': {
        'sum_modify_list': ['sum'],
        'cons_invalid': ['cons'],
        'insert_dups': ['insert']
    },
    'RingQueue': {
        'empty_vs_full': ['put'],
        'queue_full': ['put'],
        'mod_size_vs_rem': ['queueSize'],
        'abs_size': ['queueSize'],
    },
    'SLL': {
        'append_forget_head': ['IntList_append', 'rev_aux', 'rev'],
        'merge_swap_cases_spec': ['merge', 'sl_mergesort'],
        'merge_swap_cases_impl': ['merge', 'sl_mergesort'],
        'snoc_real_bug': ['rev_aux', 'rev', 'rev_loop'],
        'rev_loop_cur_to_early': ['rev_loop']
    }
}
