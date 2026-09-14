global switch_to

section .text

; =============================================================================
; SENG21213-OS :: Context Switch
;
; void switch_to(uint32_t *old_esp, uint32_t new_esp)
;
; Saves the current CPU register context on the current stack,
; stores the resulting ESP through old_esp, then restores the
; next process context from new_esp.
; =============================================================================

switch_to:
    ; Save all general-purpose registers
    pushad

    ; Get arguments.
    ; After PUSHAD, the original arguments are 32 bytes above ESP.
    mov eax, [esp + 36]        ; old_esp pointer
    mov edx, [esp + 40]        ; new_esp value

    ; Save current process stack pointer
    mov [eax], esp

    ; Switch to next process stack
    mov esp, edx

    ; Restore next process registers
    popad

    ; Return to the next process
    ret
