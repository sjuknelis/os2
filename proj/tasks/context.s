INPUT_DATA = $8001
MB_REG = $8002

SC_COPY_VECTOR = $fe
INPUT_PAGE = $7d00
SC_PAGE = $7e00
SC_WRITE_INDEX = $7f00
SP_STORE = $7f01
INPUT_WRITE_INDEX = $7f02
ZERO_ON_NMI = $7f03
TASK_INIT_VECTOR = $7f10

.import __RAM_START__, __RAM_SIZE__

.import copydata, zerobss, initlib, donelib

.export _nmi_asm, _init_task_asm, _continue_task_asm

.include "zeropage.inc"

.segment "CODE"

.proc _init_task_asm: near

.segment "CODE"

    ; Save kernel space pointer
    tsx
    stx SP_STORE

    ; Load init vector and go to task space
    ldx TASK_INIT_VECTOR
    ldy TASK_INIT_VECTOR + 1
    sta MB_REG

    ; Save task init vector
    stx $00
    sty $01

    ; Set up erase loop vector and index
    lda #0
    sta $02
    sta $03
    ldy #4

@erase_loop:
    ; Erase indexed byte
    sta ($02), y

    ; Increment index and loop if not rolled over
    iny
    bne @erase_loop

    ; Increment high byte of vector until it reaches 0x10
    inc $03
    lda $03
    cmp #$10
    bne @erase_loop
; end erase_loop

    ; Set up fake interrupt trace to init vector
    lda $00
    sta $01fe
    lda $01
    sta $01ff
    lda #%00100100
    sta $01fd

    ; Set up fake stack pointer
    ldx #$f9
    txs

    ; Initialize C code
    lda #<(__RAM_START__ + __RAM_SIZE__)
    sta sp
    lda #>(__RAM_START__ + __RAM_SIZE__)
    sta sp + 1

    jsr zerobss
    jsr copydata
    jsr initlib
    
    ; Save task stack pointer
    tsx
    stx SP_STORE

    ; Go to kernel space
    lda #0
    sta MB_REG

    ; Load kernel stack pointer
    ldx SP_STORE
    txs

    rts

.endproc

.segment "CODE"

; Parameter A: task memory bank
.proc _continue_task_asm: near

.segment "CODE"

    ; Save kernel stack pointer
    tsx
    stx SP_STORE

    ; Move task space to X and go to task space
    tax
    stx MB_REG

    ; Skip sc write if syscall is zero
    lda SC_PAGE
    beq @skip_sc_write

    ; Initialize sc write index in task space
    ldy #0
    sty SC_WRITE_INDEX

    ; Go to kernel space and initialize sc copy vector
    sty MB_REG
    sty SC_COPY_VECTOR
    txa
    ora #$60
    sta SC_COPY_VECTOR + 1

@sc_write_loop:
    ; Get byte from kernel space
    ldy #0
    lda (SC_COPY_VECTOR), y

    ; Go to task space and store byte
    stx MB_REG
    ldy SC_WRITE_INDEX
    sta SC_PAGE, y

    ; Increment sc write index in task space
    inc SC_WRITE_INDEX

    ; Go to kernel space and increment sc write index
    ldy #0
    sty MB_REG
    inc SC_COPY_VECTOR

    ; If no overflow, continue loop
    bne @sc_write_loop
; end sc_write_loop

    ; Go to task space
    stx MB_REG

@skip_sc_write:
    ; Load task stack pointer
    ldx SP_STORE
    txs

    ; Trigger IO lockout
    sta $80ff

    ; Load variables and return from NMI/init_task_asm
    pla
    tay
    pla
    tax
    pla
    
    rti

.endproc

.segment "STARTUP"

_nmi_asm:
    pha

    ; If already in kernel space, don't switch to kernel
    lda MB_REG
    bne @switch_to_kernel

    ; Clear zero to NMI flag
    lda #0
    sta ZERO_ON_NMI

    ; Write input byte to queue
    jsr @write_input_byte

    pla
    rti

@switch_to_kernel:
    txa
    pha
    tya
    pha

    ; Save task stack pointer
    tsx
    stx SP_STORE

    ; Go to kernel space, saving task space in X
    ldx MB_REG
    ldy #0
    sty MB_REG

    ; Initialize kernel sc copy vector
    sty SC_COPY_VECTOR
    txa
    ora #$60
    sta SC_COPY_VECTOR + 1

    ; Clear zero to NMI flag
    sty ZERO_ON_NMI

    ; Write input byte to queue
    jsr @write_input_byte

    ; Return to task space and initialize sc write index
    stx MB_REG
    ldy #0
    sty SC_WRITE_INDEX

@sc_write_loop:
    ; Get byte from task space
    lda SC_PAGE, y

    ; Go to kernel space and store byte
    ldy #0
    sty MB_REG
    sta (SC_COPY_VECTOR), y

    ; Increment sc write index in kernel space
    inc SC_COPY_VECTOR

    ; Go to task space and increment sc write index
    stx MB_REG
    inc SC_WRITE_INDEX

    ; If no overflow, continue loop
    ; y contains sc write index at top of loop
    ldy SC_WRITE_INDEX
    bne @sc_write_loop
; end sc_write_loop

    ; Go to kernel space
    lda #0
    sta MB_REG

    ; Load kernel stack pointer
    ldx SP_STORE
    txs

    ; Return from continue_task_asm
    rts

    ; Modifies A
@write_input_byte:
    tya
    pha

    ; Check if input byte non-zero
    lda INPUT_DATA
    beq @skip_write_input_byte

    ; Write input byte to buffer
    ldy INPUT_WRITE_INDEX
    sta INPUT_PAGE, y
    inc INPUT_WRITE_INDEX

    ; Erase input byte
    sta INPUT_DATA

@skip_write_input_byte:
    pla
    tay
    rts
; end write_input_byte