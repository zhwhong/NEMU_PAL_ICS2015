#ifndef __MISC_H__
#define __MISC_H__

make_helper(nop);
make_helper(int3);
make_helper(lea);
make_helper(leave);
make_helper(ret);
make_helper(cld);
make_helper(cwd);
make_helper(stos_b);
make_helper(stos_v);
make_helper(movs_b);
make_helper(movs_v);
make_helper(cmps_b);
make_helper(cmps_v);

#endif
