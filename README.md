# EEE3096S Practical 3: Voltage Mirror

Group 79, LXXTRI004 and SHRGIL004. UCT, second semester 2026.

Read an analogue input on PA5 with ADC channel 5 and reproduce it on PA4 with DAC
channel 1. SW0 switches between FULL12 (D = C) and COARSE6 (D = 64*floor(C/64)).
Both converters stay 12-bit in hardware; only the digital mapping changes.

Due Friday 23 October 2026 18:00 SAST on Gradescope, submitted before the demo.
Demo sessions are Friday 16 October and Friday 23 October, 14:00 to 18:00.
80 marks for the live demonstration, 20 for the report.

## Layout

| Path | What it is |
| --- | --- |
| `Practical3/` | Upstream starter project. Only `Core/Src/voltage_mirror.c` is ours |
| `docs/` | Course brief, results sheet and the blank template PDF |
| `report/` | `prac_03_lxxtri004_shrgil004.tex`, its measurement slots and figures |

## Build and check

```sh
make -C Practical3 test      # host check: all 4096 codes, 64 coarse levels, bounded writes
make -C Practical3 -j4       # firmware, build/Debug/student/Practical3.elf
make -C report check         # lists every measurement and capture still missing
```

Keep the C6 startup and `STM32F051C6TX_FLASH.ld`: 32 KiB flash, 8 KiB RAM. Import
the CubeIDE project as supplied. Regenerating from the `.ioc` overwrites the
supplied clock, LCD, button and sampling infrastructure.

The report that was actually submitted is `report/submitted/Practical3_report.pdf`,
handed in 16 October 2026. It was written separately rather than from the scaffold in
`report/`, so that scaffold still has every measurement slot empty and is kept only as
the LaTeX skeleton. Read the submitted PDF for what the group reported.

No measured value enters a report without the instrument reading behind it.
Every number lives in `report/measurements.tex`, and a slot still holding
`\bench{}` is one that has not been measured. The bench runbook is
`pracs/prac-3/Prac3-runbook.md` in the course vault.
