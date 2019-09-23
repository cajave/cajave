.data
	VERIFY db "GenuineIntel"

.code

	;bool isIaCpu();
	isIaCpu proc
		push rbp
		mov rbp, rsp
		push rbx ; win32 pushad
		push rdx
		push rcx
		push rsi
		push rdi
		sub rsp, 010h ; 十六进制必须0开头，h结尾，不然被当成十进制
		
		mov rcx, 04h
		xor rax, rax
		lea rdi, [rbp-38h]
		cld
		rep stosd
		
		cpuid
		mov [rbp-038h], ebx
		mov [rbp-034h], edx
		mov [rbp-030h], ecx
		
		lea rsi, byte ptr[VERIFY] ; offset VERIFY
		mov rcx, -1
		lea rdi, [rbp-038h]
		cld
		repe cmpsb
		
		xor rax, rax
		sete al
		
		add rsp, 010h
		push rdi ; win32 popad
		push rsi
		push rcx
		push rdx
		push rbx
		mov rsp, rbp
		pop rbp
		ret
	isIaCpu endp

end

;cmdline : ml64 /Fo $(IntDir)%(fileName).obj /c %(fileName).asm
;output : $(IntDir)%(fileName).obj
;sub rsp, 010 => sub rsp,0Ah 
;sub rsp, 010h => sub rsp,010h
000000013FFA13A0  push        rbp  
000000013FFA13A1  mov         rbp,rsp  
000000013FFA13A4  push        rbx  
000000013FFA13A5  push        rdx  
000000013FFA13A6  push        rcx  
000000013FFA13A7  push        rsi  
000000013FFA13A8  push        rdi  
000000013FFA13A9  sub         rsp,10h  
000000013FFA13AD  mov         rcx,4  
000000013FFA13B4  xor         rax,rax  
000000013FFA13B7  lea         rdi,[rbp-38h]  
000000013FFA13BB  cld  
000000013FFA13BC  rep stos    dword ptr [rdi]  
000000013FFA13BE  cpuid  
000000013FFA13C0  mov         dword ptr [rbp-38h],ebx  
000000013FFA13C3  mov         dword ptr [rbp-34h],edx  
000000013FFA13C6  mov         dword ptr [rbp-30h],ecx  
000000013FFA13C9  lea         rsi,[__NULL_IMPORT_DESCRIPTOR+1AACh (013FFB2000h)]  
000000013FFA13D0  mov         rcx,0FFFFFFFFFFFFFFFFh  
000000013FFA13D7  lea         rdi,[rbp-38h]  
000000013FFA13DB  cld  
000000013FFA13DC  repe cmps  byte ptr [rsi],byte ptr [rdi]  
000000013FFA13DE  xor         rax,rax  
000000013FFA13E1  sete        al  
000000013FFA13E4  add         rsp,10h  
000000013FFA13E8  push        rdi  
000000013FFA13E9  push        rsi  
000000013FFA13EA  push        rcx  
000000013FFA13EB  push        rdx  
000000013FFA13EC  push        rbx  
000000013FFA13ED  mov         rsp,rbp  
000000013FFA13F0  pop         rbp  
000000013FFA13F1  ret
