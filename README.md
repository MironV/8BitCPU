# 8-Bit CPU
This is a build based on [Ben Eater's original videos](https://eater.net/8bit) that go over a SAP-1 CPU based on Paul Malvino's descriptions in [Digital Computer Electronics](https://www.amazon.com/Digital-Computer-Electronics-Jerald-Malvino). This project upgrades the CPU even further with additional memory, a stack, and other features.

# Part 1: Upgrading the RAM
I'm not good at making videos, but I wanted to lay out how to upgrade the RAM in extreme detail in the hope that more folks can mod their 8-bit CPU builds with ease. You can get a sense for what we'll do in this [Imgur gallery](https://imgur.com/a/PhJFaNd).

This tutorial assumes you've finished Ben's full build and will show you how to:

* Upgrade to 256 bytes of single port RAM.
* Upgrade the program counter, memory address register, and instruction register to support 8 bits.
* Change the microcode, add support for 8 instruction steps, and add an additional reset microcode.

## General Tips

I want to call out some great resources available from the Reddit community:

* /u/lordmonoxide's [post with essential tips](https://www.reddit.com/r/beneater/comments/dskbug/what_i_have_learned_a_master_list_of_what_to_do/).
* /u/rolf-electronics's post about [his extensive SAP-3 build](https://www.reddit.com/r/beneater/comments/gkfnjp/last_update_of_my_sap3_document/).
* /u/Misterjay1234's post on [upgrading the RAM via a dual port chip](https://www.reddit.com/r/beneater/comments/ad2uko/upgrading_the_ram_module_to_256_bytes/).

For those who are new to electronics, I highly recommend two things: a cheap oscilloscope and bench power supply. You can certainly do without them, but I think they can really reduce frustration. Here's some general tips:

* **Oscilloscope:** I think that the direct visual representation of an oscilloscope vs. a multimeter or having to constantly probe with LEDs makes it much, much easier to understand what's going on. Especially if you're new to electronics! You can get a [Siglent DSO5102P which costs $240 on Amazon](https://www.amazon.com/Hantek-DSO5102P-Digital-Storage-Oscilloscope/dp/B01EJLZYN8) and will be more than enough for not just this project, but many electronics projects you'll encounter down the road.
* **Power Supply:** You can use Ben's power supply, but a proper power supply will come in handy because it'll protect you from mistakes and ensure you can always give it enough juice. Again, this is important if you're new to electronics! Just set the current limiter slightly above what your normal draw is and you'll easily be able to tell if you've shorted something without damaging your components. You can get this [Eventek KPS305D for $55 on Amazon](https://www.amazon.com/gp/product/B071RNT1CD/) which supports 30V/5A and it'll work just fine.
* **Logic Analyzer/Multimeter:** I didn't really use these during the build as I found them completely unnecessary. If you just want to play with a logic analyzer (may be better with the 6502 build), the [HiLetgo 8 Channel USB analyzer is $13 on Amazon](https://www.amazon.com/gp/product/B077LSG5P2/) and supports [PulseView](https://sigrok.org/wiki/PulseView). You'll also want to get some [breadboard jumper ribbon cables, which are $6 on Amazon](https://www.amazon.com/gp/product/B07GD2BWPY/).
* **Power and Ground:** Try to connect all your power rails as much as possible so current can flow freely. Beyond power running down each side, make sure to connect the two "halves" of your CPU with wires bridging across the bus, for each power rail. Check your voltages at the far end to make sure they're not dropping too much and that your ground is the same throughout!
* **Capacitors:** I know everyone talks about it, but I'll just reiterate that this is really critical to not getting random bugs that drive you crazy and are impossible to figure out. Some people think the "bigger the better" but the capacitor value and type needs to match the kind of noise you're expecting, so you need to be specific. Here's a recommendation:
   * [0.1 uF:](https://jameco.com/z/DC-1-50-7-Capacitor-Ceramic-Disc-0-1-micro-F-50V-plusmn-20-_15270.html) Put a couple of 0.1uF capacitors on each breadboard's power rail (ideally 1 per chip), as close to each chip's power/ground wires as possible. These are usually orange and marked "104" on the side.
   * [1 uF:](https://www.jameco.com/z/ECE-A1HKS010-1-micro-F-50V-20-Radial-Capacitor-85C-4x5x1-5mm_94161.html) Put one 1uF electrolytic capacitor on each breadboard's power rail, near where the power/ground wires enter your breadboard. These are usually black and cylindrical and they only go in one direction (there's a white line denoting the negative terminal).
   * [10 uF:](https://www.jameco.com/z/R10-25A-10uF-25-Volt-Radial-Electrolytic-Capacitor-20-85c-5x11x5mm_1946367.html) Put 1-2 big electrolytic capacitors (10uF) near where the power supply connects and then at the far end of your board. Like their 1 uF counterparts, these are usually black and cylindrical and only go in one direction.
* **LEDs:** Don't forget that LEDs only work in one direction (the negative side has the shorter leg and has a notch on the base). Be disciplined about putting resistors on LEDs or you risk not just burning out the LED but weird problems. During one part of my build, I was lazy and decided to put some on later but they actually caused random behavior with some of the chips. I only realized my mistake after wasting a good hour of debugging!
* **Soldering:** I find it much easier to solder resistors directly to LEDs. It helps tell the direction of the LED and it's easier to fit on the board. You just take the leg of the resistor and the leg of the LED, twist them together with some pliers, and then add a bit of solder. If you're new to soldering, here's what you need to know:
   * **Soldering Iron:** These days you don't need to spend a lot of money to get a great iron. The TS100 is one of the best irons available and [it's $70 on Amazon](https://www.amazon.com/gp/product/B01MD12DYT/). If you're new to soldering, get the BC2 tip because it's easier to get more heat transferred with the flat side. As you gain more experience and maybe get into SMT, you may want to explore other tips. [Upgrade to open-source firmware](https://github.com/Ralim/ts100) to improve the iron for free.
   * **Solder and Tools:** If you're a beginner, use 63/37 leaded solder. Don't use non-leaded and don't use 60/40 ([because it's not eutectic](https://fctsolder.com/eutectic-solder/)) if you're just starting out because they require more skill. Once you get the hang of things, you can switch if you're concerned about environmental/health effects. You can get an [anti-static soldering mat](https://www.amazon.com/gp/product/B07MB2DFG9/) and [multi-arm station (with fan)](https://www.amazon.com/gp/product/B0722X4XMY/) to help make things easier, but it's not necessary.
* **Debugging:** The key thing about circuits is they always do exactly what you tell them. It's just that, sometimes you don't know what you're telling them. The key to debugging effectively is to isolate the problem and try to get it down to a few possibilities. Don't just assume certain things are happening, make sure! Use the principle of deduction to narrow down problems. If it's the 7th bit, you know the issue is with the upper chip. If it's happening during only one state, isolate your signals from the system and manually drive the chip to see what's going on. Don't trust that your wires are well-connected, don't trust your power/ground, and don't trust your LEDs. Always check!
* **Learning More (Art of Electronics):** If you want to learn more about these topics and practical electronics in general, the [Art of Electronics](https://www.amazon.com/Art-Electronics-Paul-Horowitz/dp/0521809266) is probably the greatest practical reference written about the subject. While it looks like an academic textbook, it's extremely approachable and deals with only practical questions about electronics. It's an invaluable resource for Ben's build and anything you may do in the future.

## Parts List

To expand the RAM, you'll need:

* 1x Cypress CY7C199 ([Jameco](https://www.jameco.com/z/CY7C199-55PC-Cypress-Semiconductor-SRAM-Asynchronous-Memory-256KB-32Kx8-Parallel-15ns-DIP-28_2302863.html), [Datasheet](https://www.jameco.com/Jameco/Products/ProdDS/2302863.pdf)) which has 32K bytes of RAM. Something like [62256](https://www.jameco.com/Jameco/Products/ProdDS/42850.pdf) would also work. At the end of this, we'll only be able to address 256 bytes but it gives flexibility for future expansion.
* 1x 74LS161 ([Jameco](https://www.jameco.com/z/74LS161-Major-Brands-IC-74LS161-4-Bit-Synchronous-Binary-Counter-DIP-16_46818.html), [Datasheet](https://www.jameco.com/Jameco/Products/ProdDS/46818.pdf)) which is a 2nd counter chip to make the program counter 8-bit.
* 1x 74LS173 ([Jameco](https://www.jameco.com/z/74LS173-Major-Brands-IC-74LS173-4-BIT-D-TYPE-REGISTER-3-STATE-OUTPUT_46922.html), [Datasheet](https://www.ti.com/lit/ds/sdls067a/sdls067a.pdf)) which is a 2nd register chip to make the memory address register 8-bit.
* 1x 74LS32 ([Jameco](https://www.jameco.com/z/74LS32-Major-Brands-IC-74LS32-Quad-2-Input-Positive-OR-Gate_47466.html), [Datasheet](https://www.jameco.com/Jameco/Products/ProdDS/47466.pdf)) which is an OR Gate which we'll need for some signals.
* 13x 1K Resistors ([Jameco](https://www.jameco.com/z/CF1-4W102JRC-Resistor-Carbon-Film-1k-Ohm-1-4-Watt-5-_690865.html)) to use as pulldowns/pullups for signaling.
* 1x 8-bit DIP switch to program the longer addresses.
* 4x Yellow LEDs, 7x Green LEDs, 11x 220 Ohm resistors to expand the address register (yellow), program counter (green), and t-state counter (green). If you want, you can replace the instruction register LEDs to be the same color since it'll just be one 8-bit opcode.

We will reuse the [74LS245](https://www.ti.com/lit/ds/symlink/sn54ls245-sp.pdf) (bus transceiver), [74LS157](https://www.uni-kl.de/elektronik-lager/417724) (2-input mux), [74LS04](https://www.ti.com/lit/ds/symlink/sn74ls04.pdf) (inverter) from the existing build.

## Step 1: Upgrade the RAM ([Image](https://imgur.com/ZCW44TQ))

**Adding the RAM**

Since everything else depends on the RAM, we'll start there. In order to make sure everything is working well, we'll integrate the RAM into the existing 4-bit system before moving to the next step. Let's first hook up the RAM to the bus:

1. Remove the 74LS245, the two 74189s, the two 74LS157s next to the 8-bit DIP switch, the 8-bit DIP switch, and the two 74LS04s that inverted the RAM output. Make sure to keep the clock-based write circuit (74LS00) and the button-based write circuit (74LS157)!
2. You should now have an empty breadboard where the RAM was and more space where you used to program the RAM. Your memory address register and memory DIP switch should remain as-is.
3. Put the CY7C199 next to the bus (where the 245 used to be) and put a 74LS04 and 74LS32 next to it. Wire the chips all up to power and ground.
4. Take the four address lines from the memory address register and wire them to A0 - A3 on the top side of the RAM. For now, wire A4 - A14 to ground.
5. Wire IO0 - IO7 on the RAM directly to the bus.
6. Test that the RAM is working by setting Counter Out high (so your bus isn't driven by the program counter) and connecting WE to power, OE to ground, and CE to ground. Toggle the address lines and observe that random stuff is showing on the bus. Success!

**Adding the DIP Switch Writer**

So now that we know the RAM is outputting onto the bus, how do we write to it? We'll use a different method than Ben and output values onto the bus directly:

1. Put the 74LS245 you took earlier next to the clock write circuit (the 74LS00) and connect power and ground. Set the Direction to high (just like other 245s). Place the 8-bit DIP switch to the left of it (still upside down, like Ben had it).
2. Connect the top side of the 245 to the bus.
3. Connect the bottom side of the 245 to the bottom side of the DIP switch.
4. Connect each of the 8 lines on the top side of the DIP switch to ground. Connect each of the 8 lines on the bottom side of the DIP switch to power via 1K resistors. Since we're not using any chips, this lets us drive the signal high when the switch is disconnected and low when the switch is closed.

**Adding Control Logic for Read/Write**

In theory we can now read and write, but we need some control logic to do so. Let's think about what we need:

1. In program mode, we need the bus to be free so that we can use it. In run mode, we need the 245 to be disabled.
2. We need CE to be low whenever we're interacting with RAM (either writing or reading) and high otherwise.
3. We need WE low and OE high whenever we write. We need WE high and OE low at all other times, otherwise we won't be able to see what's in the RAM!

**Disconnecting the ROM in Program Mode**

Let's start by making sure the bus is free in program mode. What we'll do is disconnect the ROM whenever we're in program mode.

1. We need the ROM's OE to be low when we're in run mode and high when we're in program mode.
2. Take the mode select line going into the 74LS157 next to the write button and connect it to the 74LS04 inverter you placed.
3. Take the signal from the inverter and connect it to the ROM's OE. You'll need to connect both ROM's OE together instead of to ground. Check that the ROM disconnects when you flip the switch (all the control lines should turn off).
4. Connect 1k resistors to ground for RO, IO, CO, AO, and EO. This is necessary because when the OE is high on the ROM, it is floating and will not reliably provide signal to the 74LS04 inverters.

**Disconnecting the 245 in Run Mode**

Now, how do we get the 245 to work correctly and become disabled in run mode?

1. Connect one of the empty outputs of the 74LS157 next to the write button to the Enable line on the 74LS245. Connect the B input to power, to drive it high.
2. Let's leverage the 74LS157 circuit Ben already designed for the write, where A is hooked up to the button and B comes from the clock write circuit. Take the output of this circuit and wire it to the free A input to complete the selector circuit above.

**Connecting Read/Write Signals**

The 245 will now always be high in run mode, but in program mode it'll be low just briefly when you are writing to RAM and the button pulls the line to ground. We can get the RAM read/write to work in a similar way.

1. Take the output of Ben's original 74LS157 write circuit and run it to the 74LS04 inverter.
2. Take the original signal and run it to the WE of the RAM. This is the same as how the original RAM's WE was triggered.
3. Take the inverted signal and run it to the OE of the RAM. They will now always be the opposite of each other.

**Enabling the RAM in Program Mode or when RI/RO are active**

We're almost there! We just need to hook up the CE so the RAM is always enabled in program mode and enabled in run mode only when we send it RI or RO. We'll use an 74LS32 OR gate and put it through an inverter.

1. The RI line already goes into the clock write circuit AND gate. Add another connection from this line to an input in the 74LS32 OR gate.
2. Disconnect the RO line from the inverter, since it won't trigger low anymore. Connect the RO line to the other input of the OR gate. Take the output of the OR gate and connect it to a free input of a 2nd OR gate.
3. Take the inverted program mode signal (the one going to the ROM) and add that as an input to the 2nd OR gate.
4. We now have: RAM In | RAM Out | Program Mode. The output of this will be high, so we need to run it through an open slot on the 74LS04 inverter and connect it to CE.

**Testing!**

If you've wired everything correctly, when you're in program mode you'll always see the output of RAM on the bus except when you press the write button momentarily, at which point what's in the DIP switch will be output onto the bus and written into the RAM. When you go into run mode, the RAM will only be enabled when either RI or RO are activated. It will be outputting except when the clock write circuit triggers a write. During run mode, the 245 is completely disabled.

At this point, the new RAM should function identically to the previous RAM. Run some programs to make sure nothing went wrong during the upgrade!

## Step 2: Expand to 8 Bits ([Image](https://imgur.com/vKErzdZ))

The RAM is expanded but it won't actually do anything differently until we can give it more address lines. To do that, we need to expand the program counter and address register.

**Expanding the Program Counter**

Let's start with the program counter, which is really simple to expand.

1. Add another 74LS161 chip next to the existing one. Connect power and ground.
2. Connect the Clock, Load, and Clear lines between the two 161 chips so that they can be controlled identically.
3. Connect the Ripple Carry output of the lower chip (the one on the right) to the Enable input of the higher chip (the one on the left).
4. Connect the extra 4 input lines to the bottom of the 74LS245 transceiver and the 4 output lines to the top. Make sure you wire them correctly so that when the two 4-bit counters are combined side-by-side into 8 bits, the counting progresses naturally from right to left.
5. Connect the extra 4 lines from the 74LS245 to the bus and add 4 more LEDs so you can see what's going on.

If you've done everything correctly, you should now have an 8-bit counter. You can test it by disconnecting the control lines and manually enabling the counter.

**Expanding the Memory Address Register**

Expanding the program counter isn't very useful until the address register can actually reach the new memory. Don't worry, this part is really easy! Depending on how you laid it out, you may have to redo all the wiring or you can get by just adding things inline.

1. Replace the 4-bit DIP switch with an 8-bit one. Alternatively, you can use two DIP switches or just jumper wires. Wire one side of the switch to ground in the same alternating pattern as Ben's original design.
2. Add a 74LS157 selector next to the existing one. Add power and ground. Wire it up to the corresponding DIP switches, just like with the original.
3. Add a 74LS173 register next to the existing one. Add power, ground, and tie M/N to ground. Tie the clock, enable (G1/G2), and reset lines between the two 173s together.
4. Connect the upper half of the bus to the inputs of the new 173 register.
5. Connect the outputs of the new 173 to the new 157.
6. Add 4 LEDs with resistors to the left of the existing ones (or move the entire set of LEDs elsewhere) and wire them to the new 157 outputs.
7. Wire the new 157 outputs to A4 - A7 on the RAM. No1te that A4 is on the top and the rest are on the bottom.

If everything has been hooked up correctly, you can ensure things are working well by going into program mode and using the new upper bits or going into run mode and making the counter count to a high value and make sure it's reflected in the address register.

**Adjusting the Instruction Register**

The instruction register already holds 8 bits, but we can no longer represent both the opcode and the address in 8 bits. We'll make the instruction register only hold opcodes and we'll send the address separately. This means the current wiring is off and the LEDs are different colors, so we need to fix that.

1. Remove the 74LS245 transceiver because it's not needed. The instruction register never needs to output anything.
2. If you want, replace the multiple colors of LEDs with a single color.
3. Wire the ROM address lines to the outputs of the lower register. In Ben's 4-bit design, the upper register contains the opcode. In the new 8-bit design, both registers contain the opcode but we'll only use the lower part of the byte.

Now, you may be wondering why we're still only connecting the 4 bits to the ROM when we have 8 bits for the opcode. If you recall in Ben's design, the address lines need to signal three things: opcode, step, and flags. There are two unused address lines left, which you can use in three different ways:

1. Extra flags (e.g., negative flag)
2. Extra steps (16 vs. 8)
3. Extra instructions (32 vs. 16)

In the future I'll probably rework this to get added flexibility, but I chose to keep it simple for now and just stick to the original design.

**Adjusting the Step Counter**

There is one last bit to take care of before we program the ROM. Since we're sending our addresses separately, each instruction will take extra steps and we need to make sure we don't reset the counter too early. Since we no longer have an IO signal, we can use that as a dynamic reset signal and do away with the static reset altogether.

1. Remove the reset line going from the 74LS138 decoder to the reset circuit and instead connect what used to be the IO line (rename it to TR for "T-State Reset").
2. Add more LEDs to the 74LS138 so you can see all 8 steps.

**Testing!**

At this stage, hopefully you've seen that every individual piece is working as expected. However, we can't test anything end-to-end until we get the ROM updated, since the instructions make no sense.

## Step 3: Update the Instruction Set ([Image](https://imgur.com/rW0gdKN))

The instruction set requires only a few minimal modifications. Let's go through them:

1. Rename the `#define IO` to `#define TR`
2. Add a step to each instruction that just has `TR`. This will reset the counter early and eliminate empty steps. For example, your `NOP` now becomes `MI|CO, RO|II|CE, TR, 0`.
3. Let's deal with non-jump instructions first. `MI|CO` and `RO|II|CE` stay the same because we always need to fetch the opcode. Before, we had `IO|MI` since the instruction register's lower 4 bits contained the address. We just need to replace this with two steps: `CO|MI` and `MI|RO|CE`. These will fetch the address from the next byte instead and increment the program counter accordingly.
4. What about `LDI`? You just need to add `MI|CO` and then replace `IO|AI` with `RO|AI|CE`. `LDI` becomes more useful because now you can enter a number from 0 to 255 directly. In fact, I also created immediate versions of `ADD` and `SUB` because it was so handy.
5. `HLT`, `OUT`, and `NOP` don't need to use up more steps since there's no data to pass them. Just remember that when you calculate locations for jumps, sometimes you need to count by 2 and other times only 1!
6. The `JMP` instruction is straightforward. Instead of `IO|J` you need to use up another step and do `MI|CO, RO|J`. Notice that unlike the other instructions we don't need to increment the counter a second time since we're jumping anyway.
7. The condition jumps are only slightly trickier. The first thing is to look at the template. We need to have all branches of the instruction take up two bytes/steps even if we don't jump, otherwise the computer will think the jump address is the next instruction. Before the `TR` step you added earlier, add another step to `JC` and `JZ` that just increments the counter (`CE`).
8. Now we need to take care of the conditions. You need to expand the one line into three that mimic the `JMP` command. For example, for the case where zero flag = 0, carry flag = 1 you'd write:  `ucode[FLAGS_Z0C1][JC][2] = MI|CO; ucode[FLAGS_Z0C1][JC][3] = RO|J; ucode[FLAGS_Z0C1][JC][4] = TR;`

## Conclusion

And that's it! You now have a fully 8-bit CPU that's able to access 256 bytes of RAM and has some more efficient IPC to boot.

# Part 2: Adding a Stack, Expanding Control Lines, and Building an Assembler
You'll need to have done the work in the previous post to be able to do this. You can get a sense for what we'll do in this [Imgur gallery](https://imgur.com/a/76Yr2cO).

In this tutorial, we'll balance software and hardware improvements to make this a pretty capable machine:

* Use an Arduino and an assembler to enable faster, more complex programming.
* Expand control lines without additional ROMs, using 74LS138 decoders.
* Add a stack pointer and stack to support subroutines with 74LS193 counters.
* **Bonus:** Enable B register output and add a Schmitt trigger to clean up your clock signal.

## Parts List

To only update the hardware, you'll need:

* 2x 74LS138 ([Datasheet](https://ecee.colorado.edu/~mcclurel/sn74ls138rev5.pdf), [Jameco](https://www.jameco.com/z/74LS138-Major-Brands-IC-74LS138-3-to-8-Line-DECODER-DEMULTIPLEXER_46607.html)) which are decoders used to expand the control lines. You can reuse one from the step counter if you don't mind reading binary numbers vs. an LED for each step.
* 1x 74LS04 ([Datasheet](https://www.ti.com/lit/ds/symlink/sn74ls04.pdf), [Jameco](https://www.jameco.com/z/74LS04-Major-Brands-IC-74LS04-Hex-Inverter_46316.html)) which is an inverter to help expand the control lines.
* 2x 74LS193 ([Datasheet](http://susta.cz/fel/74/pdf/74LS193.pdf), [Jameco](https://www.jameco.com/shop/ProductDisplay?catalogId=10001&langId=-1&storeId=10001&productId=47036)) which is a 4-bit up/down counter used to create the stack pointer.
* 1x 74LS245 ([Datasheet](https://www.ti.com/lit/ds/symlink/sn54ls245-sp.pdf), [Jameco](https://www.jameco.com/z/74LS245-Major-Brands-IC-74LS245-Tri-State-Octal-Bus-Transceiver_47212.html)) which is a bus transceiver. You may have a spare one if you did my previous build.
* 1x 74LS00 ([Datasheet](https://www.ti.com/lit/ds/sdls025d/sdls025d.pdf?ts=1592683368909&ref_url=https%253A%252F%252Fwww.google.com%252F), [Jameco](https://www.jameco.com/z/74LS00-Major-Brands-IC-74LS00-Quad-2-Input-Positive-NAND-Gate-DIP-14_46252.html)) which is a NAND gate to control the stack pointer.
* 8x Green LED, 1x Yellow LED, 4x Blue LEDs, 13x 220 Ohm resistors to display the stack pointer (green), the stack address (yellow), and the additional control lines (blue).

If you want to update the toolchain, you'll need:

1. Arduino Mega 2560 ([Amazon](https://www.amazon.com/ELEGOO-ATmega2560-ATMEGA16U2-Projects-Compliant/)) to create the programmer.
2. Ribbon Jumper Cables ([Amazon](https://www.amazon.com/gp/product/B07GD2BWPY/)) to connect the Arduino to the breadboard.
3. TL866 II Plus EEPROM Programmer ([Amazon](https://www.amazon.com/gp/product/B07CDD9PGT/)) to program the ROM.

**Bonus Clock Improvement:** One additional thing I did is replace the [74LS04](https://www.ti.com/lit/ds/symlink/sn74ls04.pdf) inverter in Ben's clock circuit with a 74LS14 inverting Schmitt trigger ([datasheet](https://www.onsemi.com/pub/Collateral/SN74LS14-D.PDF), [Jameco](https://www.jameco.com/z/74LS14-Major-Brands-IC-74LS14-HEX-SCHMITT-TRIGGER-INVERTER_46640.html)). The pinouts are identical! Just drop it in, wire the existing lines, and then run the clock output through it twice (since it's inverting) to get a squeaky clean clock signal. Useful if you want to go even faster with the CPU.

## Step 1: Program with an Arduino and Assembler ([Image 1](https://imgur.com/ZYZjnZy), [Image 2](https://imgur.com/Cl4sfaR))

There's a certain delight in the physical programming of a computer with switches. This is how Bill Gates and Paul Allen [famously programmed the Altair 8800 and started Microsoft](https://www.wired.com/2008/04/dayintech-0404/). But at some point, the hardware becomes limited by how effectively you can input the software. After upgrading the RAM, I quickly felt constrained by how long it took to program everything.

You can continue to program the computer physically if you want and even after upgrading that option is still available, so this step is optional. There's probably many ways to approach the programming, but this way felt simple and in the spirit of the build. We'll use an Arduino Mega 2560, like the one in Ben's 6502 build, to program the RAM. We'll start with a homemade assembler then switch to something more robust.

**Preparing the Physical Interface**

The first thing to do is prepare the CPU to be programmed by the Arduino. We already did the hard work on this in the RAM upgrade tutorial by using the bus to write to the RAM and disconnecting the control ROM while in program mode. Now we just need to route the appropriate lines to a convenient spot on the board to plug the Arduino into.

1. This is optional, but I rewired all the DIP switches to have ground on one side, rather than alternating sides like Ben's build. This just makes it easier to route wires.
2. Wire the 8 address lines from the DIP switch, connecting the side opposite to ground (the one going to the chips) to a convenient point on the board. I put them on the far left, next to the address LEDs and above the write button circuit.
3. Wire the 8 data lines from the DIP switch, connecting the side opposite to ground (the one going to the chips) directly below the address lines. Make sure they're separated by the gutter so they're not connected.
4. Wire a line from the write button to your input area. You want to connect the side of the button that's not connected to ground (the one going to the chip).

So now you have one convenient spot with 8 address lines, 8 data lines, and a write line. If you want to get fancy, you can wire them into some kind of connector, but I found that ribbon jumper cables work nicely and keep things tidy.

The way we'll program the RAM is to enter program mode and set all the DIP switches to the high position (e.g., 11111111). Since the switches are upside-down, this means they'll all be disconnected and not driving to ground. The address and write lines will simply be floating and the data lines will be weakly pulled up by 1k resistors. Either way, the Arduino can now drive the signals going into the chips using its outputs.

**Creating the Arduino Programmer**

Now that we can interface with an Arduino, we need to write some software. If you follow [Ben's 6502 video](https://www.youtube.com/watch?v=LnzuMJLZRdU), you'll have all the knowledge you need to get this working. If you want some hints and code, see below ([source code](https://github.com/MironV/8BitCPU/blob/master/8Bit_Programmer/8Bit_Programmer.ino)):

1. Create arrays for your data and address lines. For example: `const char ADDRESS_LINES[] = {39, 41, 43, 45, 47, 49, 51, 53};`. Create your write line with `#define RAM_WRITE 3`.
2. Create functions to enable and disable your address and data lines. You want to enable them before writing. Make sure to disable them afterward so that you can still manually program using DIP switches without disconnecting the Arduino. The code looks like this (just change INPUT to OUTPUT accordingly): `for(int n = 0; n < 8; n += 1) { pinMode(ADDRESS_LINES[n], OUTPUT); }`
3. Create a function to write to an address. It'll look like `void writeData(byte writeAddress, byte writeData)` and basically use two loops, one for address and one for data, followed by toggling the write.
4. Create a char array that contains your program and data. You can use `#define` to create opcodes like `#define LDA 0x01`.
5. In your main function, loop through the program array and send it through `writeData`.

With this setup, you can now load multi-line programs in a fraction of a second! This can really come in handy with debugging by stress testing your CPU with software. Make sure to test your setup with existing programs you know run reliably. Now that you have your basic setup working, you can add 8 additional lines to read the bus and expand the program to let you read memory locations or even monitor the running of your CPU.

**Making an Assembler**

The above will serve us well but it's missing a key feature: labels. Labels are invaluable in assembly because they're so versatile. Jumps, subroutines, variables all use labels. The problem is that labels require parsing. Parsing is a fun project on the road to a compiler but not something I wanted to delve into right now--if you're interested, you can [learn about Flex and Bison](https://gnuu.org/2009/09/18/writing-your-own-toy-compiler/). Instead, I found a [custom assembler](https://github.com/hlorenzi/customasm) that lets you define your CPU's instruction set and it'll do everything else for you. Let's get it setup:

1. If you're on Windows, you can use the pre-built binaries. Otherwise, you'll need to [install Rust](https://rustup.rs/) and compile via `cargo build`.
2. Create a file called `8bit.cpu` and define your CPU instructions ([source code](https://github.com/MironV/8BitCPU/blob/master/8Bit_Programmer/8bit.cpu)). For example, LDA would be `lda {address} -> 0x01 @ address[7:0]`. What's cool is you can also now create the instruction's immediate variant instead of having to call it LDI: `lda #{value} -> 0x05 @ value[7:0]`.
3. You can now write assembly by adding `#include "8bit.cpu"` to the top of your code. There's a lot of neat features so make sure to [read the documentation](https://github.com/hlorenzi/customasm/blob/master/doc/index.md)!
4. Once you've written some assembly, you can generate the machine code using `./customasm yourprogram.s -f hexc -p`. This prints out a char array just like our Arduino program used!
5. Copy the char array into your Arduino program and send it to your CPU.

At this stage, you can start creating some pretty complex programs with ease. I would definitely play around with writing some larger programs. I actually found a bug in my hardware that was hidden for a while because my programs were never very complex!

## Step 2: Expand the Control Lines ([Image](https://imgur.com/7DcMJIY))

Before we can expand the CPU any further, we have to address the fact we're running out of control lines. An easy way to do this is to add a 3rd 28C16 ROM and be on your way. If you want something a little more involved but satisfying, read on.

Right now the control lines are [one hot encoded](https://en.wikipedia.org/wiki/One-hot). This means that if you have 4 lines, you can encode 4 states. But we know that a 4-bit binary number can encode 16 states. We'll use this principle via 74LS138 decoders, just like Ben used for the step counter.

**Choosing the Control Line Combinations**

Everything comes with trade-offs. In the case of combining control lines, it means the two control lines we choose to combine can never be activated at the same time. We can ensure this by encoding all the inputs together in the first 74LS138 and all the outputs together in a second 74LS138. We'll keep the remaining control lines directly connected.

**Rewiring the Control Lines**

If your build is anything like mine, the control lines are a bit of a mess. You'll need to be careful when rewiring to ensure it all comes back together correctly. Let's get to it:

1. Place the two 74LS138 decoders on the far right side of the breadboard with the ROMs. Connect them to power and ground.
2. You'll likely run out of inverters, so place a 74LS04 on the breadboard above your decoders. Connect it to power and ground.
3. Carefully take your inputs (`MI, RI, II, AI, BI, J`) and wire them to the outputs of the left 74LS138. Do not wire anything to O0 because that's activated by 000 which won't work for us!
4. Carefully take your outputs (`RO, CO, AO, EO`) and wire them to the outputs of the right 74LS138. Remember, do not wire anything to O0!
5. Now, the 74LS138 outputs are active low, but the ROM outputs were active high. This means you need to swap the wiring on all your existing 74LS04 inverters for the LEDs and control lines to work. Make sure you track which control lines are supposed to be active high vs. active low!
6. Wire E3 to power and E2 to ground. Connect the E1 on both 138s together, then connect it to the same line as OE on your ROMs. This will ensure that the outputs are disabled when you're in program mode. You can actually take off the 1k pull-up resistors from the previous tutorial at this stage, because the 138s actively drive the lines going to the 74LS04 inverters rather than floating like the ROMs.

At this point, you *really* need to ensure that the massive rewiring job was successful. Connect 3 jumper wires to A0-A2 and test all the combinations manually. Make sure the correct LED lights up and check with a multimeter/oscilloscope that you're getting the right signal at each chip. Catching mistakes at this point will save you a lot of headaches! Now that everything is working, let's finish up:

1. Connect A0-A2 of the left 74LS138 to the left ROM's A0-A2.
2. Connect A0-A2 of the right 74LS138 to the right ROM's A0-A2.
3. Distribute the rest of the control signals across the two ROMs.

**Changing the ROM Code**

This part is easy. We just need to update all of our `#define` with the new addresses and program the ROMs again. For clarity that we're not using one-hot encoding anymore, I recommend using hex instead of binary. So instead of `#define MI 0b0000000100000000`, we can use `#define MI 0x0100`, `#define RI 0x0200`, and so on.

**Testing**

Expanding the control lines required physically rewiring a lot of critical stuff, so small mistakes can creep up and make mysterious errors down the road. Write a program that activates each control line at least once and make sure it works properly! With your assembler and Arduino programmer, this should be trivial.

**Bonus: Adding B Register Output**

With the additional control lines, don't forget you can now add a `BO` signal easily which lets you fully use the B register.

## Step 3: Add a Stack ([Image 1](https://imgur.com/oGsOUUz), [Image 2](https://imgur.com/zep6lqw))

Adding a stack significantly expands the capability of the CPU. It enables subroutines, recursion, and handling interrupts (with some additional logic). We'll create our stack with an 8-bit stack pointer hard-coded from $0100 to $01FF, just like the 6502.

**Wiring up the Stack Pointer**

A stack pointer is conceptually similar to a program counter. It stores an address, you can read it and write to it, and it increments. The only difference between a stack pointer and a program counter is that the stack pointer must also decrement. To create our stack pointer, we'll use two 74LS193 4-bit up/down binary counters:

1. Place a 74LS00 NAND gate, 74LS245 transceiver, and two 74LS193 counters in a row next to your output register. Wire up power and ground.
2. Wire the the Carry output of the right 193 to the Count Up input of the left 193. Do the same for the Borrow output and Count Down input.
3. Connect the Clear input between the two 193s and with an active high reset line. The B register has one you can use on its 74LS173s.
4. Connect the Load input between the two 193s and to a new active low control line called `SI` on your 74LS138 decoder.
5. Connect the QA-QD outputs of the lower counter to A8-A5 and the upper counter to A4-A1. Pay special attention because the output are in a weird order (BACD) and you want to make sure the lower A is connected to A8 and the upper A is connected to A4.
6. Connect the A-D inputs of the lower counter to B8-B5 and the upper counter to B4-B1. Again, the inputs are in a weird order and on both sides of the chip so pay special attention.
7. Connect the B1-B8 outputs of the 74LS245 transceiver to the bus.
8. On the 74LS245 transceiver, connect DIR to power (high) and connect OE to a new active low control line called `SO` on your 74LS138 decoder.
9. Add 8 LEDs and resistors to the lower part of the 74LS245 transceiver (A1-A8) so you can see what's going on with the stack pointer.

**Enabling Increment & Decrement**

We've now connected everything but the Count Up and Count Down inputs. The way the 74LS193 works is that if nothing is counting, both inputs are high. If you want to increment, you keep Count Down high and pulse Count Up. To decrement, you do the opposite. We'll use a 74LS00 NAND gate for this:

1. Take the clock from the 74LS08 AND gate and make it an input into two different NAND gates on the 74LS00.
2. Take the output from one NAND gate and wire it to the Count Up input on the lower 74LS193 counter. Take the other output and wire it to the Count Down input.
3. Wire up a new active high control line called `SP` from your ROM to the NAND gate going into Count Up.
4. Wire up a new active high control line called `SM` from your ROM to the NAND gate going into Count Down.

At this point, everything should be working. Your counter should be able to reset, input a value, output a value, and increment/decrement. But the issue is it'll be writing to $0000 to $00FF in the RAM! Let's fix that.

**Accessing Higher Memory Addresses**

We need the stack to be in a different place in memory than our regular program. The problem is, we only have an 8-bit bus, so how do we tell the RAM we want a higher address? We'll use a special control line to do this:

1. Wire up an active high line called `SA` from the 28C16 ROM to A8 on the [Cypress CY7C199](https://media.digikey.com/pdf/Data%20Sheets/Cypress%20PDFs/CY7C199.pdf) RAM.
2. Add an LED and resistor so you can see when the stack is active.

That's it! Now, whenever we need the stack we can use a combination of the control line and stack pointer to access $0100 to $01FF.

**Updating the Instruction Set**

All that's left now is to create some instructions that utilize the stack. We'll need to settle some conventions before we begin:

* **Empty vs. Full Stack:** In our design, the stack pointer points to the next empty slot in memory, just like on the 6502. This is called an "empty stack" convention. ARM processors use a "full stack" convention where the stack points to the last filled slot.
* **Ascending vs. Descending Stack:** In our design, the stack pointer increases when you add something and decreases when you remove something. This is an "ascending stack" convention. Most processors use a "descending stack", so we're bucking the trend here.

If you want to add a little personal flair to your design, you can change the convention fairly easily. Let's implement push and pop ([source code](https://github.com/MironV/8BitCPU/blob/master/EEPROM_CPUMicrocode/EEPROM_CPUMicrocode.ino)):

1. Define all your new control lines, such as `#define SI 0x0700` and `#define SO 0x0005`.
2. Create two new instructions: `PSH` (1011) and `POP` (1100).
3. `PSH` starts the same as any other for the first two steps: `MI|CO` and `RO|II|CE`. The next step is to put the contents of the stack pointer into the address register via `MI|SO|SA`. Recall that `SA` is the special control line that tells the memory to access the `$01XX` bank rather than `$00XX`.
4. We then take the contents of `AO` and write it into the RAM. We can also increment the stack pointer at this stage. All of this is done via: `AO|RI|SP|SA`, followed by `TR`.
5. `POP` is pretty similar. Start off with `MI|CO` and `RO|II|CE`. We then need to take a cycle and decrement the stack pointer with `SM`. Like with `PSH`, we then set the address register with `MI|SO|SA`.
6. We now just need to output the RAM into our A register with `RO|AI|SA` and then end the instruction with `TR`.
7. Updating the assembler is easy since neither instruction has operands. For example, push is just `psh -> 0x0B`.

And that's it! Write some programs that take advantage of your new 256 byte stack to make sure everything works as expected.

## Step 4: Add Subroutine Instructions ([Image](https://imgur.com/IQ7CK49))

The last step to complete our stack is to add subroutine instructions. This allows us to write complex programs and paves the way for things like interrupt handling.

Subroutines are like a blend of push/pop instructions and a jump. Basically, when you want to call a subroutine, you save your spot in the program by pushing the program counter onto the stack, then jumping to the subroutine's location in memory. When you're done with the subroutine, you simply pop the program counter value from the stack and jump back into it.

We'll follow 6502 conventions and only save and restore the program counter for subroutines. Other CPUs may choose to save more state, but it's generally left up to the programmer to ensure they're not wiping out states in their subroutines (e.g., push the A register at the start of your subroutine if you're messing with it and restore it before you leave).

**Adding an Extra Opcode Line**

I've started running low on opcodes at this point. Luckily, we still have two free address lines we can use. To enable 5-bit opcodes, simply wire up the 4Q output of your upper 74LS173 register to A7 of your 28C16 ROM (this assumes your opcodes are at A3-A6).

**Updating the ROM Writer**

At this point, you simply need to update the Arduino writer to support 32 instructions vs. the current 16. So, for example, `UCODE_TEMPLATE[16][8]` becomes `UCODE_TEMPLATE[32][8]` and you fill in the 16 new array elements with `nop`. The problem is that the Arduino only has so much memory and with the way Ben's code is written to support conditional jumps, it starts to get tight.

I bet the code can be re-written to handle this, but I had a [TL866II Plus EEPROM programmer](https://www.amazon.com/gp/product/B07CDD9PGT/) handy from the 6502 build and I felt it would be easier to start using that instead. Converting to a regular C program is really simple ([source code](https://github.com/MironV/8BitCPU/blob/master/ROM_Programmer/makerom.c)):

1. Copy all the `#define`, global `const` arrays (don't forget to expand them from 16 to 32), and `void initUCode()`. Add `#include <stdio.h>` and `#include <string.h>` to the top.
2. In your traditional `int main (void)` C function, after initializing with `initUCode()`, make two arrays: `char ucode_upper[2048]` and `char ucode_lower[2048]`.
3. Take your existing loop code that loops through all addresses: `for (int address = 0; address < 2048; address++)`.
4. Modify instruction to be 5-bit with `int instruction = (address & 0b00011111000) >> 3;`.
5. When writing, just write to the arrays like so: `ucode_lower[address] = ucode[flags][instruction][step];` and `ucode_upper[address] = ucode[flags][instruction][step] >> 8;`.
6. Open a new file with `FILE *f = fopen("rom_upper.hex", "wb");`, write to it with `fwrite(ucode_upper, sizeof(char), sizeof(ucode_upper), f);` and close it with `fclose(f);`. Repeat this with the lower ROM too.
7. Compile your code using [gcc](https://gcc.gnu.org/) (you can use any C compiler), like so: `gcc -Wall makerom.c -o makerom`.

Running your program will spit out two binary files with the full contents of each ROM. Writing the file via the TL866II Plus requires [minipro](https://gitlab.com/DavidGriffith/minipro/) and the following command: `minipro -p CAT28C16A -w rom_upper.hex`.

**Adding Subroutine Instructions**

At this point, I cleaned up my instruction set layout a bit. I made `psh` and `pop` 1000 and 1001, respectively. I then created two new instructions: `jsr` and `rts`. These allow us to jump to a subroutine and returns from a subroutine. They're relatively simple:

1. For `jsr`, the first three steps are the same as `psh`: `MI|CO`,  `RO|II|CE`,  `MI|SO|SA`.
2. On the next step, instead of `AO` we use `CO` to save the program counter to the stack: `CO|RI|SP|SA`.
3. We then essentially read the 2nd byte to do a jump and terminate: `MI|CO`, `RO|J`.
4. For `rts`, the first four steps are the same as `pop`: `MI|CO`,  `RO|II|CE`,  `SM`, `MI|SO|SA`.
5. On the next step, instead of `AI` we use `J` to load the program counter with the contents in stack: `RO|J|SA`.
6. We're not done! If we just left this as-is, we'd jump to the 2nd byte of `jsr` which is not an opcode, but a memory address. All hell would break loose! We need to add a `CE` step to increment the program counter and then terminate.

Once you update the ROM, you should have fully functioning subroutines with 5-bit opcodes. One great way to test them is to create a recursive program to calculate something--just don't go too deep or you'll end up with a stack overflow!

## Conclusion

And that's it! Another successful upgrade of your 8-bit CPU. You now have a very capable machine and toolchain. At this point I would have a bunch of fun with the software aspects. In terms of hardware, there's a number of ways to go from here:

1. **Interrupts.** Interrupts are just special subroutines triggered by an external line. You can make one similar to how Ben did conditional jumps. The only added complexity is the need to load/save the flags register since an interrupt can happen at any time and you don't want to destroy the state. Given this would take more than 8 steps, you'd also need to add another line for the step counter (see below).
2. **ROM expansion.** At this point, address lines on the ROM are getting tight which limits any expansion possibilities. With the new approach to ROM programming, it's trivial to switch out the [28C16](http://cva.stanford.edu/classes/cs99s/datasheets/at28c16.pdf) for the [28C256](http://ww1.microchip.com/downloads/en/DeviceDoc/doc0006.pdf) that Ben uses in the 6502. These give you 4 additional address lines for flags/interrupts, opcodes, and steps.
3. **LCD output.** At this point, adding a 16x2 character LCD [like Ben uses in the 6502](https://www.youtube.com/watch?v=FY3zTUaykVo) is very possible.
4. **Segment/bank register.** It's essentially a 2nd memory address register that lets you access 256-byte segments/banks of RAM using [bank switching](https://en.wikipedia.org/wiki/Bank_switching). This lets you take full advantage of the 32K of RAM in the Cypress chip.
5. **Fast increment instructions.** Add these to registers by replacing 74LS173s with 74LS193s, allowing you to more quickly increment without going through the ALU. This is used to speed up loops and array operations.
