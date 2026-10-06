# Minimal GNU as label conventions for generated spimdisasm output.
.macro glabel name
    .globl \name
\name:
.endm

.macro endlabel name
.endm

.macro nonmatching name, size
.endm

.macro alabel name
\name:
.endm

.macro jlabel name
\name:
.endm

.macro dlabel name
    .globl \name
\name:
.endm

.macro enddlabel name
.endm
