#include <stdio.h>
#include <stdlib.h>
void triggeravx(void)
{
	char array[256];
	int i;
	asm volatile (
                "mov %0, %%rsi \n\t"
                "xor %%rcx, %%rcx \n\t"
                "vmovd %%rcx, %%xmm0 \n\t"
                "vpbroadcastd %%xmm0, %%zmm0 \n\t"
                "vmovdqu32 %%zmm0, %%zmm7 \n\t"
                "vmovdqu32 %%zmm0, %%zmm8 \n\t"
                "vmovdqu32 %%zmm0, %%zmm31 \n\t"
		"vfmadd132pd %%zmm7, %%zmm8, %%zmm31 \n\t"
                "vmovdqu32 %%zmm0, (%%rsi) \n\t"
                "vmovdqu32 %%zmm7, 0x40(%%rsi) \n\t"
                "vmovdqu32 %%zmm8, 0x80(%%rsi) \n\t"
                "vmovdqu32 %%zmm31, 0xc0(%%rsi) \n\t"
                :: "r"(array) : "memory");
	asm volatile("vzeroupper" ::: "memory");
	for(i=0;i<256;i++) {
		if (i%16==0)
			printf("====%d====\t",i);
		printf("%d ", array[i]);
		if (i%16==15)
			printf("\n");
	}
	//asm volatile("vzeroupper" ::: "memory");
}

int main(int ac, char **av)
{
//	for (;;) 
		triggeravx();
	return 0;
}
