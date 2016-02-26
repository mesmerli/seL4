/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * This software may be distributed and modified according to the terms of
 * the GNU General Public License version 2. Note that NO WARRANTY is provided.
 * See "LICENSE_GPLv2.txt" for details.
 *
 * @TAG(GD_GPL)
 */

#ifndef __ARCH_KERNEL_VSPACE_H
#define __ARCH_KERNEL_VSPACE_H

#include <config.h>
#include <types.h>
#include <api/failures.h>
#include <object/structures.h>

#define IT_ASID 1 /* initial thread's ASID */


struct lookupPTSlot_ret {
    exception_t status;
    pte_t*      ptSlot;
};
typedef struct lookupPTSlot_ret lookupPTSlot_ret_t;

struct lookupPDSlot_ret {
    exception_t status;
    pde_t*      pdSlot;
};
typedef struct lookupPDSlot_ret lookupPDSlot_ret_t;


struct lookupPDPTSlot_ret {
    exception_t status;
    pdpte_t     *pdptSlot;
};
typedef struct lookupPDPTSlot_ret lookupPDPTSlot_ret_t;

struct findVSpaceForASID_ret {
    exception_t status;
    vspace_root_t *vspace_root;
};
typedef struct findVSpaceForASID_ret findVSpaceForASID_ret_t;

void init_boot_pd(void);
void enable_paging(void);
bool_t map_kernel_window(
    uint32_t num_ioapic,
    paddr_t*   ioapic_paddrs,
    uint32_t   num_drhu,
    paddr_t*   drhu_list
);
bool_t map_kernel_window_devices(
    pte_t *pt,
    uint32_t num_ioapic,
    paddr_t*   ioapic_paddrs,
    uint32_t   num_drhu,
    paddr_t*   drhu_list
);

void init_tss(tss_t *tss);
void init_gdt(gdt_entry_t *gdt, tss_t *tss);
void init_idt_entry(idt_entry_t* idt, interrupt_t interrupt, void(*handler)(void));
vspace_root_t *getValidNativeRoot(cap_t vspace_cap);
pde_t *get_boot_pd(void);
void* map_temp_boot_page(void* entry, uint32_t large_pages);
bool_t init_vm_state(void);
void init_dtrs(void);
void map_it_pt_cap(cap_t vspace_cap, cap_t pt_cap);
void map_it_pd_cap(cap_t vspace_cap, cap_t pd_cap);
void map_it_frame_cap(cap_t vspace_cap, cap_t frame_cap);
void write_it_asid_pool(cap_t it_ap_cap, cap_t it_vspace_cap);
bool_t init_pat_msr(void);
cap_t create_it_address_space(cap_t root_cnode_cap, v_region_t it_v_reg);

/* ==================== BOOT CODE FINISHES HERE ==================== */

void idle_thread(void);
#define idleThreadStart (&idle_thread)

bool_t isVTableRoot(cap_t cap);

lookupPTSlot_ret_t lookupPTSlot(vspace_root_t *vspace, vptr_t vptr);
lookupPDSlot_ret_t lookupPDSlot(vspace_root_t *vspace, vptr_t vptr);
void copyGlobalMappings(vspace_root_t *new_vspace);
word_t* PURE lookupIPCBuffer(bool_t isReceiver, tcb_t *thread);
exception_t handleVMFault(tcb_t *thread, vm_fault_type_t vm_faultType);
void unmapPageDirectory(asid_t asid, vptr_t vaddr, pde_t *pd);
void unmapPageTable(asid_t, vptr_t vaddr, pte_t* pt);

exception_t performASIDPoolInvocation(asid_t asid, asid_pool_t* poolPtr, cte_t* vspaceCapSlot);
exception_t performASIDControlInvocation(void *frame, cte_t *slot, cte_t *parent, asid_t asid_base);
void hwASIDInvalidate(asid_t asid);
void deleteASIDPool(asid_t asid_base, asid_pool_t* pool);
void deleteASID(asid_t asid, vspace_root_t *vspace);
findVSpaceForASID_ret_t findVSpaceForASID(asid_t asid);

void unmapPage(vm_page_size_t page_size, asid_t asid, vptr_t vptr, void *pptr);
void modeUnmapPage(vm_page_size_t page_size, vspace_root_t *vroot, vptr_t vptr, void *pptr);
exception_t modeMapRemapPage(word_t invLabel, vm_page_size_t page_size, vspace_root_t *vroot, vptr_t vptr, paddr_t paddr, vm_rights_t vm_rights, vm_attributes_t vm_attr);
void setVMRoot(tcb_t *tcb);
bool_t CONST isValidVTableRoot(cap_t cap);
bool_t CONST isValidNativeRoot(cap_t cap);
exception_t checkValidIPCBuffer(vptr_t vptr, cap_t cap);
vm_rights_t CONST maskVMRights(vm_rights_t vm_rights, cap_rights_t cap_rights_mask);
void    flushTable(vspace_root_t *vspace, word_t vptr, pte_t *pt);

exception_t decodeX86MMUInvocation(word_t invLabel, word_t length, cptr_t cptr, cte_t *cte,
                                   cap_t cap, extra_caps_t excaps, word_t *buffer);

exception_t decodeX86ModeMMUInvocation(word_t invLabel, word_t length, cptr_t cptr, cte_t *cte,
                                       cap_t cap, extra_caps_t excaps, word_t *buffer);

exception_t decodeIA32PageDirectoryInvocation(word_t invLabel, word_t length, cte_t* cte, cap_t cap, extra_caps_t excaps, word_t* buffer);

/* common functions for x86 */
exception_t decodeX86FrameInvocation(word_t invLabel, word_t length, cte_t *cte, cap_t cap, extra_caps_t excaps, word_t *buffer);

uint32_t CONST WritableFromVMRights(vm_rights_t vm_rights);
uint32_t CONST SuperUserFromVMRights(vm_rights_t vm_rights);

/* the following functions have the same names, but different
 * implementations for 32-bit and 64-bit.
 */

pte_t CONST makeUserPTE(paddr_t paddr, vm_attributes_t vm_attr, vm_rights_t vm_rights);
pte_t CONST makeUserPTEInvalid(void);
pde_t CONST makeUserPDELargePage(paddr_t paddr, vm_attributes_t vm_attr, vm_rights_t vm_rights);
pde_t CONST makeUserPDEPageTable(paddr_t paddr, vm_attributes_t vm_attr);
pde_t CONST makeUserPDELargePageInvalid(void);
pde_t CONST makeUserPDEPageTableInvalid(void);

bool_t  CONST isIOSpaceFrame(cap_t cap);
#endif
