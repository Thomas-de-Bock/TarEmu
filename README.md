 # Tar
 An Atari2600 Emulator written in C++
 
 # Usage
 ## Normal Use
 Tar.exe can be run from the command-line with the ROM path as argument.
 Other switches and options include:
 - `rom` followed by the ROM path.
 - `noCrop`, displays the usually cropped out virtual VBLANK, HBLANK and VSYNC "sections".
 - `dump`, dumps the memory and CPU state after every instruction. Used to compare states for debugging purposes.
 
 Example use: `./Tar.exe -dump -rom C:/path/to/rom -noCrop`
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
 
 # Known issues
 - There are no non-standard bankswitching methods currently implemented, this means that any cartridge with any bankswitching method implemented other than F8 and F6 will most likely not work.
 - Frequencies, although mostly accurate, sound a bit off and are an octave lower. This was the closest I managed to imeplement the frequencies while still having the higher notes be bearable.
 - The values stored in the AUDCx have no impact on the noise used, there is currently no noise contained in the `noises` folder besides the square wave.
 - ADC and SBC are not fully functional in decimal mode and mismatch the A register about 1/10 times on Tom Harte's instruction test. This has effects on the score and time counters in quite a few games I tried but not much else, it is practically the only use case.
 - As there is no implementation of the polynomial counters, the `HM` offsets use measurements by Brad done for the Player graphics (https://www.biglist.com/lists/stella/archives/199804/msg00198.html), I am not sure about the reliability of these measurements, but they gave good results on implementation. Though obviously, An actual implementation of the polynomial position counters would be more preferable.
 - The way the TIA and 6507 work in sync with eachother in Tar, is that the 6507 performs the instruction and measures the amount of cycles throughout the instruction. After this the TIA catches up by that amount multiplied by 3. This is not true to hardware, which is why reads and writes to `GRPx` and the `RESxx` strobes are delayed accordingly, where precise timing is crucial, but ONLY these addresses. This shouldn't have a big (or any) impact on most games but it could affect the HMOVE behaviour where small deviations in cycle times can also play a factor.
 - The weird positional behaviour in Frogger implies some incorrect behaviour of the INTIM value and thus the timer.
