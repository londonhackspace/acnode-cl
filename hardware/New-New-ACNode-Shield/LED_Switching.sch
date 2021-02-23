EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 2 5
Title "ACNode Launchpad Overlay"
Date "2020-12-27"
Rev "1"
Comp "London Hackspace"
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L Device:LED_ABRG D?
U 1 1 5FDB67B7
P 4050 1150
AR Path="/5FDB67B7" Ref="D?"  Part="1" 
AR Path="/5FDAF572/5FDB67B7" Ref="D1"  Part="1" 
F 0 "D1" V 4096 820 50  0000 R CNN
F 1 "LED_ABRG" V 4005 820 50  0000 R CNN
F 2 "ExtraParts:Wurth_3528_LED" H 4050 1150 50  0001 C CNN
F 3 "https://www.we-online.de/katalog/datasheet/150141M173100.pdf" H 4050 1150 50  0001 C CNN
	1    4050 1150
	0    -1   -1   0   
$EndComp
Text Notes 4200 1000 0    49   ~ 0
RS 815-4332 (for example)
$Comp
L power:+3.3V #PWR?
U 1 1 5FDB67D3
P 4050 900
AR Path="/5FDB67D3" Ref="#PWR?"  Part="1" 
AR Path="/5FDAF572/5FDB67D3" Ref="#PWR0119"  Part="1" 
F 0 "#PWR0119" H 4050 750 50  0001 C CNN
F 1 "+3.3V" H 4065 1073 50  0000 C CNN
F 2 "" H 4050 900 50  0001 C CNN
F 3 "" H 4050 900 50  0001 C CNN
	1    4050 900 
	1    0    0    -1  
$EndComp
Wire Wire Line
	4050 950  4050 900 
Text Notes 850  800  0    98   ~ 0
On-Board LED
$Comp
L Device:R R?
U 1 1 5FDCD5C9
P 4050 2050
AR Path="/5FDCD5C9" Ref="R?"  Part="1" 
AR Path="/5FDAF572/5FDCD5C9" Ref="R4"  Part="1" 
F 0 "R4" H 4120 2096 50  0000 L CNN
F 1 "27R" H 4120 2005 50  0000 L CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 3980 2050 50  0001 C CNN
F 3 "~" H 4050 2050 50  0001 C CNN
	1    4050 2050
	1    0    0    -1  
$EndComp
$Comp
L Transistor_FET:2N7002 Q?
U 1 1 5FDCD5CF
P 3950 2550
AR Path="/5FDCD5CF" Ref="Q?"  Part="1" 
AR Path="/5FDAF572/5FDCD5CF" Ref="Q5"  Part="1" 
F 0 "Q5" H 4154 2596 50  0000 L CNN
F 1 "2N7002" H 4150 2450 50  0000 L CNN
F 2 "Package_TO_SOT_SMD:SOT-23" H 4150 2475 50  0001 L CIN
F 3 "https://www.fairchildsemi.com/datasheets/2N/2N7002.pdf" H 3950 2550 50  0001 L CNN
	1    3950 2550
	1    0    0    -1  
$EndComp
$Comp
L Device:R R?
U 1 1 5FDCEFE5
P 5800 2050
AR Path="/5FDCEFE5" Ref="R?"  Part="1" 
AR Path="/5FDAF572/5FDCEFE5" Ref="R5"  Part="1" 
F 0 "R5" H 5870 2096 50  0000 L CNN
F 1 "27R" H 5870 2005 50  0000 L CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 5730 2050 50  0001 C CNN
F 3 "~" H 5800 2050 50  0001 C CNN
	1    5800 2050
	1    0    0    -1  
$EndComp
$Comp
L Transistor_FET:2N7002 Q?
U 1 1 5FDCEFEB
P 5700 2550
AR Path="/5FDCEFEB" Ref="Q?"  Part="1" 
AR Path="/5FDAF572/5FDCEFEB" Ref="Q6"  Part="1" 
F 0 "Q6" H 5904 2596 50  0000 L CNN
F 1 "2N7002" H 5904 2505 50  0000 L CNN
F 2 "Package_TO_SOT_SMD:SOT-23" H 5900 2475 50  0001 L CIN
F 3 "https://www.fairchildsemi.com/datasheets/2N/2N7002.pdf" H 5700 2550 50  0001 L CNN
	1    5700 2550
	1    0    0    -1  
$EndComp
Wire Wire Line
	3850 1350 3850 1600
Wire Wire Line
	4050 1900 4050 1350
Wire Wire Line
	4050 2200 4050 2350
Wire Wire Line
	4250 1350 4250 1650
Wire Wire Line
	5800 1650 5800 1900
Wire Wire Line
	5800 2200 5800 2350
$Comp
L power:GND #PWR0120
U 1 1 5FDE6862
P 4050 3050
F 0 "#PWR0120" H 4050 2800 50  0001 C CNN
F 1 "GND" H 4055 2877 50  0000 C CNN
F 2 "" H 4050 3050 50  0001 C CNN
F 3 "" H 4050 3050 50  0001 C CNN
	1    4050 3050
	1    0    0    -1  
$EndComp
Wire Wire Line
	4050 2750 4050 2900
Connection ~ 4050 2900
Wire Wire Line
	4050 2900 4050 3050
Wire Wire Line
	5800 2750 5800 2900
$Comp
L Transistor_FET:2N7002 Q1
U 1 1 5FDFC391
P 7150 2150
F 0 "Q1" H 7354 2196 50  0000 L CNN
F 1 "2N7002" H 7354 2105 50  0000 L CNN
F 2 "Package_TO_SOT_SMD:SOT-23" H 7350 2075 50  0001 L CIN
F 3 "https://www.fairchildsemi.com/datasheets/2N/2N7002.pdf" H 7150 2150 50  0001 L CNN
	1    7150 2150
	1    0    0    -1  
$EndComp
$Comp
L Transistor_FET:2N7002 Q2
U 1 1 5FDFF38B
P 8800 2150
F 0 "Q2" H 9004 2196 50  0000 L CNN
F 1 "2N7002" H 9004 2105 50  0000 L CNN
F 2 "Package_TO_SOT_SMD:SOT-23" H 9000 2075 50  0001 L CIN
F 3 "https://www.fairchildsemi.com/datasheets/2N/2N7002.pdf" H 8800 2150 50  0001 L CNN
	1    8800 2150
	1    0    0    -1  
$EndComp
$Comp
L Transistor_FET:2N7002 Q3
U 1 1 5FE04D0B
P 10350 2150
F 0 "Q3" H 10554 2196 50  0000 L CNN
F 1 "2N7002" H 10554 2105 50  0000 L CNN
F 2 "Package_TO_SOT_SMD:SOT-23" H 10550 2075 50  0001 L CIN
F 3 "https://www.fairchildsemi.com/datasheets/2N/2N7002.pdf" H 10350 2150 50  0001 L CNN
	1    10350 2150
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0121
U 1 1 5FE094F3
P 8900 2750
F 0 "#PWR0121" H 8900 2500 50  0001 C CNN
F 1 "GND" H 8905 2577 50  0000 C CNN
F 2 "" H 8900 2750 50  0001 C CNN
F 3 "" H 8900 2750 50  0001 C CNN
	1    8900 2750
	1    0    0    -1  
$EndComp
Text Notes 7700 900  0    98   ~ 0
Direct 8P8C Reader LEDs
Wire Wire Line
	7250 2350 7250 2600
Wire Wire Line
	10450 2600 10450 2350
Wire Wire Line
	2100 2750 2100 2900
Wire Wire Line
	2100 2200 2100 2350
Wire Wire Line
	2100 1600 2100 1900
$Comp
L Transistor_FET:2N7002 Q?
U 1 1 5FDB67DA
P 2000 2550
AR Path="/5FDB67DA" Ref="Q?"  Part="1" 
AR Path="/5FDAF572/5FDB67DA" Ref="Q4"  Part="1" 
F 0 "Q4" H 2204 2596 50  0000 L CNN
F 1 "2N7002" H 2204 2505 50  0000 L CNN
F 2 "Package_TO_SOT_SMD:SOT-23" H 2200 2475 50  0001 L CIN
F 3 "https://www.fairchildsemi.com/datasheets/2N/2N7002.pdf" H 2000 2550 50  0001 L CNN
	1    2000 2550
	1    0    0    -1  
$EndComp
$Comp
L Device:R R?
U 1 1 5FDB67BE
P 2100 2050
AR Path="/5FDB67BE" Ref="R?"  Part="1" 
AR Path="/5FDAF572/5FDB67BE" Ref="R3"  Part="1" 
F 0 "R3" H 2170 2096 50  0000 L CNN
F 1 "270R" H 2170 2005 50  0000 L CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 2030 2050 50  0001 C CNN
F 3 "~" H 2100 2050 50  0001 C CNN
	1    2100 2050
	1    0    0    -1  
$EndComp
Wire Wire Line
	3750 2550 3200 2550
Wire Wire Line
	3850 1600 2100 1600
Wire Wire Line
	4050 2900 3750 2900
Wire Wire Line
	4250 1650 5800 1650
Text Label 4850 2550 0    49   ~ 0
LED_B_Trigger
Text Label 3200 2550 0    49   ~ 0
LED_G_Trigger
Text Label 1250 2550 0    49   ~ 0
LED_R_Trigger
Wire Wire Line
	1800 2550 1750 2550
Wire Wire Line
	5500 2550 5450 2550
Wire Wire Line
	10450 1950 10450 1850
Wire Wire Line
	8900 1950 8900 1850
Wire Wire Line
	7250 1950 7250 1850
Text Label 6400 2150 0    49   ~ 0
LED_R_Trigger
Text Label 8050 2150 0    49   ~ 0
LED_G_Trigger
Text Label 9600 2150 0    49   ~ 0
LED_B_Trigger
Wire Wire Line
	8900 2350 8900 2600
Wire Wire Line
	10450 2600 8900 2600
Connection ~ 8900 2600
Wire Wire Line
	8900 2600 8900 2750
Wire Wire Line
	8900 2600 7250 2600
Wire Wire Line
	10150 2150 9600 2150
Wire Wire Line
	8600 2150 8050 2150
Wire Wire Line
	6950 2150 6400 2150
$Comp
L Device:R R6
U 1 1 5FF0DB90
P 1750 2750
F 0 "R6" H 1820 2796 50  0000 L CNN
F 1 "2K2" H 1820 2705 50  0000 L CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 1680 2750 50  0001 C CNN
F 3 "~" H 1750 2750 50  0001 C CNN
	1    1750 2750
	1    0    0    -1  
$EndComp
Wire Wire Line
	2100 2900 1750 2900
Connection ~ 2100 2900
Wire Wire Line
	1750 2600 1750 2550
Connection ~ 1750 2550
Wire Wire Line
	1750 2550 1250 2550
$Comp
L Device:R R7
U 1 1 5FF11CB4
P 3750 2750
F 0 "R7" H 3820 2796 50  0000 L CNN
F 1 "2K2" H 3820 2705 50  0000 L CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 3680 2750 50  0001 C CNN
F 3 "~" H 3750 2750 50  0001 C CNN
	1    3750 2750
	1    0    0    -1  
$EndComp
Connection ~ 3750 2900
Wire Wire Line
	3750 2600 3750 2550
Connection ~ 3750 2550
$Comp
L Device:R R8
U 1 1 5FF13B9B
P 5450 2750
F 0 "R8" H 5520 2796 50  0000 L CNN
F 1 "2K2" H 5520 2705 50  0000 L CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 5380 2750 50  0001 C CNN
F 3 "~" H 5450 2750 50  0001 C CNN
	1    5450 2750
	1    0    0    -1  
$EndComp
Wire Wire Line
	5450 2900 5800 2900
Wire Wire Line
	5450 2600 5450 2550
Connection ~ 5450 2550
Wire Wire Line
	5450 2550 4850 2550
Text Notes 1650 3150 0    49   ~ 0
Pulldown resistors lower than my usual 10K to \ndischarge gate capacitance quickly
Wire Wire Line
	2100 2900 3750 2900
Wire Wire Line
	5450 2900 4050 2900
Connection ~ 5450 2900
Wire Notes Line
	6500 500  6500 7800
Text Notes 4300 1450 0    49   ~ 0
Could also consider 830-5079 (note: different pinout!)
Text Notes 2150 1850 0    49   ~ 0
Resistors chosen for about 5mA forward current.\nMay need to adjust
Text HLabel 1250 2550 0    49   Input ~ 0
LED_R_Trigger
Text HLabel 3200 2550 0    49   Input ~ 0
LED_G_Trigger
Text HLabel 4850 2550 0    49   Input ~ 0
LED_B_Trigger
Text HLabel 7250 1850 1    49   Input ~ 0
LED_Red_Cathode
Text HLabel 8900 1850 1    49   Input ~ 0
LED_Green_Cathode
Text HLabel 10450 1850 1    49   Input ~ 0
LED_Blue_Cathode
$EndSCHEMATC
