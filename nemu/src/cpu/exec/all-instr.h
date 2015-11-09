#include "prefix/prefix.h"

#include "data-mov/mov.h"
#include "data-mov/xchg.h"

#include "arith/dec.h"
#include "arith/inc.h"
#include "arith/neg.h"
#include "arith/imul.h"
#include "arith/mul.h"
#include "arith/idiv.h"
#include "arith/div.h"

#include "logic/and.h"
#include "logic/or.h"
#include "logic/not.h"
#include "logic/xor.h"
#include "logic/sar.h"
#include "logic/shl.h"
#include "logic/shr.h"
#include "logic/shrd.h"

#include "string/rep.h"
#include "string/movs.h"
#include "string/stos.h"
#include "string/cmps.h"

#include "misc/misc.h"

#include "my_op/sub.h"
#include "my_op/call.h"
#include "my_op/push.h"
#include "my_op/test.h"
#include "my_op/cmp.h"

#include "my_op/jmp.h"
#include "my_op/jcc.h"
#include "my_op/add.h"

#include "my_op/pop.h"
#include "my_op/movsx.h"
#include "my_op/movzx.h"
#include "my_op/adc.h"
#include "my_op/sbb.h"
#include "my_op/setcc.h"

#include "special/special.h"
