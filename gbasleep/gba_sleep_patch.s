@SleepHack (Release #2)
@By Dan Weiss (Dwedit)
@July 1, 2007
@fixed for ezflash 3in1 by somebody
	.text
	.arm
REG_BASE		= 0x4000000
REG_DISPCNT		= 0x00
REG_DISPSTAT	= 0x04
REG_VCOUNT		= 0x06
REG_BG0CNT		= 0x08
REG_BG1CNT		= 0x0A
REG_BG2CNT		= 0x0C
REG_BG3CNT		= 0x0E
REG_BG0HOFS		= 0x10
REG_BG0VOFS		= 0x12
REG_BG1HOFS		= 0x14
REG_BG1VOFS		= 0x16
REG_BG2HOFS		= 0x18
REG_BG2VOFS		= 0x1A
REG_BG3HOFS		= 0x1C
REG_BG3VOFS		= 0x1E
REG_WIN0H		= 0x40
REG_WIN1H		= 0x42
REG_WIN0V		= 0x44
REG_WIN1V		= 0x46
REG_WININ		= 0x48
REG_WINOUT		= 0x4A
REG_BLDCNT		= 0x50
REG_BLDALPHA	= 0x52
REG_BLDY		= 0x54
REG_SOUND1CNT_L	= 0x60
REG_SOUND1CNT_H	= 0x62
REG_SOUND1CNT_X	= 0x64
REG_SOUND2CNT_L	= 0x68
REG_SOUND2CNT_H	= 0x6C
REG_SOUND3CNT_L	= 0x70
REG_SOUND3CNT_H	= 0x72
REG_SOUND3CNT_X	= 0x74
REG_SOUND4CNT_L	= 0x78
REG_SOUND4CNT_H	= 0x7c
REG_SOUNDCNT_L		= 0x80
REG_SOUND2CNT_H		= 0x82
REG_SOUNDCNT_X		= 0x84
REG_SOUNDBIAS		= 0x88
REG_WAVE_RAM0_L		= 0x90
REG_FIFO_A_L	= 0xA0
REG_FIFO_A_H	= 0xA2
REG_FIFO_B_L	= 0xA4
REG_FIFO_B_H	= 0xA6
REG_DM0SAD		= 0xB0
REG_DM0DAD		= 0xB4
REG_DM0CNT_L	= 0xB8
REG_DM0CNT_H	= 0xBA
REG_DM1SAD		= 0xBC
REG_DM1DAD		= 0xC0
REG_DM1CNT_L	= 0xC4
REG_DM1CNT_H	= 0xC6
REG_DM2SAD		= 0xC8
REG_DM2DAD		= 0xCC
REG_DM2CNT_L	= 0xD0
REG_DM2CNT_H	= 0xD2
REG_DM3SAD		= 0xD4
REG_DM3DAD		= 0xD8
REG_DM3CNT_L	= 0xDC
REG_DM3CNT_H	= 0xDE
REG_TM0D		= 0x100
REG_TM0CNT		= 0x102
REG_IE			= 0x200
REG_IF			= 0x202
REG_P1			= 0x130
REG_P1CNT		= 0x132
REG_WAITCNT		= 0x204


install_handler:
	@r0 = address of interrupt handler
	mov r1,#0x04000000
	str r0,[r1,#-(0x04000000-0x03FFFFB4)]
	@install "my_irq"
	adr r0,my_irq
	str r0,[r1,#-(0x04000000-0x03FFFFB8)]

	ldr r0,=0xE5901200 @ldr r1,[r0,#REG_IE]
	str r0,[r1,#-(0x04000000-0x03FFFFA0)]
	ldr r0,=0xE3110801 @tst r1,#0x00010000
	str r0,[r1,#-(0x04000000-0x03FFFFA4)]
	ldr r0,=0x03110201
	str r0,[r1,#-(0x04000000-0x03FFFFA8)]
	ldr r0,=0x0510F04C @ldreq pc,[r0,#-0x50]
	str r0,[r1,#-(0x04000000-0x03FFFFAC)]
	ldr r0,=0xE510F048 @ldr pc,[r0,#-0x4C]
	str r0,[r1,#-(0x04000000-0x03FFFFB0)]

	@install tiny IRQ handler
	ldr r0,=0x03007FA0
	str r0,[r1,#-4]
	
	bx lr
my_irq:

@new feature: use keyboard interrupt to try to stop game from detecting START
@cases:

@if L+R && START
@	sleep
@if L+R && IF && IE  (this can't happen)
@if L+R && KEYINT off && IE off
@	install key interrupt
@	enable key interrupt
@if L+R && KEYINT on && IE off
@	can get here if game disabled key interrupt after we installed it
@	check if it's ours
@	if so, enable Key interrupt (not sure that it will work anyway)
@if L+R && KEYINT off && IE on
@	install key interrupt
	
	@r0 = reg_base
	@r1 = REG_IE,REG_IF
	ldr r2,[r0,#REG_P1]
	tst r2,#0x0300	@L+R?
	ldrne pc,[r0,#-(0x04000000-0x03FFFFB4)] @to IRQ routine if not pressed
	tst r2,#0x0008	@Start?
	beq sleep_now
	@from here, there's no chance of Sleep happening...
	
	@r1 = REGIF...REGIE
	@check if has a keyboard handler
	tst r2,#0x40000000
	bne has_keyint
install_key_int:
	mov r3,#0xC3000000		@interrupt on L+R+START
	orr r3,r3,#0x00080000
	str r3,[r0,#REG_P1]
install_key_int_2:
	orr r1,r1,#0x1000
	add r2,r0,#REG_IE
	strh r1,[r2]
	ldr pc,[r0,#-(0x04000000-0x03FFFFB4)] @to normal IRQ routine
has_keyint:
	@is keyint ours?
	bic r3,r2,#0xFF00
	bic r3,r3,#0x00FF
	ldr r12,=0xC3080000
	cmp r3,r12
	ldrne pc,[r0,#-(0x04000000-0x03FFFFB4)] @to normal IRQ routine
	@if keyboard interrupt bit of reg_if someone got turned on...
	tst r1,#0x10000000
	beq install_key_int_2
	mov r3,#0x1000
	add r2,r0,#REG_IE
	strh r3,[r2,#2]
	b install_key_int_2
sleep_now:
	stmfd sp!,{r4-r11,lr}
	add r1,r0,#REG_SOUND1CNT_L
	@copy and push 32 bytes
	ldmia r1!,{r2-r9}
	stmfd sp!,{r2-r9}
	@copy and push 32 bytes
	ldmia r1!,{r2-r9}
	stmfd sp!,{r2-r9}
	@r3 = contents of REG_SOUND3CNT_X


	@save old io values
	add r1,r0,#REG_IE
	ldrh r4,[r1]
	ldr r5,[r0,#REG_P1]
	ldrh r6,[r0,#REG_DISPCNT]
	
	@enable ints on Keypad, Game Pak
	ldr r1,=0xFFFF1000 @3in1 fix
	str r1,[r0,#REG_IE]
	mov r1,#0xC0000000		@interrupt on start+sel
	orr r1,r1,#0x000C0000
	str r1,[r0,#REG_P1]
	strh r0,[r0,#REG_SOUNDCNT_X]	@sound off
	orr r1,r6,#0x80
	strh r1,[r0,#REG_DISPCNT]	@LCD off
	
	swi 0x030000

	@Loop to wait for letting go of Sel+start
loop:
	mov r0,#REG_BASE
	ldr r1,[r0,#REG_P1]
	and r1,r1,#0x000C
	cmp r1,#0x000C
	bne loop
	
	@spin until VCOUNT==159
spin2:
	ldrh r1,[r0,#REG_VCOUNT]
	cmp r1,#159
	bne spin2
	@spin until VCOUNT==160
spin4:
	ldrh r1,[r0,#REG_VCOUNT]
	cmp r1,#160
	bne spin4
	@spin until VCOUNT==159
spin5:
	ldrh r1,[r0,#REG_VCOUNT]
	cmp r1,#159
	bne spin5
	@spin until VCOUNT==160
spin6:
	ldrh r1,[r0,#REG_VCOUNT]
	cmp r1,#160
	bne spin6
	@spin until VCOUNT==159
spin7:
	ldrh r1,[r0,#REG_VCOUNT]
	cmp r1,#159
	bne spin7
	
	@restore interrupts
	add r1,r0,#REG_IE
	strh r4,[r1]
	@restore joystick interrupt
	str r5,[r0,#REG_P1]
	mov r4,#0x1000 @clear the damn joystick interrupt
	strh r4,[r1,#2]
	
	@restore screen
	strh r6,[r0,#REG_DISPCNT]
	ldmfd sp!,{r2-r9}	
	@restore sound state
	str r3,[r0,#REG_SOUNDCNT_X]
	add r1,r0,#0x80
	stmia r1!,{r2-r9}
	add r1,r0,#0x60
	ldmfd sp!,{r2-r9}
	stmia r1!,{r2-r9}
	ldmfd sp!,{r4-r11,lr}	
	@spin until VCOUNT==160, triggers next vblank
spin3:
	ldrh r1,[r0,#REG_VCOUNT]
	cmp r1,#160
	bne spin3  @<insert ytmnd cliche here>
@all done!
	ldr pc,[r0,#-(0x04000000-0x03FFFFB4)] @to IRQ routine

@invocation:  (these are added by the patcher tool)
@stmfd sp!,{r0,r1,r12,lr}
@mov r12,rA
@mov r1,rB
@mov r0,r12
@bl call_address
@ldmfd sp!,{r0,r1,r12,lr}
@ldr rB,=return_adress
@bx rB
