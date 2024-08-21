; Startup code for cc65 (Single Board Computer version)

.import _main, _nmi_asm

.export __STARTUP__ : absolute = 1        ; Mark as startup
.import __RAM_START__, __RAM_SIZE__       ; Linker generated

.import copydata, zerobss, initlib, donelib

.include "zeropage.inc"

.segment "STARTUP"

_init:
  ; Initialize stack pointer to $01FF
  ldx #$ff
  txs

  ; Clear decimal mode
  cld                     

  ; Set cc65 argument stack pointer
  lda #<(__RAM_START__ + __RAM_SIZE__)
  sta sp
  lda #>(__RAM_START__ + __RAM_SIZE__)
  sta sp + 1

  ; Initialize memory storage
  jsr zerobss   ; Clear BSS segment
  jsr copydata  ; Initialize DATA segment
  jsr initlib   ; Run constructors

  ; Call main()
  jsr _main

_exit:
  ; Back from main (this is also the _exit entry)
  jsr donelib
  brk

_brk:
  ;lda #61
  ;sta $8000
  rti

.segment "VECTORS"

.addr _nmi_asm  ; NMI vector
.addr _init     ; Reset vector
.addr _brk      ; IRQ/BRK vector