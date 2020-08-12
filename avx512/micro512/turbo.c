#include <stdlib.h>

#define INSN_LOOP_LO(insn, reg) do {                                   \
  asm volatile(                                                        \
    "mov $1<<24,%%rcx;"                                                \
    ".align 32;"                                                       \
    "1:"                                                               \
    #insn " " "%%" #reg "0" "," "%%" #reg "0" "," "%%" #reg "0" ";"    \
    #insn " " "%%" #reg "1" "," "%%" #reg "1" "," "%%" #reg "1" ";"    \
    #insn " " "%%" #reg "2" "," "%%" #reg "2" "," "%%" #reg "2" ";"    \
    #insn " " "%%" #reg "3" "," "%%" #reg "3" "," "%%" #reg "3" ";"    \
    "dec %%rcx;"                                                       \
    "jnz 1b;"                                                          \
    ::: "rcx"                                                          \
  );                                                                   \
} while(0);

#define INSN_LOOP_HI(insn, reg) do {                                   \
  asm volatile(                                                        \
    "mov $1<<24,%%rcx;"                                                \
    ".align 32;"                                                       \
    "1:"                                                               \
    #insn " " "%%" #reg "31" "," "%%" #reg "31" "," "%%" #reg "31" ";" \
    #insn " " "%%" #reg "30" "," "%%" #reg "30" "," "%%" #reg "30" ";" \
    #insn " " "%%" #reg "29" "," "%%" #reg "29" "," "%%" #reg "29" ";" \
    #insn " " "%%" #reg "28" "," "%%" #reg "28" "," "%%" #reg "28" ";" \
    "dec %%rcx;"                                                       \
    "jnz 1b;"                                                          \
    ::: "rcx"                                                          \
  );                                                                   \
} while(0);

#define INSN_LOOP_LO_MASK(insn, reg) do {                              \
  asm volatile(                                                        \
    "mov $1<<24,%%rcx;"                                                \
    "kxnorq %%k1,%%k1,%%k1;"                                           \
    "kxnorq %%k2,%%k2,%%k2;"                                           \
    "kxnorq %%k3,%%k3,%%k3;"                                           \
    "kxnorq %%k4,%%k4,%%k4;"                                           \
    ".align 32;"                                                       \
    "1:"                                                               \
    #insn " " "%%" #reg "0" "," "%%" #reg "0" "," "%%" #reg "0%{%%k1%}" ";"    \
    #insn " " "%%" #reg "1" "," "%%" #reg "1" "," "%%" #reg "1%{%%k2%}" ";"    \
    #insn " " "%%" #reg "2" "," "%%" #reg "2" "," "%%" #reg "2%{%%k3%}" ";"    \
    #insn " " "%%" #reg "3" "," "%%" #reg "3" "," "%%" #reg "3%{%%k4%}" ";"    \
    "dec %%rcx;"                                                       \
    "jnz 1b;"                                                          \
    ::: "rcx"                                                          \
  );                                                                   \
} while(0);

#define INSN_LOOP_HI_MASK(insn, reg) do {                              \
  asm volatile(                                                        \
    "mov $1<<24,%%rcx;"                                                \
    "kxnorq %%k1,%%k1,%%k1;"                                           \
    "kxnorq %%k2,%%k2,%%k2;"                                           \
    "kxnorq %%k3,%%k3,%%k3;"                                           \
    "kxnorq %%k4,%%k4,%%k4;"                                           \
    ".align 32;"                                                       \
    "1:"                                                               \
    #insn " " "%%" #reg "31" "," "%%" #reg "31" "," "%%" #reg "31%{%%k1%}" ";" \
    #insn " " "%%" #reg "30" "," "%%" #reg "30" "," "%%" #reg "30%{%%k2%}" ";" \
    #insn " " "%%" #reg "29" "," "%%" #reg "29" "," "%%" #reg "29%{%%k3%}" ";" \
    #insn " " "%%" #reg "28" "," "%%" #reg "28" "," "%%" #reg "28%{%%k4%}" ";" \
    "dec %%rcx;"                                                       \
    "jnz 1b;"                                                          \
    ::: "rcx"                                                          \
  );                                                                   \
} while(0);

int main(int argc, char ** argv) {
  int x = strtoul(argv[1], 0, 10);
  asm volatile("vzeroall");
  switch(x) {
  case 0:
    INSN_LOOP_LO(vpmuludq, zmm);
    break;
  case 1:
    INSN_LOOP_HI(vpmuludq, zmm);
    break;
  case 2:
    INSN_LOOP_LO(vpmuludq, ymm);
    break;
  case 3:
    INSN_LOOP_HI(vpmuludq, ymm);
    break;
  case 4:
    INSN_LOOP_LO(vpmuludq, xmm);
    break;
  case 5:
    INSN_LOOP_HI(vpmuludq, xmm);
    break;
  case 6:
    INSN_LOOP_LO(vpaddq, zmm);
    break;
  case 7:
    INSN_LOOP_HI(vpaddq, zmm);
    break;
  case 8:
    INSN_LOOP_LO(vpaddq, ymm);
    break;
  case 9:
    INSN_LOOP_HI(vpaddq, ymm);
    break;
  case 10:
    INSN_LOOP_LO(vpaddq, xmm);
    break;
  case 11:
    INSN_LOOP_HI(vpaddq, xmm);
    break;
  case 12:
    INSN_LOOP_LO_MASK(vpmuludq, zmm);
    break;
  case 13:
    INSN_LOOP_HI_MASK(vpmuludq, zmm);
    break;
  case 14:
    INSN_LOOP_LO_MASK(vpmuludq, ymm);
    break;
  case 15:
    INSN_LOOP_HI_MASK(vpmuludq, ymm);
    break;
  case 16:
    INSN_LOOP_LO_MASK(vpmuludq, xmm);
    break;
  case 17:
    INSN_LOOP_HI_MASK(vpmuludq, xmm);
    break;
  case 18:
    INSN_LOOP_LO_MASK(vpaddq, zmm);
    break;
  case 19:
    INSN_LOOP_HI_MASK(vpaddq, zmm);
    break;
  case 20:
    INSN_LOOP_LO_MASK(vpaddq, ymm);
    break;
  case 21:
    INSN_LOOP_HI_MASK(vpaddq, ymm);
    break;
  case 22:
    INSN_LOOP_LO_MASK(vpaddq, xmm);
    break;
  case 23:
    INSN_LOOP_HI_MASK(vpaddq, xmm);
    break;
  default:
    abort();
  }
  return 0;
}

