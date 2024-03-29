EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 5
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
L power:+3.3V #PWR0101
U 1 1 5FC7659D
P 1150 1900
F 0 "#PWR0101" H 1150 1750 50  0001 C CNN
F 1 "+3.3V" H 1165 2073 50  0000 C CNN
F 2 "" H 1150 1900 50  0001 C CNN
F 3 "" H 1150 1900 50  0001 C CNN
	1    1150 1900
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR0102
U 1 1 5FC79D51
P 2800 1900
F 0 "#PWR0102" H 2800 1750 50  0001 C CNN
F 1 "+5V" H 2815 2073 50  0000 C CNN
F 2 "" H 2800 1900 50  0001 C CNN
F 3 "" H 2800 1900 50  0001 C CNN
	1    2800 1900
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0103
U 1 1 5FC7BA4E
P 2900 2350
F 0 "#PWR0103" H 2900 2100 50  0001 C CNN
F 1 "GND" H 2905 2177 50  0000 C CNN
F 2 "" H 2900 2350 50  0001 C CNN
F 3 "" H 2900 2350 50  0001 C CNN
	1    2900 2350
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0104
U 1 1 5FC8D863
P 10900 1200
F 0 "#PWR0104" H 10900 950 50  0001 C CNN
F 1 "GND" H 10905 1027 50  0000 C CNN
F 2 "" H 10900 1200 50  0001 C CNN
F 3 "" H 10900 1200 50  0001 C CNN
	1    10900 1200
	1    0    0    -1  
$EndComp
Wire Notes Line
	3250 7800 3250 450 
Text Notes 550  800  0    157  ~ 0
Launchpad Left Side
Text Notes 8400 800  0    157  ~ 0
Launchpad Right Side
$Comp
L power:+5V #PWR0106
U 1 1 5FCFE7C2
P 4650 6600
F 0 "#PWR0106" H 4650 6450 50  0001 C CNN
F 1 "+5V" H 4665 6773 50  0000 C CNN
F 2 "" H 4650 6600 50  0001 C CNN
F 3 "" H 4650 6600 50  0001 C CNN
	1    4650 6600
	1    0    0    -1  
$EndComp
Wire Wire Line
	4650 6900 4650 6600
$Comp
L power:GND #PWR0107
U 1 1 5FCFFCB7
P 4550 6550
F 0 "#PWR0107" H 4550 6300 50  0001 C CNN
F 1 "GND" H 4555 6377 50  0000 C CNN
F 2 "" H 4550 6550 50  0001 C CNN
F 3 "" H 4550 6550 50  0001 C CNN
	1    4550 6550
	-1   0    0    1   
$EndComp
Wire Wire Line
	4550 6550 4550 6900
Text Label 4450 6850 1    49   ~ 0
Reader_SerialRx
Wire Wire Line
	4450 6900 4450 6300
Wire Wire Line
	4350 6900 4350 6300
Text Label 4350 6850 1    49   ~ 0
LED_Red_Cathode
Text Label 4250 6850 1    49   ~ 0
BigButton
Wire Wire Line
	4250 6900 4250 6300
Wire Wire Line
	4150 6900 4150 6300
Text Label 4150 6850 1    49   ~ 0
Reader_SerialTx
Text Label 4050 6850 1    49   ~ 0
LED_Green_Cathode
Wire Wire Line
	4050 6900 4050 6300
Wire Wire Line
	3950 6900 3950 6300
Text Label 3950 6850 1    49   ~ 0
LED_Blue_Cathode
Text Notes 3400 6050 0    79   ~ 0
Direct 8P8C Connector
Text Notes 3300 4050 0    79   ~ 0
SPI Flash
Wire Notes Line
	5500 3900 3250 3900
$Comp
L ExtraParts:ISL8489EIBZ U1
U 1 1 5FC1CEB5
P 4700 2000
F 0 "U1" H 4900 2650 50  0000 C CNN
F 1 "ISL3176EIBZ" H 4450 1350 50  0000 C CNN
F 2 "Package_SO:SOIC-14_3.9x8.7mm_P1.27mm" H 4700 2000 50  0001 C CIN
F 3 "" H 4700 2000 50  0001 C CNN
	1    4700 2000
	1    0    0    -1  
$EndComp
Text Label 3300 1700 0    50   ~ 0
Reader_SerialRx
Wire Wire Line
	4200 1700 3300 1700
Text Label 3300 2200 0    50   ~ 0
Reader_SerialTx
Wire Wire Line
	4200 2200 3300 2200
$Comp
L power:GND #PWR0111
U 1 1 5FC26C85
P 4900 2950
F 0 "#PWR0111" H 4900 2700 50  0001 C CNN
F 1 "GND" H 4905 2777 50  0000 C CNN
F 2 "" H 4900 2950 50  0001 C CNN
F 3 "" H 4900 2950 50  0001 C CNN
	1    4900 2950
	1    0    0    -1  
$EndComp
Wire Wire Line
	4900 2700 4900 2850
Wire Wire Line
	4800 2700 4800 2850
Wire Wire Line
	4800 2850 4900 2850
Connection ~ 4900 2850
Wire Wire Line
	4900 2850 4900 2950
$Comp
L power:GND #PWR0112
U 1 1 5FC29A42
P 3950 2000
F 0 "#PWR0112" H 3950 1750 50  0001 C CNN
F 1 "GND" V 3955 1872 50  0000 R CNN
F 2 "" H 3950 2000 50  0001 C CNN
F 3 "" H 3950 2000 50  0001 C CNN
	1    3950 2000
	0    -1   -1   0   
$EndComp
$Comp
L power:+3.3V #PWR0113
U 1 1 5FC2AADF
P 4600 1150
F 0 "#PWR0113" H 4600 1000 50  0001 C CNN
F 1 "+3.3V" H 4615 1323 50  0000 C CNN
F 2 "" H 4600 1150 50  0001 C CNN
F 3 "" H 4600 1150 50  0001 C CNN
	1    4600 1150
	1    0    0    -1  
$EndComp
$Comp
L power:+3.3V #PWR0114
U 1 1 5FC2BB9F
P 3900 2400
F 0 "#PWR0114" H 3900 2250 50  0001 C CNN
F 1 "+3.3V" V 3915 2528 50  0000 L CNN
F 2 "" H 3900 2400 50  0001 C CNN
F 3 "" H 3900 2400 50  0001 C CNN
	1    3900 2400
	0    -1   -1   0   
$EndComp
Wire Wire Line
	4200 2400 3900 2400
Wire Wire Line
	4600 1300 4600 1200
$Comp
L Device:C C1
U 1 1 5FC3626B
P 4300 1200
F 0 "C1" V 4048 1200 50  0000 C CNN
F 1 "100nF" V 4139 1200 50  0000 C CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 4338 1050 50  0001 C CNN
F 3 "~" H 4300 1200 50  0001 C CNN
	1    4300 1200
	0    1    1    0   
$EndComp
Wire Wire Line
	4450 1200 4600 1200
Connection ~ 4600 1200
Wire Wire Line
	4600 1200 4600 1150
$Comp
L power:GND #PWR0115
U 1 1 5FC3C950
P 3900 1200
F 0 "#PWR0115" H 3900 950 50  0001 C CNN
F 1 "GND" V 3905 1072 50  0000 R CNN
F 2 "" H 3900 1200 50  0001 C CNN
F 3 "" H 3900 1200 50  0001 C CNN
	1    3900 1200
	0    1    1    0   
$EndComp
Wire Wire Line
	4150 1200 3900 1200
Wire Wire Line
	5200 2100 5700 2100
Text Label 5300 1600 0    50   ~ 0
422_RX+
Text Label 5300 1800 0    50   ~ 0
422_RX-
Text Label 5300 2100 0    50   ~ 0
422_TX+
Text Label 5300 2300 0    50   ~ 0
422_TX-
Wire Wire Line
	6650 1650 6250 1650
Wire Wire Line
	6650 1550 6250 1550
Text Label 6300 1650 0    50   ~ 0
422_TX+
Text Label 6300 1550 0    50   ~ 0
422_TX-
Text Label 6300 1450 0    50   ~ 0
422_RX+
Text Label 6300 1150 0    50   ~ 0
422_RX-
Wire Wire Line
	5200 2300 5700 2300
Wire Wire Line
	5200 1600 5700 1600
Wire Wire Line
	5200 1800 5700 1800
$Comp
L Device:R R1
U 1 1 5FC6A05D
P 5950 1300
F 0 "R1" H 6020 1346 50  0000 L CNN
F 1 "100R" H 6020 1255 50  0000 L CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 5880 1300 50  0001 C CNN
F 3 "~" H 5950 1300 50  0001 C CNN
	1    5950 1300
	1    0    0    -1  
$EndComp
Wire Wire Line
	5950 1150 6650 1150
Wire Wire Line
	5950 1450 6650 1450
$Comp
L power:GND #PWR0116
U 1 1 5FC6E8DC
P 6300 950
F 0 "#PWR0116" H 6300 700 50  0001 C CNN
F 1 "GND" V 6305 822 50  0000 R CNN
F 2 "" H 6300 950 50  0001 C CNN
F 3 "" H 6300 950 50  0001 C CNN
	1    6300 950 
	0    1    1    0   
$EndComp
Wire Wire Line
	6650 950  6450 950 
Wire Wire Line
	6650 1050 6450 1050
Wire Wire Line
	6450 1050 6450 950 
Connection ~ 6450 950 
Wire Wire Line
	6450 950  6300 950 
$Comp
L power:+12V #PWR0117
U 1 1 5FC73567
P 6550 1250
F 0 "#PWR0117" H 6550 1100 50  0001 C CNN
F 1 "+12V" V 6565 1378 50  0000 L CNN
F 2 "" H 6550 1250 50  0001 C CNN
F 3 "" H 6550 1250 50  0001 C CNN
	1    6550 1250
	0    -1   -1   0   
$EndComp
Wire Wire Line
	6550 1250 6600 1250
Wire Wire Line
	6650 1350 6600 1350
Wire Wire Line
	6600 1350 6600 1250
Connection ~ 6600 1250
Wire Wire Line
	6600 1250 6650 1250
Text Notes 3350 750  0    118  ~ 0
New-Style Reader Connection
Wire Notes Line
	5500 2450 5850 2450
Wire Notes Line
	5850 2450 5850 1800
Wire Notes Line
	5500 2450 5500 7800
Text Notes 5900 2100 0    98   ~ 0
Other Sheets
$Sheet
S 6400 2550 1600 300 
U 5FDAF572
F0 "LED_Switching" 79
F1 "LED_Switching.sch" 79
F2 "LED_R_Trigger" I R 8000 2600 49 
F3 "LED_G_Trigger" I R 8000 2700 49 
F4 "LED_B_Trigger" I R 8000 2800 49 
F5 "LED_Red_Cathode" I L 6400 2600 49 
F6 "LED_Green_Cathode" I L 6400 2700 49 
F7 "LED_Blue_Cathode" I L 6400 2800 49 
$EndSheet
Text Label 8500 5450 0    49   ~ 0
LED_R_Trigger
Text Label 8500 5550 0    49   ~ 0
LED_G_Trigger
Text Label 8500 5650 0    49   ~ 0
LED_B_Trigger
$Sheet
S 6400 3150 1550 350 
U 5FFE9ADC
F0 "Power" 79
F1 "Power.sch" 79
F2 "ToolEnable" I R 7950 3250 49 
F3 "~ToolOn" I R 7950 3400 49 
F4 "~DoorRelease" I L 6400 3350 50 
$EndSheet
$Sheet
S 6400 3950 1500 350 
U 600A520B
F0 "UserIO" 79
F1 "UserIO.sch" 79
$EndSheet
Text GLabel 8750 1200 0    49   Input ~ 0
BigButton
Text Label 8600 5150 0    49   ~ 0
ToolEnable
Text Label 8050 2600 0    49   ~ 0
LED_R_Trigger
Text Label 8050 2700 0    49   ~ 0
LED_G_Trigger
Text Label 8050 2800 0    49   ~ 0
LED_B_Trigger
Wire Wire Line
	8000 2600 8550 2600
Wire Wire Line
	8000 2700 8550 2700
Wire Wire Line
	8000 2800 8550 2800
Text Label 5600 2600 0    49   ~ 0
LED_Red_Cathode
Text Label 5600 2700 0    49   ~ 0
LED_Green_Cathode
Text Label 5600 2800 0    49   ~ 0
LED_Blue_Cathode
Wire Notes Line
	8300 500  8300 1800
Wire Notes Line
	8700 1800 8700 4950
Wire Notes Line
	5500 4950 8700 4950
Wire Notes Line
	5850 1800 8700 1800
Wire Wire Line
	6400 2600 5600 2600
Wire Wire Line
	5600 2700 6400 2700
Wire Wire Line
	6400 2800 5600 2800
Text Label 8400 3250 2    49   ~ 0
ToolEnable
Text Label 8400 3400 2    49   ~ 0
~ToolOn
Wire Wire Line
	7950 3250 8400 3250
Wire Wire Line
	8400 3400 7950 3400
Text GLabel 6150 4600 0    49   Input ~ 0
I2C_SCL
Text GLabel 6150 4750 0    49   Input ~ 0
I2C_SDA
Text Label 6550 4600 2    49   ~ 0
I2C_SCL
Text Label 6550 4750 2    49   ~ 0
I2C_SDA
Wire Wire Line
	6550 4600 6150 4600
Wire Wire Line
	6150 4750 6550 4750
Text GLabel 10950 1600 2    49   Input ~ 0
~Reset
Text Label 1100 2300 0    49   ~ 0
~ToolOn
$Comp
L Connector:8P8C_Shielded J1
U 1 1 603255E5
P 7050 1250
F 0 "J1" H 6720 1197 50  0000 R CNN
F 1 "8P8C_Shielded" H 6720 1288 50  0000 R CNN
F 2 "Connector_RJ:RJ45_Ninigi_GE" V 7050 1275 50  0001 C CNN
F 3 "~" V 7050 1275 50  0001 C CNN
	1    7050 1250
	-1   0    0    1   
$EndComp
$Comp
L Connector:8P8C_Shielded J2
U 1 1 6032E448
P 4350 7300
F 0 "J2" V 4453 7830 50  0000 L CNN
F 1 "8P8C_Shielded" V 4362 7830 50  0000 L CNN
F 2 "Connector_RJ:RJ45_Ninigi_GE" V 4350 7325 50  0001 C CNN
F 3 "~" V 4350 7325 50  0001 C CNN
	1    4350 7300
	0    -1   -1   0   
$EndComp
Wire Wire Line
	4850 7300 4950 7300
Wire Wire Line
	4950 7300 4950 7400
Wire Wire Line
	7250 700  7050 700 
Wire Wire Line
	7050 700  7050 750 
$Comp
L power:GND #PWR0108
U 1 1 6036D39A
P 7250 700
F 0 "#PWR0108" H 7250 450 50  0001 C CNN
F 1 "GND" H 7255 527 50  0000 C CNN
F 2 "" H 7250 700 50  0001 C CNN
F 3 "" H 7250 700 50  0001 C CNN
	1    7250 700 
	-1   0    0    1   
$EndComp
$Comp
L power:GND #PWR0109
U 1 1 6037908D
P 4950 7400
F 0 "#PWR0109" H 4950 7150 50  0001 C CNN
F 1 "GND" H 4955 7227 50  0000 C CNN
F 2 "" H 4950 7400 50  0001 C CNN
F 3 "" H 4950 7400 50  0001 C CNN
	1    4950 7400
	1    0    0    -1  
$EndComp
$Sheet
S 3400 4200 750  550 
U 603F3E51
F0 "SPIFlash" 49
F1 "SPIFlash.sch" 49
F2 "SPIFlash_CS" I R 4150 4300 49 
F3 "SPI_Clk" I R 4150 4450 49 
F4 "SPI_MOSI" I R 4150 4550 49 
F5 "SPI_MISO" I R 4150 4650 49 
$EndSheet
Wire Notes Line
	3250 5850 5500 5850
Text Label 4700 4300 2    49   ~ 0
SPIFlash_CS
Text Label 4700 4450 2    49   ~ 0
SPI_Clk
Text Label 4700 4550 2    49   ~ 0
SPI_MOSI
Text Label 4700 4650 2    49   ~ 0
SPI_MISO
Wire Wire Line
	4150 4300 4700 4300
Wire Wire Line
	4700 4450 4150 4450
Wire Wire Line
	4150 4550 4700 4550
Wire Wire Line
	4700 4650 4150 4650
$Comp
L Launchpad:EK-TM4C1294XL U2
U 1 1 5FE987ED
P 1950 2650
F 0 "U2" H 1950 3365 50  0000 C CNN
F 1 "EK-TM4C1294XL" H 1950 3274 50  0000 C CNN
F 2 "Launchpad:Launchpad" H 1950 2800 50  0001 C CNN
F 3 "" H 1950 2800 50  0001 C CNN
	1    1950 2650
	1    0    0    -1  
$EndComp
$Comp
L Launchpad:EK-TM4C1294XL U2
U 2 1 5FE9B2C5
P 1850 5300
F 0 "U2" H 1850 6015 50  0000 C CNN
F 1 "EK-TM4C1294XL" H 1850 5924 50  0000 C CNN
F 2 "Launchpad:Launchpad" H 1850 5450 50  0001 C CNN
F 3 "" H 1850 5450 50  0001 C CNN
	2    1850 5300
	1    0    0    -1  
$EndComp
$Comp
L Launchpad:EK-TM4C1294XL U2
U 3 1 5FE9D47C
P 9650 1700
F 0 "U2" H 9675 2425 50  0000 C CNN
F 1 "EK-TM4C1294XL" H 9675 2334 50  0000 C CNN
F 2 "Launchpad:Launchpad" H 9650 1850 50  0001 C CNN
F 3 "" H 9650 1850 50  0001 C CNN
	3    9650 1700
	1    0    0    -1  
$EndComp
$Comp
L Launchpad:EK-TM4C1294XL U2
U 4 1 5FE9F996
P 9800 5750
F 0 "U2" H 9725 6575 50  0000 C CNN
F 1 "EK-TM4C1294XL" H 9725 6484 50  0000 C CNN
F 2 "Launchpad:Launchpad" H 9800 5900 50  0001 C CNN
F 3 "" H 9800 5900 50  0001 C CNN
	4    9800 5750
	1    0    0    -1  
$EndComp
Text Label 10500 1600 0    50   ~ 0
~Reset
Wire Wire Line
	10900 1200 10400 1200
Wire Wire Line
	2800 1900 2800 2200
Wire Wire Line
	2800 2200 2500 2200
Wire Wire Line
	1400 2200 1150 2200
Wire Wire Line
	1150 2200 1150 1900
Wire Wire Line
	2500 2300 2900 2300
Wire Wire Line
	2900 2300 2900 2350
$Comp
L power:+3.3V #PWR06
U 1 1 5FEB5CFE
P 900 4650
F 0 "#PWR06" H 900 4500 50  0001 C CNN
F 1 "+3.3V" H 915 4823 50  0000 C CNN
F 2 "" H 900 4650 50  0001 C CNN
F 3 "" H 900 4650 50  0001 C CNN
	1    900  4650
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR07
U 1 1 5FEB6424
P 2700 4650
F 0 "#PWR07" H 2700 4500 50  0001 C CNN
F 1 "+5V" H 2715 4823 50  0000 C CNN
F 2 "" H 2700 4650 50  0001 C CNN
F 3 "" H 2700 4650 50  0001 C CNN
	1    2700 4650
	1    0    0    -1  
$EndComp
Wire Wire Line
	2450 4850 2700 4850
Wire Wire Line
	2700 4850 2700 4650
Wire Wire Line
	1250 4850 900  4850
Wire Wire Line
	900  4850 900  4650
$Comp
L power:GND #PWR08
U 1 1 5FEBB845
P 2850 5000
F 0 "#PWR08" H 2850 4750 50  0001 C CNN
F 1 "GND" H 2855 4827 50  0000 C CNN
F 2 "" H 2850 5000 50  0001 C CNN
F 3 "" H 2850 5000 50  0001 C CNN
	1    2850 5000
	1    0    0    -1  
$EndComp
Wire Wire Line
	2850 5000 2850 4950
Wire Wire Line
	2850 4950 2450 4950
Text Label 600  5050 0    50   ~ 0
Reader_SerialRx
Text Label 600  5150 0    50   ~ 0
Reader_SerialTx
Wire Wire Line
	1250 5050 600  5050
Wire Wire Line
	600  5150 1250 5150
Wire Wire Line
	8500 5450 9050 5450
Wire Wire Line
	8750 1200 8950 1200
Wire Wire Line
	9050 5550 8500 5550
Wire Wire Line
	8500 5650 9050 5650
Wire Wire Line
	9050 5150 8600 5150
Wire Wire Line
	1400 2300 1100 2300
Text Label 1000 3000 0    50   ~ 0
I2C_SCL
Text Label 1000 3100 0    50   ~ 0
I2C_SDA
Wire Wire Line
	1400 3000 1000 3000
Wire Wire Line
	1000 3100 1400 3100
Text Label 10600 1800 0    50   ~ 0
SPI_MISO
Text Label 10600 1700 0    50   ~ 0
SPI_MOSI
Wire Wire Line
	10950 1600 10400 1600
Wire Wire Line
	10600 1700 10400 1700
Wire Wire Line
	10400 1800 10600 1800
Text Label 1000 2800 0    50   ~ 0
SPI_Clk
Wire Wire Line
	1400 2800 1000 2800
Text Label 2600 2800 0    50   ~ 0
SPIFlash_CS
Wire Wire Line
	2600 2800 2500 2800
Text Label 5750 3350 0    61   ~ 0
~DoorRelease
Wire Wire Line
	6400 3350 5750 3350
Text Label 10500 6050 0    61   ~ 0
~DoorRelease
Wire Wire Line
	10400 6050 10500 6050
$Comp
L Connector_Generic:Conn_01x03 J8
U 1 1 6111755B
P 3650 1900
F 0 "J8" V 3750 1900 50  0000 C CNN
F 1 "Conn_01x03" H 3600 2100 50  0001 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x03_P2.54mm_Vertical" H 3650 1900 50  0001 C CNN
F 3 "~" H 3650 1900 50  0001 C CNN
	1    3650 1900
	-1   0    0    1   
$EndComp
$Comp
L power:+3.3V #PWR0105
U 1 1 6112247B
P 3900 1800
F 0 "#PWR0105" H 3900 1650 50  0001 C CNN
F 1 "+3.3V" V 3900 2050 50  0000 C CNN
F 2 "" H 3900 1800 50  0001 C CNN
F 3 "" H 3900 1800 50  0001 C CNN
	1    3900 1800
	0    1    1    0   
$EndComp
Wire Wire Line
	4200 1900 3850 1900
Wire Wire Line
	3850 2000 3950 2000
Wire Wire Line
	3900 1800 3850 1800
$EndSCHEMATC
