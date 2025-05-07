## Project
EEPROM Programmer Client Software

## Description
A Hardware + Software suite to program EEPROMs (28C16 and 28C64).

The corresponding hardware project can be found here:

https://github.com/ARMCoderBR/EEPROM_Prog

## Version & Date
1.0 - 2025-APR-24

## Author
Milton Maldonado Jr (ARM_Coder)

## License
GPL V2

## Target
PC Client Software: Any Linux with GCC. This project was developed on the
                    Eclipse IDE, but you don't need to install Eclipse to
                    build it.
                    
## Building the Program
To build, use the command 'make all'.
```
cd Debug
make all
```

The executable eeprom_sender will be build in the Debug subdirectory.


## Using the Program
The program requires a valid INTEL HEX file. For the 28C16, the acceptable
addresses range from 0000h to 07FFh. For the 28C64, the acceptable addresses
range from 0000h to 1FFFh.

With the hardware plugged in, the serial adapter usually will map to
/dev/ttyUSB0.

Example of use:

```
$ ./eeprom_sender  -f crt0-alpha.ihx -d /dev/ttyUSB0 
===:03000000C3000139
OK
===:02000800ED4DBC
OK
===:02001000ED4DB4
OK
===:02001800ED4DAC
OK
===:02002000ED4DA4
OK
===:02002800ED4D9C
OK
===:02003000ED4D94
OK
===:02003800ED4D8C
OK
===:1A01000031FFFFCD0A01CD43147601010078B12808110020214214EDB0C9DB
OK
===:20144300CDBC0221F0FF39F9215500E5CDA301F1CD780221D516E5CD8001F1AFDD77F8DDB0
OK

... (Lines skipped for economy) ...

===:2013BE0000394E2346235E2356CB7A283A21040039EB21080039EB010400EDB021000039EC
OK
===:2013DE004E2346235E2356DD7EFC91DD7EFD98DD7EFE9BDD7EFF9AE2FA13EE80F201142EEC
OK
===:2013FE0001183C2E00183821000039EB21080039EB010400EDB0210400394E2346235E230F
OK
===:20141E0056DD7EFC91DD7EFD98DD7EFE9BDD7EFF9AE23414EE80F23B142E0118022E00DD6B
OK
===:04143E00F9DDE1C92A
OK
===:1A061200F1C1D1D5C5F5AF6FB00610200406087929CB111730011910F7C9F7
OK
===:00000001FF
$
```

## Disclaimer

This project is supplied 'as is' with no warranty against bugs and errors.



