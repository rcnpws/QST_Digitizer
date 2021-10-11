/*
   SIS3800 VME Scaler/Counter
*/

#ifndef _SIS3800_H_
#define _SIS3800_H_

#define ADDR_SIS3800                        0xaaaaa000

#define SIS3800_NCH                         32
#define SIS3800_MODE1                       0x0004

/* Address Map */
#define SIS3800_CONTROL_REGISTER            0x000
#define SIS3800_STATUS_REGISTER             0x000
#define SIS3800_IRQ_CONTROL                 0x004
#define SIS3800_SLECTIVE_COUNT_DISABLE      0x00c
#define SIS3800_CLEAR_ALL_COUNTERS          0x020
#define SIS3800_CLOCK_SHADOW_REGISTER       0x024
#define SIS3800_GLOBAL_COUNT_ENABLE         0x028
#define SIS3800_GLOBAL_COUNT_DISABLE        0x02c
#define SIS3800_BC_CLEAR_ALL_COUNTERS       0x030
#define SIS3800_BC_CLOCK_SHADOW_REGISTER    0x034
#define SIS3800_BC_GLOBAL_COUNT_ENABLE      0x038
#define SIS3800_BC_GLOBAL_COUNT_DISABLE     0x03c
#define SIS3800_CLEAR_COUNTER_GROUP         0x040 /* - 0x04c */
#define SIS3800_ENABLE_REFERENCE_PULSER     0x050
#define SIS3800_DISABLE_REFERENCE_PULSER    0x054
#define SIS3800_RESET_REGISTER_GLOBAL       0x060
#define SIS3800_TEST_PULSE                  0x068
#define SIS3800_CLEAR_COUNTER               0x100 /* - 0x17c */
#define SIS3800_CLEAR_OVERFLOW              0x180 /* - 0x1fc */
#define SIS3800_READ_SHADOW_REGISTER        0x200 /* - 0x27c */
#define SIS3800_READ_COUNTER                0x280 /* - 0x2fc */
#define SIS3800_READ_AND_CLEAR_ALL_COUNTERS 0x300 /* - 0x37c */
#define SIS3800_OVERFLOW_REGISTER_01_08     0x380
#define SIS3800_OVERFLOW_REGISTER_09_16     0x3A0
#define SIS3800_OVERFLOW_REGISTER_17_24     0x3C0
#define SIS3800_OVERFLOW_REGISTER_25_32     0x3E0


/* Bits for Read Status Register */
#define SIS3800_STATUS_IRQ_SOURCE_2        0x40000000
#define SIS3800_STATUS_IRQ_SOURCE_1        0x20000000
#define SIS3800_STATUS_IRQ_SOURCE_0        0x10000000
#define SIS3800_VME_IRQ                    0x08000000
#define SIS3800_INTERNAL_VME_IRQ           0x04000000
#define SIS3800_STATUS_IRQ_ENABLE_S2       0x00400000
#define SIS3800_STATUS_IRQ_ENABLE_S1       0x00200000
#define SIS3800_STATUS_IRQ_ENABLE_S0       0x00100000
#define SIS3800_RESERVED                   0x00010000
#define SIS3800_GLOBAL_COUNTER_ENABLE      0x00008000
#define SIS3800_GENERAL_OVERFLOW           0x00004000
#define SIS3800_STATUS_ENABLE_REF_PULSER1  0x00002000
#define SIS3800_STATUS_BROADCAST_HANDSHAKE 0x00000080
#define SIS3800_STATUS_BROADCAST           0x00000040
#define SIS3800_STATUS_INPUT_TEST          0x00000020
#define SIS3800_STATUS_25MHZ_TEST_PULSES   0x00000010
#define SIS3800_STATUS_INPUT_BIT1          0x00000008
#define SIS3800_STATUS_INPUT_BIT0          0x00000004
#define SIS3800_STATUS_IRQ_S2_SOFTWARE_IRQ 0x00000002
#define SIS3800_STATUS_USER_LED            0x00000001

/* Bits for Write Control Register */
#define SIS3800_DISABLE_IRQ_SOURCE_2        0x40000000
#define SIS3800_DISABLE_IRQ_SOURCE_1        0x20000000
#define SIS3800_DISABLE_IRQ_SOURCE_0        0x10000000
#define SIS3800_CLEAR_RESERVED_BIT          0x01000000
#define SIS3800_ENABLE_IRQ_ENABLE_S2        0x00400000
#define SIS3800_ENABLE_IRQ_ENABLE_S1        0x00200000
#define SIS3800_ENABLE_IRQ_ENABLE_S0        0x00100000
#define SIS3800_SET_RESERVED_BIT            0x00010000
#define SIS3800_DISABLE_BROADCAST_HANDSHAKE 0x00008000
#define SIS3800_DISABLE_BROADCAST           0x00004000
#define SIS3800_DISABLE_INPUT_TEST          0x00002000
#define SIS3800_DISABLE_25MHZ_TEST_PULSES   0x00001000
#define SIS3800_CLEAR_INPUT_BIT1            0x00000800
#define SIS3800_CLEAR_INPUT_BIT0            0x00000400
#define SIS3800_CLEAR_IRQ_S2_SOFTWARE_IRQ   0x00000200
#define SIS3800_CLEAR_USER_LED              0x00000100
#define SIS3800_ENABLE_BROADCAST_HANDSHAKE  0x00000080
#define SIS3800_ENABLE_BROADCAST            0x00000040
#define SIS3800_ENABLE_INPUT_TEST           0x00000020
#define SIS3800_ENABLE_25MHZ_TEST_PULSES    0x00000010
#define SIS3800_SET_INPUT_BIT1              0x00000008
#define SIS3800_SET_INPUT_BIT0              0x00000004
#define SIS3800_SET_IRQ_S2_SOFTWARE_IRQ     0x00000002
#define SIS3800_SET_USER_LED                0x00000001

typedef struct Scaler_t{
  uint32_t val[SIS3800_NCH];
  uint32_t previous[SIS3800_NCH];
  uint32_t overflow[SIS3800_NCH];
  double rate[SIS3800_NCH];
  uint32_t scalertime;  
} Scaler_t;

const char* GetScalerName(int ch);
int InitializeScaler(int BHandle, Scaler_t * pScaler);
int ScalerRead(int BHandle, Scaler_t * pScaler);
int ScalerDuringRun(int BHandle, Scaler_t * pScaler,int isWrite, FILE* f_scaler);
int ScalerEndOfRun(int BHandle, Scaler_t * pScaler,int isWrite, FILE* f_scaler);


#endif //_SIS3800_H_
