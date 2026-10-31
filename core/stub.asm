.data
public trampoline_target
trampoline_target dq 0

.code

public trampoline_stub
trampoline_stub proc
    add rsp, 8
    jmp qword ptr [trampoline_target]
trampoline_stub endp

end
