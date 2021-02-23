EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 5 5
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
L Memory_Flash:W25Q32JVSS U3
U 1 1 603F7364
P 2450 2200
F 0 "U3" H 2200 2550 50  0000 C CNN
F 1 "W25Q32JVSS" H 2100 1800 50  0000 C CNN
F 2 "Package_SO:SOIC-8_5.23x5.23mm_P1.27mm" H 2450 2200 50  0001 C CNN
F 3 "http://www.winbond.com/resource-files/w25q32jv%20revg%2003272018%20plus.pdf" H 2450 2200 50  0001 C CNN
	1    2450 2200
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0110
U 1 1 603F736A
P 2450 2750
F 0 "#PWR0110" H 2450 2500 50  0001 C CNN
F 1 "GND" H 2455 2577 50  0000 C CNN
F 2 "" H 2450 2750 50  0001 C CNN
F 3 "" H 2450 2750 50  0001 C CNN
	1    2450 2750
	1    0    0    -1  
$EndComp
Wire Wire Line
	2450 1800 2450 1600
Wire Wire Line
	2450 2750 2450 2600
$Comp
L Device:C C2
U 1 1 603F7372
P 2700 1600
F 0 "C2" V 2550 1600 50  0000 C CNN
F 1 "100nF" V 2850 1600 50  0000 C CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 2738 1450 50  0001 C CNN
F 3 "~" H 2700 1600 50  0001 C CNN
	1    2700 1600
	0    1    1    0   
$EndComp
Wire Wire Line
	2550 1600 2450 1600
Connection ~ 2450 1600
Wire Wire Line
	2450 1600 2450 1550
Wire Wire Line
	2850 1600 2950 1600
$Comp
L power:+3.3V #PWR0128
U 1 1 603F737C
P 2450 1550
F 0 "#PWR0128" H 2450 1400 50  0001 C CNN
F 1 "+3.3V" H 2465 1723 50  0000 C CNN
F 2 "" H 2450 1550 50  0001 C CNN
F 3 "" H 2450 1550 50  0001 C CNN
	1    2450 1550
	1    0    0    -1  
$EndComp
Text Label 1350 2100 0    59   ~ 0
SPIFlash_CS
Wire Wire Line
	1950 2100 1900 2100
Wire Wire Line
	1950 2300 1350 2300
Text Label 1350 2300 0    59   ~ 0
SPI_Clk
Text Label 3000 2000 0    59   ~ 0
SPI_MOSI
Text Label 3000 2100 0    59   ~ 0
SPI_MISO
Wire Wire Line
	2950 1650 2950 1600
Wire Wire Line
	2950 2300 3450 2300
Wire Wire Line
	3450 2300 3450 1800
Wire Wire Line
	3450 1800 2450 1800
Connection ~ 2450 1800
Wire Wire Line
	2950 2400 3450 2400
Wire Wire Line
	3450 2400 3450 2300
Connection ~ 3450 2300
$Comp
L Device:R R2
U 1 1 603F7393
P 1900 1850
F 0 "R2" H 1750 1900 50  0000 L CNN
F 1 "10k" H 1700 1800 50  0000 L CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 1830 1850 50  0001 C CNN
F 3 "~" H 1900 1850 50  0001 C CNN
	1    1900 1850
	1    0    0    -1  
$EndComp
Wire Wire Line
	1900 2100 1900 2000
Connection ~ 1900 2100
Wire Wire Line
	1900 2100 1350 2100
Wire Wire Line
	1900 1700 1900 1600
Wire Wire Line
	1900 1600 2450 1600
Text Notes 1400 1300 0    49   ~ 0
Most SOIC-8 SPI Flash should fit
$Comp
L power:GND #PWR0129
U 1 1 603F739F
P 2950 1650
F 0 "#PWR0129" H 2950 1400 50  0001 C CNN
F 1 "GND" H 3050 1650 50  0000 C CNN
F 2 "" H 2950 1650 50  0001 C CNN
F 3 "" H 2950 1650 50  0001 C CNN
	1    2950 1650
	1    0    0    -1  
$EndComp
Text HLabel 1350 2100 0    49   Input ~ 0
SPIFlash_CS
Text HLabel 1350 2300 0    49   Input ~ 0
SPI_Clk
Text HLabel 3600 2000 2    49   Input ~ 0
SPI_MOSI
Text HLabel 3600 2100 2    49   Input ~ 0
SPI_MISO
Wire Wire Line
	2950 2000 3600 2000
Wire Wire Line
	2950 2100 3600 2100
Text Notes 1450 750  2    98   ~ 0
SPI Flash
$EndSCHEMATC
