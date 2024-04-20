bits 32

SAMPLERATE equ 44100 ; remember to change the timediv also
SAMPLES equ SAMPLERATE*2*144
JMAX equ 4
IMAX equ 3
NOISESTEPS equ 16

; this is an entrypoint to the synth that respects stdcall
; takes one parameter: pointer to the buffer
; this stub is 14 bytes, so when counting bytes, subtract that
section		.syntentr	code	align=1
global _render_song@4
_render_song@4:
	pushad
	mov 	edi, dword [esp+36]
	call 	_render_song_main@0
	popad
	ret 	4

; this is the real main function of the synth that assumes esi = pointer to buffer
; clobbers eax, ecx, edx & esi is advanced to the end of the buffer
section		.syntmain	code	align=1
global _render_song_main@0
_render_song_main@0:
	mov		edx, power
	xor 	eax, eax
.mainloop:
	push 	eax
	xor 	eax, eax
	fldz               ; x=0
.loop1:
	inc 	eax
	push 	eax
	xor		eax, eax
.loop2:
	inc 	eax
	call 	inner
	cmp 	al, JMAX
	jl 		.loop2
	pop 	eax
	cmp 	al, IMAX
	jl 		.loop1
	push 	eax
	fstp 	dword [esp]
	pop 	eax		
	stosd   		; store x into buffer
	pop 	eax
	inc 	eax
	cmp 	eax, SAMPLES
	jl 		.mainloop
	ret

section		.syntinnr	code	align=1
inner:
	push 	eax           	; sp=j addr sp+8=i sp+12=sampleno
	fild 	dword [esp+8] 	; i x
	fild 	dword [esp+12] 	; sampleno i x
	fdiv 	dword [c_timediv+edx-power] ; t=sampleno/88200 i x
	fild 	word [c_i3+edx-power]   ; 3 t i x
	fdiv 	st2, st0      	; 3 t i/3 x
	fld 	st1            	; t 3 t i/3 x
	fimul 	dword [esp]   	; t*j 3 t i/3 x
	fidiv 	word [c_i32+edx-power] ; t*j/32 3 t i/3 x
	fadd 	st3  			; r=t*j/32+i/3 3 t i/3 x
	fild 	word [c_i9+edx-power]
	fcomip 	st0, st1		; 9<r?
	jae 	.cont
	fstp 	st0
	fstp 	st0
	fstp 	st0
	fstp 	st0
	jmp 	.done
.cont:
	fld1          		; 1 r 3 t i/3
	fld  	st1      		; r 1 r 3 t i/3
	fprem         		; v=mod(r,1) 1 r 3 t i/3
	fsub 	st2, st0 		; v 1 r-v 3 t i/3
	fdiv 	st1, st0 		; v 1/v r-v 3 t i/3
	fimul 	word [c_i12+edx-power] ; v*12 1/v r-v 3 t i/3
	faddp 	st1, st0		; v*12+1/v r-v 3 t i/3
	fild  	dword [esp] 	; j v*12+1/v r-v 3 t i/3
	fdiv  	st0, st3   	; j/3 v*12+1/v r-v 3 t i/3
	faddp 	st1, st0   	; j/3+v*12+1/v r-v 3 t i/3
	faddp 	st4, st0   	; r-v 3 t q=j/3+v*12+1/v+i/3
	fprem         		; mod(r-v,3) 3 t q
	fidiv 	word [c_i6+edx-power]		; mod(r-v,3)/6 3 t q
	fadd 	dword [c_8_5+edx-power]	; mod(r-v,3)/6+8.5 3 t q
	call	edx    		; exp2(mod(r-v,3)/6+8.5) 3 t q
	fmul 	st2      		; t*exp2(mod(r-v,3)/6+8.5) 3 t q
	fimul 	dword [esp] 	; j*t*exp2(mod(r-v,3)/6+8.5) 3 t q
	fimul 	dword [esp+8] ; i*j*t*exp2(mod(r-v,3)/6+8.5) 3 t q
	fsin           		; o=sin(i*j*t*exp2(mod(r-v,3)/6+8.5)) 3 t q
	fld  	st2       		; t o 3 t q
	fidiv 	dword [esp] 	; t/j o 3 t q
	fidiv 	word [c_i47+edx-power] ; t/j/47 o 3 t q
	fsin           		; sin(t/j/47) o 3 t q
	fmulp 	st1, st0 		; sin(t/j/47)*o 3 t q
	fadd 	st0, st0
	fadd 	st0, st0 		; n=4*sin(t/j/40)*o 3=a t q	
	fxch 	st1, st0 		; a n t q
	test 	edi, 4
	jz 		.left
	fadd 	st2, st0 		; t+=3, stereo separation
.left:
	push 	NOISESTEPS
	pop  	eax
.loop3:	      		; a n t
	fld  	st2  		; t a n t
	fmul 	st0, st1  		; t*a a n t
	fsin      		; sin(t*a) a n t
	fdiv 	st0, st1    ; sin(s*a)/a a n t
    faddp 	st2, st0 	; a n_new t
	fmul 	dword [c_1_02+edx-power] 	; a_new n_new t
	fadd 	st2, st0 		; a_new n_new t_new 
	dec     eax
	jnz		.loop3
	fstp 	st0 		; n t q x	
	fsin  			; sin(n) t q x
	fxch 	st2, st0 	; q t sin(n) x	
	fchs 			; -q t sin(n) x
	fiadd 	word [c_i6+edx-power]		; 6-q t sin(n) q
	call 	edx  		; e=2**(6-q) t sin(n) x
	fmulp 	st2, st0 	; t e*sin(n) x
	fstp 	st0 		; e*sin(n) x
	faddp 	st1, st0 	; x+=env*sin(n)	
.done:
	pop 	eax
	ret

section		.synthelp	code	align=1
power:
    fld1          ; 1 x
    fld 	st1       ; x 1 x
    fprem         ; mod(x,1) 1 x
    f2xm1         ; 2^mod(x,1)-1 1 x
    faddp 	st1,st0 ; 2^mod(x,1) x
    fscale        ; 2^mod(x,1)*2^trunc(x) x
                  ; Equal to:
                  ; 2^x x
    fstp 	st1      ; 2^x
    ret

c_i3:
	dw 3

c_i6:
	dw 6

c_i9:
	dw 9

c_i12:
	dw 12

c_i32:
	dw 32

c_i47:
	dw 47

c_8_5 equ $-2
	dw 0x4108 ; 8.5

c_1_02 equ $-2
	dw 0x3f82 ; 1.02

c_timediv equ $-1
	db 0x44, 0xac, 0x47 ; 88200	
