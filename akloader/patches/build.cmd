@md data 2> nul
@md data\rpg 2> nul
@md data\r4 2> nul
@md data\ak2i 2> nul
@md data\r4idsn 2> nul
@md elf 2> nul
@md elf\rpg 2> nul
@md elf\r4 2> nul
@md elf\ak2i 2> nul
@md elf\r4idsn 2> nul
@rem common
@arm-eabi-as -o elf/bank_nor_1.elf bank_nor_1.s
@arm-eabi-objcopy -O binary elf/bank_nor_1.elf data/bank_nor_1.bin
@arm-eabi-as -o elf/bank_nor_2.elf bank_nor_2.s
@arm-eabi-objcopy -O binary elf/bank_nor_2.elf data/bank_nor_2.bin
@arm-eabi-as -o elf/il2.elf il2.s
@arm-eabi-objcopy -O binary elf/il2.elf data/il2.bin
@arm-eabi-as -o elf/patch7.elf patch7.s
@arm-eabi-objcopy -O binary elf/patch7.elf data/patch7.bin
@arm-eabi-as -o elf/patch7wram_sr.elf patch7wram_sr.s
@arm-eabi-objcopy -O binary elf/patch7wram_sr.elf data/patch7wram_sr.bin
@arm-eabi-as -o elf/repatch.elf repatch.s
@arm-eabi-objcopy -O binary elf/repatch.elf data/repatch.bin
@arm-eabi-as -o elf/repatch_pokemon.elf repatch_pokemon.s
@arm-eabi-objcopy -O binary elf/repatch_pokemon.elf data/repatch_pokemon.bin
@arm-eabi-as -o elf/repatch9.elf repatch9.s
@arm-eabi-objcopy -O binary elf/repatch9.elf data/repatch9.bin
@arm-eabi-as -o elf/unprot_001.elf unprot_001.s
@arm-eabi-objcopy -O binary elf/unprot_001.elf data/unprot_001.bin
@arm-eabi-as -o elf/unprot_002.elf unprot_002.s
@arm-eabi-objcopy -O binary elf/unprot_002.elf data/unprot_002.bin
@arm-eabi-as -o elf/unprot_003.elf unprot_003.s
@arm-eabi-objcopy -O binary elf/unprot_003.elf data/unprot_003.bin
@arm-eabi-as -o elf/unprot_004.elf unprot_004.s
@arm-eabi-objcopy -O binary elf/unprot_004.elf data/unprot_004.bin
@arm-eabi-as -o elf/unprot_005.elf unprot_005.s
@arm-eabi-objcopy -O binary elf/unprot_005.elf data/unprot_005.bin
@arm-eabi-as -o elf/unprot_006.elf unprot_006.s
@arm-eabi-objcopy -O binary elf/unprot_006.elf data/unprot_006.bin
@arm-eabi-as -o elf/unprot_007.elf unprot_007.s
@arm-eabi-objcopy -O binary elf/unprot_007.elf data/unprot_007.bin
@arm-eabi-as -o elf/unprot_008.elf unprot_008.s
@arm-eabi-objcopy -O binary elf/unprot_008.elf data/unprot_008.bin
@arm-eabi-as -o elf/unprot_009.elf unprot_009.s
@arm-eabi-objcopy -O binary elf/unprot_009.elf data/unprot_009.bin
@arm-eabi-as -o elf/unprot_010.elf unprot_010.s
@arm-eabi-objcopy -O binary elf/unprot_010.elf data/unprot_010.bin
@arm-eabi-as -o elf/unprot_011.elf unprot_011.s
@arm-eabi-objcopy -O binary elf/unprot_011.elf data/unprot_011.bin
@arm-eabi-as -o elf/unprot_012.elf unprot_012.s
@arm-eabi-objcopy -O binary elf/unprot_012.elf data/unprot_012.bin
@arm-eabi-as -o elf/unprot_013.elf unprot_013.s
@arm-eabi-objcopy -O binary elf/unprot_013.elf data/unprot_013.bin
@arm-eabi-as -o elf/unprot_014.elf unprot_014.s
@arm-eabi-objcopy -O binary elf/unprot_014.elf data/unprot_014.bin
@arm-eabi-as -o elf/unprot_015.elf unprot_015.s
@arm-eabi-objcopy -O binary elf/unprot_015.elf data/unprot_015.bin
@arm-eabi-as -o elf/unprot_016.elf unprot_016.s
@arm-eabi-objcopy -O binary elf/unprot_016.elf data/unprot_016.bin
@arm-eabi-as -o elf/unprot_017.elf unprot_017.s
@arm-eabi-objcopy -O binary elf/unprot_017.elf data/unprot_017.bin
@arm-eabi-as -o elf/unprot_018.elf unprot_018.s
@arm-eabi-objcopy -O binary elf/unprot_018.elf data/unprot_018.bin
@arm-eabi-as -o elf/unpatch9.elf unpatch9.s
@arm-eabi-objcopy -O binary elf/unpatch9.elf data/unpatch9.bin
@arm-eabi-as -o elf/sw_cw_ja.elf sw_cw_ja.s
@arm-eabi-objcopy -O binary elf/sw_cw_ja.elf data/sw_cw_ja.bin
@rem rpg
@arm-eabi-as -I ./include/rpg -o elf/rpg/dma4.elf dma4.s
@arm-eabi-objcopy -O binary elf/rpg/dma4.elf data/rpg/dma4.bin
@arm-eabi-as -I ./include/rpg -o elf/rpg/hb_reset.elf hb_reset.s
@arm-eabi-objcopy -O binary elf/rpg/hb_reset.elf data/rpg/hb_reset.bin
@arm-eabi-as -I ./include/rpg -o elf/rpg/resetpatch9.elf resetpatch9.s
@arm-eabi-objcopy -O binary elf/rpg/resetpatch9.elf data/rpg/resetpatch9.bin
@arm-eabi-as -I ./include/rpg -o elf/rpg/save_nand.elf save_nand.s
@arm-eabi-objcopy -O binary elf/rpg/save_nand.elf data/rpg/save_nand.bin
@arm-eabi-as -I ./include/rpg -o elf/rpg/sd_save.elf sd_save.s
@arm-eabi-objcopy -O binary elf/rpg/sd_save.elf data/rpg/sd_save.bin
@rem r4
@arm-eabi-as -I ./include/r4 -o elf/r4/dma4.elf dma4.s
@arm-eabi-objcopy -O binary elf/r4/dma4.elf data/r4/dma4.bin
@arm-eabi-as -I ./include/r4 -o elf/r4/hb_reset.elf hb_reset.s
@arm-eabi-objcopy -O binary elf/r4/hb_reset.elf data/r4/hb_reset.bin
@arm-eabi-as -I ./include/r4 -o elf/r4/resetpatch9.elf resetpatch9.s
@arm-eabi-objcopy -O binary elf/r4/resetpatch9.elf data/r4/resetpatch9.bin
@arm-eabi-as -I ./include/r4 -o elf/r4/save_nand.elf save_nand.s
@arm-eabi-objcopy -O binary elf/r4/save_nand.elf data/r4/save_nand.bin
@arm-eabi-as -I ./include/r4 -o elf/r4/sd_save.elf sd_save.s
@arm-eabi-objcopy -O binary elf/r4/sd_save.elf data/r4/sd_save.bin
@rem ak2i
@arm-eabi-as -I ./include/ak2i -o elf/ak2i/dma4.elf dma4.s
@arm-eabi-objcopy -O binary elf/ak2i/dma4.elf data/ak2i/dma4.bin
@arm-eabi-as -I ./include/ak2i -o elf/ak2i/hb_reset.elf hb_reset.s
@arm-eabi-objcopy -O binary elf/ak2i/hb_reset.elf data/ak2i/hb_reset.bin
@arm-eabi-as -I ./include/ak2i -o elf/ak2i/resetpatch9.elf resetpatch9.s
@arm-eabi-objcopy -O binary elf/ak2i/resetpatch9.elf data/ak2i/resetpatch9.bin
@arm-eabi-as -I ./include/ak2i -o elf/ak2i/save_nand.elf save_nand.s
@arm-eabi-objcopy -O binary elf/ak2i/save_nand.elf data/ak2i/save_nand.bin
@arm-eabi-as -I ./include/ak2i -o elf/ak2i/sd_save.elf sd_save.s
@arm-eabi-objcopy -O binary elf/ak2i/sd_save.elf data/ak2i/sd_save.bin
@rem r4idsn
@arm-eabi-as -I ./include/r4idsn -o elf/r4idsn/dma4.elf dma4.s
@arm-eabi-objcopy -O binary elf/r4idsn/dma4.elf data/r4idsn/dma4.bin
@arm-eabi-as -I ./include/r4idsn -o elf/r4idsn/hb_reset.elf hb_reset.s
@arm-eabi-objcopy -O binary elf/r4idsn/hb_reset.elf data/r4idsn/hb_reset.bin
@arm-eabi-as -I ./include/r4idsn -o elf/r4idsn/resetpatch9.elf resetpatch9.s
@arm-eabi-objcopy -O binary elf/r4idsn/resetpatch9.elf data/r4idsn/resetpatch9.bin
@arm-eabi-as -I ./include/r4idsn -o elf/r4idsn/save_nand.elf save_nand.s
@arm-eabi-objcopy -O binary elf/r4idsn/save_nand.elf data/r4idsn/save_nand.bin
@arm-eabi-as -I ./include/r4idsn -o elf/r4idsn/sd_save.elf sd_save.s
@arm-eabi-objcopy -O binary elf/r4idsn/sd_save.elf data/r4idsn/sd_save.bin
