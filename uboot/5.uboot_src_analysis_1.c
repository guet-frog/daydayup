��2.uboot��ϵͳ��ֲ-��5����-2.5.ubootԴ�����1-������һ�׶Ρ�

--------------------------------------------------------
			��ӭ���ʡ�����ʦ���������á��ٷ���վ
					www.zhulaoshi.org
				   �Ի�ȡ���γ̸�����Ϣ
--------------------------------------------------------
��һ���֡��½�Ŀ¼
2.5.1.start.S����
2.5.2.start.S����1
2.5.3.start.S����2
2.5.4.start.S����3
2.5.5.start.S����4
2.5.6.start.S����5
2.5.7.start.S����6
2.5.8.start.S����7
2.5.9.uboot�ض�λ���
2.5.10.start.S����8
2.5.11.start.S����9
2.5.12.start.S����10
2.5.13.start.S����11


�ڶ����֡��½ڽ���
2.5.1.start.S����
	��������ͨ���������ӽű��ҵ�start.S�ļ���Ȼ�󽲽����������SourceInsight�ҵ���Ӧ�ļ��ļ��ɡ�
2.5.2.start.S����1
	���ڿ�ʼ����start.S�ļ�����Ҫ�������ļ���ͷ���ֵļ���ͷ�ļ�������ͨ����Щͷ�ļ������������ǰ��γ��н��������ù��̹���������
2.5.3.start.S����2
	������Ҫ������uboot��SD/Nand������ʱ��16�ֽ�У��ͷ���쳣�������Ľ������Լ�����һЩ���ŵȡ�
2.5.4.start.S����3
	���ڽ����˸�λ������CPUΪSVC32ģʽ��cache��mmu���á���ȡOMpin���ж���������ѡ��ȹ��ܴ��롣
2.5.5.start.S����4
	���ڷ�����lowlevel_init.S�е���ʼ���֣�������⸴λ״̬��IO�ָ����ؿ��Ź��������幩������ȡ�
2.5.6.start.S����5
	�����ص�������жϵ�ǰ���е�ַ�Ĵ���Ƭ�Σ�����жϷ���������н���ķ�ʽ������ͬ��Ȼ���Ҫ�����˻���ʼ��ʱ�ӵĴ���Ρ�
2.5.7.start.S����6
	�����ص㽲��Ͳ��ʼ����DDR��ʼ���ʹ��ڳ�ʼ�����ֵĴ��룬���Ҹ��ߴ��uboot����ʱ��"OK"����δ�ӡ�����ġ�
2.5.8.start.S����7
	�ܽ�lowlevel_init�����е����й��������ҷ���start.S�м���������������uboot�ڶ��׶ε��ض�λ���֡�
2.5.9.uboot�ض�λ���
	���ڽ���uboot���ض�λ����copy_bl2�������������SD���ض�λһ�������Ӧ���Ǻ���������ġ�
2.5.10.start.S����8
	���ڿ�ʼ����MMU�������ַӳ�䣬��Ҫ�������ַӳ��Ļ������ۺ�ʵ��ԭ����cache�����õȡ�
2.5.11.start.S����9
	���ڷ������������ж�ʽҳ�������Ĺ��̴��룬ͨ���򵥷����ô��ѧ�ῴҳ��������п��Կ����ڴ�ӳ���ϵ��
2.5.12.start.S����10
	�������ҳ���Ľ�����ܽᣬ������׵��ô�ҿ���X210�������ַӳ���ͼ�꣬ͬʱ����MMU��صĲ��֡�
2.5.13.start.S����11
	���ڽ���start.S�����Ĳ��֣���������bss�εȣ��Լ�������ת��uboot�ڶ��׶εĴ��롣���½�����

�������֡����ü�¼
2.5.1.start.S����
2.5.1.1��u-boot.lds���ҵ�start.S���
(1)��C������������Ŀ����ھ���main����������C���Թ涨�ģ�������Ʃ��˵һ����10000��.c�ļ�����Ŀ����һ��Ҫ�������ļ����ǰ�����main�������Ǹ��ļ���
(2)��uboot����Ϊ�л��׶β��룬��˲���ֱ����main.c��������������ȡ�������ӽű���ENTRY�����ĵط���ENTRY(_start)���_start�������ڵ��ļ����������������ʼ�ļ���_start���ڴ��Ĵ�����������������ʼ���롣

2.5.1.2��SourceInsight������ҵ��ļ�
(1)��ǰ״��������֪����uboot�е�1000����ļ�����һ�����Ž�_start���������ǲ�֪������������ĸ��ļ��С����������Ҫ����һ��������������Ŀ���ļ��е����ã�Ҫʹ��SourceInsight���������ܡ�
(2)����SI����������һ��7��_start��Ȼ���������������7����������2����api_example��2����onenand��صģ�����������Ҫ�ҵġ�ʣ��3������uboot/cpu/s5pc11x/start.S�ļ��С�
(3)Ȼ�����start.S�ļ��У�����57���о���_start��ŵĶ��崦�����Ǻ����Ǿ��ҵ�������uboot����ڴ��룬���ǵ�57�С�

2.5.1.3��SI�����ļ�����
(1)���ϣ��ҵ���start.S�ļ����������Ǿʹ�start.S�ļ���ʼ����uboot��һ�׶Ρ�
(2)��SI�У��������֪������Ҫ�ҵ��ļ������֣����������ֲ�֪�������ĸ�Ŀ¼�£�����Ҫ�����ҵ���������ļ�����������SI���ȴ��ұߵĹ�����Ŀ������Ŀ��Ȼ����������Ǹ�����������ļ�Ϊ��λ������ģ���Ȼ��������������������Ҫ�ҵ��ļ������֡������������ʱ��SI�ڲ��ϰ����ǽ���ƥ�䣬��ʹ�㲻�ǵ��ļ���ȫ��ֻ�Ǵ�żǵ����֣�Ҳ�ܰ������ҵ���Ҫ�ҵ��ļ���


2.5.2.start.S����1
2.5.2.1�����򵥵�ͷ�ļ�����
(1)#include <config.h>��config.h����includeĿ¼�µģ�����ļ�����Դ���б������ڵ��ļ����������ù������Զ����ɵ��ļ��������mkconfig�ű���������ļ���������ʵ�ǰ�����һ��ͷ�ļ���#include <configs/x210_sd.h>".
(2)���������󣬷���start.S�а����ĵ�һ��ͷ�ļ����ǣ�include/configs/x210_sd.h������ļ�������uboot��ֲʱ�������ļ����������Ǻö�ꡣ������ͷ�ļ�������include/configs/x210_sd.h�ļ���start.S�ļ����������������֮���ڷ���start.S�ļ�ʱ����ҪҪ���ǵľ���x210_sd.h�ļ���
(3)#include <version.h>��	�����и���
include/version.h�а�����include/version_autogenerated.h�����ͷ�ļ��������ù������Զ����ɵġ������һ�����ݣ�#define U_BOOT_VERSION "U-Boot 1.3.4"�������涨��ĺ�U_BOOT_VERSION��ֵ��һ���ַ������ַ����еİ汾����Ϣ������Makefile�е�����ֵ��������ڳ����лᱻ���ã���uboot���������лᴮ�ڴ�ӡ��uboot�İ汾�ţ��Ǹ��汾����Ϣ���Ǵ������ġ�
(4)#include <asm/proc/domain.h>��asmĿ¼����uboot�е�ԭ��Ŀ¼��uboot�б�����û�����Ŀ¼�ġ�asmĿ¼������ʱ������һ���������ӣ�ʵ��ָ����Ǿ���asm-arm�������һ�½ڷ���mkconfig�ű�ʱ��.
(5)�����������֣�ʵ���ļ��ǣ�include/asm-arm/proc-armv/domain.h
(6)��������Կ���֮ǰ����ʱ�����ķ������ӵ����ã����û����Щ�������������ʱ����ͨ��������Ϊ�Ҳ���ͷ�ļ���������uboot������windows�Ĺ����ļ��������ñ��룬��Ϊwindows��û�з������ӣ�
˼����Ϊʲôstart.S��ֱ�Ӱ���asm-arm/proc-armv/domain.h����Ҫ��asm/proc/domain.h�������������Ҫ��Ϊ�˿���ֲ�ԡ���Ϊ���ֱ�Ӱ�������start
.S�ļ���CPU�ܹ�����Ӳ�����й��ˣ�����ֲ�ԾͲ��ˡ�Ʃ����Ҫ��uboot��ֲ��mips�ܹ��£���start.SԴ���������е�ͷ�ļ�����ȫ��Ҫ�޸ġ��������˷�������֮����start.S��Դ���벻�øģ�ֻ��Ҫ�ھ����Ӳ����ֲʱ���ò�ͬ�������ķ�������ָ��Ĳ�ͬ������Ծ��п���ֲ�ԡ�


2.5.3.start.S����2
2.5.3.1�����������16�ֽ�ͷ��
(1)����н�������SD������/Nand��������������ͷ��Ҫ16�ֽڵ�У��ͷ����mkv210image.c�о���Ϊ�˼������У��ͷ����������ǰ���������ʱ����û������16�ֽ�У��ͷ����Ϊ��1�����������usb����ֱ�����صķ�ʽ����������Ҫ16�ֽ�У��ͷ��irom application note����2�������SD������mkv210image.c�л��ԭ����ǰ��16�ֽڵ�У��ͷ��
(2)uboot����start.S���ڿ�ͷλ�÷���16�ֽڵ����ռλ�����ռλ��16�ֽ�ֻ�Ǳ�֤��ʽ��image��ͷ��ȷʵ��16�ֽڣ�������16�ֽڵ������ǲ��Եģ�������Ҫ����ȥ����У���Ȼ���������ġ�

2.5.3.2���쳣�������Ĺ���
(1)�쳣��������Ӳ�������ģ�����ֻ�ǲ���Ӳ���������ʵ������
(2)�쳣��������ÿ���쳣��Ӧ�ñ������������������������쳣���ܷ��ˡ�����������uboot�в�δ�ǳ�ϸ�µĴ��������쳣��
(3)��λ�쳣���Ĵ����ǣ�b reset�������CPU��λ������ȥִ�е���Ч������reset���Ĵ��룬���reset���Ŵ�����������������Ĵ��뿪ʼ�ĵط���

2.5.3.3���е���˼��deadbeef
(1).balignl 16,0xdeadbeef.	��һ��ָ�����õ�ǰ��ַ�����Ų��������ǰ��ַ���������Զ�����ߵ�ֱַ�����룬��������ߵ���Щ�ڴ�Ҫ��0xdeadbeef����䡣
(2)0xdeadbeef����һ��ʮ�����Ƶ����֣�������ֺ�����˼�����������ֵ�ʮ��������ȫ��abcdef֮�е���ĸ��������8����ĸ�պ������Ӣ�ĵ�dead beef���������ʣ�������˼�ǻ�ţ�⡣
(3)ΪʲôҪ������ʣ���ʱ����Ч�ʵ�Ҫ����ʱ����Ӳ��������Ҫ��

2.5.3.4��TEXT_BASE��
(1)��100�����TEXT_BASE�����ϸ��γ��з���Makefileʱ�������Ǹ����ý׶ε�TEXT_BASE����ʵ������������ʱָ����uboot�����ӵ�ַ����ֵ����c3e00000��
(2)Դ�����к�����Makefile�кܶ�����ǿ��Ի������͵ġ�����˵��Щ���ŵ�ֵ���Դ�Makefile�д��ݵ�Դ�����С�


2.5.4.start.S����3
(1)CFG_PHY_UBOOT_BASE 	33e00000	uboot��DDR�е�������ַ

2.5.4.1������CPUΪSVCģʽ
(1)msr	cpsr_c, #0xd3	��CPU����Ϊ��ֹFIQ IRQ��ARM״̬��SVCģʽ�� // �ж�һ��ʼ����Ϊ��ֹ, ��ֹһ��ʼ�жϴ���δ����OK, �����ϵ�����ж��ܷ�
(2)��ʵARM CPU�ڸ�λʱĬ�Ͼͻ����SVCģʽ���������ﻹ��ʹ������������ΪSVCģʽ������uboot����ʱCPUһֱ����SVCģʽ��	// ������������Ӳ����Ԥ��

2.5.4.2������L2��L1cache��MMU
(1)bl	disable_l2cache		// ��ֹL2 cache
(2)bl	set_l2cache_auxctrl_cycle	// l2 cache��س�ʼ��
(3)bl	enable_l2cache		// ʹ��l2 cache
(4)ˢ��L1 cache��icache��dcache��
(5)�ر�MMU
�ܽ᣺������5�����Ǻ�CPU��cache��mmu�йصģ�����ȥϸ�������֪�����ɡ�

2.5.4.3��ʶ���ݴ���������ѡ��
(1)��������������SoC��OM5:OM0��6�����ŵĸߵ͵�ƽ�����ġ�
(2)ʵ������210�ڲ���һ���Ĵ�������ַ��0xE0000004��������Ĵ����е�ֵ��Ӳ������OM���ŵ����ö��Զ�����ֵ�ġ����ֵ��ӳ�ľ���OM���ŵĽӷ�����ƽ�ߵͣ���Ҳ��������������������˭��
(3)���Ǵ����п���ͨ����ȡ����Ĵ�����ֵȻ���ж���ֵ��ȷ����ǰѡ�е�����������Nand����SD���������ġ�
(4)start.S��225-227��ִ�������r2�Ĵ����д洢��һ�����֣�������ֵ���ĳ���ض�ֵʱ�ͱ�ʾSD������������һ���ض�ֵʱ��ʾ��Nand������������
(5)260���и�r3�и�ֵ#BOOT_MMCSD(0x03)�������SD����ʱʵ�ʻᱻִ�У����ִ������һ�δ����r3�д洢��0x03���Ժ��á�

2.5.4.4������ջ��SRAM�е�ջ��������lowlevel_init
(1)284-286�е�һ������ջ���������ջ����SRAM�����õģ���Ϊ��ǰ�������뻹��SRAM�����У���ʱDDR��δ����ʼ���������á�ջ��ַ0xd0036000���Լ�ָ���ģ�ָ����ԭ��������ռ�ֻ��ջ�ã����ᱻ����ռ�á�
(2)�ڵ��ú���ǰ��ʼ��ջ����Ҫԭ�����ڱ����õĺ����ڻ����ٴε��ú�������BLֻ�Ὣ���ص�ַ�洢��LR�У���������ֻ��һ��LR�������ڵڶ�����ú���ǰҪ�Ƚ�LR��ջ������������ʱ��һ��ķ��ص�ַ�Ͷ��ˡ�


2.5.5.start.S����4
(1)ʹ��SourceInsight��Reference���ܣ��ҵ�lowlevel_init���������ĵط�������uboot/board/samsumg/x210/lowlevel_init.S�С�
2.5.5.1����鸴λ״̬
(1)����CPU�������ָ�λ�����Ʃ��ֱ�����ϵ硢��������˯��(�͹���)״̬�µĻ��ѵȣ���Щ��������ڸ�λ�����������ڸ�λ������Ҫȥ��⸴λ״̬�����жϵ��������������
(2)�ж����ָ�λ���������ڣ����ϵ�ʱDDR����Ҫ��ʼ�������õģ������������ߵ͹���״̬�µĸ�λ����Ҫ�ٴγ�ʼ��DDR��
2.5.5.2��IO״̬�ָ�
(1)�������һ���������������붼�޹أ���˲���ȥ������

2.5.5.3���ؿ��Ź�
(1)�ο�����п��Ź��½�

2.5.5.4��һЩSRAM SROM���GPIO����
(1)���������������޹أ����ù�

2.5.5.5����������
(1)lowlevel_init.S�ĵ�100-104�У������幩�����档
�ܽ᣺��ǰ100�У�lowlevel_init.S�в�û����̫������������飨���˹ؿ��Ź������������⣩��Ȼ�������110�вſ�ʼ����������Ĳ�����



2.5.6.start.S����5
2.5.6.1���жϵ�ǰ����ִ��λ��
(1)lowlevel_init.S��110-115�С�
(2)�⼸�д�������þ����ж���ǰ����ִ�е�λ����SRAM�л�����DDR�С�ΪʲôҪ������ж���ԭ��1��BL1��uboot��ǰһ���֣���SRAM����һ�ݣ���DDR��Ҳ��һ�ݣ�����������������ô��ǰ����Ӧ������SRAM�����е�BL1������ǵ͹���״̬�ĸ�λ��ʱ��Ӧ�þ�����DDR�����еġ�ԭ��2�������ж���ǰ���д���ĵ�ַ�����õģ�����ָ�������������С�Ʃ����lowlevel_init.S���ж���ǰ��������е�ַ������Ϊ��ȷ��Ҫ��Ҫִ��ʱ�ӳ�ʼ���ͳ�ʼ��DDR�Ĵ��롣�����ǰ��������SRAM�У�˵������������ôʱ�Ӻ�DDR����Ҫ��ʼ���������ǰ��������DDR�У���ô˵������������ʱ�Ӻ�DDR�������ٴγ�ʼ����
(2)bic	r1, pc, r0	������������ǣ���pc��ֵ�е�ĳЩbitλ��0��ʣ��һЩ�����bitλ��ֵ��r1��r0��Ϊ1����Щλ���㣩����ڣ�r1 = pc & ~(ff000fff)
ldr	r2, _TEXT_BASE	�������ӵ�ַ��r2��Ȼ��r2����Ӧλ��0ʣ���ض�λ��
(3)���Ƚ�r1��r2.
�ܽ᣺��һ�δ�����ͨ����ȡ��ǰ���е�ַ�����ӵ�ַ��Ȼ����������ַ��Ա��Ƿ���ȣ����ж���ǰ��������SRAM�У�����ȣ�����DDR�У���ȣ����Ӷ������Ƿ����������ʱ�Ӻ�DDR��ʼ����

2.5.6.2��system_clock_init
(1)ʹ��SI�������ܣ�ȷ������������ڵ�ǰ�ļ���205�У�һֱ����385�С������ʼ��ʱ�ӵĹ��̺�����г�ʼ���Ĺ���һ���ģ�ֻ�Ǹ��������������û�����д�ġ�
(2)��x210_sd.h��300�е�428�У����Ǻ�ʱ����ص�����ֵ����Щ�궨��;�����210��ʱ�������Ƕ��١�Ҳ����˵������lowlevel_init.S�ж�д���ˣ����Ǵ��������ֵ�����궨����x210_sd.h���ˡ���ˣ������ֲʱ��Ҫ����CPU��ʱ�����ã���������Ҫ�����룬ֻ��Ҫ��x210_sd.h�и�������ֵ���ɡ�


2.5.7.start.S����6
2.5.7.1��mem_ctrl_asm_init
(1)�ú���������ʼ��DDR
(2)����λ����uboot/cpu/s5pc11x/s5pc110/cpu_init.S�ļ��С�
(3)�ú���������г�ʼ��DDR������һ���ġ�ʵ��������г�ʼ��DDR�Ĵ�����Ǵ����ﳭ�ġ�����ֵҲ���Դ����ﳭ�����ǵ�ʱ���Լ���������+��Ϯ��������һ�ݡ�
(4)����ֵ����������ֵ�ο�����еĽ��ͼ������ף���һ��������н��Ĳ�һ����DMC0_MEMCONFIG_0�������������ֵΪ0x20E01323����uboot������Ϊ0x30F01313.������ò�ͬ�͵��½����ͬ��
�� �����DMC0��256MB�ڴ��ַ��Χ��0x20000000-0x2FFFFFFF; 
��uboot��DMC0��256MB�ڴ��ַ��ΧΪ0x30000000-0x3FFFFFFF��
(5)֮ǰ�������ʱ����Ϊ2��ͷ�ĵ�ַ����ʱ��û��˵��������Ϊ3��ͷ���ӷ����Ŷ���ֲ��uboot���Կ�����DMC0�������ĵ�ַ��Χ��20000000-3FFFFFFF��һ����512MB����������ʵ��ֻ����256MB�����ڴ棬SoC�������Ǹ���256MB��ѡ��ַ��Χ��
(6)�ܽ�һ�£���uboot�У����õ�������ַ��ΧΪ��0x30000000-0x4FFFFFFF��һ��512MB������30000000-3FFFFFFFΪDMC0��40000000-4FFFFFFFΪDMC1��
(7)������Ҫ���ڴ�����ֵ��x210_sd.h��438�е�468��֮�䡣������ʱ��Ҫע���������������������ͷ�ļ��п����˲�ͬʱ�������µ��ڴ�����ֵ���������ҪĿ�����ò�ͬʱ������Ŀͻ������ҵ������Լ����ڴ�����ֵ��
(8)��uboot��DMC0��DMC1�������ˣ������������ֻҪ��uboot�е�����ֵ�����ô���ȫ����ֲ��ȥ��Ӧ�����ܹ���DMC0��DMC1�������ġ�

2.5.7.2��uart_asm_init
(1)�������������ʼ������
(2)��ʼ�����˺�ͨ�����ڷ�����һ��'O'

2.5.7.3��tzpc_init
(1)trust zone��ʼ����û���������

2.5.7.4��pop {pc}�Է���
(1)����ǰͨ�����ڴ�ӡ'K'

������lowlevel_init.Sִ�������û����ô�ͻᴮ�ڴ�ӡ��"OK"��������Ӧ��������uboot�п���������������Ϣ��


2.5.8.start.S����7
�ܽ�عˣ�lowlevel_init.S���ܹ�������Щ���飺
��鸴λ״̬��IO�ָ����ؿ��Ź��������幩�����桢ʱ�ӳ�ʼ����DDR��ʼ�������ڳ�ʼ������ӡ'O'��tzpc��ʼ������ӡ'K'��
����ֵ�ù�ע�ģ��ؿ��Ź��������幩�����桢ʱ�ӳ�ʼ����DDR��ʼ������ӡ"OK"
2.5.8.1���ٴ�����ջ��DDR�е�ջ��
(1)�ٴο����幩�����档��һ����2���ǲ�����ģ��ڶ�����2�����2�������壻��������ֲʱ��һ���Źֽ������ز��Ծ��Ǿ������Ӵ������Ҫɾ�����롣
(2)֮ǰ�ڵ���lowlevel_init����ǰ���ù�1��ջ��start.S 284-287�У�����ʱ����ΪDDR��δ��ʼ������˳���ִ�ж�����SRAM�У�������SRAM�з�����һ�����ڴ���Ϊջ��������ΪDDR�Ѿ�����ʼ���ˣ����Ҫ��ջŲ�Ƶ�DDR�У�����Ҫ��������ջ�����ǵڶ��Σ�start.S 297-299�У�������ʵ�����õ�ջ�ĵ�ַ��33E00000���պ���uboot�Ĵ���ε���������š�
(3)ΪʲôҪ�ٴ�����ջ��DDR�Ѿ���ʼ���ˣ��Ѿ��д�Ƭ�ڴ�������ˣ�û��Ҫ�ٰ�ջ����SRAM�п���������ˣ�ԭ��SRAM���ڴ��С�ռ����ޣ�ջ��������Ҫע�ⲻ��ʹ�ù����ջ����ջ����������Ǽ�ʱ��ջǨ�Ƶ�DDR��Ҳ��Ϊ�˾����ܱ���ջʹ��ʱ���С��������
�п���uboot�������׶���Ҫ���ɾ�����С��Χ�����������µ�շת��Ų��

2.5.8.2���ٴ��жϵ�ǰ��ַ�Ծ����Ƿ��ض�λ
(1)�ٴ�����ͬ�Ĵ����ж����е�ַ����SRAM�л���DDR�У����������жϵ�Ŀ�Ĳ�ͬ���ϴ��ж���Ϊ�˾����Ƿ�Ҫִ�г�ʼ��ʱ�Ӻ�DDR�Ĵ��룩����ж���Ϊ�˾����Ƿ����uboot��relocate��
(2)������ʱ��ǰ�����uboot��ǰһ���֣�16kb����8kb�������Զ���SD�����ص�SRAM���������У�uboot�ĵڶ����֣���ʵ�ڶ�����������uboot��������SD����ĳ��������ͷ��N�������С���ʱuboot�ĵ�һ�׶��Ѿ����������ˣ���һ�׶θ������»��������ˣ�������֮ǰҪ�ѵڶ����ּ��ص�DDR�����ӵ�ַ����33e00000����������ع��̾ͽ��ض�λ��


2.5.9.uboot�ض�λ���
(1)D0037488����ڴ��ַ��SRAM�У������ַ�е�ֵ�Ǳ�Ӳ���Զ����õġ�Ӳ����������ʵ�ʵ�·��SD�����ĸ�ͨ���У��Ὣ�����ַ�е�ֵ����Ϊ��Ӧ�����֡�Ʃ�����Ǵ�SD0ͨ������ʱ�����ֵΪEB000000����SD2ͨ������ʱ�����ֵΪEB200000
(2)������start.S��260��ȷ���˴�MMCSD������Ȼ������278�н�#BOOT_MMCSDд����INF_REG3�Ĵ����д洢�š�Ȼ������322�ж��������ٺ�#BOOT_MMCSDȥ�Ƚϣ�ȷ���Ǵ�MMCSD������������ת��mmcsd_boot������ȥִ���ض�λ������
(3)�������ض�λ��ͨ������movi_bl2_copy������ɵģ���uboot/cpu/s5pc11x/movi.c�С���һ��C���Եĺ���
(4)copy_bl2(2, MOVI_BL2_POS, MOVI_BL2_BLKCNT,
			CFG_PHY_UBOOT_BASE, 0);
����������2��ʾͨ��2��MOVI_BL2_POS��uboot�ĵڶ�������SD���еĿ�ʼ����������������ֱ������¼ubootʱ��¼��λ����ͬ��MOVI_BL2_BLKCNT��uboot�ĳ���ռ�õ���������CFG_PHY_UBOOT_BASE���ض�λʱ��uboot�ĵڶ����ָ��Ƶ�DDR�е���ʼ��ַ��33E00000��.


2.5.10.start.S����8
2.5.10.1��ʲô�������ַ��������ַ
(1)������ַ���������豸�������ʱ����ĵ�ַ���������ʹ�õļĴ����ĵ�ַ����CPU���ʱָ���ģ��������������ַ��������ַ��Ӳ������ģ����������ʱȷ���õģ�һ��ȷ���˾Ͳ��ܸ��ˡ�
(2)һ����ʵ���ǣ��Ĵ�����������ַ���޷�ͨ������޸ĵģ��Ƕ��پ��Ƕ��٣�ֻ��ͨ����ѯ�����ֲ��ò��������������ǲ�����һ�������������ʹ�������ַ��
(3)�����ַ��˼��������������������Ӳ��������֮������һ����Σ����������ַӳ��㡣���������ַӳ�����������ֻ��Ҫ�������ַ��Ӳ������������ԭ����������ַ��ӳ��㽨��һ�������ַ��������ַ��ӳ������������������е�ʱ��������ʹ�õ������ַ��ӳ����в�ѯ�õ���Ӧ��������ַ�ٷ���Ӳ��ȥִ�У������ַ��������ַ��ӳ���ǲ�����ͨ��������ʵ�ֵģ���

2.5.10.2��MMU��Ԫ������
(1)MMU����memory management unit���ڴ������Ԫ��MMUʵ������SOC��һ��Ӳ����Ԫ��������Ҫ���ܾ���ʵ�������ַ��������ַ��ӳ�䡣
(2)MMU��Ƭ��CP15Э�������н��п��ƣ�Ҳ����˵Ҫ�ٿ�MMU���������ַӳ�䣬�������Ƕ�cp15Э�������ļĴ������б�̡�

2.5.10.3����ַӳ��Ķ�������1�����ʿ���
(1)���ʿ��ƾ��ǣ��ڹ����϶��ڴ���зֿ飬Ȼ��ÿ����ж����������ַӳ�䣬Ȼ����ÿһ���ӳ���ϵ��ͬʱ��ʵ���˷��ʿ��ƣ��Ըÿ�ɶ�����д��ֻ����ֻд�����ɷ��ʵȿ��ƣ�
(2)������C�����б���о��������һ������Segmentation fault��ʵ��������δ���ͺ�MMUʵ�ֵķ��ʿ����йء���ǰ����ֻ�ܲ����Լ���Ȩ�����ĵ�ַ��Χ�����ɸ��ڴ�飩�������ǰ����ָ����������˲��÷��ʵ��ڴ����ͻᴥ���δ���

2.5.10.4����ַӳ��Ķ�������2��cache
(1)cache�Ĺ����������ַӳ���й�ϵ��
(2)cache�ǿ��ٻ��棬��˼���Ǳ�CPU�����Ǳ�DDR�顣CPU��DDR̫���ˣ����Ǻ���һЩDDR�г��õ��������ȶ�ȡ������cache�У�Ȼ��CPUÿ����Ҫ�Ҷ���ʱ����cache���ҡ����cache���о�ֱ����cache�еģ����cache��û�вŻ�ȥDDR��Ѱ�ҡ�

�ο��Ķ���http://blog.chinaunix.net/xmlrpc.php?r=blog/article&uid=22891521&id=2109284


2.5.11.start.S����9
2.5.11.1��ʹ������ʣ�cp15��c3�Ĵ�����
(1)cp15Э�������ڲ���c0��c15��16���Ĵ�������Щ�Ĵ���ÿһ�������Լ������á�����ͨ��mrc��mcrָ����������Щ�Ĵ�������ν�Ĳ���cpЭ��������ʵ���ǲ���cp15����Щ�Ĵ�����
(2)c3�Ĵ�����mmu�е������ǿ�������ʡ�������Ǻ�MMU�ķ��ʿ����йصġ�

2.5.11.2������TTB��cp15��c2�Ĵ�����
(1)TTB����translation table base��ת��������ַ������Ҫ����ʲô��TT��translation tableת��������TTB��ʵ����ת�����Ļ���ַ��
(2)ת�����ǽ���һ�������ַӳ��Ĺؼ���ת������2���֣��������ͱ����������Ӧ�����ַ�������Ӧ������ַ��һ�Ա������ͱ����һ��ת������Ԫ���ܹ���һ���ڴ����������ַת������ӳ���л����涨�й涨���ڴ�ӳ��͹������Կ�Ϊ��λ�ģ����ڿ��ж��Ҫ�����MMU��֧�ֺ����Լ���ѡ����ARM��֧��3�ֿ��С��ϸ��1KB���ֱ�4KB����1MB����������ת�������������ɸ�ת������Ԫ���ɵģ�ÿ����Ԫ����1���ڴ�飬�����ת�������������ڴ�ռ䣨0-4G����ӳ�䡣
(3)�������������ַӳ�����Ҫ�������ǽ�������ת����
(4)ת�����������ڴ��еģ�����ʱҪ����ʼ��ַ���ڴ���Ҫxxλ���롣ת��������Ҫ����ȥ����ʹ�ã����ǽ�����ַTTB���õ�cp15��c2�Ĵ����У�Ȼ��MMU����ʱ���Զ�ȥ��ת������

2.5.11.3��ʹ��MMU��Ԫ��cp15��c1�Ĵ�����
(1)cp15��c1�Ĵ�����bit0����MMU�Ŀ��ء�ֻҪ����һ��bit��1���ɿ���MMU������MMU֮���ϲ�������ĵ�ַ�ͱ��뾭��TT��ת�����ܷ����²�������ȥִ�С�

2.5.11.4���ҵ�ӳ���������
(1)ͨ�����Ų��ң�ȷ��ת������lowlevel_init.S�ļ���593�С�

2.5.11.5��S5PV210��2�������ַ����


2.5.12.start.S����10
2.5.12.1����FL_SECTION_ENTRY
2.5.12.2��ҳ�����bitλ����
2.5.12.3����ʽҳ�����
2.5.12.4��ʵ�������֤
2.5.12.5���ܽ᣺����MMU�������ַӳ���ѧϰ

���������ת����������ת�������Կ�����һ��int���͵����飬�����е�һ��Ԫ�ؾ���һ���������ͱ���ĵ�Ԫ�������е�Ԫ��ֵ���Ǳ�����Ԫ�ص������±���Ǳ�������
ARM�Ķ�ʽӳ���г���Ϊ1MB�����һ��ӳ�䵥Ԫֻ�ܹ�1MB�ڴ棬����������4G��Χ����Ҫ4G/1MB=4096��ӳ�䵥Ԫ��Ҳ����˵��������Ԫ�ظ�����4096.ʵ������������ʱ��û�����ε���������4096����Ԫ�����ǰ�4096���ֳɼ����֣�Ȼ��ÿ������forѭ������ͬ�Ĵ�����


2.5.13.start.S����11
2.5.13.1���ٴ�����ջ
(1)����������ջ���������ջ������DDR�У�֮ǰ��Ȼ�Ѿ���DDR�����ù�һ��ջ�ˣ����Ǳ�������ջ��Ŀ���ǽ�ջ���ڱȽϺ��ʣ���ȫ�����ն����˷��ڴ棩�ĵط���
(2)����ʵ�ʽ�ջ������uboot��ʼ��ַ�Ϸ�2MB����������ȫ��ջ�ռ��ǣ�2MB-uboot��С-0x1000=1.8MB���ҡ�����ռ��û��̫�˷��ڴ棬���㹻��ȫ��

2.5.13.2������bss
(1)����bss�δ��������н���һ����ע���ʾbss�εĿ�ͷ�ͽ�β��ַ�ķ����Ǵ����ӽű�u-boot.lds�����ġ�

2.5.13.3��ldr	pc, _start_armboot
(1)start_armboot��uboot/lib_arm/board.c�У�����һ��C����ʵ�ֵĺ����������������uboot�ĵڶ��׶Ρ�����������þ��ǽ�uboot�ڶ��׶�ִ�еĺ����ĵ�ַ����pc��ʵ���Ͼ���ʹ��һ��Զ��תֱ����ת��DDR�еĵڶ��׶ο�ʼ��ַ����
(2)Զ��ת�ĺ��������仰���صĵ�ַ�͵�ǰ���е�ַ�޹أ��������ӵ�ַ�йء�������Զ��ת����ʵ�ִ�SRAM�еĵ�һ�׶���ת��DDR�еĵڶ��׶Ρ�
(3)�������Զ��ת����uboot��һ�׶κ͵ڶ��׶εķֽ��ߡ�

2.5.13.4���ܽ᣺uboot�ĵ�һ�׶�������Щ����
(1)�����쳣������
(2)����CPUΪSVCģʽ
(3)�ؿ��Ź�
(4)�����幩������
(5)ʱ�ӳ�ʼ��
(6)DDR��ʼ��
(7)���ڳ�ʼ������ӡ"OK"
(8)�ض�λ
(9)����ӳ���������MMU
(10)��ת���ڶ��׶�





























