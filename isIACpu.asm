.data
	VERIFY db "GenuineIntel"

.code

	;bool isIaCpu();
	isIaCpu proc
		push rbp
		mov rbp, rsp
		sub rsp, 010h; 十六进制必须0开头，h结尾，不然被当成十进制
		
		or rcx, 04h
		xor rax, rax
		lea rdi, [rbp-010h]
		cld
		rep stosd
		
		cpuid
		mov [rbp-010h], ebx
		mov [rbp-0ch], edx
		mov [rbp-08h], ecx
		
		lea rsi, byte ptr[VERIFY]
		or rcx, -1
		lea rdi, [rbp-010h]
		cld
		repne cmpsb
		
		xor rax, rax
		sete al
		
		add rsp, 010h
		mov rsp, rbp
		pop rbp
		ret
	isIaCpu endp

end

;cmdline : ml64 /Fo $(IntDir)%(fileName).obj /c %(fileName).asm
;output : $(IntDir)%(fileName).obj
;sub rsp, 010 => sub rsp,0Ah 
;sub rsp, 010h;
000000013FD654C0  push        rbp  
000000013FD654C1  mov         rbp,rsp  
000000013FD654C4  sub         rsp,10h  
000000013FD654C8  or          rcx,4  
000000013FD654CC  xor         rax,rax  
000000013FD654CF  lea         rdi,[rbp-10h]  
000000013FD654D3  cld  
000000013FD654D4  rep stos    dword ptr [rdi]  
000000013FD654D6  cpuid  
000000013FD654D8  mov         dword ptr [rbp-10h],ebx  
000000013FD654DB  mov         dword ptr [rbp-0Ch],edx  
000000013FD654DE  mov         dword ptr [rbp-8],ecx  
000000013FD654E1  lea         rsi,[__pobjMapEntryLast+4B0h (013FD8F000h)]  
000000013FD654E8  or          rcx,0FFFFFFFFFFFFFFFFh  
000000013FD654EC  lea         rdi,[rbp-10h]  
000000013FD654F0  cld  
000000013FD654F1  repne cmps  byte ptr [rsi],byte ptr [rdi]  
000000013FD654F3  xor         rax,rax  
000000013FD654F6  sete        al  
000000013FD654F9  add         rsp,10h  
000000013FD654FD  mov         rsp,rbp  
000000013FD65500  pop         rbp  
000000013FD65501  ret  