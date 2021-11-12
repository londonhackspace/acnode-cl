EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 4 5
Title "ACNode Launchpad Overlay"
Date "2020-12-27"
Rev "1"
Comp "London Hackspace"
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
Text Notes 700  700  0    98   ~ 0
Display
$Comp
L Connector:Conn_01x04_Female J6
U 1 1 600C1DD9
P 1150 1150
F 0 "J6" V 1088 862 50  0000 R CNN
F 1 "Conn_01x04_Female" V 997 862 50  0000 R CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x04_P2.54mm_Vertical" H 1150 1150 50  0001 C CNN
F 3 "~" H 1150 1150 50  0001 C CNN
	1    1150 1150
	0    -1   -1   0   
$EndComp
$Comp
L power:GND #PWR03
U 1 1 600C30F8
P 1150 1850
F 0 "#PWR03" H 1150 1600 50  0001 C CNN
F 1 "GND" H 1155 1677 50  0000 C CNN
F 2 "" H 1150 1850 50  0001 C CNN
F 3 "" H 1150 1850 50  0001 C CNN
	1    1150 1850
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR02
U 1 1 600C3553
P 800 1350
F 0 "#PWR02" H 800 1200 50  0001 C CNN
F 1 "+5V" H 815 1523 50  0000 C CNN
F 2 "" H 800 1350 50  0001 C CNN
F 3 "" H 800 1350 50  0001 C CNN
	1    800  1350
	1    0    0    -1  
$EndComp
Wire Wire Line
	1150 1350 1150 1850
Wire Wire Line
	800  1350 800  1500
Wire Wire Line
	800  1500 1050 1500
Wire Wire Line
	1050 1500 1050 1350
Text Notes 750  1100 0    59   ~ 0
0.96" OLED I2C Display\nDisplays system status, making maintainence\nand debugging easier
Wire Notes Line
	3100 2150 3100 500 
Wire Notes Line
	450  2150 11200 2150
Text Notes 3250 700  0    98   ~ 0
Buttons
$Comp
L Switch:SW_Push SW1
U 1 1 600DF6C9
P 3700 1450
F 0 "SW1" V 3746 1402 50  0000 R CNN
F 1 "SW_Push" V 3655 1402 50  0000 R CNN
F 2 "Button_Switch_THT:SW_PUSH_6mm" H 3700 1650 50  0001 C CNN
F 3 "~" H 3700 1650 50  0001 C CNN
	1    3700 1450
	0    -1   -1   0   
$EndComp
$Comp
L power:GND #PWR04
U 1 1 600E013C
P 3700 1900
F 0 "#PWR04" H 3700 1650 50  0001 C CNN
F 1 "GND" H 3705 1727 50  0000 C CNN
F 2 "" H 3700 1900 50  0001 C CNN
F 3 "" H 3700 1900 50  0001 C CNN
	1    3700 1900
	1    0    0    -1  
$EndComp
Wire Wire Line
	3700 1900 3700 1650
Wire Wire Line
	3700 1250 3700 900 
Text Notes 3550 1700 1    59   ~ 0
Reset Button
$Comp
L Switch:SW_Push SW2
U 1 1 600EC78F
P 4600 1450
F 0 "SW2" V 4646 1402 50  0000 R CNN
F 1 "SW_Push" V 4555 1402 50  0000 R CNN
F 2 "Button_Switch_THT:SW_PUSH_6mm" H 4600 1650 50  0001 C CNN
F 3 "~" H 4600 1650 50  0001 C CNN
	1    4600 1450
	0    -1   -1   0   
$EndComp
$Comp
L power:GND #PWR05
U 1 1 600EEBA2
P 4600 1900
F 0 "#PWR05" H 4600 1650 50  0001 C CNN
F 1 "GND" H 4605 1727 50  0000 C CNN
F 2 "" H 4600 1900 50  0001 C CNN
F 3 "" H 4600 1900 50  0001 C CNN
	1    4600 1900
	1    0    0    -1  
$EndComp
Wire Wire Line
	4600 1900 4600 1650
Wire Wire Line
	4600 1250 4600 750 
Text Notes 4450 1900 1    59   ~ 0
The Button\n(Doorbell/Reader Button)
Text GLabel 1350 1900 2    49   Input ~ 0
I2C_SCL
Text GLabel 1400 1750 2    49   Input ~ 0
I2C_SDA
Wire Wire Line
	1350 1750 1400 1750
Wire Wire Line
	1350 1350 1350 1750
Wire Wire Line
	1250 1900 1350 1900
Wire Wire Line
	1250 1350 1250 1900
Text GLabel 3700 900  2    49   Input ~ 0
~Reset
Text GLabel 4700 750  2    49   Input ~ 0
BigButton
Wire Wire Line
	4700 750  4600 750 
$EndSCHEMATC
