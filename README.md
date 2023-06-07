 # TarEmu
 Emulator for the Atari 2600 VCS written in C++.
 
 # Usage
  Tar is currently not available for any platforms but windows.
 ## Normal Use
 Tar.exe can be run from the command-line with the ROM path as argument.
 Other switches and options include:
 - `rom` followed by the ROM path.
 - `noCrop`, displays the usually cropped out virtual VBLANK, HBLANK and VSYNC "sections".
 - `dump`, dumps the memory and CPU state after every instruction. Used to compare states for debugging purposes.
 
 Example use: `./Tar.exe -dump -rom "C:/path/to/rom" -noCrop`
 
 ## Debug use
 The Tar emulator implements Tom Harte's 6502 Json instruction test (https://github.com/TomHarte/ProcessorTests/tree/main/6502), which can be accessed with the `--jsonTest` flag.
 Other switches and options include:
 - `ignoreStatus`, does not check for mismatches in the processor status register.
 - `ignoreCycles`, does not check for mismatches in the instruction cycle times.
 - `ignoreRegisters`, does not check for mismatches in the processor registers.
 - `ignoreMemory`, does not check for mismatches in memory.
 - `noBCD`, skips all ADC and SBC tests where decimal mode is enabled.
 - `0xAA`, hex values with the `0x` prefix can be used to specify opcodes to test.
 - `x`, non-hex numbers can be used to specify the amount of tests to perform per instruction.

Example use: `./Tar.exe --jsonTest 0xFD 500 -ignoreCycles -ignoreMemory`

Note that, after testing all specifed instructions, it generates a report of the performance of different addressing modes. This proved useful in early development.
 
 # Building
 Tar can be built in Visual Studio 2019 or later. It is recomended to build in Release mode no matter what as the SFML debug dll's are not included anyway.
 After building, the `tables` and `noises` folders have to be moved into the build folder, along with the SFML dll's. 
 The `tests` folder can also be put in the same directory if you plan on using the CPU instruction tester. The Json test files are not included, they can be downloaded and placed directly into the `tests` folder from: https://github.com/TomHarte/ProcessorTests/tree/main/6502/v1
 
 # Compatability
 No game cartridges implementing bankswitching methods other than F8 or F6 will not work. Same goes for games with any controller other than the joystick.
 This list only mentions some of the ROMS I tried. I appreciate any other compatibility reviews added to this list (granted they use the standard bankswitching methods and joystick).
 
 ## Pitfall
 Works pretty well and is completely playable, the only bugs I noticed is the timer starting minutes at 100 seconds instead of 60 and not decrementing the minute timer. That and the logs dissapearing off screen a little bit earlier than they should. Recomended playing with audio off because of the obnoxious beeping noise (see known issues).
 
 ![image](https://github.com/Thomas-de-Bock/TarEmu/assets/78592830/c08679ea-dd76-4d79-84db-9129ed94c3e2)
 
 ## Donkey Kong
 Works very well and is completely playable, though the Pauline and Hammer sprites jitter left and right at times. Audio matches the game very well.
 
 ![image](https://github.com/Thomas-de-Bock/TarEmu/assets/78592830/ed1c2cbd-b2f7-406f-ab8a-04bddb9d917f)
 
 ## Midnight Magic
 Works almost perfectly, the only bug I noticed is some weird purple artifacts on and outside the board. Audio matches the game very well.
 
 ![image](https://github.com/Thomas-de-Bock/TarEmu/assets/78592830/f5e50508-c6b1-43f0-903b-495c82beff6d)

 ## Frogger
 Works okay, because of INTIM timer issues (see known issues), the positioning of the sprites on the road tends to be a bit buggy.
 
 ![image](https://github.com/Thomas-de-Bock/TarEmu/assets/78592830/ff64e398-083f-42f1-a28f-a2bc97d3cd3a)

 ## Jr. Pac-Man
 Works almost perfectly, the only bug I found is the walls being drawn incorrectly in some places.
 ![image](https://github.com/Thomas-de-Bock/TarEmu/assets/78592830/30f498f8-d9a0-462c-9b74-d1b38bc79598)

 


 # Known issues
 - There are no non-standard bankswitching methods currently implemented, this means that any cartridge with any bankswitching method implemented other than F8 and F6 will most likely not work.
 - Frequencies, although mostly accurate, sound a bit off and are an octave lower. This was the closest I managed to imeplement the frequencies while still having the higher notes be bearable.
 - The values stored in the AUDCx have no impact on the noise used, there is currently no noise contained in the `noises` folder besides the square wave.
 - ADC and SBC are not fully functional in decimal mode and mismatch the A register about 1/10 times on Tom Harte's instruction test. This has effects on the score and time counters in quite a few games I tried but not much else, it is practically the only use case.
 - As there is no implementation of the polynomial counters, the `HM` offsets use measurements by Brad done for the Player graphics (https://www.biglist.com/lists/stella/archives/199804/msg00198.html), I am not sure about the reliability of these measurements, but they gave good results on implementation. Though obviously, An actual implementation of the polynomial position counters would be more preferable.
 - The way the TIA and 6507 work in sync with eachother in Tar, is that the 6507 performs the instruction and measures the amount of cycles throughout the instruction. After this the TIA catches up by that amount multiplied by 3. This is not true to hardware, which is why reads and writes to `GRPx` and the `RESxx` strobes are delayed accordingly, where precise timing is crucial, but ONLY these addresses. This shouldn't have a big (or any) impact on most games but it could affect the HMOVE behaviour where small deviations in cycle times can also play a factor.
 - The weird positional behaviour in Frogger implies some incorrect behaviour of the INTIM value and thus the timer.
 - Although implemented, there are still some cycle inaccuracies on conditional jumps when crossing page boundaries.
