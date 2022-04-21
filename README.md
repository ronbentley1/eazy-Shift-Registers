# eazy-Shift-Registers
A library to support multiple shift registers, SIPOs, as a continuous array and in banks of registers.


# Introduction - More from Less

Arduino microcontrollers are great for offering readily usable digital and analogue I/O pins for connecting to the outside world; some boards have few, some have many (eg MEGA 2560). But sometimes there just aren't enough for our project or we want to keep their use to a minimum. This is where the humble and low cost Serial-Parallel Integrated Circuit (SIPO IC) comes to our rescue!


SIPO ICs offer us significant reductions in microcontroller pin usage, a factor of at least 3:8 for a singly wired 8bit IC and much, much better ratios if we connect them in a cascade (daisy chain). For example, if we cascade 4 SIPO ICs this improvement increases to 3:32! Even greater efficiencies can be leveraged.


If you have already dabbled or used in anger SIPO ICs in your projects then you will know that they can present their own challenges - usually lots of wiring, difficult to use software drivers/methods to mention just two. So anything that improves our lot would be a good thing? Of course. This is where the ez_SIPO8 library can provide a considerable range of easy to use functions and methods that can make project design using SIPO ICs a dream. Even more than that, the SIPO8 design architecture can lead to innovation and highly scalable project designs.


The SIPO8 non-blocking library has been designed to support up to 255 8bit SIPO ICs, configured into banks, each of which may contain (map) from one to many individual SIPO ICs in a cascaded arrangement.

Indeed, the theoretical maximum number of SIPO outputs that the library can support is a massive 2040 output pins, arranged into banks of SIPO ICs of any number. A single bank of SIPO ICs requires its own 3-wire microcontroller digital pins to drive it, but a single such interface can support banks of any size, even beyond eight SIPO ICs which seems to be a current limitation of some implementations.

By using the SIPO8 library in your projects you will be able to design and craft straight forward, innovative and elegant solutions incorporating SIPO ICs, even if it is just one, or dozens.


The following article provides a synopsis of the SIPO8 library, but a complete and comprehensive description including worked examples can be found in the User Guide. In addition, understanding of the SIPO8 library is further supplemented with a summarised crib sheet and six standalone tutorials each with associated downloadable sketches/examples.


# The SIPO8 Design Objectives

For those readers who have seen some of my previous articles, eg ez_switch_lib, you will know that I like to inject a quotation or two not necessarily from the world of computing (it is surprising how these two worlds can align!). Here we go...

- A computer is a machine for constructing mappings from input to output. Michael Kirby

- Creativity is an input to innovation and change is the output from innovation. Braden Kelley

- Through great input you get great output. RZA

These three quotations are particularly pertinent in nicely summarising the essence of the design architecture of the SIPO8 library. At the outset, a number of key objectives were established for the design of the SIPO8 library, these being a library that provided/supported:


- a design that would maximise the number of physical output pins that could be supported/driven by the standard range of Arduino microcontrollers
- a representation of physical SIPO ICs and their output pins into the virtual, allowing independent programmatic manipulation of outputs
- a logical, flexible and straight forward design - one that would allow the end user developer to easily manage/control a large number of outputs discretely, or as a set of outputs; a design architecture that would be modular and granular
- recognition that many common SIPO ICs have a standard hardware interface which could be leveraged and used to good advantage – SIPO IC hardware transparency
- a solution that provides support for many 100s of outputs
- a rich set of functional capabilities to define, control and drive SIPO ICs, individually or in a cascaded arrangement, as individual banks
- the capability to group, through cascading/daisy chaining, any number of SIPO ICs in a single arrangement (bank), even beyond eight ICs in a single cascade
- the capability to define any number of differently sized SIPO banks, single or multiple and cascaded SIPO ICs
- the capability to work with SIPOs of different and mixed bit/pin size, e.g. 8, 16, 32 or 64bit SIPO ICs
- tools for the end user to examine key data during development


The SIPO8 library meets and exceeds all of the above design objectives and, whilst under development, new capabilities also became evident, not foreseen at the outset (e.g. bank interleaving).


The end result is a non-blocking library that offers a wide range of features and capabilities to command and control any number of SIPO ICs, wired singly or in cascades.


# Key Features

In summary, the key features of the SIPO8 library are:

- virtual mapping of all physical SIPO IC output pins
- ability to create multiple banks of SPIO ICs, of varying bank size
- rich set of functions/methods to manage and update virtual SIPO output pins using absolute (array level pin addressing) and relative pin/bank addressing (bank level pin addressing)
- updating of physical SIPO IC output pins as an entire array or individually as separate banks
- SIPO IC transparency - any SIPO IC 8bit, 16bit, 32bit, etc that conforms to same 3-wire interface and cascade capabilities as the standard 74HC595 IC
- the ability to interleave different banks of SIPO ICs using the same 3-wire interface
- any number of user definable timers
- comprehensive reporting of array pool/bank status and associated parameters
- comprehensive documentations - User Guide, Crib Sheet, example sketches and tuorials

# And Finally

Hopefully, at this point, you will know if the SIPO8 library is worthy of further exploration. Certainly, if you have SIPO IC projects in the pipeline, as a hobbyist or as a professional, then the library will be very helpful in developing these.

<END>
  
