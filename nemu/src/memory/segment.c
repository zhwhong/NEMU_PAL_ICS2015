#include "nemu.h"

lnaddr_t seg_translate(swaddr_t addr, size_t len, SELECTOR current_sreg) {
	if (cpu.cr0.protect_enable == 0)
		return addr;		//not protect model
	if (current_sreg.val == cpu.cs.selector) {
		Assert(addr+len < cpu.cs.seg_limit, "segment out limit, %x, %d, %x", addr, (int)len, cpu.cs.seg_limit);
		return cpu.cs.seg_base + addr;
	}
	else if (current_sreg.val == cpu.ds.selector) {
		Assert(addr+len < cpu.ds.seg_limit, "segment out limit, %x, %d, %x", addr, (int)len, cpu.ds.seg_limit);
		return cpu.ds.seg_base + addr;
	}
	else if (current_sreg.val == cpu.es.selector) {
		Assert(addr+len < cpu.es.seg_limit, "segment out limit, %x, %d, %x", addr, (int)len, cpu.es.seg_limit);
		return cpu.es.seg_base + addr;
	}
	else if (current_sreg.val == cpu.ss.selector) {
		Assert(addr+len < cpu.ss.seg_limit, "segment out limit, %x, %d, %x", addr, (int)len, cpu.ss.seg_limit);
		return cpu.ss.seg_base + addr;
	}
	else 
		return addr;
	
}