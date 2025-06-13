/*
WASH----------------...---------\
  |           |                 |
  |           X                 X
  |       C1 <1_0>          Cn <n_0>
<WASH>   |    |     ...    |    |
  X       \-X<1_1>          \-X<n_1>
   \---       |                 |
       \      |                 s|
       |      |                 |
VENT-X---------------------------------<PI>----<P>-----<PO>----<gFET>----<GO>------<COL>----WASTE
                                        x               x                 x          x
                                        |               |                 |          |
                                        WASTE           WASTE             WASH       COLLECTION
*/

// https://www.google.com/url?sa=i&url=https%3A%2F%2Fwww.pjrc.com%2Fteensy%2Fpinout.html&psig=AOvVaw1VaQ4t1I5yIEQFiEOk-Zp5&ust=1713369738385000&source=images&cd=vfe&opi=89978449&ved=0CBAQjRxqFwoTCOD_4uaNx4UDFQAAAAAdAAAAABAE

// WASH ALL ___ -t 10 -r 100
#ifndef PIN_MAPPING
#define PIN_MAPPING

#define CHEMICAL1_SSR_00 11
#define CHEMICAL1_SSR_01 12
#define CHEMICAL2_SSR_00 24
#define CHEMICAL2_SSR_01 25
#define CHEMICAL3_SSR_00 26
#define CHEMICAL3_SSR_01 27
#define CHEMICAL4_SSR_00 28
#define CHEMICAL4_SSR_01 29
#define CHEMICAL5_SSR_00 30
#define CHEMICAL5_SSR_01 31
#define CHEMICAL6_SSR_00 32
#define CHEMICAL6_SSR_01 33
#define CHEMICAL7_SSR_00 34
#define CHEMICAL7_SSR_01 35
#define CHEMICAL8_SSR_00 36
#define CHEMICAL8_SSR_01 37
#define VENT_SSR 13
#define GI_SSR 39
#define PO_SSR 38
#define PI_SSR 41
#define WASH_SSR 14
#define COL_SSR 40

const int CHEMICAL_SSR_ARRAY[8][2] = {
    {CHEMICAL1_SSR_00, CHEMICAL1_SSR_01},
    {CHEMICAL2_SSR_00, CHEMICAL2_SSR_01},
    {CHEMICAL3_SSR_00, CHEMICAL3_SSR_01},
    {CHEMICAL4_SSR_00, CHEMICAL4_SSR_01},
    {CHEMICAL5_SSR_00, CHEMICAL5_SSR_01},
    {CHEMICAL6_SSR_00, CHEMICAL6_SSR_01},
    {CHEMICAL7_SSR_00, CHEMICAL7_SSR_01},
    {CHEMICAL8_SSR_00, CHEMICAL8_SSR_01}};

const int PUMP_ARRAY[] = {
    CHEMICAL1_SSR_00,
    CHEMICAL1_SSR_01,
    CHEMICAL2_SSR_00,
    CHEMICAL2_SSR_01,
    CHEMICAL3_SSR_00,
    CHEMICAL3_SSR_01,
    CHEMICAL4_SSR_00,
    CHEMICAL4_SSR_01,
    CHEMICAL5_SSR_00,
    CHEMICAL5_SSR_01,
    CHEMICAL6_SSR_00,
    CHEMICAL6_SSR_01,
    CHEMICAL7_SSR_00,
    CHEMICAL7_SSR_01,
    CHEMICAL8_SSR_00,
    CHEMICAL8_SSR_01,
    VENT_SSR,
    COL_SSR,
    GI_SSR,
    PO_SSR,
    PI_SSR,
    WASH_SSR,
};

#define EXTRA_SRR_00 9
#define EXTRA_SRR_01 10
#define EXTRA_SRR_02 15
#define EXTRA_SRR_03 16

const int EXTRA_SSR_ARRAY[] = {
    EXTRA_SRR_00,
    EXTRA_SRR_01,
    EXTRA_SRR_02,
    EXTRA_SRR_03};

#define NEO_PIXEL 5

#define ENA 0
#define ON_OFF 1
#define HIGH_LOW 7
#define DIR 8

#define LED_POWER 4
#define LED_STATUS 3

const int LED_ARRAY[]{
    LED_POWER,
    LED_STATUS};

#define SDA 18
#define SCL 19

#define CONN_00 17
#define CONN_01 20
#define CONN_02 21
#define CONN_03 22
#define CONN_04 23
#define CONN_05 2
#define CONN_06 6

const int SSR_ARRAY[] = {
    CHEMICAL1_SSR_00,
    CHEMICAL1_SSR_01,
    CHEMICAL2_SSR_00,
    CHEMICAL2_SSR_01,
    CHEMICAL3_SSR_00,
    CHEMICAL3_SSR_01,
    CHEMICAL4_SSR_00,
    CHEMICAL4_SSR_01,
    CHEMICAL5_SSR_00,
    CHEMICAL5_SSR_01,
    CHEMICAL6_SSR_00,
    CHEMICAL6_SSR_01,
    CHEMICAL7_SSR_00,
    CHEMICAL7_SSR_01,
    CHEMICAL8_SSR_00,
    CHEMICAL8_SSR_01,
    VENT_SSR,
    COL_SSR,
    GI_SSR,
    PO_SSR,
    PI_SSR,
    WASH_SSR,
    EXTRA_SRR_00,
    EXTRA_SRR_01,
    EXTRA_SRR_02,
    EXTRA_SRR_03,
    LED_POWER,
    LED_STATUS};

#endif