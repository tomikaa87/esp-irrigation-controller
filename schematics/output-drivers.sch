EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 2 2
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L Device:R R?
U 1 1 5EF2BBE9
P 6500 2400
F 0 "R?" V 6293 2400 50  0000 C CNN
F 1 "100" V 6384 2400 50  0000 C CNN
F 2 "" V 6430 2400 50  0001 C CNN
F 3 "~" H 6500 2400 50  0001 C CNN
	1    6500 2400
	0    1    1    0   
$EndComp
$Comp
L Device:R R?
U 1 1 5EF2BBEF
P 6800 2650
F 0 "R?" V 6593 2650 50  0000 C CNN
F 1 "10k" V 6684 2650 50  0000 C CNN
F 2 "" V 6730 2650 50  0001 C CNN
F 3 "~" H 6800 2650 50  0001 C CNN
	1    6800 2650
	1    0    0    -1  
$EndComp
$Comp
L Device:Q_NMOS_DGS Q?
U 1 1 5EF2BBF5
P 7150 2400
F 0 "Q?" H 7355 2446 50  0000 L CNN
F 1 "PSMN017-30EL" H 7355 2355 50  0000 L CNN
F 2 "" H 7350 2500 50  0001 C CNN
F 3 "~" H 7150 2400 50  0001 C CNN
	1    7150 2400
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR?
U 1 1 5EF2BBFB
P 6800 2900
F 0 "#PWR?" H 6800 2650 50  0001 C CNN
F 1 "GND" H 6805 2727 50  0000 C CNN
F 2 "" H 6800 2900 50  0001 C CNN
F 3 "" H 6800 2900 50  0001 C CNN
	1    6800 2900
	1    0    0    -1  
$EndComp
Wire Wire Line
	6650 2400 6800 2400
Wire Wire Line
	6800 2500 6800 2400
Connection ~ 6800 2400
Wire Wire Line
	6800 2400 6950 2400
$Comp
L Device:LED D?
U 1 1 5EF2BC06
P 6950 2050
F 0 "D?" H 6943 1795 50  0000 C CNN
F 1 "LED" H 6943 1886 50  0000 C CNN
F 2 "" H 6950 2050 50  0001 C CNN
F 3 "~" H 6950 2050 50  0001 C CNN
	1    6950 2050
	-1   0    0    1   
$EndComp
$Comp
L Device:R R?
U 1 1 5EF2BC0C
P 6600 2050
F 0 "R?" V 6393 2050 50  0000 C CNN
F 1 "10k" V 6484 2050 50  0000 C CNN
F 2 "" V 6530 2050 50  0001 C CNN
F 3 "~" H 6600 2050 50  0001 C CNN
	1    6600 2050
	0    1    1    0   
$EndComp
$Comp
L power:+12V #PWR?
U 1 1 5EF2BC12
P 7250 1550
F 0 "#PWR?" H 7250 1400 50  0001 C CNN
F 1 "+12V" H 7265 1723 50  0000 C CNN
F 2 "" H 7250 1550 50  0001 C CNN
F 3 "" H 7250 1550 50  0001 C CNN
	1    7250 1550
	1    0    0    -1  
$EndComp
Wire Wire Line
	7250 1550 7250 1650
Wire Wire Line
	7250 2000 7250 2050
Wire Wire Line
	6400 2050 6450 2050
Wire Wire Line
	6750 2050 6800 2050
Wire Wire Line
	6800 2800 6800 2900
$Comp
L power:GND #PWR?
U 1 1 5EF2BC1F
P 7250 2900
F 0 "#PWR?" H 7250 2650 50  0001 C CNN
F 1 "GND" H 7255 2727 50  0000 C CNN
F 2 "" H 7250 2900 50  0001 C CNN
F 3 "" H 7250 2900 50  0001 C CNN
	1    7250 2900
	1    0    0    -1  
$EndComp
Wire Wire Line
	7250 2600 7250 2900
Wire Wire Line
	6400 1650 7250 1650
Connection ~ 7250 1650
Wire Wire Line
	7100 2050 7250 2050
$Comp
L Diode:1N4001 D?
U 1 1 5EF2BC29
P 7250 1850
F 0 "D?" V 7204 1929 50  0000 L CNN
F 1 "1N4001" V 7295 1929 50  0000 L CNN
F 2 "Diode_THT:D_DO-41_SOD81_P10.16mm_Horizontal" H 7250 1675 50  0001 C CNN
F 3 "http://www.vishay.com/docs/88503/1n4001.pdf" H 7250 1850 50  0001 C CNN
	1    7250 1850
	0    1    1    0   
$EndComp
Wire Wire Line
	7250 1700 7250 1650
Connection ~ 7250 2050
Wire Wire Line
	7250 2050 7250 2200
Wire Wire Line
	6400 1650 6400 2050
$Comp
L Device:R R?
U 1 1 5EF2BC33
P 8950 2400
F 0 "R?" V 8743 2400 50  0000 C CNN
F 1 "100" V 8834 2400 50  0000 C CNN
F 2 "" V 8880 2400 50  0001 C CNN
F 3 "~" H 8950 2400 50  0001 C CNN
	1    8950 2400
	0    1    1    0   
$EndComp
$Comp
L Device:R R?
U 1 1 5EF2BC39
P 9250 2650
F 0 "R?" V 9043 2650 50  0000 C CNN
F 1 "10k" V 9134 2650 50  0000 C CNN
F 2 "" V 9180 2650 50  0001 C CNN
F 3 "~" H 9250 2650 50  0001 C CNN
	1    9250 2650
	1    0    0    -1  
$EndComp
$Comp
L Device:Q_NMOS_DGS Q?
U 1 1 5EF2BC3F
P 9600 2400
F 0 "Q?" H 9805 2446 50  0000 L CNN
F 1 "PSMN017-30EL" H 9805 2355 50  0000 L CNN
F 2 "" H 9800 2500 50  0001 C CNN
F 3 "~" H 9600 2400 50  0001 C CNN
	1    9600 2400
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR?
U 1 1 5EF2BC45
P 9250 2900
F 0 "#PWR?" H 9250 2650 50  0001 C CNN
F 1 "GND" H 9255 2727 50  0000 C CNN
F 2 "" H 9250 2900 50  0001 C CNN
F 3 "" H 9250 2900 50  0001 C CNN
	1    9250 2900
	1    0    0    -1  
$EndComp
Wire Wire Line
	9100 2400 9250 2400
Wire Wire Line
	9250 2500 9250 2400
Connection ~ 9250 2400
Wire Wire Line
	9250 2400 9400 2400
$Comp
L Device:LED D?
U 1 1 5EF2BC50
P 9400 2050
F 0 "D?" H 9393 1795 50  0000 C CNN
F 1 "LED" H 9393 1886 50  0000 C CNN
F 2 "" H 9400 2050 50  0001 C CNN
F 3 "~" H 9400 2050 50  0001 C CNN
	1    9400 2050
	-1   0    0    1   
$EndComp
$Comp
L Device:R R?
U 1 1 5EF2BC56
P 9050 2050
F 0 "R?" V 8843 2050 50  0000 C CNN
F 1 "10k" V 8934 2050 50  0000 C CNN
F 2 "" V 8980 2050 50  0001 C CNN
F 3 "~" H 9050 2050 50  0001 C CNN
	1    9050 2050
	0    1    1    0   
$EndComp
$Comp
L power:+12V #PWR?
U 1 1 5EF2BC5C
P 9700 1550
F 0 "#PWR?" H 9700 1400 50  0001 C CNN
F 1 "+12V" H 9715 1723 50  0000 C CNN
F 2 "" H 9700 1550 50  0001 C CNN
F 3 "" H 9700 1550 50  0001 C CNN
	1    9700 1550
	1    0    0    -1  
$EndComp
Wire Wire Line
	9700 1550 9700 1650
Wire Wire Line
	8800 2400 8650 2400
Wire Wire Line
	9700 2000 9700 2050
Wire Wire Line
	8850 2050 8900 2050
Wire Wire Line
	9200 2050 9250 2050
Wire Wire Line
	9250 2800 9250 2900
$Comp
L power:GND #PWR?
U 1 1 5EF2BC69
P 9700 2900
F 0 "#PWR?" H 9700 2650 50  0001 C CNN
F 1 "GND" H 9705 2727 50  0000 C CNN
F 2 "" H 9700 2900 50  0001 C CNN
F 3 "" H 9700 2900 50  0001 C CNN
	1    9700 2900
	1    0    0    -1  
$EndComp
Wire Wire Line
	9700 2600 9700 2900
Wire Wire Line
	8850 1650 9700 1650
Connection ~ 9700 1650
Wire Wire Line
	9550 2050 9700 2050
$Comp
L Diode:1N4001 D?
U 1 1 5EF2BC73
P 9700 1850
F 0 "D?" V 9654 1929 50  0000 L CNN
F 1 "1N4001" V 9745 1929 50  0000 L CNN
F 2 "Diode_THT:D_DO-41_SOD81_P10.16mm_Horizontal" H 9700 1675 50  0001 C CNN
F 3 "http://www.vishay.com/docs/88503/1n4001.pdf" H 9700 1850 50  0001 C CNN
	1    9700 1850
	0    1    1    0   
$EndComp
Wire Wire Line
	9700 1700 9700 1650
Connection ~ 9700 2050
Wire Wire Line
	9700 2050 9700 2200
Wire Wire Line
	8850 1650 8850 2050
$Comp
L Device:R R?
U 1 1 5EF2BD11
P 6500 4700
F 0 "R?" V 6293 4700 50  0000 C CNN
F 1 "100" V 6384 4700 50  0000 C CNN
F 2 "" V 6430 4700 50  0001 C CNN
F 3 "~" H 6500 4700 50  0001 C CNN
	1    6500 4700
	0    1    1    0   
$EndComp
$Comp
L Device:R R?
U 1 1 5EF2BD17
P 6800 4950
F 0 "R?" V 6593 4950 50  0000 C CNN
F 1 "10k" V 6684 4950 50  0000 C CNN
F 2 "" V 6730 4950 50  0001 C CNN
F 3 "~" H 6800 4950 50  0001 C CNN
	1    6800 4950
	1    0    0    -1  
$EndComp
$Comp
L Device:Q_NMOS_DGS Q?
U 1 1 5EF2BD1D
P 7150 4700
F 0 "Q?" H 7355 4746 50  0000 L CNN
F 1 "PSMN017-30EL" H 7355 4655 50  0000 L CNN
F 2 "" H 7350 4800 50  0001 C CNN
F 3 "~" H 7150 4700 50  0001 C CNN
	1    7150 4700
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR?
U 1 1 5EF2BD23
P 6800 5200
F 0 "#PWR?" H 6800 4950 50  0001 C CNN
F 1 "GND" H 6805 5027 50  0000 C CNN
F 2 "" H 6800 5200 50  0001 C CNN
F 3 "" H 6800 5200 50  0001 C CNN
	1    6800 5200
	1    0    0    -1  
$EndComp
Wire Wire Line
	6650 4700 6800 4700
Wire Wire Line
	6800 4800 6800 4700
Connection ~ 6800 4700
Wire Wire Line
	6800 4700 6950 4700
$Comp
L Device:LED D?
U 1 1 5EF2BD2E
P 6950 4350
F 0 "D?" H 6943 4095 50  0000 C CNN
F 1 "LED" H 6943 4186 50  0000 C CNN
F 2 "" H 6950 4350 50  0001 C CNN
F 3 "~" H 6950 4350 50  0001 C CNN
	1    6950 4350
	-1   0    0    1   
$EndComp
$Comp
L Device:R R?
U 1 1 5EF2BD34
P 6600 4350
F 0 "R?" V 6393 4350 50  0000 C CNN
F 1 "10k" V 6484 4350 50  0000 C CNN
F 2 "" V 6530 4350 50  0001 C CNN
F 3 "~" H 6600 4350 50  0001 C CNN
	1    6600 4350
	0    1    1    0   
$EndComp
$Comp
L power:+12V #PWR?
U 1 1 5EF2BD3A
P 7250 3850
F 0 "#PWR?" H 7250 3700 50  0001 C CNN
F 1 "+12V" H 7265 4023 50  0000 C CNN
F 2 "" H 7250 3850 50  0001 C CNN
F 3 "" H 7250 3850 50  0001 C CNN
	1    7250 3850
	1    0    0    -1  
$EndComp
Wire Wire Line
	7250 3850 7250 3950
Wire Wire Line
	7250 4300 7250 4350
Wire Wire Line
	6400 4350 6450 4350
Wire Wire Line
	6750 4350 6800 4350
Wire Wire Line
	6800 5100 6800 5200
$Comp
L power:GND #PWR?
U 1 1 5EF2BD47
P 7250 5200
F 0 "#PWR?" H 7250 4950 50  0001 C CNN
F 1 "GND" H 7255 5027 50  0000 C CNN
F 2 "" H 7250 5200 50  0001 C CNN
F 3 "" H 7250 5200 50  0001 C CNN
	1    7250 5200
	1    0    0    -1  
$EndComp
Wire Wire Line
	7250 4900 7250 5200
Wire Wire Line
	6400 3950 7250 3950
Connection ~ 7250 3950
Wire Wire Line
	7100 4350 7250 4350
$Comp
L Diode:1N4001 D?
U 1 1 5EF2BD51
P 7250 4150
F 0 "D?" V 7204 4229 50  0000 L CNN
F 1 "1N4001" V 7295 4229 50  0000 L CNN
F 2 "Diode_THT:D_DO-41_SOD81_P10.16mm_Horizontal" H 7250 3975 50  0001 C CNN
F 3 "http://www.vishay.com/docs/88503/1n4001.pdf" H 7250 4150 50  0001 C CNN
	1    7250 4150
	0    1    1    0   
$EndComp
Wire Wire Line
	7250 4000 7250 3950
Connection ~ 7250 4350
Wire Wire Line
	7250 4350 7250 4500
Wire Wire Line
	6400 3950 6400 4350
$Comp
L Device:R R?
U 1 1 5EF2BD5B
P 8950 4700
F 0 "R?" V 8743 4700 50  0000 C CNN
F 1 "100" V 8834 4700 50  0000 C CNN
F 2 "" V 8880 4700 50  0001 C CNN
F 3 "~" H 8950 4700 50  0001 C CNN
	1    8950 4700
	0    1    1    0   
$EndComp
$Comp
L Device:R R?
U 1 1 5EF2BD61
P 9250 4950
F 0 "R?" V 9043 4950 50  0000 C CNN
F 1 "10k" V 9134 4950 50  0000 C CNN
F 2 "" V 9180 4950 50  0001 C CNN
F 3 "~" H 9250 4950 50  0001 C CNN
	1    9250 4950
	1    0    0    -1  
$EndComp
$Comp
L Device:Q_NMOS_DGS Q?
U 1 1 5EF2BD67
P 9600 4700
F 0 "Q?" H 9805 4746 50  0000 L CNN
F 1 "PSMN017-30EL" H 9805 4655 50  0000 L CNN
F 2 "" H 9800 4800 50  0001 C CNN
F 3 "~" H 9600 4700 50  0001 C CNN
	1    9600 4700
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR?
U 1 1 5EF2BD6D
P 9250 5200
F 0 "#PWR?" H 9250 4950 50  0001 C CNN
F 1 "GND" H 9255 5027 50  0000 C CNN
F 2 "" H 9250 5200 50  0001 C CNN
F 3 "" H 9250 5200 50  0001 C CNN
	1    9250 5200
	1    0    0    -1  
$EndComp
Wire Wire Line
	9100 4700 9250 4700
Wire Wire Line
	9250 4800 9250 4700
Connection ~ 9250 4700
Wire Wire Line
	9250 4700 9400 4700
$Comp
L Device:LED D?
U 1 1 5EF2BD78
P 9400 4350
F 0 "D?" H 9393 4095 50  0000 C CNN
F 1 "LED" H 9393 4186 50  0000 C CNN
F 2 "" H 9400 4350 50  0001 C CNN
F 3 "~" H 9400 4350 50  0001 C CNN
	1    9400 4350
	-1   0    0    1   
$EndComp
$Comp
L Device:R R?
U 1 1 5EF2BD7E
P 9050 4350
F 0 "R?" V 8843 4350 50  0000 C CNN
F 1 "10k" V 8934 4350 50  0000 C CNN
F 2 "" V 8980 4350 50  0001 C CNN
F 3 "~" H 9050 4350 50  0001 C CNN
	1    9050 4350
	0    1    1    0   
$EndComp
$Comp
L power:+12V #PWR?
U 1 1 5EF2BD84
P 9700 3850
F 0 "#PWR?" H 9700 3700 50  0001 C CNN
F 1 "+12V" H 9715 4023 50  0000 C CNN
F 2 "" H 9700 3850 50  0001 C CNN
F 3 "" H 9700 3850 50  0001 C CNN
	1    9700 3850
	1    0    0    -1  
$EndComp
Wire Wire Line
	9700 3850 9700 3950
Wire Wire Line
	8800 4700 8650 4700
Wire Wire Line
	9700 4300 9700 4350
Wire Wire Line
	8850 4350 8900 4350
Wire Wire Line
	9200 4350 9250 4350
Wire Wire Line
	9250 5100 9250 5200
$Comp
L power:GND #PWR?
U 1 1 5EF2BD91
P 9700 5200
F 0 "#PWR?" H 9700 4950 50  0001 C CNN
F 1 "GND" H 9705 5027 50  0000 C CNN
F 2 "" H 9700 5200 50  0001 C CNN
F 3 "" H 9700 5200 50  0001 C CNN
	1    9700 5200
	1    0    0    -1  
$EndComp
Wire Wire Line
	9700 4900 9700 5200
Wire Wire Line
	8850 3950 9700 3950
Connection ~ 9700 3950
Wire Wire Line
	9550 4350 9700 4350
$Comp
L Diode:1N4001 D?
U 1 1 5EF2BD9B
P 9700 4150
F 0 "D?" V 9654 4229 50  0000 L CNN
F 1 "1N4001" V 9745 4229 50  0000 L CNN
F 2 "Diode_THT:D_DO-41_SOD81_P10.16mm_Horizontal" H 9700 3975 50  0001 C CNN
F 3 "http://www.vishay.com/docs/88503/1n4001.pdf" H 9700 4150 50  0001 C CNN
	1    9700 4150
	0    1    1    0   
$EndComp
Wire Wire Line
	9700 4000 9700 3950
Connection ~ 9700 4350
Wire Wire Line
	9700 4350 9700 4500
Wire Wire Line
	8850 3950 8850 4350
Wire Wire Line
	6350 4700 6200 4700
Wire Wire Line
	3950 3950 3950 4350
Wire Wire Line
	4800 4350 4800 4500
Connection ~ 4800 4350
Wire Wire Line
	4800 4000 4800 3950
$Comp
L Diode:1N4001 D?
U 1 1 5EF2BD07
P 4800 4150
F 0 "D?" V 4754 4229 50  0000 L CNN
F 1 "1N4001" V 4845 4229 50  0000 L CNN
F 2 "Diode_THT:D_DO-41_SOD81_P10.16mm_Horizontal" H 4800 3975 50  0001 C CNN
F 3 "http://www.vishay.com/docs/88503/1n4001.pdf" H 4800 4150 50  0001 C CNN
	1    4800 4150
	0    1    1    0   
$EndComp
Wire Wire Line
	4650 4350 4800 4350
Connection ~ 4800 3950
Wire Wire Line
	3950 3950 4800 3950
Wire Wire Line
	4800 4900 4800 5200
$Comp
L power:GND #PWR?
U 1 1 5EF2BCFD
P 4800 5200
F 0 "#PWR?" H 4800 4950 50  0001 C CNN
F 1 "GND" H 4805 5027 50  0000 C CNN
F 2 "" H 4800 5200 50  0001 C CNN
F 3 "" H 4800 5200 50  0001 C CNN
	1    4800 5200
	1    0    0    -1  
$EndComp
Wire Wire Line
	4350 5100 4350 5200
Wire Wire Line
	4300 4350 4350 4350
Wire Wire Line
	3950 4350 4000 4350
Wire Wire Line
	4800 4300 4800 4350
Wire Wire Line
	3900 4700 3750 4700
Wire Wire Line
	4800 3850 4800 3950
$Comp
L power:+12V #PWR?
U 1 1 5EF2BCF0
P 4800 3850
F 0 "#PWR?" H 4800 3700 50  0001 C CNN
F 1 "+12V" H 4815 4023 50  0000 C CNN
F 2 "" H 4800 3850 50  0001 C CNN
F 3 "" H 4800 3850 50  0001 C CNN
	1    4800 3850
	1    0    0    -1  
$EndComp
$Comp
L Device:R R?
U 1 1 5EF2BCEA
P 4150 4350
F 0 "R?" V 3943 4350 50  0000 C CNN
F 1 "10k" V 4034 4350 50  0000 C CNN
F 2 "" V 4080 4350 50  0001 C CNN
F 3 "~" H 4150 4350 50  0001 C CNN
	1    4150 4350
	0    1    1    0   
$EndComp
$Comp
L Device:LED D?
U 1 1 5EF2BCE4
P 4500 4350
F 0 "D?" H 4493 4095 50  0000 C CNN
F 1 "LED" H 4493 4186 50  0000 C CNN
F 2 "" H 4500 4350 50  0001 C CNN
F 3 "~" H 4500 4350 50  0001 C CNN
	1    4500 4350
	-1   0    0    1   
$EndComp
Wire Wire Line
	4350 4700 4500 4700
Connection ~ 4350 4700
Wire Wire Line
	4350 4800 4350 4700
Wire Wire Line
	4200 4700 4350 4700
$Comp
L power:GND #PWR?
U 1 1 5EF2BCD9
P 4350 5200
F 0 "#PWR?" H 4350 4950 50  0001 C CNN
F 1 "GND" H 4355 5027 50  0000 C CNN
F 2 "" H 4350 5200 50  0001 C CNN
F 3 "" H 4350 5200 50  0001 C CNN
	1    4350 5200
	1    0    0    -1  
$EndComp
$Comp
L Device:Q_NMOS_DGS Q?
U 1 1 5EF2BCD3
P 4700 4700
F 0 "Q?" H 4905 4746 50  0000 L CNN
F 1 "PSMN017-30EL" H 4905 4655 50  0000 L CNN
F 2 "" H 4900 4800 50  0001 C CNN
F 3 "~" H 4700 4700 50  0001 C CNN
	1    4700 4700
	1    0    0    -1  
$EndComp
$Comp
L Device:R R?
U 1 1 5EF2BCCD
P 4350 4950
F 0 "R?" V 4143 4950 50  0000 C CNN
F 1 "10k" V 4234 4950 50  0000 C CNN
F 2 "" V 4280 4950 50  0001 C CNN
F 3 "~" H 4350 4950 50  0001 C CNN
	1    4350 4950
	1    0    0    -1  
$EndComp
$Comp
L Device:R R?
U 1 1 5EF2BCC7
P 4050 4700
F 0 "R?" V 3843 4700 50  0000 C CNN
F 1 "100" V 3934 4700 50  0000 C CNN
F 2 "" V 3980 4700 50  0001 C CNN
F 3 "~" H 4050 4700 50  0001 C CNN
	1    4050 4700
	0    1    1    0   
$EndComp
Wire Wire Line
	1500 3950 1500 4350
Wire Wire Line
	2350 4350 2350 4500
Connection ~ 2350 4350
Wire Wire Line
	2350 4000 2350 3950
$Comp
L Diode:1N4001 D?
U 1 1 5EF2BCBD
P 2350 4150
F 0 "D?" V 2304 4229 50  0000 L CNN
F 1 "1N4001" V 2395 4229 50  0000 L CNN
F 2 "Diode_THT:D_DO-41_SOD81_P10.16mm_Horizontal" H 2350 3975 50  0001 C CNN
F 3 "http://www.vishay.com/docs/88503/1n4001.pdf" H 2350 4150 50  0001 C CNN
	1    2350 4150
	0    1    1    0   
$EndComp
Wire Wire Line
	2200 4350 2350 4350
Connection ~ 2350 3950
Wire Wire Line
	1500 3950 2350 3950
Wire Wire Line
	2350 4900 2350 5200
$Comp
L power:GND #PWR?
U 1 1 5EF2BCB3
P 2350 5200
F 0 "#PWR?" H 2350 4950 50  0001 C CNN
F 1 "GND" H 2355 5027 50  0000 C CNN
F 2 "" H 2350 5200 50  0001 C CNN
F 3 "" H 2350 5200 50  0001 C CNN
	1    2350 5200
	1    0    0    -1  
$EndComp
Wire Wire Line
	1900 5100 1900 5200
Wire Wire Line
	1850 4350 1900 4350
Wire Wire Line
	1500 4350 1550 4350
Wire Wire Line
	2350 4300 2350 4350
Wire Wire Line
	1450 4700 1300 4700
Wire Wire Line
	2350 3850 2350 3950
$Comp
L power:+12V #PWR?
U 1 1 5EF2BCA6
P 2350 3850
F 0 "#PWR?" H 2350 3700 50  0001 C CNN
F 1 "+12V" H 2365 4023 50  0000 C CNN
F 2 "" H 2350 3850 50  0001 C CNN
F 3 "" H 2350 3850 50  0001 C CNN
	1    2350 3850
	1    0    0    -1  
$EndComp
$Comp
L Device:R R?
U 1 1 5EF2BCA0
P 1700 4350
F 0 "R?" V 1493 4350 50  0000 C CNN
F 1 "10k" V 1584 4350 50  0000 C CNN
F 2 "" V 1630 4350 50  0001 C CNN
F 3 "~" H 1700 4350 50  0001 C CNN
	1    1700 4350
	0    1    1    0   
$EndComp
$Comp
L Device:LED D?
U 1 1 5EF2BC9A
P 2050 4350
F 0 "D?" H 2043 4095 50  0000 C CNN
F 1 "LED" H 2043 4186 50  0000 C CNN
F 2 "" H 2050 4350 50  0001 C CNN
F 3 "~" H 2050 4350 50  0001 C CNN
	1    2050 4350
	-1   0    0    1   
$EndComp
Wire Wire Line
	1900 4700 2050 4700
Connection ~ 1900 4700
Wire Wire Line
	1900 4800 1900 4700
Wire Wire Line
	1750 4700 1900 4700
$Comp
L power:GND #PWR?
U 1 1 5EF2BC8F
P 1900 5200
F 0 "#PWR?" H 1900 4950 50  0001 C CNN
F 1 "GND" H 1905 5027 50  0000 C CNN
F 2 "" H 1900 5200 50  0001 C CNN
F 3 "" H 1900 5200 50  0001 C CNN
	1    1900 5200
	1    0    0    -1  
$EndComp
$Comp
L Device:Q_NMOS_DGS Q?
U 1 1 5EF2BC89
P 2250 4700
F 0 "Q?" H 2455 4746 50  0000 L CNN
F 1 "PSMN017-30EL" H 2455 4655 50  0000 L CNN
F 2 "" H 2450 4800 50  0001 C CNN
F 3 "~" H 2250 4700 50  0001 C CNN
	1    2250 4700
	1    0    0    -1  
$EndComp
$Comp
L Device:R R?
U 1 1 5EF2BC83
P 1900 4950
F 0 "R?" V 1693 4950 50  0000 C CNN
F 1 "10k" V 1784 4950 50  0000 C CNN
F 2 "" V 1830 4950 50  0001 C CNN
F 3 "~" H 1900 4950 50  0001 C CNN
	1    1900 4950
	1    0    0    -1  
$EndComp
$Comp
L Device:R R?
U 1 1 5EF2BC7D
P 1600 4700
F 0 "R?" V 1393 4700 50  0000 C CNN
F 1 "100" V 1484 4700 50  0000 C CNN
F 2 "" V 1530 4700 50  0001 C CNN
F 3 "~" H 1600 4700 50  0001 C CNN
	1    1600 4700
	0    1    1    0   
$EndComp
Wire Wire Line
	6350 2400 6200 2400
Wire Wire Line
	3950 1650 3950 2050
Wire Wire Line
	4800 2050 4800 2200
Connection ~ 4800 2050
Wire Wire Line
	4800 1700 4800 1650
$Comp
L Diode:1N4001 D?
U 1 1 5EF2BBDF
P 4800 1850
F 0 "D?" V 4754 1929 50  0000 L CNN
F 1 "1N4001" V 4845 1929 50  0000 L CNN
F 2 "Diode_THT:D_DO-41_SOD81_P10.16mm_Horizontal" H 4800 1675 50  0001 C CNN
F 3 "http://www.vishay.com/docs/88503/1n4001.pdf" H 4800 1850 50  0001 C CNN
	1    4800 1850
	0    1    1    0   
$EndComp
Wire Wire Line
	4650 2050 4800 2050
Connection ~ 4800 1650
Wire Wire Line
	3950 1650 4800 1650
Wire Wire Line
	4800 2600 4800 2900
$Comp
L power:GND #PWR?
U 1 1 5EF2BBD5
P 4800 2900
F 0 "#PWR?" H 4800 2650 50  0001 C CNN
F 1 "GND" H 4805 2727 50  0000 C CNN
F 2 "" H 4800 2900 50  0001 C CNN
F 3 "" H 4800 2900 50  0001 C CNN
	1    4800 2900
	1    0    0    -1  
$EndComp
Wire Wire Line
	4350 2800 4350 2900
Wire Wire Line
	4300 2050 4350 2050
Wire Wire Line
	3950 2050 4000 2050
Wire Wire Line
	4800 2000 4800 2050
Wire Wire Line
	3900 2400 3750 2400
Wire Wire Line
	4800 1550 4800 1650
$Comp
L power:+12V #PWR?
U 1 1 5EF2BBC8
P 4800 1550
F 0 "#PWR?" H 4800 1400 50  0001 C CNN
F 1 "+12V" H 4815 1723 50  0000 C CNN
F 2 "" H 4800 1550 50  0001 C CNN
F 3 "" H 4800 1550 50  0001 C CNN
	1    4800 1550
	1    0    0    -1  
$EndComp
$Comp
L Device:R R?
U 1 1 5EF2BBC2
P 4150 2050
F 0 "R?" V 3943 2050 50  0000 C CNN
F 1 "10k" V 4034 2050 50  0000 C CNN
F 2 "" V 4080 2050 50  0001 C CNN
F 3 "~" H 4150 2050 50  0001 C CNN
	1    4150 2050
	0    1    1    0   
$EndComp
$Comp
L Device:LED D?
U 1 1 5EF2BBBC
P 4500 2050
F 0 "D?" H 4493 1795 50  0000 C CNN
F 1 "LED" H 4493 1886 50  0000 C CNN
F 2 "" H 4500 2050 50  0001 C CNN
F 3 "~" H 4500 2050 50  0001 C CNN
	1    4500 2050
	-1   0    0    1   
$EndComp
Wire Wire Line
	4350 2400 4500 2400
Connection ~ 4350 2400
Wire Wire Line
	4350 2500 4350 2400
Wire Wire Line
	4200 2400 4350 2400
$Comp
L power:GND #PWR?
U 1 1 5EF2BBB1
P 4350 2900
F 0 "#PWR?" H 4350 2650 50  0001 C CNN
F 1 "GND" H 4355 2727 50  0000 C CNN
F 2 "" H 4350 2900 50  0001 C CNN
F 3 "" H 4350 2900 50  0001 C CNN
	1    4350 2900
	1    0    0    -1  
$EndComp
$Comp
L Device:Q_NMOS_DGS Q?
U 1 1 5EF2BBAB
P 4700 2400
F 0 "Q?" H 4905 2446 50  0000 L CNN
F 1 "PSMN017-30EL" H 4905 2355 50  0000 L CNN
F 2 "" H 4900 2500 50  0001 C CNN
F 3 "~" H 4700 2400 50  0001 C CNN
	1    4700 2400
	1    0    0    -1  
$EndComp
$Comp
L Device:R R?
U 1 1 5EF2BBA5
P 4350 2650
F 0 "R?" V 4143 2650 50  0000 C CNN
F 1 "10k" V 4234 2650 50  0000 C CNN
F 2 "" V 4280 2650 50  0001 C CNN
F 3 "~" H 4350 2650 50  0001 C CNN
	1    4350 2650
	1    0    0    -1  
$EndComp
$Comp
L Device:R R?
U 1 1 5EF2BB9F
P 4050 2400
F 0 "R?" V 3843 2400 50  0000 C CNN
F 1 "100" V 3934 2400 50  0000 C CNN
F 2 "" V 3980 2400 50  0001 C CNN
F 3 "~" H 4050 2400 50  0001 C CNN
	1    4050 2400
	0    1    1    0   
$EndComp
Wire Wire Line
	1500 1650 1500 2050
Wire Wire Line
	2350 2050 2350 2200
Connection ~ 2350 2050
Wire Wire Line
	2350 1700 2350 1650
$Comp
L Diode:1N4001 D?
U 1 1 5EF2BB95
P 2350 1850
F 0 "D?" V 2304 1929 50  0000 L CNN
F 1 "1N4001" V 2395 1929 50  0000 L CNN
F 2 "Diode_THT:D_DO-41_SOD81_P10.16mm_Horizontal" H 2350 1675 50  0001 C CNN
F 3 "http://www.vishay.com/docs/88503/1n4001.pdf" H 2350 1850 50  0001 C CNN
	1    2350 1850
	0    1    1    0   
$EndComp
Wire Wire Line
	2200 2050 2350 2050
Connection ~ 2350 1650
Wire Wire Line
	1500 1650 2350 1650
Wire Wire Line
	2350 2600 2350 2900
$Comp
L power:GND #PWR?
U 1 1 5EF2BB8B
P 2350 2900
F 0 "#PWR?" H 2350 2650 50  0001 C CNN
F 1 "GND" H 2355 2727 50  0000 C CNN
F 2 "" H 2350 2900 50  0001 C CNN
F 3 "" H 2350 2900 50  0001 C CNN
	1    2350 2900
	1    0    0    -1  
$EndComp
Wire Wire Line
	1900 2800 1900 2900
Wire Wire Line
	1850 2050 1900 2050
Wire Wire Line
	1500 2050 1550 2050
Wire Wire Line
	2350 2000 2350 2050
Wire Wire Line
	1450 2400 1300 2400
Wire Wire Line
	2350 1550 2350 1650
$Comp
L power:+12V #PWR?
U 1 1 5EF2BB7E
P 2350 1550
F 0 "#PWR?" H 2350 1400 50  0001 C CNN
F 1 "+12V" H 2365 1723 50  0000 C CNN
F 2 "" H 2350 1550 50  0001 C CNN
F 3 "" H 2350 1550 50  0001 C CNN
	1    2350 1550
	1    0    0    -1  
$EndComp
$Comp
L Device:R R?
U 1 1 5EF2BB78
P 1700 2050
F 0 "R?" V 1493 2050 50  0000 C CNN
F 1 "10k" V 1584 2050 50  0000 C CNN
F 2 "" V 1630 2050 50  0001 C CNN
F 3 "~" H 1700 2050 50  0001 C CNN
	1    1700 2050
	0    1    1    0   
$EndComp
$Comp
L Device:LED D?
U 1 1 5EF2BB72
P 2050 2050
F 0 "D?" H 2043 1795 50  0000 C CNN
F 1 "LED" H 2043 1886 50  0000 C CNN
F 2 "" H 2050 2050 50  0001 C CNN
F 3 "~" H 2050 2050 50  0001 C CNN
	1    2050 2050
	-1   0    0    1   
$EndComp
Wire Wire Line
	1900 2400 2050 2400
Connection ~ 1900 2400
Wire Wire Line
	1900 2500 1900 2400
Wire Wire Line
	1750 2400 1900 2400
$Comp
L power:GND #PWR?
U 1 1 5EF2BB67
P 1900 2900
F 0 "#PWR?" H 1900 2650 50  0001 C CNN
F 1 "GND" H 1905 2727 50  0000 C CNN
F 2 "" H 1900 2900 50  0001 C CNN
F 3 "" H 1900 2900 50  0001 C CNN
	1    1900 2900
	1    0    0    -1  
$EndComp
$Comp
L Device:Q_NMOS_DGS Q?
U 1 1 5EF2BB61
P 2250 2400
F 0 "Q?" H 2455 2446 50  0000 L CNN
F 1 "PSMN017-30EL" H 2455 2355 50  0000 L CNN
F 2 "" H 2450 2500 50  0001 C CNN
F 3 "~" H 2250 2400 50  0001 C CNN
	1    2250 2400
	1    0    0    -1  
$EndComp
$Comp
L Device:R R?
U 1 1 5EF2BB5B
P 1900 2650
F 0 "R?" V 1693 2650 50  0000 C CNN
F 1 "10k" V 1784 2650 50  0000 C CNN
F 2 "" V 1830 2650 50  0001 C CNN
F 3 "~" H 1900 2650 50  0001 C CNN
	1    1900 2650
	1    0    0    -1  
$EndComp
$Comp
L Device:R R?
U 1 1 5EF2BB55
P 1600 2400
F 0 "R?" V 1393 2400 50  0000 C CNN
F 1 "100" V 1484 2400 50  0000 C CNN
F 2 "" V 1530 2400 50  0001 C CNN
F 3 "~" H 1600 2400 50  0001 C CNN
	1    1600 2400
	0    1    1    0   
$EndComp
$Comp
L Connector:Screw_Terminal_01x02 J?
U 1 1 5EF63AC1
P 3000 1800
F 0 "J?" H 3080 1792 50  0000 L CNN
F 1 "OUT_0" H 3080 1701 50  0000 L CNN
F 2 "" H 3000 1800 50  0001 C CNN
F 3 "~" H 3000 1800 50  0001 C CNN
	1    3000 1800
	1    0    0    -1  
$EndComp
$Comp
L Connector:Screw_Terminal_01x02 J?
U 1 1 5EF64EBB
P 5450 1800
F 0 "J?" H 5530 1792 50  0000 L CNN
F 1 "OUT_1" H 5530 1701 50  0000 L CNN
F 2 "" H 5450 1800 50  0001 C CNN
F 3 "~" H 5450 1800 50  0001 C CNN
	1    5450 1800
	1    0    0    -1  
$EndComp
$Comp
L Connector:Screw_Terminal_01x02 J?
U 1 1 5EF6557B
P 7900 1800
F 0 "J?" H 7980 1792 50  0000 L CNN
F 1 "OUT_2" H 7980 1701 50  0000 L CNN
F 2 "" H 7900 1800 50  0001 C CNN
F 3 "~" H 7900 1800 50  0001 C CNN
	1    7900 1800
	1    0    0    -1  
$EndComp
$Comp
L Connector:Screw_Terminal_01x02 J?
U 1 1 5EF6623A
P 10350 1800
F 0 "J?" H 10430 1792 50  0000 L CNN
F 1 "OUT_3" H 10430 1701 50  0000 L CNN
F 2 "" H 10350 1800 50  0001 C CNN
F 3 "~" H 10350 1800 50  0001 C CNN
	1    10350 1800
	1    0    0    -1  
$EndComp
$Comp
L Connector:Screw_Terminal_01x02 J?
U 1 1 5EF66B7C
P 10350 4100
F 0 "J?" H 10430 4092 50  0000 L CNN
F 1 "OUT_7" H 10430 4001 50  0000 L CNN
F 2 "" H 10350 4100 50  0001 C CNN
F 3 "~" H 10350 4100 50  0001 C CNN
	1    10350 4100
	1    0    0    -1  
$EndComp
$Comp
L Connector:Screw_Terminal_01x02 J?
U 1 1 5EF67362
P 7900 4100
F 0 "J?" H 7980 4092 50  0000 L CNN
F 1 "OUT_6" H 7980 4001 50  0000 L CNN
F 2 "" H 7900 4100 50  0001 C CNN
F 3 "~" H 7900 4100 50  0001 C CNN
	1    7900 4100
	1    0    0    -1  
$EndComp
$Comp
L Connector:Screw_Terminal_01x02 J?
U 1 1 5EF67BAB
P 3000 4100
F 0 "J?" H 3080 4092 50  0000 L CNN
F 1 "OUT_4" H 3080 4001 50  0000 L CNN
F 2 "" H 3000 4100 50  0001 C CNN
F 3 "~" H 3000 4100 50  0001 C CNN
	1    3000 4100
	1    0    0    -1  
$EndComp
$Comp
L Connector:Screw_Terminal_01x02 J?
U 1 1 5EF68633
P 5450 4100
F 0 "J?" H 5530 4092 50  0000 L CNN
F 1 "OUT_5" H 5530 4001 50  0000 L CNN
F 2 "" H 5450 4100 50  0001 C CNN
F 3 "~" H 5450 4100 50  0001 C CNN
	1    5450 4100
	1    0    0    -1  
$EndComp
Wire Wire Line
	2750 1650 2750 1800
Wire Wire Line
	2750 1800 2800 1800
Wire Wire Line
	2350 1650 2750 1650
Wire Wire Line
	2750 2050 2750 1900
Wire Wire Line
	2750 1900 2800 1900
Wire Wire Line
	5200 1650 5200 1800
Wire Wire Line
	5200 1800 5250 1800
Wire Wire Line
	4800 1650 5200 1650
Wire Wire Line
	2350 2050 2750 2050
Wire Wire Line
	5200 2050 5200 1900
Wire Wire Line
	5200 1900 5250 1900
Wire Wire Line
	4800 2050 5200 2050
Wire Wire Line
	7650 1650 7650 1800
Wire Wire Line
	7650 1800 7700 1800
Wire Wire Line
	7250 1650 7650 1650
Wire Wire Line
	7650 2050 7650 1900
Wire Wire Line
	7650 1900 7700 1900
Wire Wire Line
	7250 2050 7650 2050
Wire Wire Line
	10100 1650 10100 1800
Wire Wire Line
	10100 1800 10150 1800
Wire Wire Line
	9700 1650 10100 1650
Wire Wire Line
	10100 2050 10100 1900
Wire Wire Line
	10100 1900 10150 1900
Wire Wire Line
	9700 2050 10100 2050
Wire Wire Line
	10100 3950 10100 4100
Wire Wire Line
	10100 4100 10150 4100
Wire Wire Line
	9700 3950 10100 3950
Wire Wire Line
	10100 4350 10100 4200
Wire Wire Line
	10100 4200 10150 4200
Wire Wire Line
	9700 4350 10100 4350
Wire Wire Line
	7650 3950 7650 4100
Wire Wire Line
	7650 4100 7700 4100
Wire Wire Line
	7250 3950 7650 3950
Wire Wire Line
	7650 4350 7650 4200
Wire Wire Line
	7650 4200 7700 4200
Wire Wire Line
	7250 4350 7650 4350
Wire Wire Line
	5200 3950 5200 4100
Wire Wire Line
	5200 4100 5250 4100
Wire Wire Line
	4800 3950 5200 3950
Wire Wire Line
	5200 4350 5200 4200
Wire Wire Line
	5200 4200 5250 4200
Wire Wire Line
	4800 4350 5200 4350
Wire Wire Line
	2750 3950 2750 4100
Wire Wire Line
	2750 4100 2800 4100
Wire Wire Line
	2350 3950 2750 3950
Wire Wire Line
	2750 4350 2750 4200
Wire Wire Line
	2750 4200 2800 4200
Wire Wire Line
	2350 4350 2750 4350
Text GLabel 1300 2400 0    50   Input ~ 0
~OUT_0_DRIVE~
Text GLabel 3750 2400 0    50   Input ~ 0
~OUT_1_DRIVE~
Text GLabel 6200 2400 0    50   Input ~ 0
~OUT_2_DRIVE~
Text GLabel 8650 2400 0    50   Input ~ 0
~OUT_3_DRIVE~
Text GLabel 8650 4700 0    50   Input ~ 0
~OUT_7_DRIVE~
Text GLabel 6200 4700 0    50   Input ~ 0
~OUT_6_DRIVE~
Text GLabel 3750 4700 0    50   Input ~ 0
~OUT_5_DRIVE~
Text GLabel 1300 4700 0    50   Input ~ 0
~OUT_4_DRIVE~
$EndSCHEMATC
